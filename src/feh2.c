/****************************************************************************

Name
   feh2.c -- code-generator back-end for ick parser

DESCRIPTION
   This module provides storage manglement and code degeneration
   for the INTERCAL compiler. Optimizations (formerly in this file)
   were split into dekludge.c.

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
#include <assert.h>
#include "sizes.h"
#include "ick.h"
#include "parser.h"
#include "fiddle.h"
#include "ick_lose.h"
#include "feh.h"

/* AIS: Destaticed for dekludge.c */
int emitlineno;   /* line number for errors encountered during emit */

/*@-exportlocal@*/ /* the parser uses this */
bool mark112 = 0; /* AIS: Mark the ick_next generated tuple for W112 */
/*@=exportlocal@*/

/* AIS: From perpet.c */
extern bool pickcompile;
extern bool ick_clcsemantics;

/*************************************************************************
 *
 * Node allocation functions.
 *
 * Nodes are used to represent expression trees. The emit() function
 * deallocates them.
 *
 **************************************************************************/

/*@partial@*/ node *newnode(void)
/* allocate and zero out a new expression node */
{
  node* temp;
  temp=calloc(sizeof(node), 1);
  if(!temp) ick_lose(IE345, 0, (const char*) NULL);
  return temp;
}

/*@partial@*/ node *cons(int type, /*@null@*/ /*@keep@*/ node *car, /*@null@*/ /*@keep@*/ node *cdr)
{
    node *np = newnode();

    np->opcode = type;
    np->lval = car;
    np->rval = cdr;

    return(np);
}

/*************************************************************************
 *
 * Variable-name mapping
 *
 * This permits us to optimize use of variable storage at runtime
 *
 **************************************************************************/

unsigned long intern(int type, unsigned long index)
{
    atom	*x;

    /* AIS: Allow use of a modifiable constant 0 or >65535. */
    if ((index < 1LU || index > 65535LU) && type!=MESH)
	ick_lose(IE200, iyylineno, (const char *)NULL);

    /*@-branchstate@*/
    if (!oblist)
    {
	/* initialize oblist and obdex */
	oblist = malloc(ALLOC_CHUNK * sizeof(atom));
	if (!oblist)
	    ick_lose(IE345, iyylineno, (const char *)NULL);
	obdex = oblist;
	obcount = ALLOC_CHUNK;
    }
    else
    {
      /* if it's already on the oblist, return its intindex */
      for (x = oblist; x < obdex; x++)
	if (x->type == type && x->extindex == index)
	  return(x->intindex);
    }
    /*@=branchstate@*/
    assert(oblist != NULL);

    /* else we must intern a new symbol */
    /* AIS: Splint doesn't understand what's going on here at all. Disabling
       the warnings; I've checked this and think it's correct. */
    /*@-usedef@*/ /*@-usereleased@*/ /*@-branchstate@*/
    if (obdex >= oblist + obcount)
    {
	obcount += ALLOC_CHUNK;
	x = realloc(oblist, obcount * sizeof(atom));
	if (!x)
	    ick_lose(IE333, iyylineno, (const char *)NULL);
	obdex = x + (obdex - oblist);
	oblist = x;
    }
    /*@=branchstate@*/ /*@=usereleased@*/ /*@=usedef@*/
    obdex->type = type;
    obdex->extindex = index;
    obdex->memloc = 0; /* AIS: not placed in memory yet */
    if (type == ick_ONESPOT)
      obdex->intindex = (unsigned)nonespots++;
    if (type == ick_TWOSPOT)
      obdex->intindex = (unsigned)ntwospots++;
    if (type == ick_TAIL)
      obdex->intindex = (unsigned)ntails++;
    if (type == ick_HYBRID)
      obdex->intindex = (unsigned)nhybrids++;
    if (type == MESH) /* AIS: count meshes too */
      obdex->intindex = (unsigned)nmeshes++;
    ++obdex;

    /*@-usedef@*/
    return(obdex[-1].intindex);
    /*@=usedef@*/
}

/*************************************************************************
 *
 * This function insures a label is valid.
 *
 **************************************************************************/

/* AIS: I haven't modified this function, but I have repurposed it without
   changing the code; this function must not now have side effects (apart from
   an error exit), because some labels are initialised in the preprocessor
   without causing this. */
void checklabel(int label)
{
    if (label < 1 || label > 65535)
	ick_lose(IE197, iyylineno, (const char *)NULL);
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
  for (tp = tuples; tp < tuples + ick_lineno; tp++)
  {
    if((tp->type == COME_FROM || tp->type == NEXTFROMLABEL)
	&& tp->u.target == (unsigned)(tn-tuples+1))
      index--;
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
      /* AIS: Splint doesn't understand lazy allocation, which is why it
	 thinks I'm treating an unqualified as an only (I am, but a lazy
	 list doesn't fit any of Splint's storage classes); also, I am
	 completely destroying the tuples, because any nodes in them ought
	 to have been deallocated in prexpr. */
      /*@-unqualifiedtrans@*/ /*@-compdestroy@*/
      free(tuples);
      tuples = NULL;
      /*@=unqualifiedtrans@*/ /*@=compdestroy@*/
    }
    nmeshes = nonespots = ntwospots = ntails = nhybrids = 0;
    obdex = oblist;
    ick_lineno = 0;
    /* AIS: It's easier to mark tuples as 'always allocated', because
       it usually is, and just supress the warnings. Maybe the 'proper'
       way to do it would be to assert that tuples was non-null everywhere,
       but again this is just problems with Splint not understanding how
       lazy allocation works. So I tell Splint that it's allocated everywhere
       and just supress the warnings it produces when it isn't. */
    /*@-globstate@*/
}
/*@=globstate@*/

/*@out@*/ /*@dependent@*/ tuple *newtuple(void)
/* allocate and zero out a new expression tuple */
{
  /* Patch by Joris Huizer: must leave at least 1 tuple empty */
  if (ick_lineno >= tuplecount - 1 || tuples == NULL)
  {
    tuplecount += ALLOC_CHUNK;
    if (tuples)
	tuples = (tuple *)realloc(tuples, tuplecount * sizeof(tuple));
    else
	tuples = (tuple *)malloc(tuplecount * sizeof(tuple));
    if (!tuples)
      ick_lose(IE666, iyylineno, (const char *)NULL);
    memset(tuples + ick_lineno, 0, (tuplecount - ick_lineno) * sizeof(tuple));
  }
  if(mark112) tuples[ick_lineno].warn112 = true; mark112 = false; /* AIS */
  /* Yes, tuples is strictly speaking 'partial' at this point, but it's going
     to be filled in later, and isn't marked as partial due to it not being
     partial through most of the code, and you can't write out on a global.
     So instead I'm just suppressing the warning, because it doesn't lead to
     a problem long-term. */
  /*@-compdef@*/
  return(tuples + ick_lineno++);
  /*@=compdef@*/
}

void tupleswap(int distback1, int distback2)
{
  tuple temp;
  memcpy(&temp, &tuples[ick_lineno-distback1], sizeof(tuple));
  memcpy(&tuples[ick_lineno-distback1], &tuples[ick_lineno-distback2], sizeof(tuple));
  memcpy(&tuples[ick_lineno-distback2], &temp, sizeof(tuple));
  /* Splint doesn't understand memcpy, and so falsely things this is a memory leak. */
  /*@-compdestroy@*/
}
/*@=compdestroy@*/

void ppinit(int tuplecount)
{
  while(tuplecount)
  {
    /* 0 is an impossible exechance; make sure it's set for tuple elements. */
    if(!tuples[ick_lineno-tuplecount].exechance)
      tuples[ick_lineno-tuplecount].exechance=100;
    /* The onceagainflag also needs to be set. */
    tuples[ick_lineno-tuplecount].onceagainflag=onceagain_NORMAL;
    tuplecount--;
  }
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
    /*@-nullderef@*/ /* AIS: because the opcode defines whether lval or rval are nonnull */
    if (np->opcode == MESH || np->opcode == ick_ONESPOT || np->opcode == ick_TAIL)
	np->width = 16;
    else if (np->opcode == ick_TWOSPOT || np->opcode == ick_HYBRID
	     || np->opcode == MINGLE || np->opcode == MESH32
	     || np->opcode == UNKNOWNOP /* AIS */)
	np->width = 32;
    else if (np->opcode == AND || np->opcode == OR || np->opcode == XOR ||
	     np->opcode == FIN ||
	     (np->opcode >= WHIRL && np->opcode <= WHIRL5))
	np->width = np->rval->width;
    else if (np->opcode == SELECT)
	np->width = np->rval->width;	/* n-bit select has an n-bit result */
    else if (np->opcode == INTERSECTION) /* AIS */
        np->width = (np->rval ?
		     np->lval ? np->rval->width == 16 ? np->lval->width : 32 :
		     np->rval->width : np->lval ? np->lval->width : 32);
    else if (np->opcode == BADCHAR) /* AIS */
        np->width = 16;
    else if (np->opcode == SUB)
	np->width = np->lval->width;	/* type of the array */
    else if (np->opcode == SLAT || np->opcode == BACKSLAT)
      np->width = np->lval->width; /* AIS: \ and / return their left arg */
    /*@=nullderef@*/
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
    for (tp = tuples; tp < tuples + ick_lineno; tp++)
	if (tp->type == GETS)
	    if (tp->u.node->lval->width == 16 && tp->u.node->rval->width == 32)
		ick_lose(IE275, tp - tuples + 1, (const char *)NULL);
*/

    /* check for duplicate labels */
    for (tp = tuples; tp < tuples + ick_lineno; tp++)
	if (tp->label)
	    for (up = tuples; up < tuples + ick_lineno; up++)
		if (tp != up && tp->label == up->label)
		  ick_lose(IE182, tp - tuples + 1, (const char *)NULL);

    /*
     * Check that every NEXT, ABSTAIN, REINSTATE and COME_FROM actually has a
     * legitimate target label.
     */
    notpast1900 = 1;
    for (tp = tuples; tp < tuples + ick_lineno; tp++)
    {
      if (tp->label == 1900) notpast1900 = false; /* AIS */
        if (tp->type == NEXT
	    || tp->type == ABSTAIN || tp->type == REINSTATE
	    || tp->type == COME_FROM || tp->type == FROM
	    || tp->type == NEXTFROMLABEL) /* AIS: added FROM, NEXTFROMLABEL. */
	{
	    bool	foundit = false;

	    if (tp->u.target >= 1900 && tp->u.target <= 1998)
	    {
	      /* AIS: This program uses syslib.i's random number feature... or are
	         we in syslib already? */
	      if(notpast1900) coopt = 0;
	    }

	    if (tp->u.target > 65535 && !tp->preproc) /* AIS */
	      ick_lose(IE197, tp - tuples + 1, (const char*) NULL);

	    for (up = tuples; up < tuples + ick_lineno; up++)
		if (tp->u.target == up->label)
		{
		    foundit = true;
		    break;
		}

	    if (!foundit)
	    {
	      /* AIS: Added the pickcompile check. Syslib has to be optimized
		 for PICs, so syslib.i isn't imported and so none of the lables
		 in it will appear in the program. Also added the useickec
		 check, as that's another legitimate way for a NEXT to target
		 a nonexistent line label */
		if (tp->type == NEXT && !useickec &&
		    (!pickcompile||tp->u.target<1000||tp->u.target>1999))
		    ick_lose(IE129, tp - tuples + 1, (const char *)NULL);
		else if (tp->type == NEXT) /* AIS */
		{tp->nexttarget=0; continue;}
		else if (useickec) /* AIS */
		  continue;
		/* AIS: NEXTFROMLABEL's basically identical to COME_FROM */
		else if (tp->type == COME_FROM || tp->type == NEXTFROMLABEL)
		    ick_lose(IE444, tp - tuples + 1, (const char *)NULL);
		else
		    ick_lose(IE139, tp - tuples + 1, (const char *)NULL);
	    }
	    /* tell the other tuple if it is a COME FROM target */
	    /* AIS: NEXTFROMLABEL again */
	    else if (tp->type == COME_FROM || tp->type == NEXTFROMLABEL)
	    {
	        if (up->ncomefrom && !multithread) /* AIS: multithread check */
		    ick_lose(IE555, iyylineno, (const char *)NULL);
		else
                    up->ncomefrom++; /* AIS: to handle multiple COME FROMs */
	    }
	    /* this substitutes line numbers for label numbers
	       AIS: COME FROM now uses this too. This changes the logic
	       slightly so that an !foundit condition would fall through,
	       but as long as ick_lose doesn't return, it's not a problem.
	       (I removed the else before the if.) */
	    if (tp->type != NEXT)
	    {
	      /* AIS: added this useickec condition. */
	      if(!useickec || (tp->type!=NEXTFROMLABEL && tp->type!=COME_FROM))
		 tp->u.target = (unsigned)(up - tuples + 1);
	    }
	    else /* AIS */
	    {
	      tp->nexttarget = (unsigned)(up - tuples + 1);
	      up->nextable = true;
	    }
	}
    }
}

