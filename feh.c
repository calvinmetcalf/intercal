/****************************************************************************

NAME
   feh.c -- code-generator back-end for ick parser

DESCRIPTION
   This module provides storage manglement and code degeneration
for the INTERCAL compiler.

****************************************************************************/
/*LINTLIBRARY */
#include <stdio.h>
#include <signal.h>
#include "ick.h"
#include "y.tab.h"
#include "lose.h"

extern unsigned int mingle(), select();
extern unsigned int and16(), or16(), xor16(), and32(), or32(), xor32();

extern char linebuf[];
extern int  yydebug, yylineno;

/* compilation options */
static bool dooptimize;	/* do optimizations? (controlled by -O) */
static bool clockface;	/* set up output to do IIII for IV */

#define SKELETON	"ick-wrapper.c"

int lineno;	/* current source line number; lose() uses it */

int yyerror()
{
    lose(yylineno, E017);
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
}

/*************************************************************************
 *
 * Variable-name mapping
 *
 * This permits us to optimize use of variable storage at runtime
 *
 **************************************************************************/

/* this maps the `external' name of a variable to an internal array index */
typedef struct
{
    int	type;
    int extindex;
    int intindex;
}
atom;

static atom oblist[MAXSTASHES], *obdex;
static int nonespots, ntwospots, ntails, nhybrids;

unsigned int intern(type, index)
{
    atom	*x;

    /* if it's already on the oblist, return its intindex */
    for (x = oblist; x < obdex; x++)
	if (x->type == type && x->extindex == index)
	    return(x->intindex);

    /* else we must intern a new symbol */
    if (obdex >= oblist + MAXSTASHES)
	lose(yylineno, E333);
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

static tuple tuples[MAXLINES];

static void treset()
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
	lose(yylineno, E666);
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

static void typecast(np)
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
    else if (np->opcode==TWOSPOT ||np->opcode==HYBRID ||np->opcode==MINGLE)
	np->width = 32;
    else if (np->opcode == AND || np->opcode == OR || np->opcode == XOR)
	np->width = np->rval->width;
    else if (np->opcode == SELECT)
	np->width = np->rval->width;	/* n-bit select has an n-bit result */
    else if (np->opcode == SUB)
	np->width = np->rval->width;	/* tpe of the array */
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

static void codecheck()
{
    tuple	*tp, *up;

    /* check for assignment type mismatches */
    for (tp = tuples; tp < tuples + lineno; tp++)
	if (tp->type == GETS)
	    if (tp->u.node->lval->width == 16 && tp->u.node->lval->width == 32)
		lose(tp - tuples + 1, E275);

    /* check for duplicate labels */
    for (tp = tuples; tp < tuples + lineno; tp++)
	if (tp->label)
	    for (up = tuples; up < tuples + lineno; up++)
		if (tp != up && tp->label == up->label)
		    lose(tp - tuples + 1, E182);

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
		    lose(tp - tuples + 1, E129);
		else if (tp->type == COME_FROM)
		    lose(tp - tuples + 1, E444);
		else
		    lose(tp - tuples + 1, E139);
	    }
	    /* this substitutes line numbers for label numbers */
	    else if (tp->type != NEXT)
	    {
		if (tp->type == COME_FROM)
		    if (up->comefrom)
			lose(yylineno, E555);
		    else
			up->comefrom = tp - tuples + 1;
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

/* optimizer node codes -- must make sure these don't collide with y.tab.h */
#define TESTNZ	300	/* test for non-zero */

#define ISCONSTANT(np, v)	(np->opcode == MESH && np->constant == v)

static void rfree(np)
/* recursively free the given node and all nodes underneath */
node	*np;
{
    if (np->lval != (node *)NULL)
	rfree(np->lval);
    if (np->rval != (node *)NULL)
	rfree(np->rval);
    free(np);    
}

static int requal(mp, np)
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

static void optimize(np)
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

    /* fold MINGLE operations on constants */
    if (np->opcode==MINGLE && (np->lval->opcode==MESH&&np->rval->opcode==MESH))
    {
	np->opcode = MESH;
	np->constant = mingle(np->lval->constant, np->rval->constant);
	free(np->lval);
	free(np->rval);
    }

    /* fold SELECT operations on constants */
    if (np->opcode == SELECT && (np->lval->opcode==MESH && np->rval->opcode==MESH))
    {
	np->opcode = MESH;
	np->constant = select(np->lval->constant, np->rval->constant);
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
    if (np->opcode == AND && np->rval->opcode == MESH)
    {
	np->opcode = MESH;
	np->constant = xor16(np->lval->constant, np->rval->constant);
	free(np->rval);
    }
}

/*************************************************************************
 *
 * Code generation
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

typedef struct
{
    int	value;
    char *name;
}
assoc;

/*
 * If the order of statement-token defines in ick.y ever changes,
 * this will need to be resolved.
 */
static char *enablers[] =
{
    /* GETS      */	"calculating",
    /* RESIZE    */	"calculating",
    /* NEXT      */	"nexting",
    /* FORGET    */	"forgetting",
    /* RESUME    */	"resuming",
    /* STASH     */	"stashing",
    /* RETRIEVE  */	"retrieving",
    /* IGNORE    */	"ignoring",
    /* REMEMBER  */	"remembering",
    /* ABSTAIN   */	"abstaining",
    /* REINSTATE */	"reinstating",
    /* DISABLE   */	"abstaining",
    /* ENABLE    */	"reinstating",
    /* GIVE_UP   */	"givingup",	/* never used */
    /* READ_OUT  */	"readingout",
    /* WRITE_IN  */	"writingin",
    /* COME_FROM */	"comingfrom",
};

static assoc vartypes[] =
{
    ONESPOT,	"ONESPOT",
    TWOSPOT,	"TWOSPOT",
    TAIL,	"TAIL",
    HYBRID,	"HYBRID",
    0,		(char *)NULL
};

static assoc forgetbits[] =
{
    ONESPOT,	"oneforget",
    TWOSPOT,	"twoforget",
    TAIL,	"tailforget",
    HYBRID,	"hyforget",
    0,		(char *)NULL
};

static assoc varstores[] =
{
    ONESPOT,	"onespot",
    TWOSPOT,	"twospot",
    TAIL,	"tails",
    HYBRID,	"hybrids",
    0,		(char *)NULL
};

static assoc typedefs[] =
{
    ONESPOT,	"short",
    TWOSPOT,	"int",
    TAIL,	"unsigned short *",
    HYBRID,	"unsigned int *",
    0,		(char *)NULL
};

static char *nameof(value, table)
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

static void prvar(np, fp)
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
	(void) fprintf(fp, "TAIL, %d", np->constant);
	break;

    case HYBRID:
	(void) fprintf(fp, "HYBRID, %d", np->constant);
	break;

    case SUB:
	dim = 0;
	for (sp = np->rval; sp; sp = sp->rval)
	    dim++;
	(void) fprintf(fp, "%s, %d, %d, ",
		       nameof(np->lval->opcode, vartypes),
		       np->lval->constant,
		       dim);
	for (np = np->rval; np; np = np->rval)
	{
	    extern void prexpr();

	    prexpr(np->lval, fp);
	    (void) fprintf(fp, ", ");
        }
	(void) fprintf(" 0");
	break;
    }
}

