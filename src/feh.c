/****************************************************************************

NAME
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

static int emitlineno;   /* line number for errors encountered during emit */

/*************************************************************************
 *
 * Node allocation functions.
 *
 * Nodes are used to represent expresion trees. The emit() function
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

    /* if it's already on the oblist, return its intindex */
    for (x = oblist; x < obdex; x++)
	if (x->type == type && x->extindex == index)
	    return(x->intindex);

    /* else we must intern a new symbol */
    if (obdex >= oblist + MAXVARS)
	lose(E333, yylineno, (char *)NULL);
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
 * Tuple allocation functions.
 *
 **************************************************************************/

void treset(void)
{
    memset(tuples, '\0', sizeof(tuple) * MAXLINES);
    nonespots = ntwospots = ntails = nhybrids = 0;
    obdex = oblist;
    lineno = 0;
}

tuple *newtuple(void)
/* allocate and zero out a new expression tuple */
{
    if (lineno >= MAXLINES)
    {
	lose(E666, yylineno, (char *)NULL);
	return NULL;
    }
    else
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
 **************************************************************************/

void codecheck(void)
{
    tuple	*tp, *up;

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
    for (tp = tuples; tp < tuples + lineno; tp++)
	if (tp->type == NEXT
	    || tp->type == ABSTAIN || tp->type == REINSTATE
	    || tp->type == COME_FROM)
	{
	    bool	foundit = FALSE;

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
	        if (up->comefrom)
		    lose(E555, yylineno, (char *)NULL);
		else
		    up->comefrom = tp - tuples + 1;
	    }
	    /* this substitutes line numbers for label numbers */
	    else if (tp->type != NEXT)
	    {
		tp->u.target = up - tuples + 1;
	    }
	}
}

/*************************************************************************
 *
 * Optimizer code.
 *
 * It's not a very good optimizer, is it?
 *
 **************************************************************************/

#define ISCONSTANT(np, v)	((np->opcode == MESH || np->opcode == MESH32) && np->constant == v)

static void rfree(node *np)
/* recursively free the given node and all nodes underneath */
{
    if (np->lval != (node *)NULL)
	rfree(np->lval);
    if (np->rval != (node *)NULL)
	rfree(np->rval);
    free(np);    
}

static int requal(node *mp, node *np)
/* do two node trees represent the same expression? */
{
    if (mp == (node *)NULL && np == (node *)NULL)
	return(TRUE);
    else if (mp == (node *)NULL || np == (node *)NULL)
	return(FALSE);
    else if (mp->opcode != np->opcode || mp->constant != np->constant)
	return(FALSE);
    else if (!requal(mp->lval, np->lval) || !requal(mp->lval, np->lval))
	return(FALSE);
    else
	return(TRUE);
}

