/****************************************************************************

Name
   feh.c -- code-generator back-end for ick parser

DESCRIPTION
   This module provides storage manglement, code degeneration,
and optimizations of dubious value for the INTERCAL compiler.

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sizes.h"
#include "ick.h"
#include "y.tab.h"
#include "fiddle.h"
#include "lose.h"
#include "feh.h"

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

static int emitlineno;   /* line number for errors encountered during emit */

static void checknodeactbits(node *np); /* AIS: This prototype needed early */

int mark112 = 0; /* AIS: Mark the next generated tuple for W112 */

/*************************************************************************
 *
 * Node allocation functions.
 *
 * Nodes are used to represent expression trees. The emit() function
 * deallocates them.
 *
 **************************************************************************/

node *newnode(void)
/* allocate and zero out a new expression node */
{
    return((node *)calloc(sizeof(node), 1));
}

node *cons(int type, node *car, node *cdr)
{
    node *np = newnode();

    np->opcode = type;
    np->lval = car;
    np->rval = cdr;

    return(np);
}

/* By AIS. This function looks remarkably like a C++ copy-constructor to me,
   just with extra arrows in. */
node *nodecopy(node* n)
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
static bool nodessame(node* n1, node* n2)
{
  if(!n1) return !n2;
  if(!n2) return 0;
  if(n1->opcode!=n2->opcode) return 0;
  if(!nodessame(n1->lval,n2->lval)) return 0;
  if(!nodessame(n1->rval,n2->rval)) return 0;
  switch(n1->opcode)
  {
  case ONESPOT:
  case TWOSPOT:
  case HYBRID:
  case TAIL:
  case MESH:
  case MESH32:
    return n1->constant == n2->constant;
  case AND:
  case OR:
  case XOR:
    return n1->width == n2->width;
  default:
    return 1;
  }
}

/*************************************************************************
 *
 * Variable-name mapping
 *
 * This permits us to optimize use of variable storage at runtime
 *
 **************************************************************************/

unsigned int intern(int type, int index)
{
    atom	*x;

    if (index < 1 || index > 65535)
	lose(E200, yylineno, (char *)NULL);

    if (!oblist)
    {
	/* initialize oblist and obdex */
	oblist = malloc(ALLOC_CHUNK * sizeof(atom));
	if (!oblist)
	    lose(E778, yylineno, (char *)NULL);
	obdex = oblist;
	obcount = ALLOC_CHUNK;
    }

    /* if it's already on the oblist, return its intindex */
    for (x = oblist; x < obdex; x++)
	if (x->type == type && x->extindex == index)
	    return(x->intindex);

    /* else we must intern a new symbol */
    if (obdex >= oblist + obcount)
    {
	obcount += ALLOC_CHUNK;
	x = realloc(oblist, obcount * sizeof(atom));
	if (!x)
	    lose(E333, yylineno, (char *)NULL);
	obdex = x + (obdex - oblist);
	oblist = x;
    }
    obdex->type = type;
    obdex->extindex = index;
    if (type == ONESPOT)
	obdex->intindex = nonespots++;
    if (type == TWOSPOT)
	obdex->intindex = ntwospots++;
    if (type == TAIL)
	obdex->intindex = ntails++;
    if (type == HYBRID)
	obdex->intindex = nhybrids++;
    ++obdex;

    return(obdex[-1].intindex);
}

/*************************************************************************
 *
 * This function insures a label is valid.
 *
 **************************************************************************/

void checklabel(int label)
{
    if (label < 1 || label > 65535)
	lose(E197, yylineno, (char *)NULL);
}

/*************************************************************************
 *
 * AIS: Search for the indexth COME_FROM sucking in the given tuple.
 *      Return an int representing the COME_FROM's tn-tuples+1, or -1.
 *      index is based at 1, not 0 as is usual for C.
 *
 ***************************************************************************/

int comefromsearch(tuple* tn, unsigned int index)
{
  tuple* tp;
  for (tp = tuples; tp < tuples + lineno; tp++)
  {
    if(tp->type == COME_FROM && tp->u.target == tn-tuples+1) index--;
    if(!index) return tp-tuples+1;
  }
  return -1;
}

/*************************************************************************
 *
 * Tuple allocation functions.
 *
 **************************************************************************/

void treset(void)
{
    tuplecount = 0;
    if (tuples)
    {
	free(tuples);
	tuples = NULL;
    }
    nonespots = ntwospots = ntails = nhybrids = 0;
    obdex = oblist;
    lineno = 0;
}

tuple *newtuple(void)
/* allocate and zero out a new expression tuple */
{
    if (lineno >= tuplecount)
    {
	tuplecount += ALLOC_CHUNK;
	if (tuples)
	    tuples = realloc(tuples, tuplecount * sizeof(tuple));
	else
	    tuples = malloc(tuplecount * sizeof(tuple));
	if (!tuples)
	{
	    lose(E666, yylineno, (char *)NULL);
	    return NULL;
	}
	memset(tuples + lineno, 0, (tuplecount - lineno) * sizeof(tuple));
    }
    if(mark112) tuples[lineno].warn112 = 1; mark112 = 0; /* AIS */
    return(tuples + lineno++);
}

/*************************************************************************
 *
 * The typecaster
 *
 * The theory here is that we associate a type with each node in order to
 * know what widths of unary-logical operator to use.
 *
 **************************************************************************/

void typecast(node *np)
{
    /* recurse so we typecast each node after all its subnodes */
    if (np == (node *)NULL)
	return;
    else if (np->lval != (node *)NULL)
	typecast(np->lval);
    if (np->rval != (node *)NULL)
	typecast(np->rval);

    /*
     * This is an entire set of type-deducing machinery right here.
     */
    if (np->opcode == MESH || np->opcode == ONESPOT || np->opcode == TAIL)
	np->width = 16;
    else if (np->opcode == TWOSPOT || np->opcode == HYBRID
		|| np->opcode == MINGLE || np->opcode == MESH32)
	np->width = 32;
    else if (np->opcode == AND || np->opcode == OR || np->opcode == XOR ||
	     np->opcode == FIN ||
	     (np->opcode >= WHIRL && np->opcode <= WHIRL5))
	np->width = np->rval->width;
    else if (np->opcode == SELECT)
	np->width = np->rval->width;	/* n-bit select has an n-bit result */
    else if (np->opcode == SUB)
	np->width = np->lval->width;	/* type of the array */
}

/*************************************************************************
 *
 * The codechecker
 *
 * This checks for nasties like mismatched types in assignments that
 * can be detected at compile time -- also for errors that could cause
 * the compilation of the generated C to fail, like generated gotos to
 * nonexistent labels or duplicate labels.
 *
 * AIS: codecheck has another important job, that of filling in information
 *      about COME FROM suckpoints and ABSTAIN/REINSTATE command numbers
 *      into the tuples.
 *
 **************************************************************************/

void codecheck(void)
{
    tuple	*tp, *up;
    int         notpast1900; /* AIS */

    /* check for assignment type mismatches */
    /* This check can't be done at compile time---RTFM.  [LHH] */
/*
    for (tp = tuples; tp < tuples + lineno; tp++)
	if (tp->type == GETS)
	    if (tp->u.node->lval->width == 16 && tp->u.node->rval->width == 32)
		lose(E275, tp - tuples + 1, (char *)NULL);
*/

    /* check for duplicate labels */
    for (tp = tuples; tp < tuples + lineno; tp++)
	if (tp->label)
	    for (up = tuples; up < tuples + lineno; up++)
		if (tp != up && tp->label == up->label)
		    lose(E182, tp - tuples + 1, (char *)NULL);

    /*
     * Check that every NEXT, ABSTAIN, REINSTATE and COME_FROM actually has a
     * legitimate target label.
     */
    notpast1900 = TRUE;
    for (tp = tuples; tp < tuples + lineno; tp++)
    {
      if (tp->label == 1900) notpast1900 = FALSE; /* AIS */
        if (tp->type == NEXT
	    || tp->type == ABSTAIN || tp->type == REINSTATE
	    || tp->type == COME_FROM || tp->type == FROM) /* AIS: added FROM. */
	{
	    bool	foundit = FALSE;

	    if (tp->u.target >= 1900 && tp->u.target <= 1998)
	    {
	      /* AIS: This program uses syslib.i's random number feature... or are
	         we in syslib already? */
	      if(notpast1900) coopt = 0;
	    }
	    
	    for (up = tuples; up < tuples + lineno; up++)
		if (tp->u.target == up->label)
		{
		    foundit = TRUE;
		    break;
		}

	    if (!foundit)
	    {
		if (tp->type == NEXT)
		    lose(E129, tp - tuples + 1, (char *)NULL);
		else if (tp->type == COME_FROM)
		    lose(E444, tp - tuples + 1, (char *)NULL);
		else
		    lose(E139, tp - tuples + 1, (char *)NULL);
	    }
	    /* tell the other tuple if it is a COME FROM target */
	    else if (tp->type == COME_FROM)
	    {
	        if (up->ncomefrom && !multithread) /* AIS: multithread check */
		    lose(E555, yylineno, (char *)NULL);
		else
                    up->ncomefrom++; /* AIS: to handle multiple COME FROMs */
	    }
	    /* this substitutes line numbers for label numbers
	       AIS: COME FROM now uses this too. This changes the logic
	       slightly so that an !foundit condition would fall through,
	       but as long as lose doesn't return, it's not a problem.
	       (I removed the else before the if.) */
	    if (tp->type != NEXT)
	    {
		tp->u.target = up - tuples + 1;
	    }
	    else /* AIS */
	    {
	      tp->nexttarget = up - tuples + 1;
	      up->nextable = TRUE;
	    }
	}
    }
}

/* AIS: Checks if an abstention could affect a tuple. */
int abstainmatch(int npconstant, int tptype)
{
  if(npconstant == tptype) return 1;
  if(npconstant == ABSTAIN)
    if(tptype == FROM || tptype == MANYFROM || tptype == DISABLE) return 1;
  if(npconstant == REINSTATE)
    if(tptype == DISABLE) return 1;
  return 0;
}

/* AIS: Added the third argument to prexpr and prvar. It specifies
   whether the node should be freed or not. I added the third
   argument in all calls of prexpr/prvar. This protoype has been moved
   up through the file so it can be used earlier. */
static void prexpr(node *np, FILE *fp, int freenode);

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
 * At the moment, it just checks for unused !abstained[] guards on
 * statements and removes them (possibly removing comments altogether).
 * This is minor, but should clear up degenerated code substantially.
 * It also allows the code for gets to replace assign() with the faster =
 * in cases where this doesn't affect the behaviour of the program.
 * See the function itself for what I did to NEXT.
 *
 *************************************************************************/

