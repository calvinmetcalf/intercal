/*****************************************************************************

NAME
    ick.y -- grammar for the INTERCAL language

DESCRIPTION
   This YACC grammar parses the INTERCAL language by Don R. Woods and
James M. Lyon.

*****************************************************************************/

%{
#include <stdio.h>
#include "ick.h"
#include "lose.h"

extern node *newnode(), *cons();
extern tuple *newtuple();
extern unsigned int intern();
extern int yylineno;

static node *rlist;	/* pointer to current right-hand node list */
static node *llist;	/* pointer to current left-hand node list */
static node *np;	/* variable for building node lists */

#define ACTION(x, nt, nn)	x = newtuple(); x->type = nt; x->u.node = nn
#define TARGET(x, nt, nn)	x = newtuple(); x->type = nt; x->u.target = nn
%}

%start program

%union
{
    int		numval;		/* a numeric value */
    tuple	*tuple;		/* a code tuple */
    node	*node;		/* an expression-tree node */
}

/*
 * Don't change this statement token list gratuitously!
 * Some code in feh.c depends on GETS being the least
 * statement type and on the order of the ones following.
 */
%token GETS RESIZE NEXT FORGET RESUME STASH RETRIEVE IGNORE REMEMBER ABSTAIN
%token REINSTATE DISABLE ENABLE GIVE_UP READ_OUT WRITE_IN COME_FROM

%token DO PLEASE NOT MESH ONESPOT TWOSPOT TAIL HYBRID
%token MINGLE SELECT SPARK EARS SUB BY GERUND BADCHAR

%token <numval> NUMBER UNARY OHOHSEVEN GERUND LABEL
%token <node> INTERSECTION

%token NEWLINE

%type <node> expr varlist outlist variable constant lvalue inlist outlist
%type <node> subscr byexpr scalar array initem outitem sublist
%type <tuple> perform
%type <numval> please

%%	/* beginning of rules section */

/* A program description consists of a sequence of statements */
program	:    /* EMPTY */
	|    program statemnt NEWLINE
	;

statemnt:    command
	|    error
		{splat();}
	;

/*
 * Each command consists of an optional label, followed by a preamble,
 * followed by the statement body
 */
command	:    /* EMPTY */	/* blank line is OK */
	|    please perform
		{$2->label = 0; $2->exechance = $1;}
	|    please OHOHSEVEN perform
		{$3->label = 0; $3->exechance = $2;}
	|    LABEL please perform
		{$3->label = $1; $3->exechance = $2;}
	|    LABEL please OHOHSEVEN perform
		{$4->label = $1; $4->exechance = $2;}
	;

/* There are several forms of preamble */
please	:    DO			{$$ = 100;}
	|    NOT		{$$ = 0;}
	|    DO NOT		{$$ = 0;}
	|    PLEASE		{$$ = 100;}
	|    DO PLEASE		{$$ = 100;}
	|    PLEASE DO		{$$ = 100;}
	|    PLEASE NOT		{$$ = 0;}
	;

/* Here's how to parse statement bodies */
perform :    lvalue GETS expr		{ACTION($$, GETS,cons(GETS,$1,$3));}
	|    array GETS byexpr		{ACTION($$,RESIZE,cons(RESIZE,$1,$3));}
	|    LABEL NEXT			{TARGET($$, NEXT,      $1);}
	|    FORGET expr		{ACTION($$, RESUME,    $2);}
	|    RESUME expr		{ACTION($$, RESUME,    $2);}
	|    STASH varlist		{ACTION($$, STASH,     rlist);}
	|    RETRIEVE varlist		{ACTION($$, RETRIEVE,  rlist);}
	|    IGNORE varlist		{ACTION($$, IGNORE,    rlist);}
	|    REMEMBER varlist		{ACTION($$, REMEMBER,  rlist);}
	|    ABSTAIN LABEL		{TARGET($$, ABSTAIN,   $2);}
	|    ABSTAIN gerunds		{ACTION($$, DISABLE,   rlist);}
	|    REINSTATE LABEL		{TARGET($$, REINSTATE, $2);}
	|    REINSTATE gerunds		{ACTION($$, ENABLE,    rlist);}
	|    WRITE_IN inlist		{ACTION($$, WRITE_IN,  $2);}
	|    READ_OUT outlist		{ACTION($$, READ_OUT,  $2);}
	|    GIVE_UP			{ACTION($$, GIVE_UP,   0);}
	|    COME_FROM LABEL		{TARGET($$, COME_FROM, $2);}
	|    error			{splat();}
	;

