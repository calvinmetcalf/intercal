/****************************************************************************

NAME
   feh.c -- code-generator back-end for ick parser

DESCRIPTION
   This module provides storage manglement, code degeneration,
and optimizations of dubious value for the INTERCAL compiler.

****************************************************************************/
/*LINTLIBRARY */
#include <stdio.h>
#include "sizes.h"
#include "ick.h"
#include "y.tab.h"
#include "lose.h"

int yyerror()
{
    /* lose(E017, yylineno, (char *)NULL); */
}

/*************************************************************************
 *
 * Node allocation functions.
 *
 * Nodes are used to represent expresion trees. The emit() function
 * deallocates them.
 *
 **************************************************************************/

node *newnode()
/* allocate and zero out a new expression node */
{
    return((node *)calloc(sizeof(node), 1));
}

node *cons(type, car, cdr)
int type;
node *car, *cdr;
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

unsigned int intern(type, index)
{
    atom	*x;

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
 * Tuple allocation functions.
 *
 **************************************************************************/

void treset()
{
    memset(tuples, '\0', sizeof(tuple) * MAXLINES);
    nonespots = ntwospots = ntails = nhybrids = 0;
    obdex = oblist;
    lineno = 0;
}

tuple *newtuple()
/* allocate and zero out a new expression tuple */
{
    if (lineno >= MAXLINES)
	lose(E666, yylineno, (char *)NULL);
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

void typecast(np)
node *np;
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

void codecheck()
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

void rfree(np)
/* recursively free the given node and all nodes underneath */
node	*np;
{
    if (np->lval != (node *)NULL)
	rfree(np->lval);
    if (np->rval != (node *)NULL)
	rfree(np->rval);
    free(np);    
}

int requal(mp, np)
/* do two node trees represent the same expression? */
node *mp, *np;
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

void optimize(np)
node *np;
{
    /* recurse so we simplify each node after all its subnodes */
    if (np == (node *)NULL)
	return;
    else if (np->lval != (node *)NULL)
	optimize(np->lval);
    if (np->rval != (node *)NULL)
	optimize(np->rval);

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

    /* equality test by XOR */
    if (np->opcode == TESTNZ && np->rval->opcode == XOR)
    {
	node	*tp = np->rval;

	np->opcode = EQUALS;
	np->lval = tp->lval;
	np->rval = tp->rval;
	free(np->rval);
    }

    /* fold MINGLE operations on constants */
    if (np->opcode==MINGLE && (np->lval->opcode==MESH&&np->rval->opcode==MESH))
    {
	np->opcode = MESH32;
	np->constant = mingle(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
    }

    /* fold SELECT operations on constants */
    if (np->opcode == SELECT && (np->lval->opcode==MESH && np->rval->opcode==MESH))
    {
	np->opcode = MESH;
	np->constant = iselect(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
    }

    /* fold AND operations on constants */
    if (np->opcode == AND && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = and16(np->lval->constant, np->rval->constant);
	free(np->rval);
    }

    /* fold OR operations on constants */
    if (np->opcode == OR && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = or16(np->lval->constant, np->rval->constant);
	free(np->rval);
    }

    /* fold XOR operations on constants */
    if (np->opcode == XOR && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = xor16(np->lval->constant, np->rval->constant);
	free(np->rval);
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
    ONESPOT,	"ONESPOT",
    TWOSPOT,	"TWOSPOT",
    TAIL,	"TAIL",
    HYBRID,	"HYBRID",
    0,		(char *)NULL
};

assoc forgetbits[] =
{
    ONESPOT,	"oneforget",
    TWOSPOT,	"twoforget",
    TAIL,	"tailforget",
    HYBRID,	"hyforget",
    0,		(char *)NULL
};

assoc varstores[] =
{
    ONESPOT,	"onespots",
    TWOSPOT,	"twospots",
    TAIL,	"tails",
    HYBRID,	"hybrids",
    0,		(char *)NULL
};

assoc typedefs[] =
{
    ONESPOT,	"type16",
    TWOSPOT,	"type32",
    TAIL,	"type16",
    HYBRID,	"type32",
    0,		(char *)NULL
};

char *nameof(value, table)
/* return string corresponding to value in table */
int	value;
assoc	table[];
{
    assoc	*ap;

    for (ap = table; ap->name; ap++)
	if (ap->value == value)
	    return(ap->name);
    return((char *)NULL);
}

void prvar(np, fp)
/* print out args to pass to storage manager for reference */
node	*np;
FILE	*fp;
{
    node	*sp;
    int		dim;

    switch (np->opcode)
    {
    case ONESPOT:
	(void) fprintf(fp, "onespots[%d]", np->constant);
	break;

    case TWOSPOT:
	(void) fprintf(fp, "twospots[%d]", np->constant);
	break;

    case TAIL:
	(void) fprintf(fp, "TAIL, &tails[%d]", np->constant);
	break;

    case HYBRID:
	(void) fprintf(fp, "HYBRID, &hybrids[%d]", np->constant);
	break;

    case SUB:
	{
	  void prexpr();
	  node *sp;

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

void prexpr(np, fp)
/* print out C-function equivalent of an expression */
FILE	*fp;
node	*np;
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
	  lose(E997, lineno, (char *)NULL);
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
	  lose(E997, lineno, (char *)NULL);
	(void) fprintf(fp, "whirl%d(%d, ", np->width, np->opcode - WHIRL + 1);
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case MESH:
	(void) fprintf(fp, "0x%x", np->constant);
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
    }

    (void) free(np);
}

char *nice_text(text)
char *text;
{
#define MAXNICEBUF	512
  static char buf[MAXNICEBUF];
  char *cp;
  
  for(cp = buf;*text;cp++,text++) {
      if(*text == '"' || *text == '\\') {
	  (*cp++) = '\\';
      }
      *cp = *text;
  }
  *cp = '\0';
  return buf;
}

void emit(tn, fp)
/* emit code for the given tuple */
tuple	*tn;
FILE	*fp;
{
    node *np, *sp;
    int	dim;
    char *name;
    static int make_cf_target = 0;

    if (yydebug || compile_only)
	(void) fprintf(fp, "    /* line %03d: %s */\n",
		   tn - tuples + 1,
		   textlines[tn-tuples+1]);

    /* don't make a label if we are emitting the jump to a COME FROM */
    if (make_cf_target)
	tn->label = 0;
    if (tn->label)
	(void) fprintf(fp, "L%d:\n", tn->label);

    /* if this is a COME FROM statement, just make a label and jump to end */
    if (tn->type == COME_FROM && !make_cf_target)
    {
	(void) fprintf(fp, "C%d:\n", tn->u.target);
	goto comefrom;
    }

    /* emit conditional-execution prefixes */
    (void) fprintf(fp, "    if (");
    if (tn->exechance < 100)
	(void) fprintf(fp, "roll(%d) && ", tn->exechance);
    (void) fprintf(fp, "!abstained[(lineno = %d)-1])\n", tn - tuples + 1);
    (void) fprintf(fp, "    {\n");

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
	(void) fprintf(fp,", %s[%d], ", nameof(sp->opcode, forgetbits),
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
	(void) fprintf(fp, ", %s[%d]", nameof(np->lval->opcode, forgetbits),
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
	    (void) fprintf(fp, "\tstash(%s, %d, %s+%d);\n",
			  nameof(np->opcode, vartypes),
			  np->constant,
			  nameof(np->opcode, varstores), np->constant);
	break;

    case RETRIEVE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\tretrieve(%s+%d, %s, %d, %s[%d]);\n",
			   nameof(np->opcode, varstores), np->constant,
			   nameof(np->opcode, vartypes),
			   np->constant,
			   nameof(np->opcode, forgetbits),
			   np->constant);
	break;

    case IGNORE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t%s[%d] = TRUE;\n",
		       nameof(np->opcode, forgetbits),
		       np->constant);
	break;

    case REMEMBER:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t%s[%d] = FALSE;\n",
		       nameof(np->opcode, forgetbits),
		       np->constant);
	break;

    case ABSTAIN:
	(void) fprintf(fp, "\tabstained[%d-1] = TRUE;\n", tn->u.target);
	break;

    case REINSTATE:
	(void) fprintf(fp, "\tabstained[%d-1] = FALSE;\n", tn->u.target);
	break;

    case ENABLE:
	for (np = tn->u.node; np; np = np->rval)
	{
	    (void) fprintf(fp,
			   "\tint i;\n\n\tfor (i = 0; i < sizeof(linetype)/sizeof(int); i++)\n");
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
			   "\tint i;\n\n\tfor (i = 0; i < sizeof(linetype)/sizeof(int); i++)\n");
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
	    (void) fprintf(fp, ", %s[%d]",
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
	    (void) fprintf(fp,", %s[%d]", nameof(sp->opcode, forgetbits),
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

    case COME_FROM:
	(void) fprintf(fp,"\tgoto C%d;\n", tn->u.target);
	break;

    case SPLATTERED:
	/*
	fprintf(stderr,"compiling a splat... line = %d (%s)\n",
		tn->lineno,textlines[tn->lineno]);
		*/
	(void) fprintf(fp,"\t(void) puts(\"%s\");\n",
		       nice_text(textlines[tn->lineno]));
	(void) fprintf(fp,
		       "\texit(%d);\n", tn->lineno);
	break;

    default:
	lose(E778, tn - tuples + 1, (char *)NULL);
	break;
    }

    (void) fprintf(fp, "    }\n");

  comefrom:

    /* if the statement that was just degenerated was a COME FROM target,
       emit the code for the jump to the COME FROM. */

    if (tn->comefrom && !make_cf_target) {
      make_cf_target = 1;
      emit(tuples + tn->comefrom - 1, fp);
      make_cf_target = 0;
    }
}

/* feh.c ends here */