static void prexpr(np, fp)
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
	(void) fprintf(fp, "select(");
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
	(void) fprintf(fp, "or%d(", np->width);
	prexpr(np->rval, fp);
	(void) fprintf(fp, ")");
	break;

    case MESH:
	(void) fprintf(fp, "0x%x", np->constant);
	break;

    case ONESPOT:
    case TWOSPOT:
	prvar(np, fp);
	break;

    case SUB:
	(void) fprintf(fp, "aget(%s");
	prvar(np, fp);
	(void) fprintf(fp, ")");
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

void emit(tn, fp)
/* emit code for the given tuple */
tuple	*tn;
FILE	*fp;
{
    node *np, *sp;
    int	dim;

    if (yydebug)
	(void) fprintf(fp, "    /* line %03d: %s */\n",
		   tn - tuples + 1,
		   textlines[tn-tuples+1]);

    if (tn->label)
	(void) fprintf(fp, "L%d:\n", tn->label);

    if (tn->type == COME_FROM)
    {
	(void) fprintf(fp, "C%d:\n", tn - tuples + 1);
	return;
    }

    /* emit conditional-execution prefixes */
    if (tn->type == NEXT)
    {
	(void) fprintf(fp, "    if (skipto == %d) skipto = 0; else\n", tn-tuples+1);
	(void) fprintf(fp, "    if (!skipto && ");
    }
    else
	(void) fprintf(fp, "    if (!skipto && ", tn - tuples + 1);
    if (tn->exechance && tn->exechance < 100)
	(void) fprintf(fp, "roll(%d) && ", tn->exechance);
    if (tn->type == GIVE_UP)
	(void) fprintf(fp, "!abstained[lineno = %d])\n", tn - tuples + 1);
    else
	(void) fprintf(fp, "!abstained[lineno = %d] && %s)\n",
		  tn-tuples + 1, enablers[tn->type-GETS]);
    (void) fprintf(fp, "    {\n");

    if (tn->comefrom)
	(void) fprintf(fp,
	       "\tif (!abstained[%d] && comingfrom)\t/* COME FROM */\n\t    goto C%d;\n",
	       tn->comefrom, tn->comefrom);

    /* now emit the code for the statement body */
    switch(tn->type)
    {
    case GETS:
	np = tn->u.node;
	(void) fprintf(fp,"\tif (!%s[%d])\n\t    ",
		       nameof(np->lval->opcode, forgetbits),
		       np->constant);
	if (np->opcode == ONESPOT || np->opcode == TWOSPOT)
	{
	    prvar(np->lval, fp);
	    (void) fprintf(fp, " = ");
	    prexpr(tn->u.node->rval, fp);
	}
	else
	{
	    (void) fprintf("aput(");
	    prexpr(tn->u.node->rval, fp);
	    (void) fprintf(", ");
	    prvar(np->lval, fp);
	    (void) fprintf(fp, ")");
	}
	(void) fprintf(fp, ";\n");
	break;

    case RESIZE:
	(void) fprintf(fp, "\tresize(");
	prvar(tn->u.node->lval, fp);
	dim = 0;
	for (sp = np->rval; sp; sp = sp->rval)
	    dim++;
	(void) fprintf(fp, " %d,", dim);
	for (np = np->rval; np; np = np->rval)
	{
	    prexpr(np->lval, fp);
	    fprintf(fp, ", ");
        }
	(void) fprintf(fp, "0");
        (void) fprintf(fp, ");\n");
	break;

    case NEXT:
	(void) fprintf(fp, "\tpushnext(%d); goto L%d;\n",tn - tuples + 1,tn->u.target);
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
	    (void) fprintf(fp, "\tstash(%s, %d, %s[%d]);\n",
			  nameof(np->opcode, vartypes),
			  np->constant,
			  nameof(np->opcode, varstores), np->constant);
	break;

    case RETRIEVE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\tretrieve(&%s[%d], %s, %d);\n",
			  nameof(np->opcode, varstores), np->constant,
			  nameof(np->opcode, vartypes),
			  np->constant);
	break;

    case IGNORE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t%s[%d] = TRUE;\n",
		       nameof(np->lval->opcode, forgetbits),
		       np->constant);
	break;

    case REMEMBER:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp,"\t%s[%d] = FALSE;\n",
		       nameof(np->lval->opcode, forgetbits),
		       np->constant);
	break;

    case ABSTAIN:
	(void) fprintf(fp, "\tabstained[%d] = FALSE;\n", tn->u.target);
	break;

    case REINSTATE:
	(void) fprintf(fp, "\tabstained[%d] = TRUE;\n", tn->u.target);
	break;

    case ENABLE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\t%s = TRUE;\n", enablers[np->constant-GETS]);
	break;

    case DISABLE:
	for (np = tn->u.node; np; np = np->rval)
	    (void) fprintf(fp, "\t%s = FALSE;\n", enablers[np->constant-GETS]);
	break;

    case GIVE_UP:
	(void) fprintf(fp, "\treturn(0);\n");
	break;

    case WRITE_IN:
	for (np = tn->u.node; np; np = np->rval)
	{
	    np = tn->u.node;
	    (void) fprintf(fp,"\tif (!%s[%d])\n\t    ",
			   nameof(np->lval->opcode, forgetbits),
			   np->constant);
	    if (np->opcode == ONESPOT || np->opcode == TWOSPOT)
	    {
		prvar(np->lval, fp);
		(void) fprintf(fp, " = pin()");
	    }
	    else
	    {
		(void) fprintf("aput(pin(), ");
		prvar(np->lval, fp);
		(void) fprintf(fp, ")");
	    }
	    (void) fprintf(fp, ";\n");
	}
	break;

    case READ_OUT:
	for (np = tn->u.node; np; np = np->rval)
	{
	    (void) fprintf(fp, "\tpout(");
	    prexpr(np->lval, fp);
	    (void) fprintf(fp, ");\n");
	}
	break;

    case SPLATTERED:
	(void) fprintf(fp,
		       "(void) puts(\"%s\");\n", textlines[tn - tuples]);
	(void) fprintf(fp,
		       "exit(%d);\n", tn - tuples);
	break;

    default:
	lose(tn - tuples + 1, E778);
	break;
    }

    (void) fprintf(fp, "    }\n");
}