void optimizef(void)
{
  tuple* tp, *tpa, *tpb;
  atom* op;
  node* np;
  if(!flowoptimize) lose(E778, yylineno, (char *) NULL);
  for (tp = tuples; tp < tuples + lineno; tp++) tp->abstainable = 0;
  /* abstainable holds whether a line's abstention status can change */
  for (op = oblist; op < obdex; op++) op->ignorable = 0;
  /* ignorable holds whether a variable's ignorance status can change */
  for (tp = tuples; tp < tuples + lineno; tp++)
  {
    if(tp->maybe) tp->abstainable = 1;
    if(tp->exechance < 0) tp->initabstain = 1;
    if(tp->exechance != 100 && tp->exechance != -100) tp->abstainable = 1;
    if(tp->onceagainflag != onceagain_NORMAL) tp->abstainable = 1;
    if(tp->type == ABSTAIN || tp->type == FROM)
    {
      tpa = tp->u.target - 1 + tuples;
      if(tpa->exechance >= 0) tpa->abstainable = 1;
    }
    if(tp->type == REINSTATE)
    {
      tpa = tp->u.target - 1 + tuples;
      if(tpa->exechance < 0) tpa->abstainable = 1;
    }
    if(tp->type == DISABLE || tp->type == MANYFROM)
    {
      for (tpa = tuples; tpa < tuples + lineno; tpa++)
      {
	np = tp->u.node;
	if(tp->type == MANYFROM) np = np->rval;
	for(; np; np = np -> rval)
	  if(abstainmatch(np->constant, tpa->type))
	    if(tpa->exechance >= 0) tpa->abstainable = 1;
      }
    }
    if(tp->type == ENABLE)
    {
      for (tpa = tuples; tpa < tuples + lineno; tpa++)
      {
	np = tp->u.node;
	for(; np; np = np -> rval)
	  if(abstainmatch(np->constant, tpa->type))
	    if(tpa->exechance < 0) tpa->abstainable = 1;
      }
    }
    if(tp->type == GETS)
      checknodeactbits(tp->u.node->rval);
    /* If optdata shows that the value must always fit in the variable,
       and the variable cannot be ignored, assign can be replaced by the
       cheaper =. */
    if(tp->type == IGNORE)
    {
      for (np = tp->u.node; np; np = np->rval)
      {
	for (op = oblist; op < obdex; op++)
	{
	  if(op->type == np->opcode && op->intindex == np->constant)
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
     as the next NEXT-control statement, the idiom won't quite work properly.
     So to handle this, we need to push the original return address on the
     NEXT stack if block 1 is taken, unless the next statement is a FORGET #1.
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
  if(compucomesused) return; /* That's one check out of the way! */

  np = (node*) NULL;
  for (tp = tuples; tp < tuples + lineno; tp++)
  {
    if(tp->type == GETS)
    {
      if(tp->u.node->rval->opcode == C_AND1ADD1 ||
	 tp->u.node->rval->opcode == C_2SUBAND1 ||
	 (tp->u.node->rval->opcode == C_1PLUS ||
	  tp->u.node->rval->opcode == C_2MINUS) &&
	 tp->u.node->rval->rval->optdata == 1)
      {
	/* This won't catch all <1-or-2-expressions>, but will get
	   most of them. */
	if(tp->u.node->lval->opcode != SUB) np = tp->u.node->lval;
      }
      else if(np&&nodessame(np, tp->u.node->lval)) np = (node*) NULL;
      if(tp->nextable) np = (node*) NULL;
      if(tp->maybe||tp->abstainable) np = (node*) NULL;
      if(tp->ncomefrom&&multithread) np = (node*) NULL;
      if(np)
      {	/* IGNORING np might prevent it getting its <1-or-2-value>. */
	atom* op;
	int ignorable = 1;
	for(op = oblist; op < obdex; op++)
	{
	  if(op->type == np->opcode &&
	     op->intindex == np->constant)
	  {
	    ignorable &= op->ignorable;
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
	if(tpb->type == RESUME && ((tpb->u.node->opcode == C_AND1ADD1 ||
				    tpb->u.node->opcode == C_2SUBAND1 ||
				    (tpb->u.node->opcode == C_1PLUS ||
				     tpb->u.node->opcode == C_2MINUS) &&
				    tpb->u.node->rval->optdata == 1) ||
				   np&&nodessame(tpb->u.node,np))&&
	   !tpb->abstainable) /* No COMING FROM a nonabstainable RESUME line! */
	{
	  tp->optversion = TRUE;
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
}

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

static int optimize_pass1(node *np);
static int optimize_pass2(node *np);
static void checkforintercaloperators(node *np);
static void checkW534(node *np);

static node* optdebugnode;

/* This function by AIS */
void optimize(node *np)
{
  int optflag;
  checknodeactbits(np);
  checkW534(np); /* This must be done before optimization, and depends on
		    checknodeactbits. */
  if(optdebug == 1) explexpr(np,stderr);
  if(optdebug == 1) fprintf(stderr," becomes ");
  if(optdebug >= 2) optdebugnode = np;
  optimize_pass1(np); /* Optimize idioms */
  if(Base!=2)
  {
    if(optdebug && optdebug != 3) explexpr(np,stderr);
    if(optdebug == 3) prexpr(np,stderr,0);
    if(optdebug) fprintf(stderr,"\n");
    if(optdebug >= 2) fprintf(stderr,"-----\n");
    return;
  }
  do
  {
    checknodeactbits(np); /* Determine bitranges */
    optflag  = optimize_pass2(np); /* Optimize based on those bitranges */
    optflag |= optimize_pass1(np); 
  } while(optflag); /* Keep optimizing until no optimizations are found */
  if(optdebug && optdebug != 3) explexpr(np,stderr);
  if(optdebug == 3) prexpr(np,stderr,0);
  if(optdebug) fprintf(stderr,"\n");
  if(optdebug >= 2) fprintf(stderr,"-----\n");
  checkforintercaloperators(np);
  if(optuple->type == RESUME && !np->optdata) optuple->warn622 = 1;
}

/* By AIS. This relies on free'd pointers being NULLed. */
static void nodefree(node *np)
{
  if(!np) return;
  nodefree(np->lval);
  nodefree(np->rval);
  free(np);
}

/* By AIS. This checks W534. */
static void checkW534(node *np)
{
  if(!np) return;
  if(np->opcode == AND || np->opcode == OR || np->opcode == XOR)
  {
    if(np->rval->opcode == SELECT && np->rval->rval->width == 32 &&
       !(np->rval->rval->optdata&0xffff0000lu))
    {
      /* This looks suspicious, in that C-INTERCAL will do an op32,
	 but INTERCAL-72 would have done an op16. */
      optuple->warn534=1;
    }
  }
  checkW534(np->lval);
  checkW534(np->rval);
}

/* By AIS. This checks W018. */
static void checkforintercaloperators(node *np)
{
  if(!np) return;
  switch(np->opcode)
  { /* This only comes up in binary. */
  case AND: case OR: case XOR: case MINGLE: case SELECT:
    optuple->warn018 = 1;
  default:
    checkforintercaloperators(np->lval);
    checkforintercaloperators(np->rval);
    return;
  }
}

/* By AIS (with a few code fragments copied from elsewhere in this file) */
static void checknodeactbits(node *np)
{
  if (np == (node *)NULL)
    return;
  else if (np->lval != (node *)NULL)
    checknodeactbits(np->lval);
  if (np->rval != (node *)NULL)
    checknodeactbits(np->rval);

  switch (np->opcode)
  {
  case MINGLE:
    if(np->lval->optdata & 0xffff0000LU) optuple->warn276 = 1;
    if(np->rval->optdata & 0xffff0000LU) optuple->warn276 = 1;
    np->optdata = mingle(np->lval->optdata & 0xffff,
			 np->rval->optdata & 0xffff);
    /* The bitmask is needed because the output of ~ might always be 16-bit
       at runtime, but appear 32-bit at compile-time. But this is somewhat
       suspicious, so we can at least give a warning (W276) if -l is used. */
    break;

  case SELECT:
    np->optdata = np->lval->optdata & np->rval->optdata;
    np->optdata = iselect(np->optdata, np->optdata);
    break;

  case AND:
    np->optdata = (np->width==16?and16:and32)(np->rval->optdata);
    break;

  case OR:
  case XOR:
    np->optdata = (np->width==16?or16:or32)(np->rval->optdata);
    /* This is or in both cases. */
    break;

  case FIN:
  case WHIRL:
  case WHIRL2:
  case WHIRL3:
  case WHIRL4:
  case WHIRL5: /* We must be in binary to reach this point, so: */
    lose(E997, emitlineno, (char*) NULL);
    break;

  case MESH:
  case MESH32:
    np->optdata = np->constant; /* It's trivial to tell which bits
				   can be nonzero! */
    break;

  case ONESPOT:
  case TWOSPOT:
  case TAIL:
  case HYBRID:
  case SUB:
    np->optdata = np->width == 16 ? 0xffff : 0xffffffffLU;
    break;

    /* cases from here down are generated by optimize_pass1 */
  case C_AND:
    np->optdata = np->lval->optdata & np->rval->optdata;
    break;

  case C_OR:
  case C_XOR:
    np->optdata = np->lval->optdata | np->rval->optdata;
    /* bitwise-or is correct in both cases */
    break;

  case C_NOT:
    np->optdata = np->width == 16 ? 0xffff : 0xffffffffLU;
    break;	

  case C_NOTEQUAL:
  case C_NNAND:
  case C_ISNONZERO:
  case C_XORGREATER:
    np->optdata = 1; /* that's what !! does! */
    break;

  case C_AND1ADD1:
  case C_2SUBAND1:
    np->optdata = 3; /* the value could be 1 or 2 */
    break;

  case C_1PLUS:
    np->optdata = np->rval->optdata | (np->rval->optdata << 1);
    /* this relies on unsigned overflow handling to work correctly */
    break;

  case C_2MINUS:
    if(np->rval->optdata == 1) np->optdata = 3;
    else np->optdata = (np->width == 16 ? 0xffff : 0xfffffffflu);
    /* alright, so this might not be ruthless enough, but C_2MINUS is
       only meant to be generated when the condition is true anyway */
    break;

  case C_LSHIFT:
    np->optdata = (np->lval->optdata & np->rval->optdata) << 1;
    break;

  case C_LSHIFTIN1:
    np->optdata = ((np->lval->optdata & np->rval->optdata) << 1) | 1;
    break;

  case C_LSHIFT2:
    np->optdata = (np->lval->optdata & np->rval->optdata) << 2;
    break;

  case C_LSHIFT8:
    np->optdata = (np->lval->optdata & np->rval->optdata) << 8;
    break;

  case C_RSHIFTBY:
    if(np->rval->opcode == MESH || np->rval->opcode == MESH32)
      np->optdata = np->lval->optdata >> np->rval->constant;
    else np->optdata = (np->width == 16 ? 0xffff : 0xffffffffLU);
    /* Just like C_2MINUS, play safe if the RHS isn't a constant */
    break;

  case C_RSHIFT:
    np->optdata = (np->lval->optdata & np->rval->optdata) >> 1;
    break;

  case GETS:
    /* Of course, this doesn't return a value. So this uses the default
       code just below it, which is why it doesn't end in break;. This
       has its own case so that W276 can be given on an assignment. */
    if(np->lval->optdata == 0xffff &&
       np->rval->optdata & 0xffff0000lu) optuple->warn276 = 1;
    /* no break;! */

  default:
    if(np->opcode == BY && !np->lval->optdata) optuple->warn239 = 1;
    np->optdata = (np->width == 16 ? 0xffff : 0xffffffffLU);
    /* Some values of opcode are used as placeholders, to save more than 1
       piece of information in a node. The optdata for these is probably
       irrelevant, but just in case, we mark all possible bits as active. */
    break;
  }
}

/* This function by AIS. Pass 2 removes redundant logical operations,
   mostly explicit and implied C_AND. It destroys information needed by
   Pass 1, which is why it's in a separate function. */
static int optimize_pass2(node *np)
{
  node *op;
  int opted = 0;
  /* AIS: Paraphrased from code in the old 'optimize' function */
  /* recurse so we simplify each node after all its subnodes */
  if (np == (node *)NULL)
    return 0;
  else if (np->lval != (node *)NULL)
    opted |= optimize_pass2(np->lval);
  if (np->rval != (node *)NULL)
    opted |= optimize_pass2(np->rval);
  /* End of paraphrase */
  
  /* Put constants of AND-like opcodes on the right */
  if((np->opcode == C_AND || np->opcode == C_NNAND ||
      np->opcode == C_LSHIFT || np->opcode == C_LSHIFTIN1 ||
      np->opcode == C_LSHIFT2 || np->opcode == C_LSHIFT8 ||
      np->opcode == C_RSHIFT) &&
     (np->lval->opcode == MESH || np->lval->opcode == MESH32))
  {
    OPTING(andlikeconstmove)
    op = np->lval;
    np->lval = np->rval;
    np->rval = op;
  }

  /* The implicit &1 in C_AND1ADD1 and C_2SUBAND1 may be removable. */
  if(np->opcode == C_AND1ADD1 && np->rval->optdata == 1)
  {
    OPTING(impand1a1)
    np->opcode = C_1PLUS;
  }
  if(np->opcode == C_2SUBAND1 && np->rval->optdata == 1)
  {
    OPTING(impand12m)
    np->opcode = C_2MINUS;
  }

  if(np->opcode == C_AND && (np->rval->opcode == MESH ||
			     np->rval->opcode == MESH32) &&
     !(np->lval->optdata & ~np->rval->constant))
  {
    OPTING(unneededand)
    /* This AND is unnecessary. optdata stays the same. */
    op = np->lval;
    free(np->rval);
    *np = *op;
    free(op);
  }

  if((np->opcode == C_NNAND || np->opcode == C_RSHIFT) &&
     (np->rval->opcode == MESH || np->rval->opcode == MESH32) &&
     !(np->lval->optdata & ~np->rval->constant))
  {
    /* The implicit AND here is unneccesary. Eliminate it. */
    OPTING(unneededimpand)
    if(np->opcode == C_RSHIFT)
    {
      np->rval->constant = 1;
      np->rval->optdata = 1;
      np->opcode = C_RSHIFTBY;
    }
    else
    {
      np->opcode = C_ISNONZERO;
      free(np->rval);
      np->rval = np->lval;
      np->lval = (node*) NULL;
    }
  }
  
  if((np->opcode == C_AND || np->opcode == C_NNAND ||
      np->opcode == C_LSHIFT || np->opcode == C_LSHIFTIN1 ||
      np->opcode == C_LSHIFT2 || np->opcode == C_LSHIFT8 ||
      np->opcode == C_RSHIFT) && np->lval->opcode == C_AND &&
     (np->rval->opcode == MESH || np->rval->opcode == MESH32) &&
     (np->lval->rval->opcode == MESH || np->lval->rval->opcode == MESH32))
  {
    OPTING(combineand)
    /* Combine the two ANDs into one. (The AND at np may be implicit.)
       Leave opdata the same. */
    np->rval->constant &= np->lval->rval->constant;
    op = np->lval;
    free(op->rval);
    np->lval = op->lval;
    free(op);
  }
  
  if((np->opcode == C_AND || np->opcode == C_NNAND ||
      np->opcode == C_LSHIFT || np->opcode == C_LSHIFTIN1 ||
      np->opcode == C_LSHIFT2 || np->opcode == C_LSHIFT8 ||
      np->opcode == C_RSHIFT) &&
     (np->rval->opcode == MESH || np->rval->opcode == MESH32) &&
     (~np->lval->optdata & np->rval->constant))
  {
    OPTING(weakenand)
    /* Reduce the constant in this C_AND-like expression
       to aid further optimization */
    np->rval->constant &= np->lval->optdata;
  }
  return opted;
}

/* Pass 1. Optimizes idioms. */
static int optimize_pass1(node *np) /* AIS: was 'optimize'. Recursive calls in
				       here have been updated. */
{
    node	  *mingleop, *op, *op2;
    unsigned long temp;
    int           temp2;
    int           opted = 0;
    
    /* recurse so we simplify each node after all its subnodes
       AIS: Keep track of whether we've optimized anything. I added
       all uses of the variable 'opted', and a return value. */
    if (np == (node *)NULL)
	return 0;
    else if (np->lval != (node *)NULL)
	opted |= optimize_pass1(np->lval);
    if (np->rval != (node *)NULL)
	opted |= optimize_pass1(np->rval);

    /*
     * OK, now do complete folding of constant expressions.
     */

    /* fold MINGLE operations on constants */
    if (np->opcode==MINGLE && (np->lval->opcode==MESH&&np->rval->opcode==MESH))
    {
      OPTING(minglefold)
	np->opcode = MESH32;
	np->constant = mingle(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
	np->lval=(node*)NULL;
	np->rval=(node*)NULL;
    }

    /* fold SELECT operations on constants */
    if (np->opcode == SELECT
	&& ((np->lval->opcode==MESH || np->lval->opcode==MESH32)
	    && (np->rval->opcode==MESH || np->rval->opcode==MESH32)))
    {
      OPTING(selectfold)
	np->opcode = np->rval->opcode;
	np->constant = iselect(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
	np->lval=(node*)NULL;
	np->rval=(node*)NULL;
    }

    /* fold AND operations on 16-bit constants */
    if (np->opcode == AND && np->rval->opcode == MESH)
    {
      OPTING(and16fold)
	np->opcode = MESH;
	np->constant = and16(np->rval->constant);
	free(np->rval);
	np->rval=(node*)NULL;
    }

    /* fold AND operations on 32-bit constants */
    if (np->opcode == AND && np->rval->opcode == MESH32)
    {
      OPTING(and32fold)
	np->opcode = MESH32;
	np->constant = and32(np->rval->constant);
	free(np->rval);
	np->rval=(node*)NULL;
    }

    /* fold OR operations on 16-bit constants */
    if (np->opcode == OR && np->rval->opcode == MESH)
    {
      OPTING(or16fold)
	np->opcode = MESH;
	np->constant = or16(np->rval->constant);
	free(np->rval);
	np->rval=(node*)NULL;
    }

    /* fold OR operations on 32-bit constants */
    if (np->opcode == OR && np->rval->opcode == MESH32)
    {
      OPTING(or32fold)
	np->opcode = MESH32;
	np->constant = or32(np->rval->constant);
	free(np->rval);
	np->rval=(node*)NULL;
    }

    /* fold XOR operations on 16-bit constants */
    if (np->opcode == XOR && np->rval->opcode == MESH)
    {
      OPTING(xor16fold)
	np->opcode = MESH;
	np->constant = xor16(np->rval->constant);
	free(np->rval);
	np->rval=(node*)NULL;
    }

    /* fold XOR operations on 32-bit constants */
    if (np->opcode == XOR && np->rval->opcode == MESH32)
    {
      OPTING(xor32fold)
	np->opcode = MESH32;
	np->constant = xor32(np->rval->constant);
	free(np->rval);
	np->rval=(node*)NULL;
    }

    /*
     * Less trivial stuff begins here
     */

    /* following optimizations only work in binary */
    if (Base != 2)
      return opted;

    /* recognize the idioms for various C binary logical operations */
    if (np->opcode == SELECT
	&& np->rval->opcode == MESH32 && np->rval->constant == 0x55555555
	&& (op = np->lval) 
	    && (op->opcode == AND || op->opcode == OR || op->opcode == XOR)
	&& (mingleop = op->rval) && mingleop->opcode == MINGLE
	&& mingleop->rval->width == 16
	&& mingleop->lval->width == 16)
    {
      OPTING(cbinlogop)
        free(np->rval); /* AIS */

        np->lval = mingleop->lval;
	np->rval = mingleop->rval;

	switch(op->opcode)
        {
	case AND: np->opcode = C_AND; break;
	case OR: np->opcode = C_OR; break;
	case XOR: np->opcode = C_XOR; break;
	default: lose(E778, yylineno, (char *)NULL);
        }

	free(mingleop); free(op);
    }

    /* AIS: Sometimes C binary logical operations have a smaller rval->constant. */
    if (np->opcode == SELECT
	&& (np->rval->opcode == MESH32||np->rval->opcode == MESH)
	&& (op = np->lval) && !(np->rval->constant&0xAAAAAAAALU)
	    && (op->opcode == AND || op->opcode == OR || op->opcode == XOR)
	&& (mingleop = op->rval) && mingleop->opcode == MINGLE
	&& mingleop->rval->width == 16
	&& mingleop->lval->width == 16)
    {
      unsigned long i=iselect(np->rval->constant,0x55555555LU);
      if(i==iselect(i,i))
      {
	OPTING(smallcbinlogop)
	np->lval = mingleop;
	
	switch(op->opcode)
	{
	case AND: np->lval->opcode = C_AND; break;
	case OR: np->lval->opcode = C_OR; break;
	case XOR: np->lval->opcode = C_XOR; break;
	default: lose(E778, yylineno, (char *)NULL);
	}
	np->opcode = C_AND;
	np->rval->constant=i; /* np->rval->opcode already correct */
	
	free(op);
      }  
    }
    
    /* recognize idioms for ~ */
    if (np->opcode == C_XOR)
    {
      /* AIS: Warning! We have to check again for C_XOR, else we will
	 be making use of the freed np->lval. This is not strictly necessary in
	 the first case.  I added extra conditions to prevent np->lval being
	 used after deallocation, which is how I discovered the error in the
	 first place. (Some code elsewhere was messing up, and I suspected
	 memory allocation problems.) */
	if (np->opcode == C_XOR && np->rval->width == 16
	    && np->lval->opcode == MESH &&np->lval->constant == 0xffff)
	{
	  OPTING(cnot1)
	    np->opcode = C_NOT;
	    free(np->lval);
	    np->lval=(node*)NULL;
	}

	if (np->opcode == C_XOR && np->lval->width == 16
	    && np->rval->opcode == MESH && np->rval->constant == 0xffff)
	{
	  OPTING(cnot2)
	    np->opcode = C_NOT;
	    free(np->rval);
	    np->rval = np->lval;
	    np->lval=(node*)NULL;
	}

	if (np->opcode == C_XOR && np->rval->width == 32
	    && np->lval->opcode == MESH32 && np->lval->constant == 0xffffffffLU)
	{
	  OPTING(cnot3)
	    np->opcode = C_NOT;
	    free(np->lval);
	    np->lval=(node*)NULL;
	}

	if (np->opcode == C_XOR && np->lval->width == 32
	    && np->rval->opcode == MESH32 && np->rval->constant == 0xffffffffLU)
	{
	  OPTING(cnot4)
	    np->opcode = C_NOT;
	    free(np->rval);
	    np->rval = np->lval;
	    np->lval=(node*)NULL;
	}
    }

    /* AIS: I wrote everything from here to the end of this function. */
    
    /* recognize the idiom x~'pow(2,n)-1' */
    if (np->opcode==SELECT&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
	iselect(np->rval->constant,np->rval->constant)==np->rval->constant)
    {
      OPTING(pow2m1select)
      np->opcode = C_AND; /* This is cheaper than both SELECT and C_NNAND.
			     lval, rval already correct. */
    }

    /* recognize the idiom x~pow(2,n). */
    if (np->opcode==SELECT&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
	iselect(np->rval->constant,np->rval->constant)==1) /* see, INTERCAL
							      operators can be
							      useful in C! */
    {
      OPTING(pow2select)
      np->opcode = C_NNAND; /* lval, rval already correct. C_NNAND is cheaper
			       than iselect. */
      if(np->rval->constant==1) np->opcode = C_AND;
    }

    /* recognize select-mingle combinations which can be replaced by &, | and
       single bitshifts (there are five of these in total)
       Most of these idioms are used when doing 32-bit logical operations
       separately on odd and even bits. Although these optimizations can speed
       up code containing them a bit, their main purpose is to replace the
       expressions with a consistent, short form that later optimizations can
       recognize. (In fact, some later optimizations are effectively
       optimizations on the degenerated C code, not on the INTERCAL original.) */
    if (np->opcode==MINGLE&&np->lval->opcode==SELECT&&
	np->lval->rval->opcode==MESH32&&np->lval->rval->constant==0xaaaaaaaa)
    {
      OPTING(selectmingle1)
      np->opcode=C_OR;
      np->lval->opcode=C_AND;
      op=np->rval;
      np->rval=newnode();
      np->rval->opcode=MINGLE;
      np->rval->lval=newnode();
      np->rval->lval->opcode=MESH;
      np->rval->lval->width=16;
      np->rval->lval->constant=0;
      np->rval->rval=op;
      np->rval->width=32;
      optimize_pass1(np->rval);
      optimize_pass1(np->lval);
      /* replace (a~0xaaaaaaaa)$b with (a&0xaaaaaaaa)|(0$b)
	 if b is a constant, the recursive call to optimize does
	 constant folding */
      if(np->opcode==C_OR&&np->lval->opcode==C_AND&&
	 np->rval->opcode==MESH32&&np->rval->constant==0)
      { /* check the conditions again in case it's been optimized */
	/* get rid of the C_OR */
	np->opcode=C_AND;
	op=np->lval;
	np->lval=op->lval;
	np->rval->constant=op->rval->constant;
	free(op->rval);
	free(op);
	optimize_pass1(np->lval);
      }
      else if(np->opcode==C_OR&&np->lval->opcode==C_AND&&
	      np->rval->opcode==MESH32&&np->rval->constant==0x55555555)
      { /* check the conditions again in case it's been optimized */
	/* get rid of the C_AND */
	op=np->lval;
	np->lval=op->lval;
	free(op->rval);
	free(op);
	optimize_pass1(np->lval);
      }
    }

    if (np->opcode==MINGLE&&np->rval->opcode==SELECT&&
	np->rval->rval->opcode==MESH32&&np->rval->rval->constant==0x55555555)
    {
      OPTING(selectmingle2)
      np->opcode=C_OR;
      np->rval->opcode=C_AND;
      op=np->lval;
      np->lval=newnode();
      np->lval->opcode=MINGLE;
      np->lval->rval=newnode();
      np->lval->rval->opcode=MESH;
      np->lval->rval->width=16;
      np->lval->rval->constant=0;
      np->lval->lval=op;
      np->lval->width=32;
      optimize_pass1(np->lval);
      optimize_pass1(np->rval);
      /* replace b$(a~0x55555555) with (a&0x55555555)|(b$0)
	 if b is a constant, the recursive call to optimize does
	 constant folding */
      if(np->opcode==C_OR&&np->rval->opcode==C_AND&&
	 np->lval->opcode==MESH32&&np->lval->constant==0)
      { /* check the conditions again in case it's been optimized */
	/* get rid of the C_OR */
	np->opcode=C_AND;
	op=np->rval;
	np->rval=op->lval;
	np->lval->constant=op->rval->constant;
	free(op->rval);
	free(op);
	optimize_pass1(np->rval);
      }
      else if(np->opcode==C_OR&&np->rval->opcode==C_AND&&
	      np->lval->opcode==MESH32&&np->lval->constant==0xaaaaaaaa)
      { /* check the conditions again in case it's been optimized */
	/* get rid of the C_AND */
	op=np->rval;
	np->rval=op->lval;
	free(op->rval);
	free(op);
	optimize_pass1(np->rval);
      }
    }

    if (np->opcode==MINGLE&&np->lval->opcode==SELECT&&
	np->lval->rval->opcode==MESH32&&np->lval->rval->constant==0x55555555)
    {
      OPTING(selectmingle3)
      np->opcode=C_OR;
      np->lval->opcode=C_LSHIFT; /* (a&b)<<1 */
      op=np->rval;
      np->rval=newnode();
      np->rval->opcode=MINGLE;
      np->rval->lval=newnode();
      np->rval->lval->opcode=MESH;
      np->rval->lval->width=16;
      np->rval->lval->constant=0;
      np->rval->rval=op;
      np->rval->width=32;
      optimize_pass1(np->rval);
      optimize_pass1(np->lval);
      /* replace (a~0x55555555)$b with ((a&0x55555555)<<1)|(0$b)
	 if b is a constant, the recursive call to optimize does
	 constant folding */
      if(np->opcode==C_OR&&np->lval->opcode==C_LSHIFT&&
	 np->rval->opcode==MESH32&&np->rval->constant==0)
      { /* check the conditions again in case it's been optimized */
	/* get rid of the C_OR */
	np->opcode=C_LSHIFT;
	op=np->lval;
	np->lval=op->lval;
	np->rval->constant=op->rval->constant;
	free(op->rval);
	free(op);
	optimize_pass1(np->lval); /* 32-bit logic recognition */
      }
    }

    if (np->opcode==MINGLE&&np->rval->opcode==SELECT&&
	np->rval->rval->opcode==MESH32&&np->rval->rval->constant==0xaaaaaaaa)
    {
      OPTING(selectmingle4)
      np->opcode=C_OR;
      np->rval->opcode=C_RSHIFT; /* (a&b)>>1 */
      op=np->lval;
      np->lval=newnode();
      np->lval->opcode=MINGLE;
      np->lval->rval=newnode();
      np->lval->rval->opcode=MESH;
      np->lval->rval->width=16;
      np->lval->rval->constant=0;
      np->lval->lval=op;
      np->lval->width=32;
      optimize_pass1(np->lval);
      optimize_pass1(np->rval);
      /* replace b$(a~0xaaaaaaaa) with ((a&0xaaaaaaaa)>>1)|(b$0)
	 if b is a constant, the recursive call to optimize does
	 constant folding */
      if(np->opcode==C_OR&&np->rval->opcode==C_RSHIFT&&
	 np->lval->opcode==MESH32&&np->lval->constant==0)
      { /* check for these conditions again in case it's been optimized */
	/* get rid of the C_OR */
	np->opcode=C_RSHIFT;
	op=np->rval;
	np->rval=op->lval;
	np->lval->constant=op->rval->constant;
	free(op->rval);
	free(op);
	optimize_pass1(np->rval);
      }
    }

    if(np->opcode==MINGLE&&(np->lval->opcode==MESH||
			    np->lval->opcode==MESH32)&&
       np->lval->constant==0&&np->rval->opcode==C_LSHIFT&&
       (np->rval->rval->opcode==MESH||np->rval->rval->opcode==MESH32)&&
       np->rval->rval->constant==0x7fff&&np->rval->lval->opcode==SELECT&&
       np->rval->lval->rval->opcode==MESH32&&np->rval->lval->rval->constant==
       0x2aaaaaaa)
    {
      OPTING(selectmingle5)
      /* this is equivalent to (np->rval->lval->lval&0x2aaaaaaa)<<1 */
      np->opcode=C_LSHIFT;
      np->lval->opcode=MESH32;
      np->lval->constant=0x2aaaaaaa;
      op=np->rval;
      np->rval=op->lval->lval;
      free(op->lval->rval);
      free(op->lval);
      free(op->rval);
      free(op);
    }
    
    /* recognize the idiom (x~x)~1 (already optimized to (x~x)&1). */

    if (np->opcode==C_AND&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
	np->rval->constant==1&&np->lval->opcode==SELECT&&
	nodessame(np->lval->lval,np->lval->rval))
/*	(np->lval->lval->opcode==ONESPOT||np->lval->lval->opcode==TWOSPOT)&&
	np->lval->lval->opcode==np->lval->rval->opcode&&
	np->lval->lval->constant==np->lval->rval->constant)*/
    {
      OPTING(xselxsel1)
      free(np->rval);
      np->rval=np->lval->rval;
      nodefree(np->lval->lval);
      free(np->lval);
      np->lval=(node*) NULL;
      np->opcode = C_ISNONZERO;
    }

    /* recognize the idiom (x~y)~1 (already optimized to (x~y)&1) in other cases.
    if (np->opcode==C_AND&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
	np->rval->constant==1&&np->lval->opcode==SELECT)
    {
      OPTING(xselysel1)
      np->opcode = C_NNAND;
      free(np->rval);
      np->rval = np->lval->rval;
      op = np->lval;
      np->lval = np->lval->lval;
      free(op);
      }
      This idiom is commented out because I thought it simplified to !!(x & y),
      and of course it doesn't. The next two idioms are (hopefully correct)
      idioms that were inspired by this idea.
    */

    /* recognize the idiom (x ~ k) ~ 1 (ie (x ~ k) & 1) for constant k */
    if(np->opcode==C_AND&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
       np->rval->constant==1&&np->lval->opcode==SELECT&&
       (np->lval->rval->opcode==MESH||np->lval->rval->opcode==MESH32))
    {
      OPTING(xselksel1)
      np->opcode = C_NNAND;
      free(np->rval);
      np->rval = np->lval->rval;
      op = np->lval;
      np->lval = np->lval->lval;
      free(op);
      temp = 1;
      if(np->rval->constant == 0) temp = 0; else
	while(!(np->rval->constant % 2))
	{
	  np->rval->constant /= 2;
	  temp *= 2;
	}
      np->rval->constant = temp;
      /* So (x ~ k) ~ 1 optimizes to !!(x & n) where n is the lowest set
	 bit in k. */
    }

    /* recognize the idiom (k ~ x) ~ 1 (ie (k ~ x) & 1) where k is a power
       of 2 minus 1 */
    if (np->opcode==C_AND&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
	np->rval->constant==1&&np->lval->opcode==SELECT&&
        (np->lval->lval->opcode==MESH||np->lval->lval->opcode==MESH32)&&
        iselect(np->lval->lval->constant,np->lval->lval->constant)==
        np->lval->lval->constant)
    {
      OPTING(p2m1selxsel1)
      np->opcode = C_NNAND;
      free(np->rval);
      np->rval = np->lval->rval;
      op = np->lval;
      np->lval = np->lval->lval;
      free(op);
    }    

    /* recognize idioms for left-shift and right-shift */
    /* 16- or 32-bit right-shift by 1, not necessarily all the bits */
    if(np->opcode==SELECT&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
       iselect(np->rval->constant,np->rval->constant)<<1==np->rval->constant)
    {
      OPTING(rshift1)
      np->opcode = C_RSHIFT; /* Keep rval as it tells us what to & with */
    }

    /* 16- or 32-bit right-shift by a higher number */
    if(np->opcode==SELECT&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
       iselect(xor32(np->rval->constant),xor32(np->rval->constant))==3&&
       !(np->rval->constant&1))
    {
      OPTING(rshiftby)
      np->opcode = C_RSHIFTBY;
      temp2 = 0;
      temp = np->rval->constant;
      while(!(np->rval->constant&1))
      {
	np->rval->constant>>=1;
	temp2++;
      }
      np->rval->constant = temp2;
      op = np->lval;
      np->lval = newnode();
      np->lval->width = np->width;
      np->lval->rval = newnode();
      np->lval->opcode = C_AND;      
      np->lval->rval->opcode = np->rval->opcode;
      np->rval->opcode = MESH;
      np->lval->rval->width = np->rval->width;
      np->rval->width = 16;
      np->lval->rval->constant = temp;
      np->lval->lval = op;
      optimize_pass1(np->lval); /* not sure what good this will do, but it's good practice
			     to optimize newly created nodes, in case someone invents
			     a new optimization (this caught me out elsewhere) */
      /* The joke with this optimization is that it comes out with more nodes than
	 when it started. However, under most implementations of C, & and >> are
	 significantly cheaper than iselect. */
    }
    
    /* 16-bit left-shift by 1 */
    if(np->opcode==SELECT&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
       np->lval->opcode==MINGLE&&(np->rval->constant & 0x55555555LU)==1&&
       (np->lval->rval->opcode==MESH||np->lval->rval->opcode==MESH32)&&
       (np->lval->rval->constant==0||np->lval->rval->constant==1))
    {
      unsigned long i=iselect(np->rval->constant, 0xaaaaaaaaLU);
      if(i==iselect(i,i))
      {
	OPTING(lshift16by1)
	np->opcode = np->lval->rval->constant?C_LSHIFTIN1:C_LSHIFT;
	/* We can shift either a 1 or a 0 into the right end of the number. */
	np->rval->constant=i; /* New effect of this node: (lval->lval & i)<<1 */
	/* We know that np->rval has the right opcode. */
	op=np->lval->lval;
	free(np->lval);
	np->lval=op;
      }
    }

    /* 32-bit left-shift of a variable by 1 */
    if(np->opcode==C_OR&&np->lval->opcode==C_LSHIFT&&
       np->rval->opcode==C_LSHIFT)
      do
      { /* this is so we can break; out of the if */
	temp=0;
	if(np->lval->lval->opcode==MESH32&&
	   np->lval->lval->constant==0x55555555) temp++;
	if(np->lval->rval->opcode==MESH32&&
	   np->lval->rval->constant==0x55555555) temp++;
	if(np->rval->lval->opcode==MESH32&&
	   np->rval->lval->constant==0x55555555) temp++;
	if(np->rval->rval->opcode==MESH32&&
	   np->rval->rval->constant==0x55555555) temp++;
	if(temp!=1) break;
	temp=0;
	if(np->lval->lval->opcode==MESH32&&
	   np->lval->lval->constant==0x2aaaaaaa) temp++;
	if(np->lval->rval->opcode==MESH32&&
	   np->lval->rval->constant==0x2aaaaaaa) temp++;
	if(np->rval->lval->opcode==MESH32&&
	   np->rval->lval->constant==0x2aaaaaaa) temp++;
	if(np->rval->rval->opcode==MESH32&&
	   np->rval->rval->constant==0x2aaaaaaa) temp++;
	if(temp!=1) break;
	/* found the two required constants. Now we need two
	   identical TWOSPOTs on opposite sides of the C_OR. */
	temp=0xffffffff; /* who'd use that many variables? */
	if(np->lval->lval->opcode==TWOSPOT&&
	   np->rval->lval->opcode==TWOSPOT&&
	   np->lval->lval->constant==
	   np->rval->lval->constant) temp=np->lval->lval->constant;
	if(np->lval->lval->opcode==TWOSPOT&&
	   np->rval->rval->opcode==TWOSPOT&&
	   np->lval->lval->constant==
	   np->rval->rval->constant) temp=np->lval->lval->constant;
	if(np->lval->rval->opcode==TWOSPOT&&
	   np->rval->lval->opcode==TWOSPOT&&
	   np->lval->rval->constant==
	   np->rval->lval->constant) temp=np->lval->rval->constant;
	if(np->lval->rval->opcode==TWOSPOT&&
	   np->rval->rval->opcode==TWOSPOT&&
	   np->lval->rval->constant==
	   np->rval->rval->constant) temp=np->lval->rval->constant;
	if(temp==0xfffffffff) break;
	/* recognized a 32-bit leftshift */
	OPTING(lshift32by1)
	free(np->lval->lval);
	free(np->lval->rval);
	free(np->rval->lval);
	free(np->rval->rval);
	np->lval->opcode=TWOSPOT;
	np->lval->constant=temp;
	np->rval->opcode=MESH32;
	np->rval->constant=0x7fffffff;
	np->lval->lval=(node*)NULL;
	np->lval->rval=(node*)NULL;
	np->rval->lval=(node*)NULL;
	np->rval->rval=(node*)NULL;
	np->opcode=C_LSHIFT;
      } while(0);

    /* 16-bit left-shift by 2 */
    if(np->opcode==C_LSHIFT&&np->lval->opcode==C_LSHIFT&&
       np->rval->constant==np->lval->rval->constant)
    {
      OPTING(lshift16by2)
      op=np->lval->lval;
      np->opcode=C_LSHIFT2;
      free(np->lval);
      np->lval=op;
      np->rval->constant>>=1;
    }

    /* An idiom for 16-bit left-shift by 8 is caught later. */

    /* recognize the idiom '?x$#y'~#3, where y is 1 or 2
       The ~ is already optimized to &. */
    if(np->opcode==C_AND&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
       np->rval->constant==3&&np->lval->opcode==XOR&&
       np->lval->rval->opcode==MINGLE&&(mingleop=np->lval->rval)&&
       (mingleop->rval->opcode==MESH||mingleop->rval->opcode==MESH32)&&
       (mingleop->rval->constant==1||mingleop->rval->constant==2))
    {
      OPTING(xorto1or2)
      /* This depends only on the rightmost bit of x, and y.
	 For y=1:
	 If 0, answer = 0x1
	 If 1, answer = 0x2
	 For y=2:
	 If 0, answer = 0x2
	 Id 1, answer = 0x1
      */
      np->opcode = mingleop->rval->constant==1?C_AND1ADD1:C_2SUBAND1;
      free(np->rval);
      np->rval=mingleop->lval;
      free(mingleop->rval);
      free(mingleop);
      free(np->lval);
      free(mingleop->rval);
      np->lval=(node*)NULL;

      /* np->rval could be an iselect of a variable with itself,
	 and not have been caught yet (because the &1 was implied,
	 not explicit). Catch this now. */
      if(np->rval->opcode==SELECT&&
	 (np->rval->lval->opcode==ONESPOT||np->rval->lval->opcode==TWOSPOT)&&
	 np->rval->lval->opcode==np->rval->rval->opcode&&
	 np->rval->lval->constant==np->rval->rval->constant)
      {
	np->rval->opcode=C_ISNONZERO;
	free(np->rval->lval);
	np->rval->lval=(node*)NULL;
      }

      /* Even if that didn't work, iselect can still be replaced by nnand
	 (because we have an implicit (x~y)&1) */
      if(np->rval->opcode==SELECT) np->rval->opcode=C_NNAND;
      
      /* If we know np->rval must be 0 or 1 at runtime, we can optimize further
         (to avoid (1&x)&1, (x&1)&1 or (!!x)&1) */
      if(np->rval->opcode==C_NNAND||np->rval->opcode==C_ISNONZERO||
	 (np->rval->opcode==C_AND&&
	  (np->rval->lval->opcode==MESH||np->rval->lval->opcode==MESH32)
	  &&np->rval->lval->constant==1)
	 ||(np->rval->opcode==C_AND&&
	    (np->rval->rval->opcode==MESH||np->rval->rval->opcode==MESH32)
	    &&np->rval->rval->constant==1)	    
	)
      {
	np->opcode=np->opcode==C_AND1ADD1?C_1PLUS:C_2MINUS;
      }
    }
    
    /* optimize a select with 0xffffffff (already optimized to &) */
    if(np->opcode==C_AND&&np->rval->opcode==MESH32&&
       np->rval->constant==0xffffffff)
    {
      OPTING(noopand1r)
      free(np->rval);
      op=np->lval;
      *np=*op; /* copy the whole node */
      np->width=32;
      free(op);
    }
    if(np->opcode==C_AND&&np->lval->opcode==MESH32&&
       np->lval->constant==0xffffffff)
    {
      OPTING(noopand1l)
      free(np->lval);
      op=np->rval;
      *np=*op; /* copy the whole node */
      np->width=32;
      free(op);
    }

    /* optimize complementing the odd or even bits of a 32-bit number */
    /* complement odd bits, zero even bits */
    if(np->opcode==C_LSHIFT&&np->rval->opcode==MESH32&&
       np->rval->constant==0x55555555&&np->lval->opcode==XOR&&
       np->lval->width==32&&np->lval->rval->opcode==C_OR&&
       ((np->lval->rval->rval->opcode==MESH32&&np->lval->rval->rval->
	 constant==0x55555555&&((temp2 = 0),1))||
	(np->lval->rval->lval->opcode==MESH32&&np->lval->rval->lval->
	 constant==0x55555555&&(temp2 = 1))))
    {
      OPTING(com1z0l)
      /* optimizations so far have given us code which sets all even
	 bits of a 32-bit expression, xor32s it, selects the even bits
	 and left-shifts. This is equivalent to complementing all odd
	 bits of the number and zeroing the even bits. */
      np->rval->constant = 0xaaaaaaaa;
      np->lval->rval->opcode = C_AND;
      op = temp2 ? np->lval->rval->lval : np->lval->rval->rval;
      op->constant = 0xaaaaaaaa;
      op = np->lval->rval;
      free(np->lval);
      np->lval = op;
      np->opcode = C_XOR;
    }

    /* same optimization with np's operands exchanged */
    if(np->opcode==C_LSHIFT&&np->lval->opcode==MESH32&&
       np->lval->constant==0x55555555&&np->rval->opcode==XOR&&
       np->rval->width==32&&np->rval->rval->opcode==C_OR&&
       ((np->rval->rval->rval->opcode==MESH32&&np->rval->rval->rval->
	 constant==0x55555555&&((temp2 = 0),1))||
	(np->rval->rval->lval->opcode==MESH32&&np->rval->rval->lval->
	 constant==0x55555555&&(temp2 = 1))))
    {
      OPTING(com1z0r)
      /* optimizations so far have given us code which sets all even
	 bits of a 32-bit expression, xor32s it, selects the even bits
	 and left-shifts. This is equivalent to complementing all odd
	 bits of the number and zeroing the even bits. */
      np->lval->constant = 0xaaaaaaaa;
      np->rval->rval->opcode = C_AND;
      op = temp2 ? np->rval->rval->lval : np->rval->rval->rval;
      op->constant = 0xaaaaaaaa;
      op = np->rval->rval;
      free(np->rval);
      np->rval = op;
      np->opcode = C_XOR;
    }

    /* complement even bits, zero odd bits */
    if(np->opcode==C_AND&&np->rval->opcode==MESH32&&
       np->rval->constant==0x55555555&&np->lval->opcode==XOR&&
       np->lval->width==32&&np->lval->rval->opcode==C_OR&&
       np->lval->rval->rval->opcode==MESH32&&
       np->lval->rval->rval->constant==0x55555555&&
       np->lval->rval->lval->opcode==C_LSHIFT&&
       np->lval->rval->lval->rval->opcode==MESH32&&
       np->lval->rval->lval->rval->constant==0x55555555)
    {
      OPTING(com0z1)
      /* There are another 7 ways this expression could be written.
	 However, this is the only way likely to be generated by
	 the rest of the optimizer. */
      op=np->lval->rval->lval->lval;
      free(np->lval->rval->lval->rval);
      free(np->lval->rval->lval);
      free(np->lval->rval->rval);
      np->lval->opcode = C_AND;
      np->opcode = C_XOR;
      np->lval->lval = op;
      np->lval->rval->opcode = MESH32;
      np->lval->rval->lval = (node*) NULL;
      np->lval->rval->rval = (node*) NULL;
      np->lval->rval->constant = 0x55555555;
      /* and np->rval is already correct! */
    }

    /* optimize no-op C_OR, C_AND, C_XOR
       (some of these are produced as a result of earlier optimizations).
       This sort of thing was also used in the days of INTERCAL-72 to
       convert information to 16- or 32-bit representations. As C-INTERCAL
       is almost untyped and the typechecker has already run, this can be
       done safely. One of these cases has already been considered earlier
       (the one generated by the code (something)~'#65535$#65535'), but it
       is included it again to complete the list. */
    /* (something op constant) */
    if(np->opcode==C_OR&&(np->lval->opcode==MESH||np->lval->opcode==MESH32)
       &&np->lval->constant==0)
    {
      OPTING(nooporl)
      free(np->lval);
      op=np->rval;
      *np=*op;
      free(op);
    }
    if(np->opcode==C_XOR&&(np->lval->opcode==MESH||np->lval->opcode==MESH32)
       &&np->lval->constant==0)
    {
      OPTING(noopxorl)
      free(np->lval);
      op=np->rval;
      *np=*op;
      free(op);
    }
    if(np->opcode==C_AND&&(np->lval->opcode==MESH||np->lval->opcode==MESH32)
       &&np->lval->constant==0xffffffff)
    {
      OPTING(noopandl)
      free(np->lval);
      op=np->rval;
      *np=*op;
      free(op);
    }
    if(np->opcode==C_OR&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)
       &&np->rval->constant==0)
    {
      OPTING(nooporr)
      free(np->rval);
      op=np->lval;
      *np=*op;
      free(op);
    }
    if(np->opcode==C_XOR&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)
       &&np->rval->constant==0)
    {
      OPTING(noopxorr)
      free(np->rval);
      op=np->lval;
      *np=*op;
      free(op);
    }
    if(np->opcode==C_AND&&(np->rval->opcode==MESH||np->rval->opcode==MESH32)
       &&np->rval->constant==0xffffffff)
    {
      OPTING(noopandr)
      free(np->rval);
      op=np->lval;
      *np=*op;
      free(op);
    }
    /* something op itself */
    if((np->opcode==C_AND||np->opcode==C_OR)&&nodessame(np->lval,np->rval))
    {
      OPTING(andoritself)
      nodefree(np->rval);
      op=np->lval;
      *np=*op;
      free(op);
    }
    if(np->opcode==C_XOR&&nodessame(np->lval,np->rval))
    {
      OPTING(xoritself)
      nodefree(np->lval);
      nodefree(np->rval);
      np->opcode = (np->width==16?MESH:MESH32);
      np->lval = (node*) NULL;
      np->rval = (node*) NULL;
      np->constant = 0;
    }
    /* repetitive implicit C_AND */
    if(np->opcode==C_NNAND&&nodessame(np->lval,np->rval))
    {
      OPTING(reptimpand)
      nodefree(np->lval);
      np->lval = (node*) NULL;
      np->opcode = C_ISNONZERO;
    }

    /* optimize 32-bit complement */
    if(np->opcode==C_OR&&np->lval->opcode==C_XOR&&np->lval->lval->opcode==C_AND&&
       np->rval->opcode==C_XOR&&np->rval->lval->opcode==C_AND&&
       np->lval->rval->opcode==MESH32&&np->lval->lval->rval->opcode==MESH32&&
       np->rval->rval->opcode==MESH32&&np->rval->lval->rval->opcode==MESH32&&
       np->lval->rval->constant==np->lval->lval->rval->constant&&
       np->rval->rval->constant==np->rval->lval->rval->constant&&
       (np->lval->rval->constant^np->rval->rval->constant)==0xffffffff&&
       nodessame(np->lval->lval->lval,np->rval->lval->lval))
    {
      OPTING(com32)
      np->opcode = C_NOT;
      np->width = 32;
      free(np->rval->lval->lval);
      free(np->rval->lval->rval);
      free(np->rval->lval);
      free(np->rval->rval);
      free(np->rval);
      op = np->lval->lval->lval;
      free(np->lval->lval->rval);
      free(np->lval->lval);
      free(np->lval->rval);
      free(np->lval);
      np->rval = op;
      np->lval = (node*)NULL;
    }
    
    /* If constant is on the left, move it to the right. This will
       simplify checking for the remaining optimizations, while not
       altering the effect of code that cannot be optimized further.
       (There are furious debates within the C community as to
       whether a constant should be written on the left or right of
       an operation. The argument in favour of 'constant on the left'
       is mainly that it's better at detecting a mistype of = for ==.
       This code assumes that ick won't make that mistake.) */
    if(np->opcode==C_LSHIFT||np->opcode==C_AND||np->opcode==C_OR||
       np->opcode==C_XOR||np->opcode==C_NNAND||np->opcode==C_RSHIFT)
    { /* all the above opcodes are symmetrical wrt their operands */
      if(np->lval->opcode==MESH||np->lval->opcode==MESH32)
      {
	OPTING(constswitch)
	op = np->lval;
	np->lval = np->rval;
	np->rval = op;
      }
    }

    /* 32-bit logical operations on alternate bits.
       At this point, they have been optimized
       to an expression of the form op32(oddbits | evenbits)&0x55555555,
       which is good but still uses the INTERCAL operator op32. We should
       be able to optimize this using & | ^ and bitshifts. */
    if((np->opcode==C_AND||np->opcode==C_LSHIFT)&&np->rval->opcode==MESH32&&
       np->rval->constant==0x55555555&&np->lval->width==32&&
       (np->lval->opcode==XOR||np->lval->opcode==OR||
	np->lval->opcode==AND)&&np->lval->rval->opcode==C_OR)
      do /* so we can break; out */
      {
	/* Check to see if the op32 is a C_OR of an oddbits and an
	   evenbits */
	op=np->lval->rval;
	temp2=0; /* |=1 for oddbits found, |=2 for evenbits found */
	if(op->lval->opcode==C_AND&&op->lval->rval->opcode==MESH32&&
	   op->lval->rval->constant==0x55555555) temp2|=2;
	if(op->lval->opcode==C_AND&&op->lval->rval->opcode==MESH32&&
	   op->lval->rval->constant==0xaaaaaaaa) temp2|=1;
	if(op->rval->opcode==C_AND&&op->rval->rval->opcode==MESH32&&
	   op->rval->rval->constant==0x55555555) temp2|=2;
	if(op->rval->opcode==C_AND&&op->rval->rval->opcode==MESH32&&
	   op->rval->rval->constant==0xaaaaaaaa) temp2|=1;
	if(op->lval->opcode==C_LSHIFT&&op->lval->rval->opcode==MESH32&&
	   op->lval->rval->constant==0x55555555) temp2|=1;
	if(op->lval->opcode==C_RSHIFT&&op->lval->rval->opcode==MESH32&&
	   op->lval->rval->constant==0xaaaaaaaa) temp2|=2;
	if(op->rval->opcode==C_LSHIFT&&op->rval->rval->opcode==MESH32&&
	   op->rval->rval->constant==0x55555555) temp2|=1;
	if(op->rval->opcode==C_RSHIFT&&op->rval->rval->opcode==MESH32&&
	   op->rval->rval->constant==0xaaaaaaaa) temp2|=2;
	if(temp2!=3) break;
	/* this expression can be optimized */
	OPTING(shiftand)
	if(np->lval->opcode==XOR) op->opcode=C_XOR;
	if(np->lval->opcode==OR) op->opcode=C_OR;
	if(np->lval->opcode==AND) op->opcode=C_AND;
	if(np->opcode==C_LSHIFT)
	{
	  /* odd-bit-ize the evenbits we found */
	  if(op->lval->rval->constant==0x55555555) op->lval->opcode=C_LSHIFT;
	  if(op->rval->rval->constant==0x55555555) op->rval->opcode=C_LSHIFT;
	  if(op->lval->rval->constant==0xaaaaaaaa) op->lval->opcode=C_AND;
	  if(op->rval->rval->constant==0xaaaaaaaa) op->rval->opcode=C_AND;
	} else {
	  /* even-bit-ize the oddbits we found */
	  if(op->lval->rval->constant==0x55555555) op->lval->opcode=C_AND;
	  if(op->rval->rval->constant==0x55555555) op->rval->opcode=C_AND;
	  if(op->lval->rval->constant==0xaaaaaaaa) op->lval->opcode=C_RSHIFT;
	  if(op->rval->rval->constant==0xaaaaaaaa) op->rval->opcode=C_RSHIFT;
	}
	/* What np should be is now in op */
	free(np->lval);
	free(np->rval);
	*np=*op;
      } while(0);

    /* Optimize 32-bit logical expressions on the whole number.
       At this point, the most common idiom for a 32-bit logical operation
       (we write it here as %) looks like this:
       ((a&0xaaaaaaaa)%(b&0xaaaaaaaa))|((a&0x55555555)%(b&0x55555555))
       So first, we use distributive laws to simplify such constructions
       further.

       The distribution is done at a lower level of the expression tree,
       but this is where it is done in the sequence of optimizations needed
       to optimize a 32-bit expression. These optimizations are unlikely to
       help in 16-bit code, because it's unlikely a programmer would take the
       effort to code an expression like the ones below unless he/she had to.

       Expressions that distribute:
       (a&k)&(b&k) == (a&b)&k
       (a|k)&(b|k) == (a&b)|k
       (a&k)|(b&k) == (a|b)&k
       (a|k)|(b|k) == (a|b)|k
       (a&k)^(b&k) == (a^b)&k */
    if((np->opcode==C_AND||np->opcode==C_OR||np->opcode==C_XOR)&&
       (np->lval->opcode==C_AND||np->lval->opcode==C_OR)&&
       !(np->lval->opcode==C_OR&&np->opcode==C_XOR)&&
       np->lval->opcode==np->rval->opcode)
    {
      if(nodessame(np->lval->lval,np->rval->lval))
      {
	OPTING(distribll)
	nodefree(np->lval->lval);
	np->lval->lval = np->rval->rval;
	np->lval->opcode = np->opcode;
	np->opcode = np->rval->opcode;
	op = np->rval;
	np->rval = np->rval->lval;
	free(op);
      }
      else if(nodessame(np->lval->rval,np->rval->lval))
      {
	OPTING(distribrl)
	nodefree(np->lval->rval);
	np->lval->rval = np->rval->rval;
	np->lval->opcode = np->opcode;
	np->opcode = np->rval->opcode;
	op = np->rval;
	np->rval = np->rval->lval;
	free(op);
      }
      else if(nodessame(np->lval->lval,np->rval->rval))
      {
	OPTING(distriblr)
	nodefree(np->lval->lval);
	np->lval->lval = np->rval->lval;
	np->lval->opcode = np->opcode;
	np->opcode = np->rval->opcode;
	op = np->rval;
	np->rval = np->rval->rval;
	free(op);
      }
      else if(nodessame(np->lval->rval,np->rval->rval))
      {
	OPTING(distribrr)
	nodefree(np->lval->rval);
	np->lval->rval = np->rval->lval;
	np->lval->opcode = np->opcode;
	np->opcode = np->rval->opcode;
	op = np->rval;
	np->rval = np->rval->rval;
	free(op);
      }
    }

    /* At this point, the 32-bit logical expression looks like this:
       (with % substituting for &,|, or ^)
       (0xaaaaaaaa|0x55555555)&(a%b)
       Clearly, what is needed is a bit of constant folding on C_OR. We
       might as well do C_AND and C_XOR while we're at it. This will get
       done during the second run of optimize_pass1. */
    if((np->opcode==C_AND||np->opcode==C_OR||np->opcode==C_XOR)&&
       (np->lval->opcode==MESH||np->lval->opcode==MESH32)&&
       (np->rval->opcode==MESH||np->rval->opcode==MESH32))
    {
      OPTING(clogopfold)
      switch(np->opcode)
      {
      case C_AND: np->constant = np->lval->constant & np->rval->constant; break;
      case C_OR:  np->constant = np->lval->constant | np->rval->constant; break;
      case C_XOR: np->constant = np->lval->constant ^ np->rval->constant; break;
      default: lose(E778, yylineno, (char*) NULL);
      }
      free(np->lval);
      np->lval = (node*) NULL;
      free(np->rval);
      np->rval = (node*) NULL;
      np->opcode = np->width == 16 ? MESH : MESH32;
    }

    /* Somewhat incomprehensibly, there is a 32-bit right-shift in the
       standard library that has been done using separation into even
       and odd bits! This code recognizes that idiom, even though
       surely it would have been simpler to select against
       0xfffffffe? (This would have been optimized much earlier in this
       function, and would also be shorter and clearer. Perhaps the
       obfuscation is the reason that it was written in separate bits.) */
    
    /* Selecting something with 0x55555554, then mingling with 0,
       right-shifts the even bits onto the odd bits. */
    if(np->opcode==MINGLE&&np->lval->opcode==SELECT&&
       (np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
       np->rval->constant==0&&np->lval->rval->opcode==MESH32&&
       np->lval->rval->constant==0x55555554)
    {
      OPTING(selminglershift)
      np->opcode = C_RSHIFT;
      np->rval->opcode = MESH32;
      np->rval->width = 32;
      np->rval->constant = 0x55555554;
      free(np->lval->rval);
      op = np->lval->lval;
      free(np->lval);
      np->lval = op;
    }

    /* Since it's such a stupid idiom anyway, we'll just optimize it the
       way round that the standard library has it. It looks like this:
       (((a&0x55555554)>>1)|((a&0xaaaaaaaa)>>1)) */
    if(np->opcode==C_OR&&np->lval->opcode==C_RSHIFT&&
       np->rval->opcode==C_RSHIFT&&np->lval->rval->opcode==MESH32&&
       np->lval->rval->constant==0x55555554&&
       np->rval->rval->opcode==MESH32&&np->rval->rval->constant==
       0xaaaaaaaa&&np->lval->lval->opcode==TWOSPOT&&
       np->rval->lval->opcode==TWOSPOT&&np->lval->lval->constant==
       np->rval->lval->constant)
    {
      OPTING(sillysyslibidiom)
      op = np->lval;
      free(np->rval->lval);
      free(np->rval->rval);
      free(np->rval);
      op->rval->constant = 0xfffffffe;
      *np = *op;
      free(op);
      optuple->warn128 = 1;
    }

    /* A more useful idiom in the standard library is on the
       internal subroutine labelled (1525). It's an 8-bit
       left-shift. In INTERCAL, this is done with repeated
       mingling and selection. So far, the optimiser can only
       recognize two leftshifts in the inside, because the rest
       of the expression is written assuming that the low-bits
       of the result are unset (which is correct after a two-bit
       left-shift). This code recognizes the entire idiom straight
       off, rather than optimizing in stages, after the optimization
       of the inside to a double-leftshift. */
    if(np->opcode==SELECT&&np->rval->opcode==MESH32&&
       np->rval->constant==0xaaaaff&&np->lval->opcode==MINGLE&&
       (np->lval->rval->opcode==MESH||np->lval->rval->opcode==MESH32)&&
       np->lval->rval->constant==0&&np->lval->lval->opcode==SELECT&&
       np->lval->lval->rval->opcode==MESH32&&
       np->lval->lval->rval->constant==0xaaaaaaf&&
       np->lval->lval->lval->opcode==MINGLE&&(
	 np->lval->lval->lval->rval->opcode==MESH||
	 np->lval->lval->lval->rval->opcode==MESH32)&&
       np->lval->lval->lval->rval->constant==0&&
       np->lval->lval->lval->lval->opcode==C_LSHIFT2&&(
	 np->lval->lval->lval->lval->rval->opcode==MESH||
	 np->lval->lval->lval->lval->rval->opcode==MESH32)&&
       np->lval->lval->lval->lval->rval->constant==0x3fff)
    {
      OPTING(lshift8)
      /* left-shift np->lval->lval->lval->lval->lval by 8 */
      op = np->lval->lval->lval->lval->lval;
      free(np->lval->lval->lval->lval->rval);
      free(np->lval->lval->lval->lval);
      free(np->lval->lval->lval->rval);
      free(np->lval->lval->lval);
      free(np->lval->lval->rval);
      free(np->lval->lval);
      free(np->lval->rval);
      free(np->lval);
      np->lval = op;
      np->opcode = C_LSHIFT8;
      np->rval->constant = 0x00ffffff;
    }

    /* recognize an idiom for ==, != */
    if(np->opcode == C_ISNONZERO && np->rval->opcode == C_XOR)
    {
      OPTING(xortoequal)
      np->opcode = C_NOTEQUAL;
      op = np->rval;
      np->lval = op->lval;
      np->rval = op->rval;
      free(op);
      /* So !!(a ^ b), as in 1 + !!(a^b), has become (a!=b). */
    }

    /* Some of the remaining idioms depend on active-bit analysis */
    checknodeactbits(np);
    /* C binary logical operations can be done in 'expanded' form, that is, on
       just oddbits or evenbits of a 32-bit number. */
    if((np->opcode==C_AND || np->opcode==C_LSHIFT || np->opcode == C_LSHIFT2 ||
	np->opcode==C_LSHIFT8 || np->opcode==C_LSHIFTIN1 || np->opcode==C_NNAND)
       && (np->rval->opcode==MESH32 || np->rval->opcode==MESH)
       && !(np->rval->constant & 0xaaaaaaaaLU))
    {
      unsigned long i=iselect(np->rval->constant, 0x55555555LU);
      if(i==iselect(i,i) && (np->lval->opcode == OR ||
			     np->lval->opcode == AND ||
			     np->lval->opcode == XOR) &&
	 np->lval->rval->opcode==C_OR)
      {
	if(!(np->lval->rval->lval->optdata & 0x55555555LU) &&
	   !(np->lval->rval->rval->optdata & 0xaaaaaaaaLU) &&
	   (np->lval->rval->lval->opcode == C_AND ||
	    np->lval->rval->lval->opcode == C_LSHIFT ||
	    np->lval->rval->lval->opcode == MESH ||
	    np->lval->rval->lval->opcode == MESH32))
	{ /* recognized the idiom. Swap the C_OR round so we don't have to write
	     the code for this out twice. */
	  OPTING(clogopexpswitch)
	  op=np->lval->rval->rval;
	  np->lval->rval->rval=np->lval->rval->lval;
	  np->lval->rval->lval=op;
	}
	if(!(np->lval->rval->lval->optdata & 0xaaaaaaaaLU) &&
	   !(np->lval->rval->rval->optdata & 0x55555555LU) &&
	   (np->lval->rval->rval->opcode == C_AND ||
	    np->lval->rval->rval->opcode == C_LSHIFT ||
	    np->lval->rval->lval->opcode == MESH ||
	    np->lval->rval->lval->opcode == MESH32))
	{
	  OPTING(clogopexpand)
	  op = np->lval->rval;
	  switch(np->lval->opcode)
	  {
	  case AND: op->opcode = C_AND; break;
	  case OR:  op->opcode = C_OR;  break;
	  case XOR: op->opcode = C_XOR; break;
	  default: lose(E778, yylineno, (char*) NULL);
	  }
	  free(np->lval);
	  np->lval = op;
	  if(np->lval->rval->opcode == MESH || np->lval->rval->opcode == MESH32)
	    np->lval->rval->constant >>= 1;
	  else np->lval->rval->opcode = np->lval->rval->opcode == C_AND ?
		 C_RSHIFT : C_AND;
	  if(np->opcode == C_LSHIFT&&(np->lval->lval->opcode == C_AND ||
				      np->lval->lval->opcode == C_RSHIFT ||
				      np->lval->lval->opcode == MESH ||
				      np->lval->lval->opcode == MESH32))
	  {
	    /* Instead of doing the calculation on the evenbits, do it on the
	       oddbits; that'll save a bitshift (possibly even save two
	       bitshifts). */
	    np->opcode = C_AND;
	    if(np->lval->lval->opcode == MESH ||
	       np->lval->lval->opcode == MESH32) np->lval->lval->constant <<= 1;
	    else np->lval->lval->opcode = np->lval->lval->opcode == C_AND ?
		   C_LSHIFT : C_AND;
	    if(np->lval->rval->opcode == MESH ||
	       np->lval->rval->opcode == MESH32) np->lval->rval->constant <<= 1;
	    else np->lval->rval->opcode = np->lval->rval->opcode == C_AND ?
		   C_LSHIFT : C_AND;
	    np->rval->constant <<= 1;
	  }
	  checknodeactbits(np); /* To calculate an improved active-bits value */
	}
      }
    }
    
    /* One common situation in which the previous idiom is used is a 32-bit
       logic operation with a constant. The distributive law will not catch
       such cases because even bits of the constant are used on one side and
       odd bits of the constant are used on the other side, making the
       expressions different. In syslib, the idiom is only used with the
       constant equal to 0x80000000, leading to another problem; the logic
       operation may not have been written at all on one side of the
       expression, due to one parity set of bits having no effect. The
       following code catches three cases involving 32-bit logical operations
       with constants. Of course, some loony could theoretically write the
       16-bit version like this... */
    if(np->opcode==C_OR&&(np->lval->opcode==C_AND||np->lval->opcode==C_XOR||
			  np->lval->opcode==C_OR||np->lval->opcode==C_LSHIFT||
			  np->lval->opcode==C_RSHIFT)&&
       (np->rval->opcode==C_AND||np->rval->opcode==C_XOR||
	np->rval->opcode==C_OR||np->rval->opcode==C_LSHIFT||
	np->rval->opcode==C_RSHIFT)&&
       !(np->lval->optdata&np->rval->optdata))
    {
      op = np->lval;
      op2 = np->rval;
      /* rely on the 'constant on the right' optimization */
      if(op->opcode!=C_LSHIFT&&op->opcode!=C_RSHIFT)
	if(op->lval->opcode==C_AND || op->lval->opcode==C_LSHIFT)
	  if(op->rval->opcode==MESH || op->rval->opcode==MESH32) op=op->lval;
      if(op2->opcode!=C_LSHIFT&&op2->opcode!=C_RSHIFT)
	if(op2->lval->opcode==C_AND || op2->lval->opcode==C_LSHIFT)
	  if(op2->rval->opcode==MESH || op2->rval->opcode==MESH32) op2=op2->lval;
      if(nodessame(op->lval,op2->lval) &&
	 (op->rval->opcode==MESH||op->rval->opcode==MESH32) &&
	 (op2->rval->opcode==MESH||op2->rval->opcode==MESH32) &&
	 !(op->rval->constant&op2->rval->constant) &&
	 (op->opcode==op2->opcode) &&
	 (op->opcode==C_AND||op->opcode==C_LSHIFT||op->opcode==C_RSHIFT)&&
	 (op2->opcode==C_AND||op2->opcode==C_LSHIFT||op2->opcode==C_RSHIFT))
      {
	/* Now op and op2 are & or implied & (only <<, >>, & are at all likely
	   to be here), and have the same lval. The only check left is that
	   this does actually correspond to (something32 op constant), in
	   other words that there is exactly one possibility for op. */
	if((np->lval==op||np->rval==op2||np->lval->opcode==np->rval->opcode)
	   &&(np->lval!=op||np->rval!=op2))
	{
	  OPTING(clogopwithconst)
	  if(np->lval!=op) temp2=np->lval->opcode;
	  if(np->rval!=op2) temp2=np->rval->opcode;
	  if(np->lval==op) temp=np->rval->rval->constant;
	  else if(np->rval==op2) temp=np->lval->rval->constant;
	  else switch(temp2)
	  {
	  case C_AND:
	    temp = np->rval->rval->constant & np->lval->rval->constant; break;
	  case C_OR:
	    temp = np->rval->rval->constant | np->lval->rval->constant; break;
	  case C_XOR:
	    temp = np->rval->rval->constant ^ np->lval->rval->constant; break;
	  default: lose(E778, yylineno, (char*)NULL);
	  }
	  np->opcode = temp2;
	  np->rval->opcode = np->width==16 ? MESH : MESH32;
	  np->rval->constant = temp;
	  temp = op->rval->constant | op2->rval->constant;
	  nodefree(np->rval->lval);
	  nodefree(np->rval->rval);
	  np->rval->lval = (node*) NULL;
	  np->rval->rval = (node*) NULL;
	  if(np->lval!=op)
	  {
	    op2 = np->lval;
	    np->lval = op;
	    free(op2);
	  }
	  np->lval->rval->constant = temp;
	  if(np->lval->rval->constant>0xffff) np->lval->rval->opcode = MESH32;
	  optimize_pass1(np->lval);
	  /* it's pretty likely that there's an & with 0xffffffff here, although
	     not guaranteed, so reoptimize it */
	}
      }
    }

    /* Greater than/less than. The longest idiom in syslib, it stretches over 2
       statements. The first statement will already be optimized to
       :a <- (:b ^ :c) or similar. At this point in the code, the second
       statement looks like ((:c ~ :a) ~ ((? (:a ~ :a)) ^ 0x80000000)) or
       similar. (That's a 32-bit ?.) The idiom, obviously, also could work
       with a 16-bit ? and 0x8000. The point is, it returns the bit of :c
       that corresponds to the most significant set bit in :a, which is the
       most significant bit that differs between :b and :c. So if this bit is
       a 1, it means that :c is greater; if it's a 0, it means that :b is
       greater, or the numbers are equal. optimize_pass1 only optimizes within
       an expression, so we're going to render it as :c > (:a ^ :c). */
    if(np->opcode==SELECT&&np->lval->opcode==SELECT&&np->rval->opcode==C_XOR&&
       (np->rval->rval->opcode==MESH||np->rval->rval->opcode==MESH32)&&
       np->rval->lval->opcode==XOR&&np->rval->lval->rval->opcode==SELECT&&
       ((np->rval->lval->width==32&&np->rval->rval->constant==0x80000000LU)||
	(np->rval->lval->width==16&&np->rval->rval->constant==0x8000))&&
       nodessame(np->lval->rval,np->rval->lval->rval->lval)&&
       nodessame(np->lval->rval,np->rval->lval->rval->rval))
    {
      temp=np->rval->lval->width;
      if(temp==32||(np->lval->lval->width==16&&np->lval->rval->width==16&&
		    np->rval->lval->rval->lval->width==16&&
		    np->rval->lval->rval->rval->width==16))
      {
	OPTING(greaterthan)
	nodefree(np->rval);
	op = np->lval;
	*np=*op;
	free(op);
	np->opcode = C_XORGREATER;
	np->optdata = 1;
      }
    }

    /* Many previous optimizations generate Boolean values. In INTERCAL,
       Boolean algebra on these will be done with the familiar mingle,
       unary, select routine. However, in many cases the 'select' will be
       an implied &1, which could have been computed by many of the earlier
       optimizations. These Boolean algebra operations are caught here. */
    if((np->opcode==C_AND1ADD1||np->opcode==C_2SUBAND1||
	((np->opcode==C_AND||np->opcode==C_NNAND||np->opcode==C_LSHIFT||
	  np->opcode==C_RSHIFT||np->opcode==C_LSHIFT2||np->opcode==C_LSHIFT8||
	  np->opcode==C_LSHIFTIN1)&&
	 (np->rval->opcode==MESH||np->rval->opcode==MESH32)&&
	 np->rval->constant==1)))
    {
      if(np->opcode==C_AND1ADD1||np->opcode==C_2SUBAND1) op = np->rval;
      else op = np->lval;
      if((op->opcode==AND||op->opcode==OR||op->opcode==XOR)&&
	 op->rval->opcode==MINGLE)
      {
	OPTING(clogopand1)
	mingleop = op->rval;
	switch(op->opcode)
	{
	case AND: mingleop->opcode = C_AND; break;
	case OR: mingleop->opcode = C_OR; break;
	case XOR: mingleop->opcode = C_XOR; break;
	default: lose(E778, yylineno, (char *) NULL);
	}
	free(op);
	if(np->opcode==C_AND1ADD1||np->opcode==C_2SUBAND1)
	  np->rval = mingleop;
	else np->lval = mingleop;
      }
    }

    /* The next optimization is aided by splitting C_NNANDs when they are
       the argument to C_OR. Two cases. */

    if(np->opcode == C_OR && np->lval->opcode == C_NNAND)
    {
      OPTING(splitnnandinorl)
      op = np->lval;
      np->lval = cons(C_ISNONZERO, (node *) NULL, op);
      np->lval->width = np->lval->rval->width;
      np->lval->rval->opcode = C_AND;
      np->lval->optdata = 1;
    }
    
    if(np->opcode == C_OR && np->rval->opcode == C_NNAND)
    {
      OPTING(splitnnandinorr)
      op = np->rval;
      np->rval = cons(C_ISNONZERO, (node *) NULL, op);
      np->rval->width = np->rval->rval->width;
      np->rval->rval->opcode = C_AND;
      np->rval->optdata = 1;
    }    
    
    /* We can save a small amount of time, and aid further optimization,
       by undistributing C_ISNONZERO across C_OR. This is used in some tests
       which produce Booolean output, when testing the oddbits and
       evenbits separately. Unfortunately, this doesn't work for C_AND. */
    if(np->opcode == C_OR && np->lval->opcode == C_ISNONZERO &&
       np->rval->opcode == C_ISNONZERO)
    {
      OPTING(distribnon0overor)
      np->opcode = C_ISNONZERO;
      np->rval->opcode = C_OR;
      np->rval->lval = np->lval->rval;
      free(np->lval);
      np->lval = 0;
      optimize_pass1(np->rval); /* reoptimize this */
    }

    /* Recognize a C logical operation on just the oddbits or evenbits of a
       number. Two cases (evenbits and oddbits). */
    
    if (np->opcode==SELECT && np->rval->opcode==MESH32 &&
	np->rval->constant==0x55555555LU &&
        (np->lval->opcode==AND||np->lval->opcode==OR||np->lval->opcode==XOR) &&
	np->lval->rval->opcode == C_OR &&
	((np->lval->rval->lval->opcode == C_LSHIFT &&
	  np->lval->rval->rval->opcode == C_AND) ||
	 (np->lval->rval->lval->opcode == C_AND &&
	  np->lval->rval->rval->opcode == C_LSHIFT)) &&
	np->lval->rval->lval->rval->opcode == MESH32 &&
	np->lval->rval->rval->rval->opcode == MESH32 &&
	np->lval->rval->lval->rval->constant == 0x55555555LU &&
	np->lval->rval->rval->rval->constant == 0x55555555LU)
    {
      OPTING(clogopevenbits)
      switch(np->lval->opcode)
      {
      case AND: np->lval->rval->opcode = C_AND; break;
      case OR:  np->lval->rval->opcode = C_OR;  break;
      case XOR: np->lval->rval->opcode = C_XOR; break;
      default:  lose(E778, yylineno, (char *) NULL);
      }
      op = np->lval->rval;
      free(np->lval);
      np->lval = op;
      op = np->lval->lval->lval;
      free(np->lval->lval->rval);
      free(np->lval->lval);
      np->lval->lval = op;
      op = np->lval->rval->lval;
      free(np->lval->rval->rval);
      free(np->lval->rval);
      np->lval->rval = op;
    }
    
    if (np->opcode==SELECT && np->rval->opcode==MESH32 &&
	np->rval->constant==0x55555555LU &&
        (np->lval->opcode==AND||np->lval->opcode==OR||np->lval->opcode==XOR) &&
	np->lval->rval->opcode == C_OR &&
	((np->lval->rval->lval->opcode == C_RSHIFT &&
	  np->lval->rval->rval->opcode == C_AND) ||
	 (np->lval->rval->lval->opcode == C_AND &&
	  np->lval->rval->rval->opcode == C_RSHIFT)) &&
	np->lval->rval->lval->rval->opcode == MESH32 &&
	np->lval->rval->rval->rval->opcode == MESH32 &&
	np->lval->rval->lval->rval->constant == 0xAAAAAAAALU &&
	np->lval->rval->rval->rval->constant == 0xAAAAAAAALU)
    {
      OPTING(clogopoddbits)
      switch(np->lval->opcode)
      {
      case AND: np->lval->rval->opcode = C_AND; break;
      case OR:  np->lval->rval->opcode = C_OR;  break;
      case XOR: np->lval->rval->opcode = C_XOR; break;
      default:  lose(E778, yylineno, (char *) NULL);
      }
      op = np->lval->rval;
      free(np->lval);
      np->lval = op;
      op = np->lval->lval->lval;
      free(np->lval->lval->rval);
      free(np->lval->lval);
      np->lval->lval = op;
      op = np->lval->rval->lval;
      free(np->lval->rval->rval);
      free(np->lval->rval);
      np->lval->rval = op;
      np->rval->constant = 0xAAAAAAAALU;
    }

    /* !!(a ~ b) can be weakened to !!(a & b) */
    if(np->opcode == C_ISNONZERO && np->rval->opcode == SELECT)
    {
      OPTING(non0seltonnand)
      np->lval = np->rval->lval;
      op = np->rval->rval;
      free(np->rval);
      np->rval = op;
      np->opcode = C_NNAND;
    }
    return opted;
}

/*************************************************************************
 *
 * Code degeneration
 *
 * The theory behind this crock is that we've been handed a pointer to
 * a tuple representing a single INTERCAL statement, possibly with an
 * expression tree hanging off it and twisting slowly, slowly in the wind.
 *
 * Our mission, should we choose to accept it, is to emit C code which,
 * when linked to the INTERCAL run-time support, will do something
 * resembling the right thing.
 *
 **************************************************************************/

/*
 * If the order of statement-token defines in ick.y ever changes,
 * this will need to be reordered.
 */
char *enablers[MAXTYPES] =
{
    "GETS",
    "RESIZE",
    "NEXT",
    "GO_AHEAD", /* AIS: Added for backtracking */
    "GO_BACK",  /* AIS: Added for backtracking */
    "FORGET",
    "RESUME",
    "STASH",
    "RETRIEVE",
    "IGNORE",
    "REMEMBER",
    "ABSTAIN",
    "REINSTATE",
    "DISABLE",
    "ENABLE",
    "MANYFROM", /* AIS: Added ABSTAIN expr FROM gerunds */
    "GIVE_UP",
    "READ_OUT",
    "WRITE_IN",
    "COME_FROM",
    "COMPUCOME", /* AIS: Added COMPUCOME */
    "TRY_AGAIN", /* AIS: Added TRY AGAIN */
    "FROM", /* AIS: ABSTAIN expr FROM LABEL */
};

assoc vartypes[] =
{
    { ONESPOT,	"ONESPOT" },
    { TWOSPOT,	"TWOSPOT" },
    { TAIL,	"TAIL" },
    { HYBRID,	"HYBRID" },
    { 0,	(char *)NULL }
};

static assoc forgetbits[] =
{
    { ONESPOT,	"oneforget" },
    { TWOSPOT,	"twoforget" },
    { TAIL,	"tailforget" },
    { HYBRID,	"hyforget" },
    { 0,	(char *)NULL }
};

static assoc varstores[] =
{
    { ONESPOT,	"onespots" },
    { TWOSPOT,	"twospots" },
    { TAIL,	"tails" },
    { HYBRID,	"hybrids" },
    { 0,	(char *)NULL }
};

static assoc typedefs[] =
{
    { ONESPOT,	"type16" },
    { TWOSPOT,	"type32" },
    { TAIL,	"type16" },
    { HYBRID,	"type32" },
    { 0,	(char *)NULL }
};

char *nameof(int value, assoc table[])
/* return string corresponding to value in table */
{
    assoc	*ap;

    for (ap = table; ap->name; ap++)
	if (ap->value == value)
	    return(ap->name);
    return((char *)NULL);
}

/* AIS: Code for printing explanations (mixed C/INTERCAL code that lets
   the user know what the meaning of an expression is). This is paraphrased
   from the prexpr/prvar code lower down. It's passed to yuk so that the
   explain ('e') command works. It's also included in the degenerated C
   code when the option -c is used, so the person looking at the code can
   debug both the INTERCAL and ick itself more effectively, and used by
   -h to produce its optimizer-debug output. */

unsigned long varextern(unsigned long intern, int vartype)
{
  atom *x;
  if(!oblist) lose(E778, emitlineno, (char*) NULL);
  for (x = oblist; x < obdex; x++)
    if (x->type == vartype && x->intindex == intern)
      return(x->extindex);
  lose(E778, emitlineno, (char*) NULL);
}

static void explvar(node* np, FILE* fp)
{
  node *sp;
  switch(np->opcode)
  {
  case ONESPOT:
    (void) fprintf(fp, ".%lu", varextern(np->constant,ONESPOT));
    break;
  case TWOSPOT:
    (void) fprintf(fp, ":%lu", varextern(np->constant,TWOSPOT));
    break;
  case TAIL:
    (void) fprintf(fp, ",%lu", varextern(np->constant,TAIL));
    break;
  case HYBRID:
    (void) fprintf(fp, ";%lu", varextern(np->constant,HYBRID));
    break;
  case SUB:
    (void) fprintf(fp, "(");
    explvar(np->lval, fp);
    (void) fprintf(fp, " SUB ");
    for (sp = np->rval ; sp ; sp = sp->rval)
      explexpr(sp->lval, fp);
    (void) fprintf(fp, ")");
    break;
  default:
    lose(E778, emitlineno, (char*) NULL);
  }
}

/* unlike prexpr, this doesn't free its operands */
void explexpr(node* np, FILE* fp)
{
  if(!np) return;
  switch (np->opcode)
  {
  case MINGLE:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " $ ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case SELECT:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " ~ ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case AND:
    (void) fprintf(fp, "(& ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case OR:
    (void) fprintf(fp, "(V ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case XOR:
    (void) fprintf(fp, "(? ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case FIN:
    if (Base < 3)
      lose(E997, emitlineno, (char *)NULL);
    (void) fprintf(fp, "(^ ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case WHIRL:
  case WHIRL2:
  case WHIRL3:
  case WHIRL4:
  case WHIRL5:
    if (np->opcode - WHIRL + 3 > Base)
      lose(E997, emitlineno, (char *)NULL);
    if(np->opcode == WHIRL)
      (void) fprintf(fp, "(@ ");
    else
      (void) fprintf(fp, "(%d@ ", np->opcode - WHIRL + 1);
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case MESH:
    (void) fprintf(fp, "0x%lx", np->constant);
    break;

  case MESH32:
    (void) fprintf(fp, "0x%lx", np->constant);
    break;

  case ONESPOT:
  case TWOSPOT:
  case TAIL:
  case HYBRID:
  case SUB:
    explvar(np, fp);
    break;

    /* cases from here down are generated by the optimizer */
  case C_AND:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_OR:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " | ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_XOR:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " ^ ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;
	
  case C_NOT:
    (void) fprintf(fp, "(~ ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;	

  case C_NOTEQUAL:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " != ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_NNAND:
    (void) fprintf(fp, "(!!(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, "))");
    break;

  case C_AND1ADD1:
    (void) fprintf(fp, "((");
    explexpr(np->rval,fp);
    (void) fprintf(fp, " & 0x1) + 0x1)");
    break;

  case C_1PLUS:
    (void) fprintf(fp, "(");
    explexpr(np->rval,fp);
    (void) fprintf(fp, " + 0x1)");
    break;

  case C_2MINUS:
    (void) fprintf(fp, "(0x2 - ");
    explexpr(np->rval,fp);
    (void) fprintf(fp, ")");
    break;

  case C_2SUBAND1:
    (void) fprintf(fp, "(0x2 - (");
    explexpr(np->rval,fp);
    (void) fprintf(fp, " & 0x1))");
    break;

  case C_LSHIFT:
    (void) fprintf(fp, "((");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ") << 0x1)");
    break;

  case C_LSHIFTIN1:
    (void) fprintf(fp, "(((");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ") << 0x1) | 0x1)");
    break;

  case C_LSHIFT2:
    (void) fprintf(fp, "((");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ") << 0x2)");
    break;

  case C_LSHIFT8:
    (void) fprintf(fp, "((");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ") << 0x8)");
    break;

  case C_RSHIFTBY:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " >> ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_RSHIFT:
    (void) fprintf(fp, "((");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " & ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ") >> 0x1)");
    break;

  case C_XORGREATER:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " > (");
    explexpr(np->rval, fp);
    (void) fprintf(fp, " ^ ");
    explexpr(np->lval, fp);
    (void) fprintf(fp, "))");
    break;
      
  case C_ISNONZERO:
    (void) fprintf(fp, "(!!");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case INTERSECTION:
    explexpr(np->lval, fp);
    (void) fprintf(fp, " + ");
    explexpr(np->rval, fp);
    break;

  case GETS:
  case RESIZE:
    explexpr(np->lval, fp);
    (void) fprintf(fp, " <- ");
    explexpr(np->rval, fp);
    break;

  case BY:
    explexpr(np->lval, fp);
    (void) fprintf(fp, " BY ");
    explexpr(np->rval, fp);
    break;
    
  default: 
    lose(E778, emitlineno, (char*) NULL);
    break;
  }
}

/* AIS: Added the third argument to prexpr and prvar. It specifies
   whether the node should be freed or not. I added the third
   argument in all calls of prexpr/prvar. */

/* AIS: I moved prexpr's prototype higher in the file. */

static void prvar(node *np, FILE *fp, int freenode)
/* print out args to pass to storage manager for reference */
{
    node	*sp;
    int		dim;

    switch (np->opcode)
    {
    case ONESPOT:
	(void) fprintf(fp, "onespots[%lu]", np->constant);
	break;

    case TWOSPOT:
	(void) fprintf(fp, "twospots[%lu]", np->constant);
	break;

    case TAIL:
	(void) fprintf(fp, "TAIL, &tails[%lu]", np->constant);
	break;

    case HYBRID:
	(void) fprintf(fp, "HYBRID, &hybrids[%lu]", np->constant);
	break;

    case SUB:
	{
	  (void) fprintf(fp, "aref(");
	  prvar(np->lval, fp, freenode);

	  dim = 0;
	  for (sp = np->rval ; sp ; sp = sp->rval)
	    dim++;
	  (void) fprintf(fp, ", %d", dim);

	  for (sp = np->rval ; sp ; sp = sp->rval) {
	    (void) fprintf(fp, ", ");
	    prexpr(sp->lval, fp, freenode);
	  }
	  (void) fprintf(fp, ")");
	}
	break;
    default: /* Added by AIS */
      lose(E778, emitlineno, (char*) NULL);
      break;      
    }
}

static void prexpr(node *np, FILE *fp, int freenode)
/* print out C-function equivalent of an expression */
{
    switch (np->opcode)
    {
    case MINGLE:
	(void) fprintf(fp, "mingle(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, ", ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case SELECT:
	(void) fprintf(fp, "iselect(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, ", ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case AND:
	(void) fprintf(fp, "and%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case OR:
	(void) fprintf(fp, "or%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case XOR:
	(void) fprintf(fp, "xor%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case FIN:
	if (Base < 3)
	  lose(E997, emitlineno, (char *)NULL);
	(void) fprintf(fp, "fin%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case WHIRL:
    case WHIRL2:
    case WHIRL3:
    case WHIRL4:
    case WHIRL5:
	if (np->opcode - WHIRL + 3 > Base)
	  lose(E997, emitlineno, (char *)NULL);
	(void) fprintf(fp, "whirl%d(%d, ", np->width, np->opcode - WHIRL + 1);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case MESH:
	(void) fprintf(fp, "0x%lx", np->constant);
	break;

    case MESH32:
	(void) fprintf(fp, "0x%lx", np->constant);
	break;

    case ONESPOT:
    case TWOSPOT:
    case TAIL:
    case HYBRID:
	prvar(np, fp, freenode);
	break;

    case SUB:
	(void) fprintf(fp, "*(%s*)", nameof(np->lval->opcode, typedefs));
	prvar(np, fp, freenode);
	break;

	/* cases from here down are generated by the optimizer */
    case C_AND:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, " & ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case C_OR:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, " | ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case C_XOR:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, " ^ ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;
	
    case C_NOT:
	(void) fprintf(fp, "(~");
	prexpr(np->rval, fp, freenode);
	if (np->rval->width == Small_digits)
	    (void) fprintf(fp, " & Max_small)");
	else
	    (void) fprintf(fp, " & Max_large)");
	break;	

	/* AIS: I added the rest of the cases */
    case C_NOTEQUAL:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " != ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;
	
    case C_NNAND:
	(void) fprintf(fp, "(!!(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, " & ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, "))");
	break;

    case C_AND1ADD1:
      (void) fprintf(fp, "((");
      prexpr(np->rval,fp, freenode);
      (void) fprintf(fp, " & 0x1) + 0x1)");
      break;

    case C_1PLUS:
      (void) fprintf(fp, "(");
      prexpr(np->rval,fp, freenode);
      (void) fprintf(fp, " + 0x1)");
      break;

    case C_2MINUS:
      (void) fprintf(fp, "(0x2 - ");
      prexpr(np->rval,fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_2SUBAND1:
      (void) fprintf(fp, "(0x2 - (");
      prexpr(np->rval,fp, freenode);
      (void) fprintf(fp, " & 0x1))");
      break;

    case C_LSHIFT:
      (void) fprintf(fp, "((");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " & ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ") << 0x1)");
      break;

    case C_LSHIFTIN1:
      (void) fprintf(fp, "(((");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " & ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ") << 0x1) | 0x1)");
      break;

    case C_LSHIFT2:
      (void) fprintf(fp, "((");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " & ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ") << 0x2)");
      break;

    case C_LSHIFT8:
      (void) fprintf(fp, "((");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " & ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ") << 0x8)");
      break;

    case C_RSHIFTBY:
      (void) fprintf(fp, "((");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, ") >> ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_RSHIFT:
      (void) fprintf(fp, "((");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " & ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ") >> 0x1)");
      break;
      
    case C_XORGREATER:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " > (");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, " ^ ");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, "))");
      break;

    case C_ISNONZERO:
      (void) fprintf(fp, "(!!(");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, "))");
      break;
      
    case GETS: /* AIS: this is used only if freenode == 0 */
      if(freenode) lose(E778, emitlineno, (char*) NULL);
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " = ");
      prexpr(np->rval, fp, freenode);
      break;

    default: /* Added by AIS */
      if(!freenode) break; /* Be less careful when not freeing, because
			      this is used by -hH to print out its
			      intermediate optimization stages */
      lose(E778, emitlineno, (char*) NULL);
      break;
    }

    if(freenode) (void) free(np);
}