/* gerund lists are used by ABSTAIN and REINSTATE */
gerunds	:   GERUND
		{rlist = np = newnode(); np->constant = $1;}
	|   gerunds INTERSECTION GERUND
		{
		    np->rval = newnode();
		    np = np->rval;
		    np->constant = $3;
		} 
	;

/* OK, here's what a variable reference looks like */
variable:    scalar | array;
   
lvalue	:    scalar | subscr;

scalar	:    ONESPOT NUMBER
		{
		    $$ = newnode();
		    $$->opcode = ONESPOT;
		    $$->constant = intern(ONESPOT, $2);
		}
	|    TWOSPOT NUMBER
		{
		    $$ = newnode();
		    $$->opcode = TWOSPOT;
		    $$->constant = intern(TWOSPOT, $2);
		}
	;

array	:    TAIL NUMBER
		{
		    $$ = newnode();
		    $$->opcode = TAIL;
		    $$->constant = intern(TAIL, $2);
		}
	|    HYBRID NUMBER
		{
		    $$ = newnode();
		    $$->opcode = HYBRID;
		    $$->constant = intern(HYBRID, $2);
		}
	;

/* And a constant looks like this */
constant:   MESH NUMBER
		{
		    /* enforce the 16-bit constant constraint */
		    if ($2 > 0xffff)
			lose(yylineno, E017);
		    $$ = newnode();
		    $$->opcode = MESH;
		    $$->constant = $2;
		}
	;

/* variable lists are used in STASH, RETRIEVE, IGNORE, REMEMBER */
varlist :   variable				{rlist = np = $1;}
	|   varlist INTERSECTION variable	{np = np->rval = newnode();}
	;

/* scalars and subscript exprs are permitted in WRITE IN lists */
initem	:    scalar | subscr;
inlist	:    initem INTERSECTION inlist		{$$=cons(INTERSECTION,$1,$3);}
	|    initem				{$$=cons(INTERSECTION,$1,0);}

/* scalars, subscript exprs & constants are permitted in READ OUT lists */
outitem	:    scalar | subscr | constant;
outlist	:    outitem INTERSECTION outlist	{$$=cons(INTERSECTION,$1,$3);}
	|    outitem				{$$=cons(INTERSECTION,$1,0);}
	;

/* Now the gnarly part -- expression syntax */

/* Support array subscripts (as lvalues) */
subscr  :   array SUB sublist		{$$ = cons(SUB, $1, $3);}
	;
sublist :   expr sublist		{$$ = cons(INTERSECTION, $1, $2);}
	|   expr			{$$ = cons(INTERSECTION, $1, 0);}
	;

/* Support array dimension assignment */
byexpr	:   expr BY byexpr		{$$ = cons(BY, $1, $3);}
	|   expr			{$$ = cons(BY, $1, 0);}
	;

/* here goes the general expession syntax */
preftype:    MESH | ONESPOT | TWOSPOT | TAIL | HYBRID;

expr	:   variable	{$$ = $1;}
	|   constant	{$$ = $1;}

	/* deal with the bizarre unary-op syntax */
	|    preftype UNARY NUMBER
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = newnode();
		    $$->rval->opcode = MESH;
		    $$->rval->constant = $3;
		}

	/* Now deal with the screwy unary-op interaction with grouping */
	|    SPARK UNARY expr SPARK
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = $3;
		}
	|    EARS UNARY expr EARS
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = $3;
		}

	|    expr SELECT expr		{$$ = cons(SELECT, $1, $3);}
	|    expr MINGLE expr		{$$ = cons(MINGLE, $1, $3);}
	|    SPARK expr SPARK		{$$ = $2;}
	|    EARS expr EARS		{$$ = $2;}
	|    subscr			{$$ = $1;}
	;

%%

tuple *splat()
/* try to recover from an invalid statement. */
{
    extern FILE	*yyin;
    tuple	*sp;

    /* we're going to do our own resynchronization */
    yyerrok;
    yyclearin;

    /*
     * This is called "weird sex with the lexical analyzer". The idea
     * here is to skip to the next newline, but to do it with our
     * private getc() in order to save the line's characters.
     *
     * With a bit more effort we could implement the full, hideous
     * semantics of splat, i.e. that another statement identifier
     * (DO or PLEASE) in the statement takes us back to normal parsing.
     * But I'm not feeling that twisted just now.
     */
#undef getc
    while (getc(yyin) != '\n')
	continue;
    ungetc('\n', yyin);

    /* generate a placeholder tuple for the text line */
    TARGET(sp, SPLATTERED, 0);
    return(sp);
}

/* ick.y ends here */
