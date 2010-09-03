/****************************************************************************

Name
    dekludge.c -- C-INTERCAL expression and flow optimizers

DESCRIPTION
    This file contains optimizations used by the C-INTERCAL compiler.

LICENSE TERMS
    Copyright (C) 1996 Eric S. Raymond

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

****************************************************************************/
/*LINTLIBRARY */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sizes.h"
#include "ick.h"
#include "parser.h"
#include "fiddle.h"
#include "ick_lose.h"
#include "feh.h"

extern int emitlineno; /* AIS: From feh2.c */

/* AIS: The following macro produces optimization debug data. I added all
   occurences of it in this source file. It also keeps track of whether
   optimizations have taken place. */
#define OPTING(x) if(optdebug == 2) {\
                    explexpr(optdebugnode,stderr);\
                    putc('\n',stderr); } \
                  if(optdebug == 3) {\
                    prexpr(optdebugnode,stderr,0);\
                    putc('\n',stderr); } \
                  if(optdebug) fprintf(stderr,"[%s]",#x);\
                  if(optdebug >= 2) putc('\n',stderr);\
                  opted = 1;

void checknodeactbits(node *np); /* AIS: This prototype needed early */
extern void prexpr(node *np, FILE* fp, int freenode); /* AIS */

/* By AIS. This function looks remarkably like a C++ copy-constructor to me,
   just with extra arrows in. The annotations here show that nulls are allowed
   when called recursively, but not otherwise. */
/*@-incondefs@*/
/*@null@*/ node *nodecopy(/*@null@*/ const node* n)
/*@=incondefs@*/
{
  node* np;
  if(!n) return 0;
  np = cons(n->opcode, nodecopy(n->lval), nodecopy(n->rval));
  np->constant = n->constant;
  np->optdata = n->optdata;
  np->width = n->width;
  return np;
}

/* This function by AIS. Compares expressions. In C++, I'd call this
   node::operator== . */
bool nodessame(/*@observer@*/ const node* n1, /*@observer@*/ const node* n2)
{
  if(!n1) return !n2;
  if(!n2) return 0;
  if(n1->opcode!=n2->opcode)
    return (((n1->opcode == MESH && n2->opcode == MESH32) ||
        (n1->opcode == MESH32 && n2->opcode == MESH)) &&
        n1->constant == n2->constant);
  if(!nodessame(n1->lval,n2->lval)) return 0;
  if(!nodessame(n1->rval,n2->rval)) return 0;
  switch(n1->opcode)
  {
  case ick_ONESPOT:
  case ick_TWOSPOT:
  case ick_HYBRID:
  case ick_TAIL:
  case MESH:
  case MESH32:
    return n1->constant == n2->constant;
  case AND:
  case OR:
  case XOR:
    return n1->width == n2->width;
  default:
    return true;
  }
}

/* AIS: Checks if an abstention could affect a tuple. */
static int abstainmatch(int npconstant, int tptype)
{
  if(npconstant == tptype) return 1;
  if(npconstant == ABSTAIN)
    if(tptype == FROM || tptype == MANYFROM || tptype == DISABLE) return 1;
  if(npconstant == REINSTATE)
    if(tptype == ENABLE) return 1;
  if(npconstant == GETS)
    if(tptype == RESIZE) return 1;
  if(npconstant == UNKNOWN) /* JH */
    if(tptype == SPLATTERED) return 1;
  return 0;
}


/*************************************************************************
 *
 * AIS: Flow optimizer. (I wrote the whole of optimizef.)
 *
 * This analyses the flow of the program, checking to see which lines can
 * have their guards omitted, which lines can be omitted altogether, which
 * variables can never be ignored, etc. The degenerator lower down can take
 * this information into account, producing faster and shorter source code.
 * In an ideal world, we'd end up with the produced INTERCAL looking like
 * proper C code, but somehow I think that's unlikely to happen.
 * At the moment, it just checks for unused !ick_abstained[] guards on
 * statements and removes them (possibly removing comments altogether).
 * This is minor, but should clear up degenerated code substantially.
 * It also allows the code for gets to replace ick_assign() with the faster =
 * in cases where this doesn't affect the behaviour of the program.
 * See the function itself for what I did to NEXT.
 *
 *************************************************************************/

void optimizef(void)
{
  tuple* tp, *tpa, *tpb;
  atom* op;
  node* np;
  if(!flowoptimize) ick_lose(IE778, iyylineno, (const char *) NULL);
  for (tp = tuples; tp < tuples + ick_lineno; tp++) tp->abstainable = 0;
  /* abstainable holds whether a line's abstention status can change */
  for (op = oblist; op != NULL && op < obdex; op++) op->ignorable = 0;
  /* ignorable holds whether a variable's ignorance status can change */
  for (tp = tuples; tp < tuples + ick_lineno; tp++)
  {
    /* allow for warnings to be generated during flow optimisations */
    /* AIS: I marked tuples as only deliberately, so that it produced warnings
       when aliased in an unsafe way. However, tuples isn't realloced during
       optimisation, so we can safely ignore the warning for it produced
       here. */
    /*@-onlytrans@*/
    optuple = tp;
    /*@=onlytrans@*/
    if(tp->maybe) tp->abstainable = true;
    if(tp->exechance < 0) tp->initabstain = true;
    if(tp->exechance != 100 && tp->exechance != -100) tp->abstainable = true;
    if(tp->onceagainflag != onceagain_NORMAL) tp->abstainable = true;
    if(tp->type == ABSTAIN || tp->type == FROM)
    {
      tpa = tp->u.target - 1 + tuples;
      if(tpa->exechance >= 0) tpa->abstainable = true;
    }
    if(tp->type == REINSTATE)
    {
      tpa = tp->u.target - 1 + tuples;
      if(tpa->exechance < 0) tpa->abstainable = true;
    }
    if(tp->type == DISABLE || tp->type == MANYFROM)
    {
      for (tpa = tuples; tpa < tuples + ick_lineno; tpa++)
      {
	np = tp->u.node;
	if(tp->type == MANYFROM) np = np->rval;
	for(; np; np = np -> rval)
	  if(abstainmatch((int)np->constant, (int)tpa->type))
	    if(tpa->exechance >= 0) tpa->abstainable = true;
      }
    }
    if(tp->type == ENABLE)
    {
      for (tpa = tuples; tpa < tuples + ick_lineno; tpa++)
      {
	np = tp->u.node;
	for(; np; np = np -> rval)
	  if(abstainmatch((int)np->constant, (int)tpa->type))
	    if(tpa->exechance < 0) tpa->abstainable = true;
      }
    }
    if(tp->type == GETS && ick_Base == 2 && !opoverused)
      checknodeactbits(tp->u.node->rval);
    /* If optdata shows that the value must always fit in the variable,
       and the variable cannot be ignored, ick_assign can be replaced by the
       cheaper =. */
    if(tp->type == IGNORE)
    {
      for (np = tp->u.node; np; np = np->rval)
      {
	for (op = oblist; op != NULL && op < obdex; op++)
	{
	  if(op->type == np->opcode && (unsigned long)op->intindex == np->constant)
	    op->ignorable = 1;
	}
      }
    }
    /* REMEMBERING variables has no effect on this code, because all
       variables are initially remembered anyway, and so an IGNORE would
       be needed (and caught above) to have an effect. */
  }
  /* There are some flow idioms that maybe should be optimized. The most
     common is the NEXTing idiom for if(), which looks like this:

         DO (1) NEXT
	 block 2
	 escape via NEXTING or COMEFROM
     (1) DO (2) NEXT
         DO FORGET #1
         block 1

	 and elsewhere in the program:
     (2) DO RESUME <1-or-2-condition>

     Recognizing this idiom is quite difficult because there are a number of
     problems and common variations.
     First, the FORGET might be placed elsewhere, or replaced with a higher
     FORGET later or a higher RESUME later. If there is, in fact, a RESUME #1
     as the ick_next NEXT-control statement, the idiom won't quite work properly.
     So to handle this, we need to push the original return address on the
     NEXT stack if block 1 is taken, unless the ick_next statement is a FORGET #1.
     Second, there may be abstentions or COME FROMs messing with control flow
     in the area. The flow optimizer ought to be able to detect this and not
     optimize the statement if true. (This means that a program which uses
     gerund abstention on NEXTING or RESUMING, or that uses computed COME FROM,
     will probably not benefit from this optimization). Third, how should a
     <1-or-2-condition> be detected? Throughout syslib, the most common
     condition to use is .5, which isn't inherently 1 or 2. The way round this
     seems to be to detect a <1-or-2-assignment> as the previous statement,
     again with checks for COME FROM and abstentions. (I treat MAYBE as a sort
     of abstention, albeit a temporally undecided one.) So ignorance of the
     relevant variable needs to be checked for also. The checks are done partly
     in optimizef() and partly in emit().
  */
  if(compucomesused||ick_Base!=2||opoverused) return;

  np = (node*) NULL;
  for (tp = tuples; tp < tuples + ick_lineno; tp++)
  {
    if(tp->type == GETS)
    {
/*      if(tp->u.node->rval->opcode == C_AND1ADD1 ||
	 tp->u.node->rval->opcode == C_2SUBAND1 ||
	 ((tp->u.node->rval->opcode == C_1PLUS ||
	   tp->u.node->rval->opcode == C_2MINUS) &&
	   tp->u.node->rval->rval->optdata == 1)) //debug for now */ if(0)
      {
	/* This won't catch all <1-or-2-expressions>, but will get
	   most of them. */
	if(tp->u.node->lval->opcode != SUB) np = tp->u.node->lval;
      }
      else if(np != NULL && nodessame(np, tp->u.node->lval)) np = (node*) NULL;
      if(tp->nextable) np = (node*) NULL;
      if(tp->maybe||tp->abstainable) np = (node*) NULL;
      if(tp->ncomefrom&&multithread) np = (node*) NULL;
      if(np)
      {	/* IGNORING np might prevent it getting its <1-or-2-value>. */
	atom* op2;
	int ignorable = 1;
	for(op2 = oblist; op2 != NULL && op2 < obdex; op2++)
	{
	  if(op2->type == np->opcode &&
	     (unsigned long)op2->intindex == np->constant)
	  {
	    ignorable &= op2->ignorable;
	  }
	}
	if(ignorable) np = (node*) NULL;
      }
      /* np will only have a nonnull value if it's a variable that must be
	 set to a <1-or-2-value> to reach line tp. Regardless of whether
         maybes have been parsed or not, either maybe or abstainable or
         both will be 1 on a MAYBE line. The last check is a precaution
         against MAYBE COME FROM sneakily modifying a variable (although
         I think it's unlikely that anyone would deliberately try to fool
         -f like this, it is INTERCAL after all!) */
    }
    if(tp->type == COME_FROM || tp->type == COMPUCOME) np = (node*) NULL;
    if(tp->type == NEXT)
    {
      tpa = tuples + tp->nexttarget - 1;
      if(tpa->type == NEXT && !tp->abstainable && !tpa->abstainable
	 && !tp->ncomefrom && !tpa->ncomefrom)
      {
	tpb = tuples + tpa->nexttarget - 1;
	if(tpb->type == RESUME && (/*(tpb->u.node->opcode == C_AND1ADD1 ||
				    tpb->u.node->opcode == C_2SUBAND1 ||
				    ((tpb->u.node->opcode == C_1PLUS ||
				      tpb->u.node->opcode == C_2MINUS) &&
				      tpb->u.node->rval->optdata == 1)) ||*/
				   (np != NULL && nodessame(tpb->u.node,np))) &&
				    !tpb->abstainable)
	  /* No COMING FROM a nonabstainable RESUME line! */
	{
	  tp->optversion = true;
	  free(tp->u.node);
	  tp->u.node = nodecopy(tpb->u.node);
	  /* If tp->u.node is 2, then the statement should translate to a
	     no-op (NEXT...NEXT...RESUME #2). However, if tp->u.node is 1,
	     the statement should translate to a NEXT to the line after the
	     one it's aiming for. As it's aiming for a NEXT, the solution is
	     to NEXT to the return label of the NEXT it's aiming for. This
	     won't trigger any COME FROMs or ONCE/AGAIN flags, or even MAYBEs,
	     on the commands missed out, so the code has checked that they
	     aren't there. */
	}
      }
      np = (node*) NULL;
    }
  }
  /*@-nullstate@*/ /* no tuples->u.node can't be null */
}
/*@=nullstate@*/

/*************************************************************************
 *
 * Expression optimizer.
 *
 * It's not a very good optimizer, is it?
 *
 * AIS: All free'd pointers set to NULL, to help trace memory problems.
 *      The optimizer can now recognize the majority of syslib's idioms.
 *      The optimizer will make multiple passes. Although I am now happy
 *      with the quality of this optimizer, I'll keep the paragraph above
 *      this one for history's sake. The optimizing functions return 1
 *      if any optimizations were made, apart from optimize itself, which
 *      is void.
 *
 **************************************************************************/

extern int optimize_pass1(node *np); /* Read from idiotism.c */
static void checkforintercaloperators(const node *np);
static void checkW534(const node *np);

node* optdebugnode;

/* This function by AIS */
void optimize(node *np)
{
  int optflag;
  if(opoverused) return; /* what chance do we have of optimizing this? */
  if(ick_Base==2)
  {
    checknodeactbits(np);
    checkW534(np); /* This must be done before optimization, and depends on
		      checknodeactbits. */
  }
  if(optdebug == 1) explexpr(np,stderr);
  if(optdebug == 1) fprintf(stderr," becomes ");
  if(optdebug >= 2) optdebugnode = np;
  if(ick_Base==2) (void) optimize_pass1(np); /* Optimize idioms; from idiotism.oil */
  if(ick_Base!=2)
  {
    if(optdebug && optdebug != 3) explexpr(np,stderr);
    if(optdebug == 3) prexpr(np,stderr,0);
    if(optdebug) fprintf(stderr,"\n");
    if(optdebug >= 2) fprintf(stderr,"-----\n");
    return;
  }
  do
  {
    optflag = optimize_pass1(np);
  } while(optflag); /* Keep optimizing until no optimizations are found */
  if(optdebug && optdebug != 3) explexpr(np,stderr);
  if(optdebug == 3) prexpr(np,stderr,0);
  if(optdebug) fprintf(stderr,"\n");
  if(optdebug >= 2) fprintf(stderr,"-----\n");
  checkforintercaloperators(np);
  if(optuple->type == RESUME && !np->optdata) optuple->warn622 = true;
}

/* By AIS. This relies on free'd pointers being NULLed. The annotations
   are basically trying to describe how the function operates. */
void nodefree(/*@keep@*/ /*@null@*/ node *np)
{
  if(!np) return;
  /*@-mustfreeonly@*/
  if(np->nextslat) return; /* don't free, has oo data */
  if(np==prevslat) return; /* likewise */
  /*@=mustfreeonly@*/
  /*@-keeptrans@*/
  nodefree(np->lval);
  nodefree(np->rval);
  free(np);
  /*@=keeptrans@*/
}

/* By AIS. This checks W534. */
static void checkW534(const node *np)
{
  if(!np) return;
  if(np->opcode == AND || np->opcode == OR || np->opcode == XOR)
  {
    if(np->rval->opcode == SELECT && np->rval->rval->width == 32 &&
       !(np->rval->rval->optdata&0xffff0000lu))
    {
      /* This looks suspicious, in that C-INTERCAL will do an op32,
	 but INTERCAL-72 would have done an op16. */
      optuple->warn534=true;
    }
  }
  checkW534(np->lval);
  checkW534(np->rval);
}

/* By AIS. This checks W018. */
static void checkforintercaloperators(const node *np)
{
  if(!np) return;
  switch(np->opcode)
  { /* This only comes up in binary. */
  case AND: case OR: case XOR: case MINGLE: case SELECT:
    optuple->warn018 = true;
    break;
  default:
    checkforintercaloperators(np->lval);
    checkforintercaloperators(np->rval);
    return;
  }
}

/* By AIS (with a few code fragments copied from elsewhere in this file)
   This generates the values of c used by the OIL idiom file idiotism.oil. */
void checknodeactbits(node *np)
{
  int temp;
  if (np == (node *)NULL)
    return;
  else if (np->lval != (node *)NULL)
    checknodeactbits(np->lval);
  if (np->rval != (node *)NULL)
    checknodeactbits(np->rval);

  switch (np->opcode)
  {
  case MINGLE:
    /*@-nullderef@*/ /* mingle has two nonnull arguments */
    if(np->lval->optdata & 0xffff0000LU) optuple->warn276 = true;
    if(np->rval->optdata & 0xffff0000LU) optuple->warn276 = true;
    np->optdata = ick_mingle((unsigned)(np->lval->optdata & 0xffff),
			     (unsigned)(np->rval->optdata & 0xffff));
    /*@=nullderef@*/
    /* The bitmask is needed because the output of ~ might always be 16-bit
       at runtime, but appear 32-bit at compile-time. But this is somewhat
       suspicious, so we can at least give a warning (W276) if -l is used. */
    break;

  case SELECT:
    /* The result could be the selected optdata, or have a 1 anywhere to the
       right of a 1 in the resulting selection if there are 0s in rval where
       there could have been 1s */
    /*@-nullderef@*/
    np->optdata = ick_iselect((unsigned)np->lval->optdata, (unsigned)np->rval->optdata);
    /*@=nullderef@*/
    temp=32;
    while(temp--) np->optdata|=(np->optdata>>1); /* fill in gaps in optdata */
    break;

  case AND:
    if(np->width==16)
      np->optdata = ick_and16((unsigned)np->rval->optdata);
    else
      np->optdata = ick_and32((unsigned)np->rval->optdata);
    break;

  case OR:
  case XOR:
    if(np->width==16)
      np->optdata = ick_or16((unsigned)np->rval->optdata);
    else
      np->optdata = ick_or32((unsigned)np->rval->optdata);
    /* This is or in both cases. */
    break;

  case FIN:
  case WHIRL:
  case WHIRL2:
  case WHIRL3:
  case WHIRL4:
  case WHIRL5: /* We must be in binary to reach this point, so: */
    ick_lose(IE997, emitlineno, (const char*) NULL);
    /*@-unreachable@*/
    break;
    /*@=unreachable@*/

  case MESH:
  case MESH32:
    np->optdata = np->constant; /* It's trivial to tell which bits
				   can be nonzero! */
    break;

  case ick_ONESPOT:
  case ick_TWOSPOT:
  case ick_TAIL:
  case ick_HYBRID:
  case SUB:
    np->optdata = np->width == 16 ? 0xffffLU : 0xffffffffLU;
    break;

    /* cases from here down are generated by optimize_pass1 */
  case C_AND:
    /*@-nullderef@*/
    np->optdata = np->lval->optdata & np->rval->optdata;
    /*@=nullderef@*/
    break;

  case C_OR:
  case C_XOR:
    /*@-nullderef@*/
    np->optdata = np->lval->optdata | np->rval->optdata;
    /*@=nullderef@*/
    /* bitwise-or is correct in both cases */
    break;

  case C_NOT:
    np->optdata = np->width == 16 ? 0xffffLU : 0xffffffffLU;
    break;

  case C_A:
    np->optdata = 0xffffffffLU;
    break;

  case C_NOTEQUAL:
  case C_ISEQUAL:
  case C_LOGICALNOT:
  case C_LOGICALAND:
  case C_LOGICALOR:
  case C_GREATER:
  case C_LESS:
    np->optdata = 1; /* this is a logical function */
    break;

  case C_RSHIFTBY:
    /*@-nullderef@*/
    if(np->rval->opcode == MESH || np->rval->opcode == MESH32)
      np->optdata = np->lval->optdata >> np->rval->constant;
    else np->optdata = (np->width == 16 ? 0xffffLU : 0xffffffffLU);
    /*@=nullderef@*/
    /* Play safe if the RHS isn't a constant */
    break;

  case C_LSHIFTBY:
    /*@-nullderef@*/
    if(np->rval->opcode == MESH || np->rval->opcode == MESH32)
      np->optdata = np->lval->optdata << np->rval->constant;
    else np->optdata = (np->width == 16 ? 0xffffLU : 0xffffffffLU);
    /*@=nullderef@*/
    /* Play safe if the RHS isn't a constant */
    break;

  case C_PLUS:
    /* A bit could be set if it's set in either of the numbers, or if it's set
       by a carry; so OR together the two numbers and their sum. */
    /*@-nullderef@*/
    np->optdata = np->lval->optdata | np->rval->optdata |
      (np->lval->optdata + np->rval->optdata);
    /*@=nullderef@*/
    break;

  case C_MINUS:
  case C_DIVIDEBY:
    /* The optimizer shouldn't be able to generate negative answers or
       divisions by 0, so just fill in all bits from lval rightwards */
    /*@-nullderef@*/
    np->optdata = np->lval->optdata;
    np->optdata |= np->optdata >> 1;
    np->optdata |= np->optdata >> 2;
    np->optdata |= np->optdata >> 4;
    np->optdata |= np->optdata >> 8;
    np->optdata |= np->optdata >> 16;
    /*@=nullderef@*/
    break;

  case C_MODULUS:
    /* The answer must be smaller than both inputs, but we can't tell anything
       else */
    /*@-nullderef@*/
    np->optdata = np->lval->optdata;
    if(np->rval->optdata < np->optdata) np->optdata = np->rval->optdata;
    np->optdata |= np->optdata >> 1;
    np->optdata |= np->optdata >> 2;
    np->optdata |= np->optdata >> 4;
    np->optdata |= np->optdata >> 8;
    np->optdata |= np->optdata >> 16;
    /*@=nullderef@*/
    break;

  case C_TIMES:
    /* Convolve one set of active bits with the other, ORadding the results */
    np->optdata=0;
    temp=32;
    /*@-nullderef@*/ /*@-shiftnegative@*/
    while(temp--)
      if(np->lval->optdata & (1LU << temp))
	np->optdata = np->optdata | (np->rval->optdata << temp) |
	  ((np->rval->optdata << temp) + np->optdata);
    /*@=nullderef@*/ /*@=shiftnegative@*/
    break;

  case GETS:
    /* Of course, this doesn't return a value. So this uses the default
       code just below it, which is why it doesn't end in break;. This
       has its own case so that W276 can be given on an assignment. */
    /*@-nullderef@*/
    if(np->lval->optdata == 0xffff &&
       np->rval->optdata & 0xffff0000lu) optuple->warn276 = true;
    /*@=nullderef@*/
    /*@fallthrough@*/

  case UNKNOWNOP:
  default:
    /*@-nullderef@*/
    if(np->opcode == BY && !np->lval->optdata) optuple->warn239 = true;
    /*@=nullderef@*/
    np->optdata = (np->width == 16 ? 0xffffLU : 0xffffffffLU);
    /* Some values of opcode are used as placeholders, to save more than 1
       piece of information in a node. The optdata for these is probably
       irrelevant, but just in case, we mark all possible bits as active. */
    break;
  }
}