/* AIS: Added the third argument to prexpr and prvar. It specifies
   whether the node should be freed or not. I added the third
   argument in all calls of prexpr/prvar. This protoype has been moved
   up through the file so it can be used earlier. Destaticed so it can
   be referenced by dekludge.c. */
void prexpr(node *np, FILE *fp, int freenode);
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
 * If the order of statement-token defines in parser.y ever changes,
 * this will need to be reordered.
 */
/*@observer@*/ const char *enablersm1[MAXTYPES+1] =
{
    "UNKNOWN", /* AIS: so comments can be ABSTAINED/REINSTATED */
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
    "PIN",
    "COME_FROM",
    "NEXTFROMLABEL", /* AIS */
    "NEXTFROMEXPR", /* AIS */
    "NEXTFROMGERUND", /* AIS */
    "COMPUCOME", /* AIS: Added COMPUCOME */
    "GERUCOME", /* AIS: This is COME FROM gerunds */
    "PREPROC", /* AIS: Nonexistent statement */
    "WHILE", /* AIS: statement WHILE statement */
    "TRY_AGAIN", /* AIS: Added TRY AGAIN */
    "CREATE", /* AIS */
    "COMPUCREATE", /* AIS */
    "FROM", /* AIS: ABSTAIN expr FROM LABEL */
};
const char** enablers = enablersm1+1;

const assoc vartypes[] =
{
    { ick_ONESPOT,	"ick_ONESPOT" },
    { ick_TWOSPOT,	"ick_TWOSPOT" },
    { ick_TAIL,	"ick_TAIL" },
    { ick_HYBRID,	"ick_HYBRID" },
    { 0,	(const char *)NULL }
};

static const assoc forgetbits[] =
{
    { ick_ONESPOT,	"ick_oneforget" },
    { ick_TWOSPOT,	"ick_twoforget" },
    { ick_TAIL,	"ick_tailforget" },
    { ick_HYBRID,	"ick_hyforget" },
    { 0,	(const char *)NULL }
};

/* AIS: Destatic. This is now needed in perpet.c. */
const assoc varstores[] =
{
    { ick_ONESPOT,	"ick_onespots" },
    { ick_TWOSPOT,	"ick_twospots" },
    { ick_TAIL,	"ick_tails" },
    { ick_HYBRID,	"ick_hybrids" },
    { 0,	(const char *)NULL }
};

/* AIS: A demangled version */
static const assoc varstoresdem[] =
{
    { ick_ONESPOT,	"onespots" },
    { ick_TWOSPOT,	"twospots" },
    { ick_TAIL,	"tails" },
    { ick_HYBRID,	"hybrids" },
    { 0,	(const char *)NULL }
};

static const assoc typedefs[] =
{
    { ick_ONESPOT,	"ick_type16" },
    { ick_TWOSPOT,	"ick_type32" },
    { ick_TAIL,	"ick_type16" },
    { ick_HYBRID,	"ick_type32" },
    { 0,	(const char *)NULL }
};

/*@observer@*/ /*@null@*/ const char *nameof(int value, const assoc table[])
/* return string corresponding to value in table */
{
    const assoc	*ap;

    for (ap = table; ap->name; ap++)
	if (ap->value == value)
	    return(ap->name);
    return((const char *)NULL);
}

/* AIS: Code for printing explanations (mixed C/INTERCAL code that lets
   the user know what the meaning of an expression is). This is paraphrased
   from the prexpr/prvar code lower down. It's passed to yuk so that the
   explain ('e') command works. It's also included in the degenerated C
   code when the option -c is used, so the person looking at the code can
   debug both the INTERCAL and ick itself more effectively, and used by
   -h to produce its optimizer-debug output, and used to produce the variable
   numbers used in ick_createdata. */

