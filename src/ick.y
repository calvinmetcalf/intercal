/*****************************************************************************

NAME
    ick.y -- grammar for the INTERCAL language

DESCRIPTION 
   This YACC grammar parses the INTERCAL language by designed by Don R. Woods
and James M. Lyon.  There are two syntax extensions over the original
INTERCAL-72 language; the COME FROM statement, and the prefixed forms of the
WHIRL operator.

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

*****************************************************************************/

%{
#include <stdio.h>
#include "sizes.h"
#include "ick.h"
#include "feh.h"
#include "lose.h"

extern int yyerror(char*);

/* Intervene our first-stage lexer. */
extern int lexer(void);
#define yylex() lexer()

static node *rlist;	/* pointer to current right-hand node list */
/*static node *llist;*/	/* pointer to current left-hand node list */
static node *np;	/* variable for building node lists */

extern int stbeginline;	/* line number of last seen preamble */
static int thisline;	/* line number of beginning of current statement */
static tuple *splat(void);

#define GETLINENO					\
    {if (stbeginline < 0) thisline = -stbeginline;	\
     else {thisline = stbeginline; stbeginline = 0;}}

#define ACTION(x, nt, nn)	\
    {x = newtuple(); x->type = nt; x->lineno = thisline; x->u.node = nn;}
#define TARGET(x, nt, nn)	\
    {x = newtuple(); x->type = nt; x->lineno = thisline; x->u.target = nn;}
#define NEWFANGLED	if (traditional) lose(E111,yylineno,(char*)NULL); else

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
%token MINGLE SELECT SPARK EARS SUB BY BADCHAR

%token <numval> NUMBER UNARY OHOHSEVEN GERUND LABEL
%token <node> INTERSECTION

/*
 * These are not tokens returned by the lexer, but they are used as
 * tokens elsewhere.  We define them here to insure that the values
 * will not conflict with the other tokens.  It is important that
 * WHIRL through WHIRL5 be a continuous sequence.
 */
%token SPLATTERED C_AND C_OR C_XOR C_NOT AND OR XOR FIN MESH32
%token WHIRL WHIRL2 WHIRL3 WHIRL4 WHIRL5

%type <node> expr varlist variable constant lvalue inlist outlist
%type <node> subscr byexpr scalar array initem outitem sublist
%type <node> unambig subscr1 sublist1 oparray osubscr osubscr1
%type <tuple> perform
%type <numval> please preftype

%nonassoc EARS SPARK
%nonassoc HIGHPREC

%%	/* beginning of rules section */

/* A program description consists of a sequence of statements */
program	:    /* EMPTY */
	|    program command
	;

/*
 * Each command consists of an optional label, followed by a preamble,
 * followed by an optional probability, followed by the statement body.
 * Negative exechance values indicate initial abstentions, and will be
 * made positive before code is emitted.
 */
command	:    please perform
		{$2->label = 0; $2->exechance = $1 * 100;}
	|    please OHOHSEVEN perform
		{$3->label = 0; $3->exechance = $1 * $2;}
	|    LABEL please perform
		{checklabel($1); $3->label = $1; $3->exechance = $2 * 100;}
	|    LABEL please OHOHSEVEN perform
		{checklabel($1); $4->label = $1; $4->exechance = $2 * $3;}
	|    error
		{lose(E017, yylineno, (char *)NULL);}
	;

/* There are two forms of preamble returned by the lexer */
please	:    DO			{GETLINENO; $$ = 1;}
	|    DO NOT		{GETLINENO; $$ = -1;}
	;

/* Here's how to parse statement bodies */
perform :    lvalue GETS expr	{ACTION($$, GETS,      cons(GETS,$1,$3));}
	|    array GETS byexpr	{ACTION($$, RESIZE,    cons(RESIZE,$1,$3));}
	|    LABEL NEXT		{TARGET($$, NEXT,      $1);}
	|    FORGET expr	{ACTION($$, FORGET,    $2);}
	|    RESUME expr	{ACTION($$, RESUME,    $2);}
	|    STASH varlist	{ACTION($$, STASH,     rlist);}
	|    RETRIEVE varlist	{ACTION($$, RETRIEVE,  rlist);}
	|    IGNORE varlist	{ACTION($$, IGNORE,    rlist);}
	|    REMEMBER varlist	{ACTION($$, REMEMBER,  rlist);}
	|    ABSTAIN LABEL	{TARGET($$, ABSTAIN,   $2);}
	|    ABSTAIN gerunds	{ACTION($$, DISABLE,   rlist);}
	|    REINSTATE LABEL	{TARGET($$, REINSTATE, $2);}
	|    REINSTATE gerunds	{ACTION($$, ENABLE,    rlist);}
	|    WRITE_IN inlist	{ACTION($$, WRITE_IN,  $2);}
	|    READ_OUT outlist	{ACTION($$, READ_OUT,  $2);}
	|    GIVE_UP		{ACTION($$, GIVE_UP,   0);}
	|    COME_FROM LABEL	{NEWFANGLED {TARGET($$,COME_FROM,$2)}}
	|    BADCHAR		{yyclearin; $$ = splat();}
	|    error		{yyclearin; $$ = splat();}
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

/* Array with unary operator is a special intermediate case; these
   nodes will be rearranged when the subscript list is added */
oparray :    TAIL UNARY NUMBER
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = newnode();
		    $$->rval->opcode = TAIL;
		    $$->rval->constant = intern(TAIL, $3);
		}
        |    HYBRID UNARY NUMBER
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = newnode();
		    $$->rval->opcode = HYBRID;
		    $$->rval->constant = intern(HYBRID, $3);
		}
        ;