void optimize(node *np)
{
    node	*mingleop, *op;

    /* recurse so we simplify each node after all its subnodes */
    if (np == (node *)NULL)
	return;
    else if (np->lval != (node *)NULL)
	optimize(np->lval);
    if (np->rval != (node *)NULL)
	optimize(np->rval);

    /*
     * OK, now do complete folding of constant expressions.
     */

    /* fold MINGLE operations on constants */
    if (np->opcode==MINGLE && (np->lval->opcode==MESH&&np->rval->opcode==MESH))
    {
	np->opcode = MESH32;
	np->constant = mingle(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
    }

    /* fold SELECT operations on constants */
    if (np->opcode == SELECT
	&& ((np->lval->opcode==MESH || np->lval->opcode==MESH32)
	    && (np->rval->opcode==MESH || np->rval->opcode==MESH32)))
    {
	np->opcode = np->rval->opcode;
	np->constant = iselect(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
    }

    /* fold AND operations on 16-bit constants */
    if (np->opcode == AND && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = and16(np->rval->constant);
	free(np->rval);
    }

    /* fold AND operations on 32-bit constants */
    if (np->opcode == AND && np->rval->opcode == MESH32)
    {
	np->opcode = MESH32;
	np->constant = and32(np->rval->constant);
	free(np->rval);
    }

    /* fold OR operations on 16-bit constants */
    if (np->opcode == OR && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = or16(np->rval->constant);
	free(np->rval);
    }

    /* fold OR operations on 32-bit constants */
    if (np->opcode == OR && np->rval->opcode == MESH32)
    {
	np->opcode = MESH32;
	np->constant = or32(np->rval->constant);
	free(np->rval);
    }

    /* fold XOR operations on 16-bit constants */
    if (np->opcode == XOR && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = xor16(np->rval->constant);
	free(np->rval);
    }

    /* fold XOR operations on 32-bit constants */
    if (np->opcode == XOR && np->rval->opcode == MESH32)
    {
	np->opcode = MESH32;
	np->constant = xor32(np->rval->constant);
	free(np->rval);
    }

    /*
     * Less trivial stuff begins here
     */

    /* equality test by XOR */
    if (np->opcode == TESTNZ && np->rval->opcode == XOR)
    {
	node	*tp = np->rval;

	np->opcode = EQUALS;
	np->lval = tp->lval;
	np->rval = tp->rval;
	free(np->rval);
    }

    /* check for non-zeroness test */
    if (np->opcode == SELECT && ISCONSTANT(np->rval, 1) && requal(np->lval->rval, np->lval->lval))
    {
	rfree(np->rval);
	np->rval = np->lval->rval;
	rfree(np->lval->lval);
	np->lval = (node *)NULL;
	np->opcode = TESTNZ;
	np->width = 16;
    }

    /* following optimizations only work in binary */
    if (Base != 2)
      return;

    /* recognize the idioms for various C binary logical operations */
    if (np->opcode == SELECT
	&& np->rval->opcode == MESH32 && np->rval->constant == 0x55555555
	&& (op = np->lval) 
	    && (op->opcode == AND || op->opcode == OR || op->opcode == XOR)
	&& (mingleop = op->rval) && mingleop->opcode == MINGLE
	&& mingleop->rval->width == 16
	&& mingleop->lval->width == 16)
    {
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

    /* recognize idioms for ~ */
    if (np->opcode == XOR)
    {
	if (np->rval->width == 16
	    && np->lval->opcode == MESH &&np->lval->constant == 0xffff)
	{
	    np->opcode = C_NOT;
	    free(np->lval);
	}

	if (np->lval->width == 16
	    && np->rval->opcode == MESH && np->rval->constant == 0xffff)
	{
	    np->opcode = C_NOT;
	    np->rval = np->lval;
	    free(np->rval);
	}

	if (np->rval->width == 32
	    && np->lval->opcode == MESH32 && np->lval->constant == 0xffffffffL)
	{
	    np->opcode = C_NOT;
	    free(np->lval);
	}

	if (np->lval->width == 32
	    && np->rval->opcode == MESH32 && np->rval->constant == 0xffffffffL)
	{
	    np->opcode = C_NOT;
	    np->rval = np->lval;
	    free(np->rval);
	}
    }
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
    "GIVE_UP",
    "READ_OUT",
    "WRITE_IN",
    "COME_FROM",
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

static void prexpr(node *np, FILE *fp);

static void prvar(node *np, FILE *fp)
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
	  prvar(np->lval, fp);

	  dim = 0;
	  for (sp = np->rval ; sp ; sp = sp->rval)
	    dim++;
	  (void) fprintf(fp, ", %d", dim);

	  for (sp = np->rval ; sp ; sp = sp->rval) {
	    (void) fprintf(fp, ", ");
	    prexpr(sp->lval, fp);
	  }
	  (void) fprintf(fp, ")");
	}
	break;
    }
}

static void prexpr(node *np, FILE *fp)
/* print out C-function equivalent of an expression */
{
    switch (np->opcode)
    {
    case MINGLE:
	(void) fprintf(fp, "mingle(");
	prexpr(np->lval, fp);
	(void) fprintf(fp, ", ");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case SELECT:
	(void) fprintf(fp, "iselect(");
	prexpr(np->lval, fp);
	(void) fprintf(fp, ", ");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case AND:
	(void) fprintf(fp, "and%d(", np->width);
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case OR:
	(void) fprintf(fp, "or%d(", np->width);
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case XOR:
	(void) fprintf(fp, "xor%d(", np->width);
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case EQUALS:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp);
	(void) fprintf(fp, " == ");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case FIN:
	if (Base < 3)
	  lose(E997, emitlineno, (char *)NULL);
	(void) fprintf(fp, "fin%d(", np->width);
	prexpr(np->rval, fp);
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
	prexpr(np->rval, fp);
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
	prvar(np, fp);
	break;

    case SUB:
	(void) fprintf(fp, "*(%s*)", nameof(np->lval->opcode, typedefs));
	prvar(np, fp);
	break;

	/* cases from here down are generated by the optimizer */
    case TESTNZ:
	(void) fprintf(fp, "(");
	prexpr(np->rval, fp);
	(void) fprintf(fp, " != 0)");
	break;

    case C_AND:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp);
	(void) fprintf(fp, " & ");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case C_OR:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp);
	(void) fprintf(fp, " | ");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case C_XOR:
	(void) fprintf(fp, "(");
	prexpr(np->lval, fp);
	(void) fprintf(fp, " ^ ");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case C_NOT:
	(void) fprintf(fp, "(~");
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;
    }

    (void) free(np);
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
    if (i) {
      (*cp++) = '\n';
      (*cp++) = '\t';
    }
    for (text = texts[i] ; *text ; cp++, text++) {
      if(*text == '"' || *text == '\\') {
	(*cp++) = '\\';
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
    (void) fprintf(fp, "    if (");
    if (tn->exechance < 100)
	(void) fprintf(fp, "roll(%d) && ", tn->exechance);
    (void) fprintf(fp, "!abstained[%d]) {\n", tn - tuples);
}

void emit(tuple *tn, FILE *fp)
/* emit code for the given tuple */
{
    node *np, *sp;
    int	dim;

    /* grind out label and source dump */
    if (yydebug || compile_only)
	(void) fprintf(fp, "    /* line %03d */\n", tn->lineno);
    if (tn->label)
	(void) fprintf(fp, "L%d:", tn->label);
    if (yydebug || compile_only)
	(void) fprintf(fp, "\t/* %s */", textlines[tn->lineno]);
    (void) fputc('\n', fp);

    /* set up the "next" lexical line number for error messages */
    if (tn->type == NEXT) {
	tuple *up;
	for (up = tuples; up < tuples + lineno; up++)
	    if (tn->u.target == up->label) {
		emitlineno = up->lineno;
		break;
	    }
    } else if (tn->comefrom)
	emitlineno = tuples[tn->comefrom-1].lineno;
    else if (tn < tuples + lineno - 1)
	emitlineno = tn[1].lineno;
    else
	emitlineno = yylineno;
    (void) fprintf(fp, "    lineno = %d;\n", emitlineno);

    /* emit random compiler bug */
    if (!nocompilerbug)
    {
#ifdef USG
	if ((lrand48() & 127) == 127)
#else
	if ((rand() & 127) == 127)
#endif
	    (void) fprintf(fp, "    lose(E774, lineno, (char *)0);\n");
    }

    /* emit conditional-execution prefixes */
    if (tn->type != COME_FROM)
	emit_guard(tn, fp);

    /* now emit the code for the statement body */
    switch(tn->type)
    {
    case GETS:
	np = tn->u.node;
	if (np->lval->opcode != SUB) {
	  sp = np->lval;
	  (void) fprintf(fp,"\t(void) assign((char*)&");
	}
	else {
	  sp = np->lval->lval;
	  (void) fprintf(fp,"\t(void) assign(");
	}
	prvar(np->lval, fp);
	(void) fprintf(fp,", %s", nameof(sp->opcode, vartypes));
	(void) fprintf(fp,", %s[%lu], ", nameof(sp->opcode, forgetbits),
			   sp->constant);
	prexpr(np->rval, fp);
	(void) fprintf(fp,");\n");
	break;

    case RESIZE:
	np = tn->u.node;
	dim = 0;
	for (sp = np->rval; sp; sp = sp->rval)
	  dim++;
	(void) fprintf(fp, "\tresize(");
	prvar(np->lval, fp);
	(void) fprintf(fp, ", %s[%lu]", nameof(np->lval->opcode, forgetbits),
		       np->lval->constant);
	(void) fprintf(fp, ", %d", dim);
	for (sp = np->rval; sp; sp = sp->rval) {
	  (void) fprintf(fp, ", ");
	  prexpr(sp->lval, fp);
        }
	(void) fprintf(fp, ");\n");
	break;

    case NEXT:
	(void) fprintf(fp,
		       "\tpushnext(%d); goto L%d; N%d:;\n",
		       tn - tuples + 1, tn->u.target, tn - tuples + 1);
	break;

    case RESUME:
	(void) fprintf(fp, "\tskipto = resume(");
	prexpr(tn->u.node, fp);
	(void) fprintf(fp, "); goto top;\n");
	break;

    case FORGET:
	(void) fprintf(fp, "\tpopnext(");
	prexpr(tn->u.node, fp);
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

    case ABSTAIN:
	(void) fprintf(fp, "\tabstained[%d] = TRUE;\n", tn->u.target - 1);
	break;

    case REINSTATE:
	(void) fprintf(fp, "\tabstained[%d] = FALSE;\n", tn->u.target - 1);
	break;

    case ENABLE:
	for (np = tn->u.node; np; np = np->rval)
	{
	    (void) fprintf(fp,
			   "\tint i;\n\n\tfor (i = 0; i < (int)(sizeof(linetype)/sizeof(int)); i++)\n");
	    (void) fprintf(fp,
			   "\t    if (linetype[i] == %s)\n", enablers[np->constant-GETS]);
	    (void) fprintf(fp,
			   "\t\tabstained[i] = FALSE;\n");
	}
	break;

    case DISABLE:
	for (np = tn->u.node; np; np = np->rval)
	{
	    (void) fprintf(fp,
			   "\tint i;\n\n\tfor (i = 0; i < (int)(sizeof(linetype)/sizeof(int)); i++)\n");
	    (void) fprintf(fp,
			   "\t    if (linetype[i] == %s)\n", enablers[np->constant-GETS]);
	    (void) fprintf(fp,
			   "\t\tabstained[i] = TRUE;\n");
	}
	break;

    case GIVE_UP:
	(void) fprintf(fp, "\treturn(0);\n");
	break;

    case WRITE_IN:
	for (np = tn->u.node; np; np = np->rval) {
	  if (np->lval->opcode == TAIL || np->lval->opcode == HYBRID) {
	    (void) fprintf(fp,"\tbinin(");
	    prvar(np->lval, fp);
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
	    prvar(np->lval, fp);
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
	    prvar(np->lval, fp);
	    (void) fprintf(fp,");\n");
	  }
	  else {
	    (void) fprintf(fp, "\tpout(");
	    prexpr(np->lval, fp);
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
	(void) fprintf(fp, "C%d:\n", tn->u.target);
	break;

    default:
	lose(E778, emitlineno, (char *)NULL);
	break;
    }

    if (tn->type != COME_FROM)
	(void) fprintf(fp, "    }\n");

    /*
     * If the statement that was just degenerated was a COME FROM target,
     * emit the code for the jump to the COME FROM.
     */
    if (tn->comefrom) {
	if (yydebug || compile_only)
	    (void) fprintf(fp,
			   "    /* line %03d is a suck point for the COME FROM at line %03d */\n",
			   tn->lineno, tuples[tn->comefrom-1].lineno);
	emit_guard(tuples + tn->comefrom - 1, fp);
	(void) fprintf(fp,
		       "\tgoto C%d;\n    }\n",
		       tuples[tn->comefrom-1].u.target);
    }
}

/* feh.c ends here */