unsigned long varextern(unsigned long intern, int vartype)
{
  atom *x;
  if(!oblist) ick_lose(IE778, emitlineno, (const char*) NULL);
  for (x = oblist; x < obdex; x++)
    if (x->type == vartype && (unsigned long)x->intindex == intern)
      return(x->extindex);
  if(vartype==MESH) return 0; /* the mesh wasn't used after all */
  ick_lose(IE778, emitlineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

static void explvar(node* np, FILE* fp)
{
  node *sp;
  switch(np->opcode)
  {
  case ick_ONESPOT:
    (void) fprintf(fp, ".%lu", varextern(np->constant,ick_ONESPOT));
    break;
  case ick_TWOSPOT:
    (void) fprintf(fp, ":%lu", varextern(np->constant,ick_TWOSPOT));
    break;
  case ick_TAIL:
    (void) fprintf(fp, ",%lu", varextern(np->constant,ick_TAIL));
    break;
  case ick_HYBRID:
    (void) fprintf(fp, ";%lu", varextern(np->constant,ick_HYBRID));
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
    ick_lose(IE778, emitlineno, (const char*) NULL);
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

  case UNKNOWNOP:
    (void) fprintf(fp, "(");
    explexpr(np->rval->lval, fp);
    if(np->lval->constant < 256)
      (void) fprintf(fp, " %c ", (char)np->lval->constant);
    else
      (void) fprintf(fp, " %c^H%c ",
		     (char)(np->lval->constant / 256),
		     (char)(np->lval->constant % 256));
    explexpr(np->rval->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case SLAT:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " / ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case BACKSLAT:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " \\ ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case AND:
    (void) fprintf(fp, "(&%d ", np->width);
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case OR:
    (void) fprintf(fp, "(V%d ", np->width);
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case XOR:
    (void) fprintf(fp, "(?%d ", np->width);
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case FIN:
    if (ick_Base < 3)
      ick_lose(IE997, emitlineno, (const char *)NULL);
    (void) fprintf(fp, "(^%d ", np->width);
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case WHIRL:
  case WHIRL2:
  case WHIRL3:
  case WHIRL4:
  case WHIRL5:
    if (np->opcode - WHIRL + 3 > ick_Base)
      ick_lose(IE997, emitlineno, (const char *)NULL);
    if(np->opcode == WHIRL)
      (void) fprintf(fp, "(@%d ", np->width);
    else
      (void) fprintf(fp, "(%d@%d ", np->opcode - WHIRL + 1, np->width);
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case MESH:
    if(variableconstants) /* AIS */
      (void) fprintf(fp, "meshes[%lu]", np->constant);
    else
      (void) fprintf(fp, "0x%lx", np->constant);
    break;

  case MESH32:
    (void) fprintf(fp, "0x%lx", np->constant);
    break;

  case ick_ONESPOT:
  case ick_TWOSPOT:
  case ick_TAIL:
  case ick_HYBRID:
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
    (void) fprintf(fp, "(~%d ", np->width);
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

  case C_A:
    (void) fprintf(fp, "a");
    break;

  case C_RSHIFTBY:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " >> ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_LOGICALNOT:
    (void) fprintf(fp, "(! ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_LSHIFTBY:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " << ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_PLUS:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " + ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_MINUS:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " - ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_TIMES:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " * ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_DIVIDEBY:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " / ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_MODULUS:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " %% ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_GREATER:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " > ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_LESS:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " < ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_ISEQUAL:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " == ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_LOGICALAND:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " && ");
    explexpr(np->rval, fp);
    (void) fprintf(fp, ")");
    break;

  case C_LOGICALOR:
    (void) fprintf(fp, "(");
    explexpr(np->lval, fp);
    (void) fprintf(fp, " || ");
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
    ick_lose(IE778, emitlineno, (const char*) NULL);
    /*@-unreachable@*/ break; /*@=unreachable@*/
  }
}

/* AIS: Added the third argument to prexpr and prvar. It specifies
   whether the node should be freed or not. I added the third
   argument in all calls of prexpr/prvar. */

/* AIS: I moved prexpr's prototype higher in the file.
   Destaticed so the optimizer can access it. */

static void prvar(node *np, FILE *fp, int freenode)
/* print out args to pass to storage manager for reference */
{
    node	*sp;
    int		dim;

    switch (np->opcode)
    {
    case ick_ONESPOT:
	(void) fprintf(fp, "ick_onespots[%lu]", np->constant);
	break;

    case ick_TWOSPOT:
	(void) fprintf(fp, "ick_twospots[%lu]", np->constant);
	break;

    case ick_TAIL:
	(void) fprintf(fp, "ick_TAIL, &ick_tails[%lu]", np->constant);
	break;

    case ick_HYBRID:
	(void) fprintf(fp, "ick_HYBRID, &ick_hybrids[%lu]", np->constant);
	break;

    case SUB:
	{
	  (void) fprintf(fp, "ick_aref(");
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
      ick_lose(IE778, emitlineno, (const char*) NULL);
      /*@-unreachable@*/ break; /*@=unreachable@*/
    }
}

static void ooprvar(node *np, FILE *fp, int freenode)
/* AIS: Print out the overloaded version */
{
    node	*sp;
    int		dim;

    switch (np->opcode)
    {
    case ick_ONESPOT:
	(void) fprintf(fp, "ick_oo_onespots[%lu]", np->constant);
	break;

    case ick_TWOSPOT:
	(void) fprintf(fp, "ick_oo_twospots[%lu]", np->constant);
	break;

    case ick_TAIL:
    case ick_HYBRID:
      /* This should never be reached */
      ick_lose(IE778, emitlineno, (const char*) NULL);
      /*@-unreachable@*/ break; /*@=unreachable@*/

    case SUB:
	{
	  (void) fprintf(fp, "ick_aref(");
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
    default:
      ick_lose(IE778, emitlineno, (const char*) NULL);
      /*@-unreachable@*/ break; /*@=unreachable@*/
    }
}

/* AIS: Give us a mesh with value x */
static unsigned long meshval(unsigned long x)
{
  if(variableconstants)
    return intern(MESH, x);
  else
    return x;
}

/* AIS: This is the reverse of prexpr, in a way.
   It degenerates an expression that causes *np to become equal to *target.
   If this is impossible at any point, it degenerates code that causes
   error 277 (and itself causes error 278 if the situation is inevitable).
   As for the annotations; there quite possibly are memory allocation mistakes
   here, but just about every line is a false positive (because we're operating
   at the subobject level in terms of copy/free/allocate) for Splint, and so
   disabling the warnings doesn't make the output any less useful. (When there
   are so many false positives, disabling the true positives doesn't make them
   any harder to find by eye. */
/*@-temptrans@*/ /*@-kepttrans@*/ /*@-compdestroy@*/ /*@-branchstate@*/
 /*@-nullpass@*/
static void revprexpr(node *np, FILE *fp, node *target)
{
  node* temp;
  switch (np->opcode)
  {
  case MINGLE:
    /* We can use select to determine what np->lval and np->rval have
       to become equal to, as long as we're in base 2. */
    if(ick_Base!=2)
    {
      fprintf(fp, "  ick_lose(IE277, ick_lineno, (const char*) NULL);\n");
      ick_lwarn(W278, emitlineno, (const char*) NULL);
      break;
    }
    temp=cons(MESH,0,0);
    temp->constant=meshval(0xAAAAAAAALU);
    temp->width=32;
    temp=cons(SELECT,target,temp);
    temp->width=target->width;
    revprexpr(np->lval, fp, temp);
    free(temp->rval);
    free(temp);
    temp=cons(MESH,0,0);
    temp->constant=meshval(0x55555555LU);
    temp->width=32;
    temp=cons(SELECT,target,temp);
    temp->width=target->width;
    revprexpr(np->rval, fp, temp);
    free(temp->rval);
    free(temp);
    break;

  case SELECT:
    /* Set the left of the select to the target, and the right
       to 0xffffffff or 0xffff. This only works in base 2. */
    if(ick_Base!=2)
    {
      fprintf(fp, "  ick_lose(IE277, ick_lineno, (const char*) NULL);\n");
      ick_lwarn(W278, emitlineno, (const char*) NULL);
      break;
    }
    temp=cons(MESH,0,0);
    temp->constant=meshval(target->width==32?0xFFFFFFFFLU:0xFFFFLU);
    temp->width=target->width;
    revprexpr(np->lval, fp, target);
    revprexpr(np->rval, fp, temp);
    free(temp);
    break;

  case UNKNOWNOP: /* don't be silly */
    fprintf(fp, "  ick_lose(IE277, ick_lineno, (const char*) NULL);\n");
    ick_lwarn(W278, emitlineno, (const char*) NULL);
    break;

  case BACKSLAT:
    /* Unimplemented. This isn't even in the parser yet, so it's a
       ick_mystery how we got here. */
    ick_lose(IE778, emitlineno, (const char*) NULL);
    /*@-unreachable@*/ break; /*@=unreachable@*/

  case SLAT:
    /* We need to set the true value of the LHS... */

    /* Copied and modified from the GETS code */
    if(!pickcompile)
    {
      (void) fprintf(fp,"  (void) ick_assign((char*)&");
      prvar(np->lval, fp, 0);
      (void) fprintf(fp,", %s", nameof(np->lval->opcode, vartypes));
      (void) fprintf(fp,", %s[%lu], ", nameof(np->lval->opcode, forgetbits),
		     np->lval->constant);
      prexpr(target, fp, 0);
      (void) fprintf(fp,");  \n");
    }
    else /* AIS: Added this case for the simpler PIC assignment rules */
    {
      (void) fprintf(fp,"\t""if(ignore%s%lu) ",
		     nameof(np->lval->opcode,varstores),
		     np->lval->constant);
      prexpr(np->lval, fp, 0);
      (void) fprintf(fp, " = ");
      prexpr(target, fp, 0);
      (void) fprintf(fp, ";  \n");
    }

    /* ... and we need to cause overloading to happen. This is a copy
       of part of the code for SLAT, modified to work in this context. */
    ooprvar(np->lval, fp, 0);
    /* Do something highly non-portable with pointers that should work
       anyway. Each pointer needs to be given a unique code; so we use
       the hex representation of np casted to an unsigned long.
       Technically speaking, np->rval could be casted to anything; but
       all implementations I've ever seen cast unique pointers to unique
       numbers, which is good enough for our purposes. */
    (void) fprintf(fp, ".get=ick_og%lx;\n  ", (unsigned long)np->rval);
    ooprvar(np->lval, fp, 0);
    (void) fprintf(fp, ".set=ick_os%lx;\n", (unsigned long)np->rval);
    break;

  case AND:
  case OR:
  case XOR:
  case FIN:
  case WHIRL:
  case WHIRL2:
  case WHIRL3:
  case WHIRL4:
  case WHIRL5:
    temp=cons(np->opcode-AND+REV_AND,0,target);
    temp->width=temp->rval->width=np->width;
    revprexpr(np->rval, fp, temp);
    free(temp);
    break;

  case MESH:
    if(!variableconstants)
    {
      /* Can't set a mesh in this case */
      fprintf(fp, "  ick_lose(IE277, ick_lineno, (const char*) NULL);\n");
      ick_lwarn(W278, emitlineno, (const char*) NULL);
      break;
    }
    (void) fprintf(fp,"  meshes[%lu] = ",np->constant);
    prexpr(target, fp, 0);
    (void) fprintf(fp,";\n");
    break;

  case ick_ONESPOT:
  case ick_TWOSPOT:
  case ick_TAIL:
  case ick_HYBRID:
  case SUB:
    /* Copy the code for the GETS statement; this is almost the same
       thing. Modified because we're assigning target to np, not
       np->lval to np->rval, and to not free(). */
    if(opoverused&&
       (np->opcode==ick_ONESPOT||np->opcode==ick_TWOSPOT)) /* AIS */
    {
      (void) fprintf(fp,"  ");
      ooprvar(np, fp, 0);
      (void) fprintf(fp,".set(");
      prexpr(target, fp, 0);
      (void) fprintf(fp,",os%dspot%lu);\n",
		     ((np->opcode==ick_TWOSPOT)?1:0)+1, np->constant);
    }
    else if(!pickcompile)
    {
      node* sp;

      if (np->opcode != SUB) {
	sp = np;
	(void) fprintf(fp,"  (void) ick_assign((char*)&");
      }
      else {
	sp = np->lval;
	(void) fprintf(fp,"  (void) ick_assign(");
      }
      prvar(np, fp, 0);
      (void) fprintf(fp,", %s", nameof(sp->opcode, vartypes));
      (void) fprintf(fp,", %s[%lu], ", nameof(sp->opcode, forgetbits),
		     sp->constant);
      prexpr(target, fp, 0);
      (void) fprintf(fp,");\n");
    }
    else /* AIS: Added this case for the simpler PIC assignment rules */
    {
      (void) fprintf(fp,"  if(ignore%s%lu) ",
		     nameof(np->opcode,varstores),
		     np->constant);
      prexpr(np, fp, 0);
      (void) fprintf(fp, " = ");
      prexpr(target, fp, 0);
      (void) fprintf(fp, ";\n");
    }
    break;

    /* cases from here down are generated by the optimizer, and so
       should never come up here and are errors. The exception is
       C_A, which should only ever appear in a target expression,
       so is also an error. */
  case MESH32:
  case C_AND:
  case C_OR:
  case C_XOR:
  case C_NOT:
  case C_NOTEQUAL:
  case C_A:
  case C_RSHIFTBY:
  case C_LOGICALNOT:
  case C_LSHIFTBY:
  case C_PLUS:
  case C_MINUS:
  case C_TIMES:
  case C_DIVIDEBY:
  case C_MODULUS:
  case C_GREATER:
  case C_LESS:
  case C_ISEQUAL:
  case C_LOGICALAND:
  case C_LOGICALOR:
  case GETS: /* should never come up */
  default:
    ick_lose(IE778, emitlineno, (const char*) NULL);
    /*@-unreachable@*/ break; /*@=unreachable@*/
  }
}
/*@=temptrans@*/ /*@=kepttrans@*/ /*@=compdestroy@*/ /*@=branchstate@*/

/*@observer@*/ static char* E000string; /* AIS */

static int prunknownstr(node*, FILE*); /* AIS */

/* AIS: Destaticed */
/* Splint doesn't understand the concept of a function which might or might
   not free its argument. That's a pity, because its checking would come in
   useful here, but as it is we have to annotate memory checking off for this
   function. */
/*@-temptrans@*/ /*@-onlytrans@*/ /*@-compdestroy@*/ /*@-branchstate@*/
void prexpr(node *np, FILE *fp, int freenode)
/* print out C-function equivalent of an expression */
{
    int tempint;
    switch (np->opcode)
    {
    case MINGLE:
	(void) fprintf(fp, "ick_mingle(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, ", ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case SELECT:
	(void) fprintf(fp, "ick_iselect(");
	prexpr(np->lval, fp, freenode);
	(void) fprintf(fp, ", ");
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case UNKNOWNOP: /* AIS */
      if(!useickec || !createsused)
      {
	/* CREATEd operators require -ea */
	(void) fprintf(fp, "(ick_lose(IE000, ick_lineno, \"%s\"),0)",
		       E000string);
	break;
      }
      /* We need to do the same as UNKNOWN statements, but as an expression.
	 This is achieved with the helper function ick_dounop in ick_ec.c. */
      (void) fprintf(fp, "ick_dounop(\"");
      (void) prunknownstr(np->lval, fp);
      if(freenode) free(np->lval);
      (void) fprintf(fp, "\", ");
      prexpr(np->rval->lval, fp, 0);
      (void) fprintf(fp, ", ");
      prexpr(np->rval->rval, fp, 0);
      (void) fprintf(fp,
		     ", ick_lineno, %luUL, %luUL, %luUL"
		     ", ick_og%lx, ick_og%lx, og2spot%lu"
		     ", ick_os%lx, ick_os%lx, os2spot%lu, \"%s\")",
		     intern(ick_TWOSPOT, 1601),
		     intern(ick_TWOSPOT, 1602),
		     intern(ick_TWOSPOT, 1603),
		     (unsigned long) np->rval->lval,
		     (unsigned long) np->rval->rval,
		     intern(ick_TWOSPOT, 1603),
		     (unsigned long) np->rval->lval,
		     (unsigned long) np->rval->rval,
		     intern(ick_TWOSPOT, 1603),
		     E000string);
      if(freenode) free(np->rval);
      break;

    case BACKSLAT: /* AIS */
      /* Unimplemented. This isn't even in the parser yet, so it's a
	 ick_mystery how we got here. */
      ick_lose(IE778, emitlineno, (const char*) NULL);
      /*@-unreachable@*/ break; /*@=unreachable@*/

    case SLAT: /* AIS */
      (void) fprintf(fp,"((");
      ooprvar(np->lval, fp, 0);
      /* Do something highly non-portable with pointers that should work
	 anyway. Each pointer needs to be given a unique code; so we use
	 the hex representation of np casted to an unsigned long.
	 Technically speaking, np->rval could be casted to anything; but
	 all implementations I've ever seen cast unique pointers to unique
	 numbers, which is good enough for our purposes. */
      (void) fprintf(fp, ".get=ick_og%lx),(", (unsigned long)np->rval);
      ooprvar(np->lval, fp, 0);
      (void) fprintf(fp, ".set=ick_os%lx),(", (unsigned long)np->rval);
      prvar(np->lval, fp, freenode);
      /* np->rval will be freed later, when its expression is printed */
      (void) fprintf(fp, "))");
      return; /* mustn't be freed */

    case AND:
	(void) fprintf(fp, "ick_and%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case OR:
	(void) fprintf(fp, "ick_or%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case XOR:
	(void) fprintf(fp, "ick_xor%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case FIN:
	if (ick_Base < 3)
	  ick_lose(IE997, emitlineno, (const char *)NULL);
	(void) fprintf(fp, "ick_fin%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case WHIRL:
    case WHIRL2:
    case WHIRL3:
    case WHIRL4:
    case WHIRL5:
	if (np->opcode - WHIRL + 3 > ick_Base)
	  ick_lose(IE997, emitlineno, (const char *)NULL);
	(void) fprintf(fp, "ick_whirl%d(%d, ", np->width, np->opcode - WHIRL + 1);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    /* AIS: Reversed operations */
    case REV_AND:
	(void) fprintf(fp, "ick_rev_and%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case REV_OR:
	(void) fprintf(fp, "ick_rev_or%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case REV_XOR:
	(void) fprintf(fp, "ick_rev_xor%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case REV_FIN:
	if (ick_Base < 3)
	  ick_lose(IE997, emitlineno, (const char *)NULL);
	(void) fprintf(fp, "rev_fin%d(", np->width);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case REV_WHIRL:
    case REV_WHIRL2:
    case REV_WHIRL3:
    case REV_WHIRL4:
    case REV_WHIRL5:
	if (np->opcode - WHIRL + 3 > ick_Base)
	  ick_lose(IE997, emitlineno, (const char *)NULL);
	(void) fprintf(fp,
		       "rev_whirl%d(%d, ", np->width, np->opcode - WHIRL + 1);
	prexpr(np->rval, fp, freenode);
	(void) fprintf(fp, ")");
	break;

    case MESH:
      if(variableconstants) /* AIS */
	(void) fprintf(fp, "meshes[%lu]", np->constant);
      else
	(void) fprintf(fp, "0x%lx", np->constant);
      break;

    case MESH32:
	(void) fprintf(fp, "0x%lx", np->constant);
	break;

    case ick_ONESPOT:
    case ick_TWOSPOT:
    case ick_TAIL:
    case ick_HYBRID:
	if(!opoverused||np->opcode==ick_TAIL||np->opcode==ick_HYBRID)
	  prvar(np, fp, freenode);
	else /* AIS */
	{
	  ooprvar(np, fp, freenode);
	  fprintf(fp, ".get(");
	  prvar(np, fp, freenode);
	  fprintf(fp,")");
	}
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
        tempint=np->width; /* AIS */
	prexpr(np->rval, fp, freenode);
	if (tempint == ick_Small_digits)
	    (void) fprintf(fp, " & ick_Max_small)");
	else
	    (void) fprintf(fp, " & ick_Max_large)");
	break;

	/* AIS: I added the rest of the cases */
    case C_NOTEQUAL:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " != ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_A:
      (void) fprintf(fp, "a");
      break;

    case C_RSHIFTBY:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " >> ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_LOGICALNOT:
      (void) fprintf(fp, "(!");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_LSHIFTBY:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " << ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_PLUS:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " + ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_MINUS:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " - ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_TIMES:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " * ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_DIVIDEBY:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " / ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_MODULUS:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " %% ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_GREATER:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " > ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_LESS:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " < ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_ISEQUAL:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " == ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_LOGICALAND:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " && ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case C_LOGICALOR:
      (void) fprintf(fp, "(");
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " || ");
      prexpr(np->rval, fp, freenode);
      (void) fprintf(fp, ")");
      break;

    case GETS: /* AIS: this is used only if freenode == 0 */
      if(freenode) ick_lose(IE778, emitlineno, (const char*) NULL);
      prexpr(np->lval, fp, freenode);
      (void) fprintf(fp, " = ");
      prexpr(np->rval, fp, freenode);
      break;

    default: /* Added by AIS */
      if(!freenode) break; /* Be less careful when not freeing, because
			      this is used by -hH to print out its
			      intermediate optimization stages */
      ick_lose(IE778, emitlineno, (const char*) NULL);
      /*@-unreachable@*/ break; /*@=unreachable@*/
    }

    if(freenode) (void) free(np);
}
/*@=temptrans@*/ /*@=onlytrans@*/ /*@=compdestroy@*/ /*@=branchstate@*/

/* By AIS: Helper function for prunknown */
static int prunknownstr(node *np, FILE* fp)
{
  int i;
  switch(np->opcode)
  {
  case INTERSECTION:
    i=prunknownstr(np->lval, fp);
    i+=prunknownstr(np->rval, fp);
    return i;
  case BADCHAR:
    if (np->constant > 256)
      (void) fprintf(fp, "o%xx%x",
		     (unsigned int)(np->constant / 256),
		     (unsigned int)(np->constant % 256));
    else
      (void) fprintf(fp, "u%x", (unsigned int)np->constant);
    return 2;
  case US_ID: (void) fputc((char)np->constant, fp); return 0;
  case US_ELEM: (void) fputc(';', fp); return 1;
  case US_SCALAR: (void) fputc('.', fp); return 1;
  case US_ARRVAR: (void) fputc(',', fp); return 1;
  case US_EXPR: (void) fputc('~', fp); return 1;
  default: ick_lose(IE778, emitlineno, (const char*) NULL);
  }
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

/* By AIS: Helper function for prunknown */
static void prunknowncreatedata(node *np, FILE* fp)
{
  unsigned long ve;
  switch(np->opcode)
  {
  case INTERSECTION:
    prunknowncreatedata(np->lval, fp);
    prunknowncreatedata(np->rval, fp);
    return;
  case US_ID: return; /* nothing to do */
  case US_SCALAR:
    ve=varextern(np->rval->constant,np->rval->opcode);
    fprintf(fp,"\t\t{%d,0,%lu,",np->rval->width,ve);
    break;
  case US_ARRVAR: /* an array doesn't itself have a value */
    ve=varextern(np->rval->constant,np->rval->opcode);
    fprintf(fp,"\t\t{%d,1,%lu,{ick_ieg277,ick_ies277},0},\n",
	    np->rval->width,ve);
    return;
  case US_ELEM: /* these two cases are actually treated the same way, */
  case US_EXPR: /* because expressions can be assigned to */
    fprintf(fp,"\t\t{%d,0,0,",np->rval->width);
    break;
  default: ick_lose(IE778, emitlineno, (const char*) NULL);
  }
  if(createsused)
    fprintf(fp,"{ick_og%lx,ick_os%lx},",
	    (unsigned long)np->rval,(unsigned long)np->rval);
  else
    fprintf(fp,"{0,0},");
  prexpr(np->rval,fp,0);
  fprintf(fp,"},\n");
}

/* This function by AIS. Print a check to see if a just-in-case compiled
   statement actually has a meaning yet, or if we should error. */
static void prunknown(node *np, FILE* fp)
{
  static long negcounter=-65538;
  int i,j;
  fprintf(fp,"\tif((ick_skipto=ick_jicmatch(\"");
  i=prunknownstr(np, fp);
  fprintf(fp, "\")))\n\t{\n\t    ick_createdata icd[]={\n");
  prunknowncreatedata(np, fp);
  fprintf(fp, "\t    };\n");
  if(createsused)
  {
    j=i;
    while(j--)
      (void) fprintf(fp, "\t\t""ICKSTASH(ick_TWOSPOT, %lu, "
		     "ick_twospots, ick_oo_twospots);\n"
		     "\t\t""ick_oo_twospots[%lu]=icd[%d].accessors;\n",
		     intern(ick_TWOSPOT,(unsigned long)(1601+j)),
		     intern(ick_TWOSPOT,(unsigned long)(1601+j)),j);
  }
  if(useickec)
  {
    fprintf(fp,"\t\t""ick_global_createdata=icd;\n");
    fprintf(fp,"\t\t""ick_dogoto(ick_skipto,ick_lineno,1);\n");
  }
  else
  {
    fprintf(fp,"\t\t""ick_pushnext(%ld); ick_skipto=-ick_skipto; goto top; "
	    "case %ld:;\n",negcounter,negcounter);
    negcounter--;
  }
  if(createsused)
  {
    j=i;
    while(j--)
      (void) fprintf(fp, "\t\t""ICKRETRIEVE(ick_twospots, %lu, "
		     "ick_TWOSPOT, ick_twoforget, ick_oo_twospots);\n",
		     intern(ick_TWOSPOT,(unsigned long)(1601+j)));

  }
  fprintf(fp, "\t} else\n");
}

/*@dependent@*/ static char *nice_text(char *texts[], int lines)
{
#define MAXNICEBUF	512
  static char buf[MAXNICEBUF];
  char *cp, *text;
  int i;

  if (lines < 1)
    lines = 1;
  for (cp = buf, i = 0 ; i < lines ; ++i) {
    if (cp>buf+MAXNICEBUF-10)
    {
      (*cp++) = '.';
      (*cp++) = '.';
      (*cp++) = '.';
      *cp = '\0';
      return buf;
    }
    if (i) {
      (*cp++) = '\\';
      (*cp++) = 'n';
      (*cp++) = '\\';
      (*cp++) = '\n';
      (*cp++) = '\t';
    }
    for (text = texts[i] ; text != NULL && *text != '\0'; cp++, text++) {
      if (cp>buf+MAXNICEBUF-10)
      {
	(*cp++) = '.';
	(*cp++) = '.';
	(*cp++) = '.';
	*cp = '\0';
	return buf;
      }
      if(*text == '"' || *text == '\\') {
	(*cp++) = '\\';
      }
      if(*text == 'K') /* AIS: break the string so that the ick_ec
			  preprocessor doesn't trigger on the string
			  ICKNUMBERPAIR */
      {
	(*cp++) = '"';
	(*cp++) = '"';
      }
      *cp = *text;
    }
  }
  *cp = '\0';
  return buf;
}

static void emit_guard(tuple *tn, FILE *fp)
/* emit execution guard for giiven tuple (note the unbalanced trailing {!) */
{
  if(tn->maybe) /* AIS */
  {
    if(!multithread) ick_lose(IE405, emitlineno, (const char *)NULL);
    (void) fprintf(fp, "    gonebackto = setjmp(btjb); choicepoint();\n");
  }
  if(!flowoptimize || tn->abstainable) /* This condition by AIS */
  {
    (void) fprintf(fp, "    if (");
    if (tn->maybe) /* AIS */
      (void) fprintf(fp, "gonebackto == !(");
    if (tn->exechance < 100)
	(void) fprintf(fp, "ick_roll(%d) && ", tn->exechance);
    if ((tn->type != NEXT && tn->type != GO_BACK && tn->type != COME_FROM
	 && /* AIS */ tn->type != NEXTFROMLABEL && tn->type != UNKNOWN)
	|| tn->onceagainflag == onceagain_NORMAL)
      (void) fprintf(fp, "!ICKABSTAINED(%d))%s {\n", (int)(tn - tuples),
		     /* AIS */ tn->maybe?")":"");
    else /* AIS: [NEXT, GO_BACK, COME_FROM] ONCE needs specially
	    handled abstentions */
      (void) fprintf(fp, "!ick_oldabstain)%s {\n",
		     /* AIS */ tn->maybe?")":"");
  }
  else
  { /* AIS */
    if(tn->maybe) ick_lose(IE778, emitlineno, (const char*) NULL);
    if(!tn->initabstain)
    {
      if(tn->type != COMPUCOME && tn->type != GERUCOME
	 && tn->type != NEXTFROMEXPR && tn->type != NEXTFROMGERUND)
	(void) fprintf(fp, "    {\n");
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
  int i=0; /* The first textline is line 1 */
  (void) fprintf(fp, "\"\",\n");
  while(++i<iyylineno)
  {
    (void) fprintf(fp, "\"%s\",\n",nice_text(textlines + i, 0));
  }
  (void) fprintf(fp, "\"\"\n");
}

/*@-nestedextern@*/
void emit(tuple *tn, FILE *fp)
/* emit code for the given tuple */
{
    node *np, *sp;
    int	dim;
    int generatecfjump=0; /* AIS */
    static int pasttryagain=0; /* AIS */
    extern bool cdebug; /* AIS */

    /* grind out label and source dump */
    if (yydebug || cdebug || compile_only)
	(void) fprintf(fp, "    /* line %03d */\n", tn->ick_lineno);
    if (tn->label)
    {
      if(!useickec) /* AIS */
	(void) fprintf(fp, "case -%u: ; L%u:\n", tn->label, tn->label);
      else
	/* AIS: start one of ick_ec's labeled blocks. */
	(void) fprintf(fp, "ick_labeledblock(%uU,{",tn->label);
    }
    if (yydebug || cdebug || compile_only)
    {
      (void) fprintf(fp, "\t""/* %s */", textlines[tn->ick_lineno]);
      /* AIS: grind out an expression explanation */
      if (tn->type == GETS || tn->type == FORGET || tn->type == RESUME
	  || tn->type == FROM || tn->type == COMPUCOME
	  || tn->type == MANYFROM || tn->type == NEXTFROMEXPR)
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
	for (up = tuples; up < tuples + ick_lineno; up++)
	    if (tn->u.target == up->label) {
		emitlineno = up->ick_lineno;
		break;
	    }
    }
    else if (tn->ncomefrom)
    {
       /* AIS: For multithreading. Return the 1st if we're forking. */
      emitlineno = comefromsearch(tn,1);
      if(emitlineno != -1)
	emitlineno = tuples[emitlineno-1].ick_lineno;
    }
    else if (tn < tuples + ick_lineno - 1)
	emitlineno = tn[1].ick_lineno;
    else
	emitlineno = iyylineno;
    if(!pickcompile) /* AIS: PICs can't report errors,
			so don't bother with ick_lineno */
      (void) fprintf(fp, "    ick_lineno = %d;\n", emitlineno);

    /* AIS: figure out which line we're on, so E000 can be done correctly */
    if (tn < tuples + ick_lineno - 1)
      dim = tn[1].ick_lineno - tn->ick_lineno;
    else
      dim = iyylineno - tn->ick_lineno;
    if (tn->sharedline)
      ++dim;
    E000string=nice_text(textlines + tn->ick_lineno, dim);

    /* AIS: set weaving status if necessary */
    if(tn->setweave)
	(void) fprintf(fp, "    weaving = %d;\n", (tn->setweave>0)?1:0);

    /* AIS: print warnings on -l */
    if(ick_checkforbugs)
    {
      if(tn->warn112) ick_lwarn(W112, emitlineno, (const char*) NULL);
      if(tn->warn128) ick_lwarn(W128, emitlineno, (const char*) NULL);
      if(tn->warn534) ick_lwarn(W534, emitlineno, (const char*) NULL);
      if(tn->warn018) ick_lwarn(W018, emitlineno, (const char*) NULL);
      if(tn->warn016) ick_lwarn(W016, emitlineno, (const char*) NULL);
      if(tn->warn276) ick_lwarn(W276, emitlineno, (const char*) NULL);
      if(tn->warn239) ick_lwarn(W239, emitlineno, (const char*) NULL);
      if(tn->warn622) ick_lwarn(W622, emitlineno, (const char*) NULL);
    }

    /* AIS: emit debugging information */
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
    if(coopt) (void) fprintf(fp, "    ick_MYSTERYLINE;\n");

    /* AIS: If the tuple is ONCE/AGAIN flagged, we need a delayed-action
       set of its abstention status to the AGAIN-flagged status. The
       problem is that some statements, like COME FROM, need to set after
       the command has finished, and some, like NEXT, need it before the
       command has started. At the moment, only NEXT and GO_BACK have a
       ONCE/AGAIN before it, rather than after (because neither of them
       continue in the normal fashion). UNKNOWN is also handled this way,
       because CREATEd statements can be NEXT-like but not COME FROM-like. */
    if ((tn->type == NEXT || tn->type == GO_BACK || tn->type == UNKNOWN) &&
	tn->onceagainflag != onceagain_NORMAL)
    {
      /* ONCE/AGAIN has already been swapped by perpet.c in the case
	 of a preabstained statement ('DO NOT'...). So if we currently have
	 a ONCE, it means that being abstained is the attractive state,
	 and if we currently have an AGAIN, it means that being
         reinstated is the attractive state. Semantics with computed
	 ABSTAIN: Don't change the abstention count unless necessary, in
	 which case change it to 0 or 1. */
      fprintf(fp,"    ick_oldabstain = ICKABSTAINED(%d);\n", (int)(tn - tuples));
      fprintf(fp,"    ICKABSTAINED(%d) = %s;\n",
	      (int)(tn - tuples),
	      tn->onceagainflag==onceagain_ONCE ? "ick_oldabstain ? ick_oldabstain : 1"
	      : "0");
      /* This test-and-set must be atomic. As all statements are atomic anyway
         in the current version of ick, that isn't a problem, but if anyone
	 wants to try using POSIX's multithreading features, the above two
	 lines need to be a critical section. */
    }

    /* AIS: in the case of COMPUCOME, we need an extra guard unless useickec. */
    if ((!useickec && (tn->type == COMPUCOME || tn->type == NEXTFROMEXPR))
	 || tn->type == GERUCOME || tn->type == NEXTFROMGERUND)
    {
      fprintf(fp,"    if(0)\n    {\n");
      fprintf(fp,"CCF%d:\n",compucomecount++);
      if(tn->type == COMPUCOME || tn->type == NEXTFROMEXPR)
      {
	fprintf(fp,"    if(ick_skipto&&ick_skipto==");
	prexpr(tn->u.node, fp, 1);
      }
      else if(tn->type == GERUCOME || tn->type == NEXTFROMGERUND)
      {
	fprintf(fp,"    if(");
	for (np = tn->u.node; np; np = np->rval)
	{
	  if (np->constant == ABSTAIN) {
	    (void) fprintf(fp,
"linetype[truelineno] == %s || linetype[truelineno] == %s || "
"linetype[truelineno] == %s || linetype[truelineno] == %s || ",
			   enablers[np->constant-GETS],
			   enablers[np->constant-GETS+2],
			   enablers[FROM-GETS], enablers[MANYFROM-GETS]);
	  } else if (np->constant == REINSTATE) {
	    (void) fprintf(fp,
"linetype[truelineno] == %s || linetype[truelineno] == %s || ",
			   enablers[np->constant-GETS],
			   enablers[np->constant-GETS+2]);
	  } else if (np->constant == GETS) {
	    (void) fprintf(fp,
"linetype[truelineno] == %s || linetype[truelineno] == %s || ",
			   enablers[GETS-GETS],
			   enablers[RESIZE-GETS]);
	  } else if (np->constant == COME_FROM) {
	    (void) fprintf(fp,
"linetype[truelineno] == %s || linetype[truelineno] == %s || "
"linetype[truelineno] == %s || ",
			   enablers[COME_FROM-GETS],
			   enablers[COMPUCOME-GETS],
			   enablers[GERUCOME-GETS]);
	  } else if (np->constant == NEXTFROMLABEL) {
	    (void) fprintf(fp,
"linetype[truelineno] == %s || linetype[truelineno] == %s || "
"linetype[truelineno] == %s || ",
			   enablers[NEXTFROMLABEL-GETS],
			   enablers[NEXTFROMEXPR-GETS],
			   enablers[NEXTFROMGERUND-GETS]);
	  } else {
	    (void) fprintf(fp, "linetype[truelineno] == %s || ",
			   enablers[np->constant-GETS]);
	  }
	}
	fprintf(fp, "0");
      }
      fprintf(fp,") {\n");
    }

    /* AIS: With this block placed here, you can't even have a comment
       after a TRY AGAIN line. Move it below the next check if this seems
       to be undesirable behaviour. */
    if(pasttryagain) /* AIS */
    {
      ick_lose(IE993, emitlineno, (const char*)NULL);
    }

    if(flowoptimize && tn->initabstain && !tn->abstainable
       && tn->type != COMPUCOME && tn->type != COME_FROM &&
       tn->type != NEXT && tn->type != GERUCOME &&
       tn->type != NEXTFROMLABEL && tn->type != NEXTFROMEXPR &&
       tn->type != NEXTFROMGERUND) /* AIS */
      goto skipcomment; /* Look, a comment! We can completely skip all
			   degeneration of this statement (although with
			   -c, comments will appear in the degenerated
			   code in its place). The COMPUCOME condition is
			   because it is so weird. COME_FROM and NEXT are
			   exempted so labels are generated. */

    /* emit conditional-execution prefixes */
    /* AIS: added the useickec condition */
    if ((tn->type != COME_FROM && tn->type != NEXTFROMLABEL) || useickec)
	emit_guard(tn, fp);

    /* now emit the code for the statement body */
    switch(tn->type)
    {
    case GETS:
      /* AIS: variableconstants means GETS has been generalised */
      if(variableconstants)
      {
	revprexpr(tn->u.node->lval, fp, tn->u.node->rval);
	nodefree(tn->u.node);
	break;
      }

        /* Start of AIS optimization */
	np = tn->u.node;
	if(np->lval->opcode == SUB) np = np->lval;
	if(flowoptimize && ick_Base == 2 && !opoverused && !variableconstants &&
	   (np->lval->opcode == ick_TWOSPOT
	    || np->lval->opcode == ick_HYBRID
	    || !(tn->u.node->rval->optdata & ~0xffffLU)))
	{
	  atom* op;
	  int ignorable = 1;
	  assert(oblist != NULL);
	  for(op = oblist; op < obdex; op++)
	  {
	    if(op->type == np->lval->opcode &&
	       (unsigned long)op->intindex == np->lval->constant)
	    {
	      ignorable &= op->ignorable;
	    }
	  }
	  if(!ignorable)
	  { /* Variable can't be ignored, and expression must be in range */
	    (void) fprintf(fp,"\t""");
	    prexpr(tn->u.node->lval, fp, 1);
	    (void) fprintf(fp, " = ");
	    prexpr(tn->u.node->rval, fp, 1);
	    (void) fprintf(fp, ";\n");
	    break;
	  }
	}
	/* End of AIS optimization */
	if(opoverused&&
	   (tn->u.node->lval->opcode==ick_ONESPOT||
	    tn->u.node->lval->opcode==ick_TWOSPOT)) /* AIS */
	{
	  (void) fprintf(fp,"\t""");
	  ooprvar(tn->u.node->lval, fp, 1);
	  (void) fprintf(fp,".set(");
	  prexpr(tn->u.node->rval, fp, 1);
	  (void) fprintf(fp,",os%dspot%lu);\n",
			 ((tn->u.node->lval->opcode==ick_TWOSPOT)?1:0)+1,
			 tn->u.node->lval->constant);
	}
	else if(!pickcompile)
	{
	  np = tn->u.node;

	  if (np->lval->opcode != SUB) {
	    sp = np->lval;
	    (void) fprintf(fp,"\t""(void) ick_assign((char*)&");
	  }
	  else {
	    sp = np->lval->lval;
	    (void) fprintf(fp,"\t""(void) ick_assign(");
	  }
	  prvar(np->lval, fp, 1);
	  (void) fprintf(fp,", %s", nameof(sp->opcode, vartypes));
	  (void) fprintf(fp,", %s[%lu], ", nameof(sp->opcode, forgetbits),
			 sp->constant);
	  prexpr(np->rval, fp, 1);
	  (void) fprintf(fp,");\n");
	}
	else /* AIS: Added this case for the simpler PIC assignment rules */
	{
	  (void) fprintf(fp,"\t""if(ignore%s%lu) ",
			 nameof(tn->u.node->lval->opcode,varstores),
			 tn->u.node->lval->constant);
	  prexpr(tn->u.node->lval, fp, 1);
	  (void) fprintf(fp, " = ");
	  prexpr(tn->u.node->rval, fp, 1);
	  (void) fprintf(fp, ";\n");
	}
	break;

    case RESIZE:
      if(pickcompile) ick_lose(IE256, emitlineno, (const char*) NULL); /* AIS */
	np = tn->u.node;
	dim = 0;
	for (sp = np->rval; sp; sp = sp->rval)
	  dim++;
	(void) fprintf(fp, "\t""ick_resize(");
	prvar(np->lval, fp, 1);
#ifdef BOOL_VARARGS_BROKEN
	(void) fprintf(fp, ", (int)%s[%lu]", nameof(np->lval->opcode, forgetbits),
		       np->lval->constant);
#else
	(void) fprintf(fp, ", %s[%lu]", nameof(np->lval->opcode, forgetbits),
		       np->lval->constant);
#endif
	(void) fprintf(fp, ", %d", dim);
	for (sp = np->rval; sp; sp = sp->rval) {
	  (void) fprintf(fp, ", (size_t)");
	  prexpr(sp->lval, fp, 1);
        }
	(void) fprintf(fp, ");\n");
	break;

    case NEXT:
      /* AIS: if using ickec, use its features for the next */
      if(useickec)
      {
	(void) fprintf(fp,"\t""ick_dogoto(%uU,ick_lineno,1);\n",tn->u.target);
	break;
      }
      /* Start of AIS optimization */
      if(tn->u.target>=1000 && tn->u.target<=1999 && pickcompile)
      {
	/* optimize syslib call on a PIC */
	(void) fprintf(fp, "\t""syslibopt%u();\n", tn->u.target);
	break;
      }
      if(tn->optversion)
      { /* optimizef has checked that this is a valid optimization */
	(void) fprintf(fp, "\t""if(1 == ");
	prexpr(tn->u.node, fp, 1); /* frees optimizef's nodecopy */
	/* AIS: Everything now in one giant switch(), with some very strange
	   constructs (including ;{;} as a null statement; this makes
	   degenerating the code slightly easier) */
	(void) fprintf(fp, ") {ick_pushnext(%d); ick_skipto=%uU; goto top;}} case %d:;{;\n",
		       (int)(tn - tuples + 1), tn->nexttarget, (int)(tn - tuples + 1));
	break;
      }
      /* End of AIS optimization */
      (void) fprintf(fp, /* same change as above (case rather than a label) */
		     "\t""ick_pushnext(%d); goto L%u;} case %d:;{;\n",
		     (int)(tn - tuples + 1), tn->u.target, (int)(tn - tuples + 1));
      break;

    case GO_BACK: /* By AIS */
      if(!multithread) ick_lose(IE405, emitlineno, (const char*) NULL);
      (void) fprintf(fp, "\t""choiceback();\n");
      break;

    case GO_AHEAD: /* By AIS */
      if(!multithread) ick_lose(IE405, emitlineno, (const char*) NULL);
      (void) fprintf(fp, "\t""choiceahead();\n");
      break;

    case RESUME:
      if(useickec) /* AIS */
      {
	(void) fprintf(fp, "\t""ick_doresume(");
	prexpr(tn->u.node, fp, 1);
	(void) fprintf(fp, ", ick_lineno);\n");
	break;
      }
	(void) fprintf(fp, "\t""ick_skipto = ick_resume(");
	prexpr(tn->u.node, fp, 1);
	(void) fprintf(fp, "); goto top;\n");
	break;

    case FORGET:
      if(useickec) /* AIS */
      {
	(void) fprintf(fp, "\t""ick_forget(");
	prexpr(tn->u.node, fp, 1);
	(void) fprintf(fp, ");\n");
	break;
      }

	(void) fprintf(fp, "\t""ick_popnext(");
	prexpr(tn->u.node, fp, 1);
	(void) fprintf(fp, ");\n");
	break;

    case STASH:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\t""ICKSTASH(%s, %lu, %s, %s%s);\n",
			   nameof(np->opcode, vartypes),
			   np->constant,
			   nameof(np->opcode, varstores),
			   /* AIS */(opoverused&&(np->opcode==ick_ONESPOT||
						  np->opcode==ick_TWOSPOT)?
			   "ick_oo_":"0"),
			   /* AIS */(opoverused&&(np->opcode==ick_ONESPOT||
						  np->opcode==ick_TWOSPOT)?
			   nameof(np->opcode, varstoresdem):"0"));
	break;

    case RETRIEVE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\t""ICKRETRIEVE(%s, %lu, %s, %s, %s%s);\n",
			   nameof(np->opcode, varstores), np->constant,
			   nameof(np->opcode, vartypes),
			   nameof(np->opcode, forgetbits),
			   /* AIS */(opoverused&&(np->opcode==ick_ONESPOT||
						  np->opcode==ick_TWOSPOT)?
			   "ick_oo_":"0"),
			   /* AIS */(opoverused&&(np->opcode==ick_ONESPOT||
						  np->opcode==ick_TWOSPOT)?
			   nameof(np->opcode, varstoresdem):"0"));

	break;

    case IGNORE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t""ICKIGNORE(%s,%lu,%s) = true;\n",
			   nameof(np->opcode, forgetbits),
			   np->constant,
			   nameof(np->opcode, varstores));
	break;

    case REMEMBER:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t""ICKIGNORE(%s,%lu,%s) = false;\n",
			   nameof(np->opcode, forgetbits),
			   np->constant,
			   nameof(np->opcode, varstores));
	break;

	/* All abstention code has been edited by AIS to allow for the new
	   abstention rules */
    case ABSTAIN:
      /* AIS: In CLC-INTERCAL, you can't abstain a GIVE UP line, so I copied
         a modified version of Joris's REINSTATE patch here as well */
      if (!ick_clcsemantics || (tuples + tn->u.target - 1)->type != GIVE_UP)
      {
	if(!pickcompile)
	  (void) fprintf(fp, "\t""if(!ICKABSTAINED(%u)) ICKABSTAINED(%u) = 1;\n", tn->u.target - 1, tn->u.target-1);
	else
	  (void) fprintf(fp, "ICKABSTAINED(%u) = 1;\n", tn->u.target-1);
      }
      else
	(void) fprintf(fp, "\t""/* not abstaining from a GIVE UP line */\n");;
      break;

    case FROM:
      if(pickcompile) ick_lose(IE256, emitlineno, (const char*) NULL);
        (void) fprintf(fp, "\t""ICKABSTAINED(%u)+=", tn->u.target-1);
	tn->u.node->width = 32;
	prexpr(tn->u.node,fp, 1);
	(void) fprintf(fp, ";\n");
	break;

    case REINSTATE:
        /* (Joris Huizer) ensure it is not a GIVE UP statement */
        if ((tuples + tn->u.target - 1)->type != GIVE_UP)
        {
          if(!pickcompile)
            (void) fprintf(fp, "\t""if(ICKABSTAINED(%u)) ICKABSTAINED(%u)--;\n", tn->u.target - 1, tn->u.target-1);
          else
            (void) fprintf(fp, "\t""ICKABSTAINED(%u)=0;\n", tn->u.target - 1);
        }
        else
          (void) fprintf(fp, "\t""/* not reinstating a GIVE UP line */\n");
        break;

    case ENABLE:
    case DISABLE:
    case MANYFROM:
      /* AIS: This code has been rewritten to make use of the revlinetype array
	 (an optimisation that Joris Huizer came up with; however, I am not
	 using his code, but rewriting it, to make use of a single array and an
	 index to it, rather than one array for each command type, for
	 maintainability reasons. */
      if(pickcompile) ick_lose(IE256, emitlineno, (const char*) NULL);
      (void) fprintf(fp,"\tint i;\n");
      np=tn->u.node;
      if(tn->type==MANYFROM)
      {
	np=np->rval;
	fprintf(fp,"\tint j = ");
	prexpr(tn->u.node->lval, fp, 1);
	fprintf(fp,";\n");
      }
      for(; np; np = np->rval)
      {
	int npc = np->constant;
      anothertype:
	(void) fprintf(fp,"\n\tfor(i=revlineindex[%s];i<revlineindex[%s+1];i++)\n",
		       enablers[npc-GETS],enablers[npc-GETS]);
	switch(tn->type)
	{
	case ENABLE:  (void) fprintf(fp,"\t    if (ick_abstained[revlinetype[i]])"
				    "\t\tick_abstained[revlinetype[i]]--;\n"); break;
	case DISABLE: (void) fprintf(fp,"\t    if(!ick_abstained[revlinetype[i]])"
				    "\t\tick_abstained[revlinetype[i]]=1;\n"); break;
	case MANYFROM:(void) fprintf(fp,"\tick_abstained[revlinetype[i]]+=j;\n"); break;
	default:      ick_lose(IE994, emitlineno, (const char *)NULL);
	}
	switch(npc)
	{
	case GETS:          npc=RESIZE;         goto anothertype;
	case ABSTAIN:       npc=DISABLE;        goto anothertype;
	case DISABLE:       npc=FROM;           goto anothertype;
	case FROM:          npc=MANYFROM;       goto anothertype;
	case REINSTATE:     npc=ENABLE;         goto anothertype;
	case COME_FROM:     npc=COMPUCOME;      goto anothertype;
	case COMPUCOME:     npc=GERUCOME;       goto anothertype;
	case NEXTFROMLABEL: npc=NEXTFROMEXPR;   goto anothertype;
	case NEXTFROMEXPR:  npc=NEXTFROMGERUND; goto anothertype;
	default: break;
	}
      }
      break;

    case NEXTFROMEXPR:
    case NEXTFROMGERUND:
    case GERUCOME:
    case COMPUCOME: /* By AIS. Note that this doesn't even have balanced
		       braces; it's designed to work with COMPUCOME's
		       crazy guarding arrangements */
      if(pickcompile) ick_lose(IE256, emitlineno, (const char*) NULL); /* AIS */
      if(useickec) /* use ick_ec's features for next from and come from*/
      {
	if(tn->type == COMPUCOME)
	{
	  fprintf(fp,"\t""ick_docomefromif(");
	  prexpr(tn->u.node, fp, 1);
	  fprintf(fp,",ick_lineno,({int i=0;");
	  emit_guard(tn,fp); /* re-emit the guard */
	  fprintf(fp,"i=1;};i;}));\n");
	  break;
	}
	else if(tn->type == NEXTFROMEXPR)
	{
	  fprintf(fp,"\t""ick_donextfromif(");
	  prexpr(tn->u.node, fp, 1);
	  fprintf(fp,",ick_lineno,({int i=0;");
	  emit_guard(tn,fp); /* re-emit the guard */
	  fprintf(fp,"i=1;};i;}));\n");
	  break;
	}
      }
      fprintf(fp,"\t""%s;} else goto CCF%d;\n",
	      multithread?"NEXTTHREAD":useprintflow?
	      "if(ick_printflow) fprintf(stderr,\"[%d]\",ick_lineno)":"",
	      compucomecount);
      break;

    case GIVE_UP: /* AIS: Edited to allow for yuk */
      if(yukprofile||yukdebug) fprintf(fp, "\t""YUKTERM;\n");
      if(multithread) fprintf(fp, "\t""killthread();\n");
      else
      {
	if(nonespots||opoverused)
	  fprintf(fp,"\t""if(ick_onespots) free(ick_onespots);\n");
	if(ntwospots||opoverused)
	  fprintf(fp,"\t""if(ick_twospots) free(ick_twospots);\n");
	if(ntails) fprintf(fp,"\t""if(ick_tails) free(ick_tails);\n");
	if(nhybrids) fprintf(fp,"\t""if(ick_hybrids) free(ick_hybrids);\n");
	if(nonespots||opoverused)
	  fprintf(fp,"\t""if(ick_oneforget) free(ick_oneforget);\n");
	if(ntwospots||opoverused)
	  fprintf(fp,"\t""if(ick_twoforget) free(ick_twoforget);\n");
	if(ntails) fprintf(fp,"\t""if(ick_tailforget) free(ick_tailforget);\n");
	if(nhybrids) fprintf(fp,"\t""if(ick_hyforget) free(ick_hyforget);\n");
	if(opoverused)
	{
	  fprintf(fp,"\t""if(ick_oo_onespots) free(ick_oo_onespots);\n");
	  fprintf(fp,"\t""if(ick_oo_twospots) free(ick_oo_twospots);\n");
	}
	fprintf(fp,"\t""if(ick_next) free(ick_next);\n");
	if(useickec)
	  fprintf(fp,"\t""if(ick_next_jmpbufs) free(ick_next_jmpbufs);\n");
      }
      (void) fprintf(fp, "\t""exit(0);\n");
      break;

    case TRY_AGAIN: /* By AIS */
      (void) fprintf(fp, "\t""goto ick_restart;\n    }\n");
      if(yukprofile||yukdebug) fprintf(fp, "    if(yukloop) goto ick_restart;\n");
      if(yukprofile||yukdebug) fprintf(fp, "    YUKTERM;\n");
      if(multithread) fprintf(fp, "\t""killthread();\n");
      else
      {
	if(nonespots||opoverused)
	  fprintf(fp,"\t""if(ick_onespots) free(ick_onespots);\n");
	if(ntwospots||opoverused)
	  fprintf(fp,"\t""if(ick_twospots) free(ick_twospots);\n");
	if(ntails) fprintf(fp,"\t""if(ick_tails) free(ick_tails);\n");
	if(nhybrids) fprintf(fp,"\t""if(ick_hybrids) free(ick_hybrids);\n");
	if(nonespots||opoverused)
	  fprintf(fp,"\t""if(ick_oneforget) free(ick_oneforget);\n");
	if(ntwospots||opoverused)
	  fprintf(fp,"\t""if(ick_twoforget) free(ick_twoforget);\n");
	if(ntails) fprintf(fp,"\t""if(ick_tailforget) free(ick_tailforget);\n");
	if(nhybrids) fprintf(fp,"\t""if(ick_hyforget) free(ick_hyforget);\n");
	if(opoverused)
	{
	  fprintf(fp,"\t""if(ick_oo_onespots) free(ick_oo_onespots);\n");
	  fprintf(fp,"\t""if(ick_oo_twospots) free(ick_oo_twospots);\n");
	}
      }
      (void) fprintf(fp, "    {\n\treturn(0);\n");
      /* because if TRY AGAIN is the last line, falling off the end isn't an error */
      pasttryagain=1; /* flag an error if we try any more commands */
      break;

    case WRITE_IN:
      if(pickcompile) ick_lose(IE256, emitlineno, (const char*) NULL); /* AIS */
      for (np = tn->u.node; np; np = np->rval) {
	  if (np->lval->opcode == ick_TAIL || np->lval->opcode == ick_HYBRID) {
	    (void) fprintf(fp,"\t""ick_binin(");
	    prvar(np->lval, fp, 1);
	    (void) fprintf(fp, ", %s[%lu]",
			   nameof(np->lval->opcode, forgetbits),
			   np->lval->constant);
	    (void) fprintf(fp,");\n");
	  }
	  else {
	    if (np->lval->opcode != SUB) {
	      sp = np->lval;
	      (void) fprintf(fp,"\t""(void) ick_assign((char*)&");
	    }
	    else {
	      sp = np->lval->lval;
	      (void) fprintf(fp,"\t""(void) ick_assign(");
	    }
	    prvar(np->lval, fp, 1);
	    (void) fprintf(fp,", %s", nameof(sp->opcode, vartypes));
	    (void) fprintf(fp,", %s[%lu]", nameof(sp->opcode, forgetbits),
			   sp->constant);
	    (void) fprintf(fp,", ick_pin());\n");
	  }
	}
	break;

    case READ_OUT:
      if(pickcompile) ick_lose(IE256, emitlineno, (const char*) NULL); /* AIS */
	for (np = tn->u.node; np; np = np->rval)
	{
	  if (np->lval->opcode == ick_TAIL || np->lval->opcode == ick_HYBRID) {
	    (void) fprintf(fp,"\t""ick_binout(");
	    prvar(np->lval, fp, 1);
	    (void) fprintf(fp,");\n");
	  }
	  else {
	    (void) fprintf(fp, "\t""ick_pout(");
	    prexpr(np->lval, fp, 1);
	    (void) fprintf(fp, ");\n");
	  }
	}
	break;

    case PIN: /* AIS, with some code borrowed from the GETS code */
      np = tn->u.node;
      (void) fprintf(fp, "\t""TRISA = seq(~((");
      prexpr(np, fp, 0);
      (void) fprintf(fp, " >> 16) & 255));\n");
      (void) fprintf(fp, "\t""TRISB = seq(~(");
      prexpr(np, fp, 0);
      (void) fprintf(fp, " >> 24));\n");
      (void) fprintf(fp, "\t""PORTA = seq(~(");
      prexpr(np, fp, 0);
      (void) fprintf(fp, " & 255));\n");
      (void) fprintf(fp, "\t""PORTB = seq(~((");
      prexpr(np, fp, 0);
      (void) fprintf(fp, " >> 8) & 255));\n");
      {
	atom* op;
	int ignorable = 1;
	assert(oblist != NULL);
	for(op = oblist; op < obdex; op++)
	{
	  if(op->type == np->opcode &&
	     (unsigned long)op->intindex == np->constant)
	  {
	    ignorable &= op->ignorable;
	  }
	}
	if(!ignorable)
	{ /* Variable can't be ignored, and expression must be in range */
	  (void) fprintf(fp,"\t""");
	  prexpr(np, fp, 1);
	  (void) fprintf(fp, " = ");
	}
	else
	{
	  np = tn->u.node;
	  (void) fprintf(fp,"\t""if(ignore%s%lu) ",
			 nameof(np->opcode,varstores),
			 np->constant);
	  prexpr(np, fp, 1);
	  (void) fprintf(fp, " = ");
	}
      }
      (void) fprintf(fp,"(TRISB<<24) | (TRISA<<16) | (PORTB<<8) | PORTA;\n");
      break;

    case CREATE: /* By AIS */
      if(createsused == 0) goto splatme;
      (void) fprintf(fp,"\t""ick_registercreation(\"");
      (void) prunknownstr(tn->u.node, fp);
      (void) fprintf(fp,"\",%uU);\n",tn->u.target);
      break;

    case COMPUCREATE: /* By AIS */
      if(createsused == 0) goto splatme;
      (void) fprintf(fp,"\t""ick_registercreation(\"");
      (void) prunknownstr(tn->u.node->rval, fp);
      (void) fprintf(fp,"\",");
      prexpr(tn->u.node->lval, fp, 1);
      (void) fprintf(fp,");\n");
      free(tn->u.node); /* don't free the rval */
      break;

    case UNKNOWN: /* By AIS */
      /* We generate a check to see if the unknown statement has gained a
	 meaning since it was compiled, or otherwise continue to the splattered
	 case. Not for PIC-INTERCAL, though. */
      if(!pickcompile) prunknown(tn->u.node, fp);
      /*@fallthrough@*/
    case SPLATTERED:
      /* AIS: The code previously here could access unallocated memory due to
	 a bug if the comment was a COME FROM target. The problem is that
	 emitlineno (the line to show an error on) is usually the line after
	 this one, but not always, and in this case the line after this one is
	 always what we want, so I copied the relevant part of the emitlineno
	 logic here to fix the bug. */
    splatme:
      if (tn < tuples + ick_lineno - 1)
	dim = tn[1].ick_lineno - tn->ick_lineno;
      else
	dim = iyylineno - tn->ick_lineno;
      if (tn->sharedline)
	++dim;
      (void) fprintf(fp, "\t""ick_lose(IE000, %d, \"%s\");\n",
		     emitlineno, nice_text(textlines + tn->ick_lineno, dim));
      break;

    case PREPROC: /* AIS: 'DO NOTHING', but not enterable into a program. This
		     is generated by the preprocessor. */
      fprintf(fp,"\t""; /* do nothing */\n");
      break;

    case COME_FROM:
    case NEXTFROMLABEL: /* AIS */
      if(useickec) /* AIS */
      {
	if(tn->type == COME_FROM)
	{
	  fprintf(fp,"\t""ick_docomefromif(%uU,ick_lineno,({int i=0;",
		  tn->u.target);
	  emit_guard(tn,fp); /* re-emit the guard */
	  fprintf(fp,"i=1;};i;}));\n");
	  break;
	}
	else /* (tn->type == NEXTFROMLABEL) */
	{
	  fprintf(fp,"\t""ick_donextfromif(%uU,ick_lineno,({int i=0;",
		  tn->u.target);
	  emit_guard(tn,fp); /* re-emit the guard */
	  fprintf(fp,"i=1;};i;}));\n");
	  break;
	}
      }
	(void) fprintf(fp, "if(0) {C%ld: %s;%s}\n", (long)(tn-tuples+1),
		       tn->type==NEXTFROMLABEL ? "ick_pushnext(truelineno+1)":"",
		       multithread?" NEXTTHREAD;":!useprintflow?""
		       :" if(ick_printflow) "
		       "fprintf(stderr,\"[%d]\",ick_lineno);");
	/* AIS: Changed so all COME_FROMs have unique labels even if two
	   of them aim at the same line, and added the NEXT FROM case (which
	   involves hiding COME FROM labels in an unreachable if()). */
	break;

    case WHILE: /* AIS: fall through to the error, because this shouldn't
		   come up yet. */
    default:
	ick_lose(IE994, emitlineno, (const char *)NULL);
	/*@-unreachable@*/ break; /*@=unreachable@*/
    }

    if ((tn->type != COME_FROM && tn->type != NEXTFROMLABEL)
	|| /*AIS*/ useickec)
	(void) fprintf(fp, "    }\n");

 skipcomment:

    if ((!useickec && (tn->type == COMPUCOME || tn->type == NEXTFROMEXPR))
	|| tn->type == NEXTFROMGERUND  || tn->type == GERUCOME )
    { /* By AIS */
      (void) fprintf(fp,"    else goto CCF%d;\n",compucomecount);
      (void) fprintf(fp,"    ick_ccfc++;\n");
      /* Note that due to the semantics of setjmp, this has to be written as 2
	 separate ifs. The MULTICOME macro expands to a non-multithreaded or
         multithreaded function for handling a COME FROM clash. */
      (void) fprintf(fp,"    if(ick_ccfc==1||MULTICOME(%d,ick_cjb))\n"
		     "\t""if(setjmp(ick_cjb) == 0) goto CCF%d;\n",
		     emitlineno, compucomecount);
      /* Of course, emitlineno is unlikely to be helpful! */
      if(tn->type == NEXTFROMEXPR || tn->type == NEXTFROMGERUND)
      {
	/* Stack up the statement we've NEXTed from */
	(void) fprintf(fp,"    ick_pushnext(truelineno+1);\n");
      }
      (void) fprintf(fp,"    }\n");
    }

    /* AIS: Before any COMING FROM this line is done, we need to sort out
       ONCE and AGAIN situations, unless this line was a NEXT or GO_BACK.
       COME FROM is also excluded because it acts at the suckpoint, not
       at the place it's written in the program. */
    if (tn->onceagainflag != onceagain_NORMAL &&
	tn->type != NEXT && tn->type != GO_BACK && tn->type != UNKNOWN &&
	((tn->type != COME_FROM && tn->type != NEXTFROMLABEL) || useickec))
    {
      /* See my comments against the NEXT code for more information.
	 This code is placed here so COME FROM ... ONCE constructs work
	 properly (the line is ick_abstained if the COME FROM is reached in
	 execution, or its suckpoint is reached in execution). */
      fprintf(fp,"    ick_oldabstain = ICKABSTAINED(%d);\n", (int)(tn - tuples));
      fprintf(fp,"    ICKABSTAINED(%d) = %s;\n",
	      (int)(tn - tuples),
	      tn->onceagainflag==onceagain_ONCE ? "ick_oldabstain ? ick_oldabstain : 1"
	      : "0");
    }

    /* AIS: This is where we start the COME FROM suckpoint code. */

    /* AIS: The ickec version is very simple! We just finish the labeled
       block started at the start of the command. */
    if(tn->label && useickec)
      (void) fprintf(fp, "});\n");

    /* AIS: We need to keep track of how many COME FROMs are aiming here
       at runtime, if we don't have the very simple situation of no
       COMPUCOMEs and a single-thread program (in which case the check
       is done at compile-time by codecheck). Even without COMPUCOME,
       this can change in a multithread program due to abstentions. */
    if((tn->ncomefrom && multithread) || (tn->label && compucomesused)
       || gerucomesused)
      (void) fprintf(fp, "    ick_ccfc = 0;\n");
    /* AIS: For NEXTING FROM this line */
    if(nextfromsused && tn->ncomefrom)
    {
      (void) fprintf(fp, "    truelineno = %d;\n", (int)(tn-tuples));
    }
    /*
     * If the statement that was just degenerated was a COME FROM target,
     * emit the code for the jump to the COME FROM.
     * AIS: Changed most of this to allow for multithreading.
     */
    while(tn->ncomefrom && !useickec) /* acts as an if if singlethreading */
    {
      tuple* cf; /* local to this block */
      if(multithread || compucomesused) generatecfjump = 1;
      cf = tuples+comefromsearch(tn,tn->ncomefrom)-1;
      if (yydebug || compile_only)
	(void) fprintf(fp,
		       "    /* line %03d is a suck point for the COME FROM "
		       "at line %03d */\n", tn->ick_lineno, cf->ick_lineno);
      if (cf->onceagainflag != onceagain_NORMAL)
      { /* Calculate ONCE/AGAIN when the suckpoint is passed */
	fprintf(fp,"    ick_oldabstain = ICKABSTAINED(%d);\n", (int)(cf - tuples));
	fprintf(fp,"    ICKABSTAINED(%d) = %s;\n",
		(int)(cf - tuples),
		cf->onceagainflag==onceagain_ONCE ? "ick_oldabstain ? ick_oldabstain : 1"
		: "0");
      }
      emit_guard(cf, fp);
      if(multithread || compucomesused)
	(void) fprintf(fp,
		       "\t""ick_ccfc++;\tif(ick_ccfc==1||MULTICOME(%d,ick_cjb)) "
		       "if(setjmp(ick_cjb) == 1) goto C%ld;\n    }\n",
		       emitlineno, (long)(cf-tuples+1));
      else /* optimize for the simple case */
	(void) fprintf(fp, "\t""goto C%ld;\n    }\n", (long)(cf-tuples+1));
      tn->ncomefrom--;
    }

    /* AIS: If the statement has a label, it might be a
       computed COME FROM target. Also check the flag that says this
       code is needed in a multithread non-COMPUCOME program.
       If (at runtime) ick_ccfc is nonzero, we know ick_cjb has already been set;
       otherwise, set it now. In the case of a multithread non-COMPUCOME
       program, the goto will just jump to a longjmp, switching to the
       one and only one COME FROM that hasn't been given its own thread.
       However, skip all the compucomes and gerucomes if preproc is set,
       because COMING FROM a preproc should only ever be done by label. */
    if (((tn->label && compucomesused) || generatecfjump || gerucomesused) &&
        (!tn->preproc || generatecfjump) && (!useickec || gerucomesused))
    {
      if(compucomesused)
      {
	(void) fprintf(fp, "    ick_skipto = %u;\n", tn->label);
      }
      if(gerucomesused || nextfromsused)
      {
	(void) fprintf(fp, "    truelineno = %d;\n", (int)(tn-tuples));
      }
      if(generatecfjump) (void) fprintf(fp, "    if(ick_ccfc) goto CCF%s;\n",
					tn->preproc?"L":"0");
      if((compucomesused || gerucomesused) && !tn->preproc)
      { /* check all the COMPUCOMES */
	(void) fprintf(fp, "    %sif(setjmp(ick_cjb) == 0) goto CCF0;\n",
		       generatecfjump?"else ":"");
      }
      generatecfjump = 0;
      /* AIS: If NEXT FROM's used, this might be a NEXT return target.
	 Don't generate case labels for NEXT, as it has them already. */
      if(nextfromsused && tn->type != NEXT)
      {
	(void) fprintf(fp, "    case %u:;\n", (unsigned)(tn-tuples+1));
      }
    }

    /* AIS: Now we've finished the statement, let's switch to the next
       thread in a multithread program. */
    if(multithread) (void) fputs("    NEXTTHREAD;\n", fp);
    else if(useprintflow)
      (void) fputs("    if(ick_printflow) fprintf(stderr,"
		   "\"[%d]\",ick_lineno);\n",fp);
}
/*@=nestedextern@*/

/* AIS: Generate prototypes for slat expressions, args to UNKNOWN */
void emitslatproto(FILE* fp)
{
  node* np=firstslat;
  const char* t="ick_type32";
  while(np)
  {
    fprintf(fp,"%s ick_og%lx(%s);\nvoid ick_os%lx(%s, void(*)());\n",
	    t,(unsigned long)np,t,(unsigned long)np,t);
    np=np->nextslat;
  }
}

/* AIS: Generate bodies for slat expressions, args to UNKNOWN */
void emitslat(FILE* fp)
{
  node* np=firstslat;
  node* temp, *temp2;
  const char* t="ick_type32";
  while(np)
  {
    fprintf(fp,
	    "void ick_os%lx(%s a, void(*f)())\n{\n  static int l=0;\n"
	    "  if(l)\n  {\n    if(!f) ick_lose(IE994, ick_lineno, (const char *)NULL);\n"
	    "    f(a,0);\n    return;\n  }\n  l=1;\n",
	    (unsigned long)np,t);
    temp=cons(C_A, 0, 0);
    revprexpr(np, fp, temp); /* revprexpr can't free */
    fprintf(fp,"  l=0;\n}\n");
    fprintf(fp,"%s ick_og%lx(%s t)\n{\n  %s a;\n  static int l=0;\n"
	    "  if(l) return t;\n  l=1;\n  a=",
	    t,(unsigned long)np,t,t);
    prexpr(np, fp, 0);
    fprintf(fp,";\n  l=0;\n  return a;\n}\n");
    np=np->nextslat;
  }
  np=firstslat;
  /* Note that the order in which the parser assembles nodes means
     that we have to free the nodes in reverse order so we don't
     free child nodes twice. */
  temp2=0;
  while(np)
  {
    temp=np->nextslat;
    np->nextslat=temp2; /* reverse the chain */
    temp2=np;
    np=temp;
  }
  np=temp2;
  while(np)
  {
    temp=np->nextslat;
    nodefree(np);
    np=temp;
  }
  /* JH: clear firstslat */
  firstslat = 0;
}

/* feh.c ends here */