/* And a constant looks like this */
constant:   MESH NUMBER
		{
		    /* enforce the 16-bit constant constraint */
		    if ((unsigned int)$2 > Max_small)
			lose(E017, yylineno, (char *)NULL);
		    $$ = newnode();
		    $$->opcode = MESH;
		    $$->constant = $2;
		}
	;

/* variable lists are used in STASH, RETRIEVE, IGNORE, REMEMBER */
varlist :   variable				{rlist = np = $1;}
	|   varlist INTERSECTION variable	{np = np->rval = $3;
							/* newnode(); */ }
	;

/* scalars and subscript exprs are permitted in WRITE IN lists */
/* new: arrays are also permitted to allow for bitwise I/0 */
initem	:    scalar | subscr | array;
inlist	:    initem INTERSECTION inlist		{$$=cons(INTERSECTION,$1,$3);}
	|    initem				{$$=cons(INTERSECTION,$1,0);}

/* scalars, subscript exprs & constants are permitted in READ OUT lists */
/* new: arrays are also permitted to allow for bitwise I/0 */
outitem	:    scalar | subscr | constant | array;
outlist	:    outitem INTERSECTION outlist	{$$=cons(INTERSECTION,$1,$3);}
	|    outitem				{$$=cons(INTERSECTION,$1,0);}
	;

/* Now the gnarly part -- expression syntax */

/* Support array dimension assignment */
byexpr	:   expr BY byexpr		{$$ = cons(BY, $1, $3);}
	|   expr			{$$ = cons(BY, $1, 0);}
	;

/* Support array subscripts (as lvalues) */
subscr  :   subscr1		        {$$ = $1;}
        |   array SUB sublist		{$$ = cons(SUB, $1, $3);}
	;
subscr1 :   array SUB sublist1		{$$ = cons(SUB, $1, $3);}
	;
sublist :   unambig sublist             {$$ = cons(INTERSECTION, $1, $2);}
	|   unambig sublist1 	        {$$ = cons(INTERSECTION, $1, $2);}
	;
sublist1:   subscr1        		{$$ = cons(INTERSECTION, $1, 0);}
        |   osubscr1            	{$$ = cons(INTERSECTION, $1, 0);}
        |   unambig     %prec HIGHPREC	{$$ = cons(INTERSECTION, $1, 0);}
        ;

/* Unary operators with arrays act like arrays only in expressions */
osubscr :   osubscr1                    {$$ = $1;}
        |   oparray SUB sublist
		{$$ = $1; $$->rval = cons(SUB, $$->rval, $3);}
	;
osubscr1:   oparray SUB sublist1
		{$$ = $1; $$->rval = cons(SUB, $$->rval, $3);}
	;

/* here goes the general expression syntax */
expr    :   unambig	        	{$$ = $1;}
	|   unambig SELECT unambig	{$$ = cons(SELECT, $1, $3);}
	|   unambig SELECT subscr	{$$ = cons(SELECT, $1, $3);}
	|   unambig SELECT osubscr	{$$ = cons(SELECT, $1, $3);}
	|   unambig MINGLE unambig	{$$ = cons(MINGLE, $1, $3);}
	|   unambig MINGLE subscr	{$$ = cons(MINGLE, $1, $3);}
	|   unambig MINGLE osubscr	{$$ = cons(MINGLE, $1, $3);}
	|   subscr			{$$ = $1;}
	|   osubscr			{$$ = $1;}
        ;

preftype:   MESH {$$=MESH; } | ONESPOT {$$=ONESPOT;} | TWOSPOT {$$=TWOSPOT;};

unambig	:   variable	{$$ = $1;}
	|   constant	{$$ = $1;}

	/* deal with the bizarre unary-op syntax */
	|    preftype UNARY NUMBER
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = newnode();
		    $$->rval->opcode = $1;
		    if($1 == MESH) {
			    /* enforce the 16-bit constant constraint */
			    if ((unsigned int)$3 > Max_small)
				lose(E017, yylineno, (char *)NULL);
			    $$->rval->constant = $3;
		    }
		    else {
			$$->rval->constant = intern($1, $3);
		    }
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

	|    SPARK expr SPARK		{$$ = $2;}
	|    EARS expr EARS		{$$ = $2;}
	;

%%

static tuple *splat(void)
/* try to recover from an invalid statement. */
{
    tuple *sp;
    int tok, i;
    extern bool re_send_token;

    /*
     * The idea
     * here is to skip to the next DO, PLEASE or label, then unget that token.
     * which we can do with a tricky flag on the lexer (re_send_token).
     */

    /*	fprintf(stderr,"attempting to splat at line %d....\n",yylineno); */
    for(i = 0,re_send_token = FALSE;;i++) {
	tok = lexer();
	if (!tok)
	{
	    re_send_token = TRUE;
	    tok = ' ';		/* scanner must not see a NUL */
	    break;
	}
	else if (tok == DO || tok == PLEASE || tok == LABEL) {
	    re_send_token = TRUE;
	    break;
	}
    }
    /*
	fprintf(stderr,"found %d on line %d after %d other tokens.\n",
		tok,yylineno,i);
     */

    /* generate a placeholder tuple for the text line */
    TARGET(sp, SPLATTERED, 0);

    return(sp);
}

/* ick.y ends here */