/*************************************************************************
 *
 * Driver code
 *
 **************************************************************************/

void abend()
{
    lose(yylineno, E778);
}

main(argc, argv)
int	argc;
char	*argv[];
{
    extern char	*optarg;	/* set by getopt */
    extern int	optind;		/* set by getopt */
    char	buf[BUFSIZ], buf2[BUFSIZ];
    tuple	*tp;
    atom	*op;
    int	c;

    while ((c = getopt(argc, argv, "dOC")) != EOF)
    {
	switch (c)
	{
	case 'd':
	    yydebug = 1;
	    break;

	case 'C':
	    clockface = TRUE;
	    break;

	case 'O':
	    dooptimize = TRUE;
	    break;
	}
    }

    (void) signal(SIGSEGV, abend);
    (void) signal(SIGBUS, abend);

    for (; optind < argc; optind++)
	if (freopen(argv[optind], "r", stdin) == (FILE *)NULL)
	    lose(0, E777);
	else
	{
	    FILE	*ifp, *ofp;
	    int		maxabstain;

	    /* zero out tuple and oblist storage */
	    treset();

	    /* compile tuples from current input source */
	    yyparse();	

	    /* 
	     * Now propagate type information up the expression tree.
	     * We need to do this because the unary-logical operations
	     * are sensitive to the type widths of their operands, so
	     * we have to generate different code depending on the
	     * deducible type of the operand.
	     */
	    for (tp = tuples; tp < tuples + lineno; tp++)
		if (tp->type == GETS || tp->type == RESIZE)
		    typecast(tp->u.node);

	    codecheck();	/* check for compile-time errors */

	    /* perform optimizations */
	    if (dooptimize)
		for (tp = tuples; tp < tuples + lineno; tp++)
		    if (tp->type == GETS || tp->type == RESIZE)
			optimize(tp->u.node);

	    /* strip off the file extension */
	    argv[optind][strlen(argv[optind]) - 2] = '\0';

	    /* set up the generated C output file name */
	    (void) strcpy(buf, argv[optind]);
	    (void) strcat(buf, ".c");
	    if ((ofp = fopen(buf, "w")) == (FILE *)NULL)
		lose(0, E888);

	    /* now substitute in tokens in the skeleton */
	    if ((ifp = fopen(SKELETON, "r")) == (FILE *)NULL)
		lose(0, E999);
	    buf[strlen(buf) - 2] = '\0';

	    while ((c = fgetc(ifp)) != EOF)
		if (c != '$')
		    (void) fputc(c, ofp);
	        else switch(fgetc(ifp))
		{
		case 'A':			/* source name stem */
		    (void) fputs(buf, ofp);
		    break;

		case 'B':			/* # of source lines */
		    (void) fprintf(ofp, "%d", lineno);
		    break;

		case 'C':			/* initial abstentions */
		    maxabstain = 0;
		    for (tp = tuples; tp < tuples + lineno; tp++)
			if (tp->exechance == 0 && tp - tuples + 1 > maxabstain)
			    maxabstain = tp - tuples + 1;
		    if (maxabstain)
		    {
			(void) fprintf(ofp, " = {");
			for (tp = tuples; tp < tuples + maxabstain; tp++)
			    (void) fprintf(ofp, "%d, ", !tp->exechance);
			(void) fprintf(ofp, "}");
		    }
		    break;

		case 'D':			/* extern to intern map */
		    if (nonespots)
		    {
			(void) fprintf(ofp,
				       "static unsigned int onespots[%d];\n",
				       nonespots);
			(void) fprintf(ofp,
				       "static bool oneforget[%d];\n",
				       nonespots);
		    }
		    if (ntwospots)
		    {
			(void) fprintf(ofp,
				       "static unsigned int twospots[%d];\n",
				       ntwospots);
			(void) fprintf(ofp,
				       "static bool twoforget[%d];\n",
				       ntwospots);
		    }
		    if (ntails)
		    {
			(void) fprintf(ofp,
				       "static unsigned int *tails[%d];\n",
				       ntails);
			(void) fprintf(ofp,
				       "static bool tailforget[%d];\n",
				       ntails);
		    }
		    if (nhybrids)
		    {
			(void) fprintf(ofp,
				       "static unsigned int *hybrids[%d];\n",
				       nhybrids);
			(void) fprintf(ofp,
				       "static bool hyforget[%d];\n",
				       nhybrids);
		    }
		    if (yydebug)
			for (op = oblist; op < obdex; op++)
			    (void) fprintf(ofp, "/* %s %d -> %d */\n",
					   nameof(op->type, vartypes),
					   op->extindex,
					   op->intindex);
		    break;

		case 'E':
		    if (clockface)
			(void) fprintf(ofp, "clockface(TRUE);");
		    break;

		case 'F':			/* degenerated code */
		    for (tp = tuples; tp < tuples + lineno; tp++)
			emit(tp, ofp);
		    break;
		}

	    (void) fclose(ifp);
	    (void) fclose(ofp);

	    /* OK, now sic the C compiler on the results */
	    if (!yydebug)
	    {
		(void) sprintf(buf2, "cc %s.c -I. -L. -lick -o %s", buf, buf);
		(void) system(buf2);
		(void) strcat(buf, ".c");
		(void) unlink(buf);
	    }
	}
}

/* feh.c ends here */