static char *nice_text(char *texts[], int lines)
{
#define MAXNICEBUF	512
  static char buf[MAXNICEBUF];
  char *cp, *text;
  int i;

  if (lines < 1)
    lines = 1;
  for (cp = buf, i = 0 ; i < lines ; ++i) {
    if (cp>buf+MAXNICEBUF-10) goto abort;
    if (i) {
      (*cp++) = '\\';
      (*cp++) = 'n';
      (*cp++) = '\\';
      (*cp++) = '\n';
      (*cp++) = '\t';
    }
    for (text = texts[i] ; text&&*text ; cp++, text++) {
      if (cp>buf+MAXNICEBUF-10) goto abort;
      if(*text == '"' || *text == '\\') {
	(*cp++) = '\\';
      }
      *cp = *text;
    }
  }
  *cp = '\0';
  return buf;
abort: (*cp++) = '.';
  (*cp++) = '.';
  (*cp++) = '.';
  *cp = '\0';
  return buf;
}

static void emit_guard(tuple *tn, FILE *fp)
/* emit execution guard for giiven tuple (note the unbalanced trailing {!) */
{
  if(tn->maybe) /* AIS */
  {
    if(!multithread) lose(E405, emitlineno, (char *)NULL);
    (void) fprintf(fp, "    gonebackto = setjmp(btjb); choicepoint();\n");
  }
  if(!flowoptimize || tn->abstainable) /* This condition by AIS */
  {
    (void) fprintf(fp, "    if (");
    if (tn->maybe) /* AIS */
      (void) fprintf(fp, "gonebackto == !(");
    if (tn->exechance < 100)
	(void) fprintf(fp, "roll(%d) && ", tn->exechance);
    if ((tn->type != NEXT && tn->type != GO_BACK && tn->type != COME_FROM)
	|| tn->onceagainflag == onceagain_NORMAL)
      (void) fprintf(fp, "!abstained[%d])%s {\n", (int)(tn - tuples),
		     /* AIS */ tn->maybe?")":"");
    else /* AIS: [NEXT, GO_BACK, COME_FROM] ONCE needs specially
	    handled abstentions */
      (void) fprintf(fp, "!oldabstain)%s {\n",
		     /* AIS */ tn->maybe?")":"");
  }
  else
  { /* AIS */
    if(tn->maybe) lose(E778, emitlineno, (char*) NULL);
    if(!tn->initabstain)
    {
      if(tn->type != COMPUCOME) (void) fprintf(fp, "    {\n");
      else (void) fprintf(fp, "    if(1) {\n"); /* COMPUCOME specifically needs
						   an if() so it can have
						   an else. */
    }
    else (void) fprintf(fp, "    if(0) {\n"); /* for exceptional cases like
						 DON'T COME FROM #1 where we
						 need a label or an else. */
  }
}

void emittextlines(FILE *fp)
{
  int i=-1;
  while(i++<yylineno)
  {
    (void) fprintf(fp, "\"%s\",\n",nice_text(textlines + i, 0));
  }
}

void emit(tuple *tn, FILE *fp)
/* emit code for the given tuple */
{
    node *np, *sp;
    int	dim;
    int generatecfjump=0; /* AIS */
    static int pasttryagain=0; /* AIS */

    /* grind out label and source dump */
    if (yydebug || compile_only)
	(void) fprintf(fp, "    /* line %03d */\n", tn->lineno);
    if (tn->label)
	(void) fprintf(fp, "L%d:", tn->label);
    if (yydebug || compile_only)
    {
      (void) fprintf(fp, "\t/* %s */", textlines[tn->lineno]);
      /* AIS: grind out an expression explanation */
      if (tn->type == GETS || tn->type == FORGET || tn->type == RESUME
	  || tn->type == FROM || tn->type == COMPUCOME
	  || tn->type == MANYFROM)
      {
	(void) fprintf(fp, "\n\t/* Expression is ");
	explexpr(tn->type == MANYFROM ? tn->u.node->lval :
		 tn->type == GETS ? tn->u.node->rval : tn->u.node, fp);
	(void) fprintf(fp, " */");
      }
    }
    (void) fputc('\n', fp);

    /* set up the "next" lexical line number for error messages */
    if (tn->type == NEXT) {
	tuple *up;
	for (up = tuples; up < tuples + lineno; up++)
	    if (tn->u.target == up->label) {
		emitlineno = up->lineno;
		break;
	    }
    } else if (tn->ncomefrom)
      emitlineno = comefromsearch(tn,1); /* AIS: For multithreading.
					    Return the 1st if we're forking. */
    else if (tn < tuples + lineno - 1)
	emitlineno = tn[1].lineno;
    else
	emitlineno = yylineno;
    (void) fprintf(fp, "    lineno = %d;\n", emitlineno);

    /* print warnings on -l */
    if(checkforbugs)
    {
      if(tn->warn112) lwarn(W112, emitlineno, (char*) NULL);
      if(tn->warn128) lwarn(W128, emitlineno, (char*) NULL);
      if(tn->warn534) lwarn(W534, emitlineno, (char*) NULL);
      if(tn->warn018) lwarn(W018, emitlineno, (char*) NULL);
      if(tn->warn016) lwarn(W016, emitlineno, (char*) NULL);
      if(tn->warn276) lwarn(W276, emitlineno, (char*) NULL);
      if(tn->warn239) lwarn(W239, emitlineno, (char*) NULL);
      if(tn->warn622) lwarn(W622, emitlineno, (char*) NULL);
    }
    
    /* emit debugging information */
    if (yukdebug||yukprofile)
    {
	(void) fprintf(fp, "    YUK(%d,%d);\n",
		       (int)(tn-tuples),emitlineno);
    }

    /* AIS: The +mystery option on degenerated code causes the code to
       unexpectedly terminate after 4 billion commands are run, thus
       preventing an infinite loop. Of course, it will enhance the fun
       if we don't tell the user that. (This is necessary for the
       constant-output optimizer to work properly.) */
    (void) fprintf(fp, "    MYSTERYLINE;\n");
    
    /* AIS: If the tuple is ONCE/AGAIN flagged, we need a delayed-action
       set of its abstention status to the AGAIN-flagged status. The
       problem is that some statements, like COME FROM, need to set after
       the command has finished, and some, like NEXT, need it before the
       command has started. At the moment, only NEXT and GO_BACK have a
       ONCE/AGAIN before it, rather than after (because neither of them
       continue in the normal fashion). */
    if ((tn->type == NEXT || tn->type == GO_BACK) &&
	tn->onceagainflag != onceagain_NORMAL)
    {
      /* ONCE/AGAIN has already been swapped by perpetrate.c in the case
	 of a preabstained statement ('DO NOT'...). So if currently have
	 a ONCE, it means that being abstained is the attractive state,
	 and if we currently have an AGAIN, it means that being
         reinstated is the attractive state. Semantics with computed
	 ABSTAIN: Don't change the abstention count unless necessary, in
	 which case change it to 0 or 1. */
      fprintf(fp,"    oldabstain = abstained[%d];\n", (int)(tn - tuples));
      fprintf(fp,"    abstained[%d] = %s;\n",
	      (int)(tn - tuples),
	      tn->onceagainflag==onceagain_ONCE ? "oldabstain ? oldabstain : 1"
	      : "0");
      /* This test-and-set must be atomic. As all statements are atomic anyway
         in the current version of ick, that isn't a problem, but if anyone
	 wants to try using POSIX's multithreading features, the above two
	 lines need to be a critical section. */
    }
    
    /* in the case of COMPUCOME, we need an extra guard */
    if (tn->type == COMPUCOME)
    {
      fprintf(fp,"    if(0)\n    {\n");
      fprintf(fp,"CCF%d:\n",compucomecount++);
      fprintf(fp,"    if(skipto==");
      prexpr(tn->u.node, fp, 1);
      fprintf(fp,") {\n");
    }

    /* AIS: With this block placed here, you can't even have a comment
       after a TRY AGAIN line. Move it below the next check if this seems
       to be undesirable behaviour. */
    if(pasttryagain) /* AIS */
    {
      lose(E993, emitlineno, (char*)NULL);
    }

    if(flowoptimize && tn->initabstain && !tn->abstainable
       && tn->type != COMPUCOME && tn->type != COME_FROM &&
       tn->type != NEXT) /* AIS */
      goto skipcomment; /* Look, a comment! We can completely skip all
			   degeneration of this statement (although with
			   -c, comments will appear in the degenerated
			   code in its place). The COMPUCOME condition is
			   because it is so weird. COME_FROM and NEXT are
			   exempted so labels are generated. */
    
    /* emit conditional-execution prefixes */
    if (tn->type != COME_FROM)
	emit_guard(tn, fp);

    /* now emit the code for the statement body */
    switch(tn->type)
    {
    case GETS:
        /* Start of AIS optimization */
	np = tn->u.node;
	if(np->lval->opcode == SUB) np = np->lval;
	if(flowoptimize &&
	   (np->lval->opcode == TWOSPOT
	    || np->lval->opcode == HYBRID
	    || !(tn->u.node->lval->optdata & ~0xffffLU)))
	{
	  atom* op;
	  int ignorable = 1;
	  for(op = oblist; op < obdex; op++)
	  {
	    if(op->type == np->lval->opcode &&
	       op->intindex == np->lval->constant)
	    {
	      ignorable &= op->ignorable;
	    }
	  }
	  if(!ignorable)
	  { /* Variable can't be ignored, and expression must be in range */
	    (void) fprintf(fp,"\t");
	    prexpr(tn->u.node->lval, fp, 1);
	    (void) fprintf(fp, " = ");
	    prexpr(tn->u.node->rval, fp, 1);
	    (void) fprintf(fp, ";\n");
	    break;
	  }
	}
	/* End of AIS optimization */
	
	np = tn->u.node;
	
	if (np->lval->opcode != SUB) {
	  sp = np->lval;
	  (void) fprintf(fp,"\t(void) assign((char*)&");
	}
	else {
	  sp = np->lval->lval;
	  (void) fprintf(fp,"\t(void) assign(");
	}
	prvar(np->lval, fp, 1);
	(void) fprintf(fp,", %s", nameof(sp->opcode, vartypes));
	(void) fprintf(fp,", %s[%lu], ", nameof(sp->opcode, forgetbits),
			   sp->constant);
	prexpr(np->rval, fp, 1);
	(void) fprintf(fp,");\n");
	break;

    case RESIZE:
	np = tn->u.node;
	dim = 0;
	for (sp = np->rval; sp; sp = sp->rval)
	  dim++;
	(void) fprintf(fp, "\tresize(");
	prvar(np->lval, fp, 1);
	(void) fprintf(fp, ", %s[%lu]", nameof(np->lval->opcode, forgetbits),
		       np->lval->constant);
	(void) fprintf(fp, ", %d", dim);
	for (sp = np->rval; sp; sp = sp->rval) {
	  (void) fprintf(fp, ", ");
	  prexpr(sp->lval, fp, 1);
        }
	(void) fprintf(fp, ");\n");
	break;

    case NEXT:
      /* Start of AIS optimization */
      if(tn->optversion)
      { /* optimizef has checked that this is a valid optimization */
	(void) fprintf(fp, "\tif(1 == ");
	prexpr(tn->u.node, fp, 1); /* frees optimizef's nodecopy */
	(void) fprintf(fp, ") {pushnext(%d); goto N%d;} N%d:;\n",
		       (int)(tn - tuples + 1), tn->nexttarget, (int)(tn - tuples + 1));
	break;
      }
      /* End of AIS optimization */
	(void) fprintf(fp,
		       "\tpushnext(%d); goto L%d; N%d:;\n",
		       (int)(tn - tuples + 1), tn->u.target, (int)(tn - tuples + 1));
	break;

    case GO_BACK: /* By AIS */
      if(!multithread) lose(E405, emitlineno, (char*) NULL);
      (void) fprintf(fp, "\tchoiceback();\n");
      break;

    case GO_AHEAD: /* By AIS */
      if(!multithread) lose(E405, emitlineno, (char*) NULL);
      (void) fprintf(fp, "\tchoiceahead();\n");
      break;

    case RESUME:
	(void) fprintf(fp, "\tskipto = resume(");
	prexpr(tn->u.node, fp, 1);
	(void) fprintf(fp, "); goto top;\n");
	break;

    case FORGET:
	(void) fprintf(fp, "\tpopnext(");
	prexpr(tn->u.node, fp, 1);
	(void) fprintf(fp, ");\n");
	break;

    case STASH:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\tstash(%s, %lu, %s+%lu);\n",
			  nameof(np->opcode, vartypes),
			  np->constant,
			  nameof(np->opcode, varstores), np->constant);
	break;

    case RETRIEVE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\tretrieve(%s+%lu, %s, %lu, %s[%lu]);\n",
			   nameof(np->opcode, varstores), np->constant,
			   nameof(np->opcode, vartypes),
			   np->constant,
			   nameof(np->opcode, forgetbits),
			   np->constant);
	break;

    case IGNORE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t%s[%lu] = TRUE;\n",
		       nameof(np->opcode, forgetbits),
		       np->constant);
	break;

    case REMEMBER:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t%s[%lu] = FALSE;\n",
		       nameof(np->opcode, forgetbits),
		       np->constant);
	break;

	/* All abstention code has been edited by AIS to allow for the new abstention rules */
    case ABSTAIN:
	(void) fprintf(fp, "\tif(!abstained[%d]) abstained[%d] = 1;\n", tn->u.target - 1, tn->u.target-1);
	break;

    case FROM:
        (void) fprintf(fp, "\tabstained[%d]+=", tn->u.target-1);
	tn->u.node->width = 32;
	prexpr(tn->u.node,fp, 1);
	(void) fprintf(fp, ";\n");
	break;

    case REINSTATE:
	(void) fprintf(fp, "\tif(abstained[%d]) abstained[%d]--;\n", tn->u.target - 1, tn->u.target-1);
	break;

    case ENABLE:
        (void) fprintf(fp,
		       "\tint i;\n");
	for (np = tn->u.node; np; np = np->rval)
	{
	    (void) fprintf(fp,
			   "\n\tfor (i = 0; i < (int)(sizeof(linetype)/sizeof(int)); i++)\n");
	    if (np->constant == ABSTAIN) {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s || linetype[i] == %s || linetype[i] == %s || linetype[i] == %s)\n", enablers[np->constant-GETS], enablers[np->constant-GETS+2], enablers[FROM-GETS], enablers[MANYFROM-GETS]);
	    } else if (np->constant == REINSTATE) {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s || linetype[i] == %s)\n", enablers[np->constant-GETS], enablers[np->constant-GETS+2]);
	    } else {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s)\n", enablers[np->constant-GETS]);
	    }
	    (void) fprintf(fp,
			   "\t\tif(abstained[i]) abstained[i]--;\n");
	}
	break;

    case DISABLE:
        (void) fprintf(fp,
		       "\tint i;\n");
	for (np = tn->u.node; np; np = np->rval)
	{
	    (void) fprintf(fp,
			   "\n\tfor (i = 0; i < (int)(sizeof(linetype)/sizeof(int)); i++)\n");
	    if (np->constant == ABSTAIN) {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s || linetype[i] == %s || linetype[i] == %s || linetype[i] == %s)\n", enablers[np->constant-GETS], enablers[np->constant-GETS+2], enablers[FROM-GETS], enablers[MANYFROM-GETS]);
	    } else if (np->constant == REINSTATE) {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s || linetype[i] == %s)\n", enablers[np->constant-GETS], enablers[np->constant-GETS+2]);
	    } else {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s)\n", enablers[np->constant-GETS]);
	    }
	    (void) fprintf(fp,
			   "\t\tif(!abstained[i]) abstained[i] = 1;\n");
	}
	break;

    case MANYFROM:
        (void) fprintf(fp,
		       "\tint i;\n\tint j;\n\tj=");
	tn->u.node->lval->width=32;
	prexpr(tn->u.node->lval,fp, 1);
	(void) fprintf(fp,
		       ";\n");
	for (np = tn->u.node->rval; np; np = np->rval)
	{
	    (void) fprintf(fp,
			   "\n\tfor (i = 0; i < (int)(sizeof(linetype)/sizeof(int)); i++)\n");
	    if (np->constant == ABSTAIN) {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s || linetype[i] == %s || linetype[i] == %s || linetype[i] == %s)\n", enablers[np->constant-GETS], enablers[np->constant-GETS+2], enablers[FROM-GETS], enablers[MANYFROM-GETS]);
	    } else if (np->constant == REINSTATE) {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s || linetype[i] == %s)\n", enablers[np->constant-GETS], enablers[np->constant-GETS+2]);
	    } else {
	      (void) fprintf(fp,
			     "\t    if (linetype[i] == %s)\n", enablers[np->constant-GETS]);
	    }
	    (void) fprintf(fp,
			   "\t\tabstained[i] += j;\n");
	}
	break;

    case COMPUCOME: /* By AIS. Note that this doesn't even have balanced
		       braces; it's designed to work with COMPUCOME's
		       crazy guarding arrangements */
      fprintf(fp,"\t;} else goto CCF%d;\n",compucomecount);
      break;

    case GIVE_UP: /* AIS: Edited to allow for yuk */
      if(yukprofile||yukdebug) fprintf(fp, "\tYUKTERM;\n");
      if(multithread) fprintf(fp, "\tkillthread();\n");
      (void) fprintf(fp, "\treturn(0);\n");
      break;

    case TRY_AGAIN: /* By AIS */
      (void) fprintf(fp, "\tgoto ick_restart;\n    }\n");
      if(yukprofile||yukdebug) fprintf(fp, "    if(yukloop) goto ick_restart;\n");
      if(yukprofile||yukdebug) fprintf(fp, "    YUKTERM;\n");
      if(multithread) fprintf(fp, "\tkillthread();\n");
      (void) fprintf(fp, "    {\n\treturn(0);\n");
      /* because if TRY AGAIN is the last line, falling off the end isn't an error */
      pasttryagain=1; /* flag an error if we try any more commands */
      break;

    case WRITE_IN:
	for (np = tn->u.node; np; np = np->rval) {
	  if (np->lval->opcode == TAIL || np->lval->opcode == HYBRID) {
	    (void) fprintf(fp,"\tbinin(");
	    prvar(np->lval, fp, 1);
	    (void) fprintf(fp, ", %s[%lu]",
			   nameof(np->lval->opcode, forgetbits),
			   np->lval->constant);
	    (void) fprintf(fp,");\n");
	  }
	  else {
	    if (np->lval->opcode != SUB) {
	      sp = np->lval;
	      (void) fprintf(fp,"\t(void) assign((char*)&");
	    }
	    else {
	      sp = np->lval->lval;
	      (void) fprintf(fp,"\t(void) assign(");
	    }
	    prvar(np->lval, fp, 1);
	    (void) fprintf(fp,", %s", nameof(sp->opcode, vartypes));
	    (void) fprintf(fp,", %s[%lu]", nameof(sp->opcode, forgetbits),
			   sp->constant);
	    (void) fprintf(fp,", pin());\n");
	  }
	}
	break;

    case READ_OUT:
	for (np = tn->u.node; np; np = np->rval)
	{
	  if (np->lval->opcode == TAIL || np->lval->opcode == HYBRID) {
	    (void) fprintf(fp,"\tbinout(");
	    prvar(np->lval, fp, 1);
	    (void) fprintf(fp,");\n");
	  }
	  else {
	    (void) fprintf(fp, "\tpout(");
	    prexpr(np->lval, fp, 1);
	    (void) fprintf(fp, ");\n");
	  }
	}
	break;

    case SPLATTERED:
	dim = emitlineno - tn->lineno;
	if (tn->sharedline)
	    ++dim;
	(void) fprintf(fp, "\tlose(E000, %d, \"%s\");\n",
		       emitlineno, nice_text(textlines + tn->lineno, dim));
	break;

    case COME_FROM:
	(void) fprintf(fp, "C%ld:\n", (long)(tn-tuples+1));
	/* AIS: Changed so all COME_FROMs have unique labels even if two
	   of them aim at the same line. */
	break;

    default:
	lose(E778, emitlineno, (char *)NULL);
	break;
    }

    if (tn->type != COME_FROM)
	(void) fprintf(fp, "    }\n");

 skipcomment:
    
    if (tn->type == COMPUCOME)
    { /* By AIS */
      (void) fprintf(fp,"    else goto CCF%d;\n",compucomecount);
      (void) fprintf(fp,"    ccfc++;\n");
      /* Note that due to the semantics of setjmp, this has to be written as 2
	 separate ifs. The MULTICOME macro expands to a non-multithreaded or
         multithreaded function for handling a COME FROM clash. */
      (void) fprintf(fp,"    if(ccfc==1||MULTICOME(%d,cjb))\n"
		     "\tif(setjmp(cjb) == 0) goto CCF%d;\n",
		     emitlineno, compucomecount);
      /* Of course, emitlineno is unlikely to be helpful! */
      (void) fprintf(fp,"    }\n");
    }

    /* AIS: Before any COMING FROM this line is done, we need to sort out
       ONCE and AGAIN situations, unless this line was a NEXT or GO_BACK.
       COME FROM is also excluded because it acts at the suckpoint, not
       at the place it's written in the program. */
    if (tn->onceagainflag != onceagain_NORMAL &&
	(tn->type != NEXT && tn->type != GO_BACK && tn->type != COME_FROM))
    {
      /* See my comments against the NEXT code for more information.
	 This code is placed here so COME FROM ... ONCE constructs work
	 properly (the line is abstained if the COME FROM is reached in
	 execution, or its suckpoint is reached in execution). */
      fprintf(fp,"    oldabstain = abstained[%d];\n", (int)(tn - tuples));
      fprintf(fp,"    abstained[%d] = %s;\n",
	      (int)(tn - tuples),
	      tn->onceagainflag==onceagain_ONCE ? "oldabstain ? oldabstain : 1"
	      : "0");
    }

    /* AIS: This is where we start the COME FROM suckpoint code. */

    /* AIS: We need to keep track of how many COME FROMs are aiming here
       at runtime, if we don't have the very simple situation of no
       COMPUCOMEs and a single-thread program (in which case the check
       is done at compile-time by codecheck). Even without COMPUCOME,
       this can change in a multithread program due to abstentions. */
    if((tn->ncomefrom && multithread) || (tn->label && compucomesused))
      (void) fprintf(fp, "    ccfc = 0;\n");

    /*
     * If the statement that was just degenerated was a COME FROM target,
     * emit the code for the jump to the COME FROM.
     * AIS: Changed most of this to allow for multithreading.
     */
    while(tn->ncomefrom) /* acts as an if if singlethreading */
    {
      tuple* cf; /* local to this block */
      if(multithread || compucomesused) generatecfjump = 1;      
      cf = tuples+comefromsearch(tn,tn->ncomefrom)-1;
      if (yydebug || compile_only)
	(void) fprintf(fp,
		       "    /* line %03d is a suck point for the COME FROM "
		       "at line %03d */\n", tn->lineno, cf->lineno);
      if (cf->onceagainflag != onceagain_NORMAL)
      { /* Calculate ONCE/AGAIN when the suckpoint is passed */
	fprintf(fp,"    oldabstain = abstained[%d];\n", (int)(cf - tuples));
	fprintf(fp,"    abstained[%d] = %s;\n",
		(int)(cf - tuples),
		cf->onceagainflag==onceagain_ONCE ? "oldabstain ? oldabstain : 1"
		: "0");
      }      
      emit_guard(cf, fp);
      if(multithread || compucomesused)
	(void) fprintf(fp,
		       "\tccfc++;\tif(ccfc==1||MULTICOME(%d,cjb)) "
		       "if(setjmp(cjb) == 1) goto C%ld;\n    }\n",
		       emitlineno, (long)(cf-tuples+1));
      else /* optimize for the simple case */
	(void) fprintf(fp, "\tgoto C%ld;\n    }\n", (long)(cf-tuples+1));
      tn->ncomefrom--;
    }

    /* AIS: If the statement has a label, it might be a
       computed COME FROM target. Also check the flag that says this
       code is needed in a multithread non-COMPUCOME program.
       If (at runtime) ccfc is nonzero, we know cjb has already been set;
       otherwise, set it now. In the case of a multithread non-COMPUCOME
       program, the goto will just jump to a longjmp, switching to the
       one and only one COME FROM that hasn't been given its own thread. */
    if ((tn->label && compucomesused) || generatecfjump)
    {
      if(compucomesused) (void) fprintf(fp, "    skipto = %d;\n", tn->label);
      if(generatecfjump) (void) fprintf(fp, "    if(ccfc) goto CCF0;\n");
      if(compucomesused)
      { /* check all the COMPUCOMES */
	(void) fprintf(fp, "    %sif(setjmp(cjb) == 0) goto CCF0;\n",
		       generatecfjump?"else ":"");
      }
      generatecfjump = 0;
    }

    /* AIS: Now we've finished the statement, let's switch to the next
       thread in a multithread program. */
    if(multithread) (void) fputs("    NEXTTHREAD;\n", fp);
}

/* feh.c ends here */
