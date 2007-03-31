/*****************************************************************************

NAME
    ick.y -- grammar for the INTERCAL language

DESCRIPTION 
   This YACC grammar parses the INTERCAL language by designed by Don R. Woods
and James M. Lyon.  There are several syntax extensions over the original
INTERCAL-72 language.

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
#include <stdlib.h>
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

extern int mark112;    /* AIS: Mark the tuple for W112 when it's created. */
 
static tuple *splat(void);

#define GETLINENO					\
    {if (stbeginline < 0) thisline = -stbeginline;	\
     else {thisline = stbeginline; stbeginline = 0;}}

#define ACTION(x, nt, nn)	\
    {x = newtuple(); x->type = nt; x->lineno = thisline; x->u.node = nn;}
#define TARGET(x, nt, nn)	\
    {x = newtuple(); x->type = nt; x->lineno = thisline; x->u.target = nn;}
#define ACTARGET(x, nt, nn, nn2)\
    {x = newtuple(); x->type = nt; x->lineno = thisline;\
      x->u.node = nn; x->u.target = nn2;}
/* AIS : The macro above was added for ABSTAIN expr FROM. */ 
#define NEWFANGLED mark112 = 1; /* AIS: Added the mention of mark112 */ \
      if (traditional) lose(E111,yylineno,(char*)NULL); else

#define DESTACKSE1 sparkearsstack[sparkearslev--/32] >>= 1
#define DESTACKSPARKEARS DESTACKSE1; DESTACKSE1

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
 * Some code in feh2.c depends on GETS being the least
 * statement type and on the order of the ones following.
 * AIS: Note that although GETS is the lowest statement type (with index 0
 *	in feh2.c), UNKNOWN (i.e. a line that causes error 000) is an even
 *	lower statement type, with index -1. perpet.c uses indexes 1 higher.
 * AIS: Added FROM, MANYFROM, TRY_AGAIN, COMPUCOME, GERUCOME, and
 *	three NEXT FROM cases.
 * AIS: COME_FROM now merged with the label following it,
 *      to distinguish it from COMPUCOME, in the lexer. This changes
 *      the parser somewhat.
 */
%token UNKNOWN /* AIS: This is so comments can be REINSTATED */
%token GETS RESIZE NEXT GO_AHEAD GO_BACK FORGET RESUME STASH RETRIEVE IGNORE
%token REMEMBER ABSTAIN REINSTATE
%token DISABLE ENABLE MANYFROM GIVE_UP READ_OUT WRITE_IN /* AIS: */PIN
%token <numval> COME_FROM NEXTFROMLABEL
%token NEXTFROMEXPR NEXTFROMGERUND COMPUCOME GERUCOME TRY_AGAIN FROM

/* AIS: ONCE and AGAIN added, for multithread support; also, NOSPOT added,
   so that I can reserve _ for future use (it's nowhere in the grammar yet) */
%token MAYBE DO PLEASE NOT ONCE AGAIN MESH NOSPOT ONESPOT TWOSPOT TAIL HYBRID
%token MINGLE SELECT /* AIS: SPARK EARS */ SUB BY BADCHAR
/* AIS: For operand overloading */
%token SLAT BACKSLAT

%token <numval> NUMBER UNARY OHOHSEVEN GERUND LABEL
%token <node> INTERSECTION

/*
 * These are not tokens returned by the lexer, but they are used as
 * tokens elsewhere.  We define them here to insure that the values
 * will not conflict with the other tokens.  It is important that
 * WHIRL through WHIRL5 be a continuous sequence.
 */
/* AIS: Added new tokens for optimizer output */
%token SPLATTERED MESH32
%token C_AND C_OR C_XOR C_NOT C_NNAND C_ISNONZERO C_LSHIFT C_LSHIFT2
%token C_LSHIFT8 C_LSHIFTIN1 C_RSHIFT C_AND1ADD1 C_2SUBAND1 C_1PLUS C_2MINUS
%token C_XORGREATER C_RSHIFTBY C_NOTEQUAL C_A
/* The reverse unary operators have to be in the same order as the forward
   unary operators. */
%token AND OR XOR FIN WHIRL WHIRL2 WHIRL3 WHIRL4 WHIRL5
%token REV_AND REV_OR REV_XOR REV_FIN
%token REV_WHIRL REV_WHIRL2 REV_WHIRL3 REV_WHIRL4 REV_WHIRL5

%type <node> expr limexpr varlist variable constant lvalue inlist outlist
%type <node> subscr byexpr scalar scalar2s array initem outitem sublist
%type <node> unambig limunambig subscr1 sublist1 oparray osubscr osubscr1
%type <node> notanlvalue nlunambig lunambig
%type <tuple> perform mtperform
%type <numval> please preftype

%nonassoc OPENEARS OPENSPARK CLOSEEARS CLOSESPARK
%nonassoc HIGHPREC
%nonassoc UNARYPREC
%nonassoc LOWPREC
/* AIS: I reversed this precedence, to sort out the near-ambiguity.
   UNARYPREC and LOWPREC are to give the C-INTERCAL meaning of a statement
   precedence above its CLC-INTERCAL meaning. */

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
 * AIS: An exechance above 100 indicates a MAYBE situation (e.g. 4545
 * means MAYBE DO %45 ...). This means %0 should be illegal. I modified
 * all these to allow for MAYBE.
 */
command	:    please mtperform
		{$2->label = 0; $2->exechance = $1 * 100;}
	|    please OHOHSEVEN mtperform
		{$3->label = 0; $3->exechance = $1 * $2;}
	|    LABEL please mtperform
		{checklabel($1); $3->label = $1; $3->exechance = $2 * 100;}
	|    LABEL please OHOHSEVEN mtperform
		{checklabel($1); $4->label = $1; $4->exechance = $2 * $3;}
	|    error
		{lose(E017, yylineno, (char *)NULL);}
	;
/*
 * AIS: added for the ONCE/AGAIN qualifiers. It copies a pointer to the tuple,
 * so command will set the values in the original tuple via the copy.
 */

mtperform :  perform
		{$1->onceagainflag = onceagain_NORMAL; $$ = $1;}
	  |  perform ONCE
		{NEWFANGLED {$1->onceagainflag = onceagain_ONCE; $$ = $1;}}
	  |  perform AGAIN
		{NEWFANGLED {$1->onceagainflag = onceagain_AGAIN; $$ = $1;}}

/* There are two (AIS: now four) forms of preamble returned by the lexer */
please	:    DO			{GETLINENO; $$ = 1;}
	|    DO NOT		{GETLINENO; $$ = -1;}
	|    MAYBE		{NEWFANGLED {GETLINENO; $$ = 101;}}
	|    MAYBE NOT		{NEWFANGLED {GETLINENO; $$ = -101;}}
	;

/* Here's how to parse statement bodies */
perform :    lvalue GETS expr	{ACTION($$, GETS,      cons(GETS,$1,$3));}
	|    array GETS byexpr	{ACTION($$, RESIZE,    cons(RESIZE,$1,$3));}
	|    notanlvalue GETS expr %prec LOWPREC
	  {/* AIS: This is for variableconstants, and an error otherwise.*/
	   if(variableconstants) ACTION($$, GETS, cons(GETS,$1,$3))
	   else {yyclearin; $$=splat();}}
	|    LABEL NEXT		{TARGET($$, NEXT,      $1);}
	|    FORGET expr	{ACTION($$, FORGET,    $2);}
	|    RESUME expr	{ACTION($$, RESUME,    $2);}
	|    STASH varlist	{ACTION($$, STASH,     rlist);}
	|    RETRIEVE varlist	{ACTION($$, RETRIEVE,  rlist);}
	|    IGNORE varlist	{ACTION($$, IGNORE,    rlist);}
	|    REMEMBER varlist	{ACTION($$, REMEMBER,  rlist);}
	|    ABSTAIN FROM LABEL	{stbeginline=0; TARGET($$, ABSTAIN,   $3);}
	|    ABSTAIN FROM gerunds	{ACTION($$, DISABLE,   rlist);}
	|    ABSTAIN expr FROM LABEL {/* AIS */ NEWFANGLED {stbeginline=0; ACTARGET($$, FROM, $2, $4);}}
	|    ABSTAIN expr FROM gerunds {/* AIS */ NEWFANGLED {$$ = newtuple(); $$->type = MANYFROM; $$->lineno = thisline; \
	  {node* tempnode=newnode(); $$->u.node = tempnode; tempnode->lval=$2; tempnode->rval=rlist; tempnode->opcode=MANYFROM;}}}
	|    REINSTATE LABEL	{stbeginline=0; TARGET($$, REINSTATE, $2);}
	|    REINSTATE gerunds	{ACTION($$, ENABLE,    rlist);}
	|    WRITE_IN inlist	{ACTION($$, WRITE_IN,  $2);}
	|    READ_OUT outlist	{ACTION($$, READ_OUT,  $2);}
	|    PIN scalar2s	{ACTION($$, PIN,       $2);}
	|    GIVE_UP		{ACTION($$, GIVE_UP,   0);}
	|    GO_AHEAD		{/* AIS */ NEWFANGLED {ACTION($$, GO_AHEAD,  0);}}
	|    GO_BACK		{/* AIS */ NEWFANGLED {ACTION($$, GO_BACK,   0);}}
	|    TRY_AGAIN		{/* AIS */ NEWFANGLED {ACTION($$,TRY_AGAIN,0);}}
	|    COME_FROM		{/* AIS: Modified */ NEWFANGLED {TARGET($$,COME_FROM,$1);}}
	|    COMPUCOME gerunds	{/* AIS: COME FROM gerund */
				 NEWFANGLED{ACTION($$,GERUCOME,rlist);
				 compucomesused=1; gerucomesused=1;}}
	|    COMPUCOME expr	{/* AIS */NEWFANGLED {ACTION($$,COMPUCOME,$2);
				 compucomesused=1;}}
/* AIS: NEXT FROM works along the same lines as COME FROM */
	|    NEXTFROMLABEL	{NEWFANGLED {TARGET($$,NEXTFROMLABEL,$1);}
	                         nextfromsused=1;}
	|    NEXTFROMEXPR gerunds{NEWFANGLED{ACTION($$,NEXTFROMGERUND,rlist);
				 compucomesused=1; gerucomesused=1;}
                                 nextfromsused=1;}
	|    NEXTFROMEXPR expr	{NEWFANGLED {ACTION($$,NEXTFROMEXPR,$2);
				 compucomesused=1; nextfromsused=1;}}

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

scalar2s:    TWOSPOT NUMBER /* AIS: for TWOSPOTs only */
		{
		  $$ = newnode();
		  $$->opcode = TWOSPOT;
		  $$->constant = intern(TWOSPOT, $2);
		}
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
oparray :    TAIL UNARY NUMBER %prec UNARYPREC
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = newnode();
		    $$->rval->opcode = TAIL;
		    $$->rval->constant = intern(TAIL, $3);
		}
	|    HYBRID UNARY NUMBER %prec UNARYPREC
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
		    if(variableconstants) /* AIS */
		      $$->constant = intern(MESH, $2);
		    else
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
	;

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
subscr	:   subscr1			{$$ = $1;}
	|   array SUB sublist		{$$ = cons(SUB, $1, $3);}
	;
subscr1 :   array SUB sublist1		{$$ = cons(SUB, $1, $3);}
	;
sublist :   unambig sublist		{$$ = cons(INTERSECTION, $1, $2);}
	|   unambig sublist1		{$$ = cons(INTERSECTION, $1, $2);}
	;
sublist1:   subscr1			{$$ = cons(INTERSECTION, $1, 0);}
	|   osubscr1			{$$ = cons(INTERSECTION, $1, 0);}
	|   unambig	%prec HIGHPREC	{$$ = cons(INTERSECTION, $1, 0);}
	;

/* Unary operators with arrays act like arrays only in expressions */
osubscr :   osubscr1			{$$ = $1;}
	|   oparray SUB sublist
		{$$ = $1; $$->rval = cons(SUB, $$->rval, $3);}
	;
osubscr1:   oparray SUB sublist1
		{$$ = $1; $$->rval = cons(SUB, $$->rval, $3);}
	;

/* here goes the general expression syntax */
expr	:   unambig			{$$ = $1;}
/* AIS: CLC-INTERCAL allows right-association of SELECT and MINGLE.
   (Strangely, that simplifies this section somewhat.) */
	|   unambig SELECT expr		{$$ = cons(SELECT, $1, $3);}
	|   unambig MINGLE expr		{$$ = cons(MINGLE, $1, $3);}
/* AIS: Operand overloading */
	|   scalar SLAT expr		{NEWFANGLED{$$ = cons(SLAT, $1, $3);
					 opoverused=1; if(!firstslat)
					 firstslat=$$; else
					 prevslat->nextslat=$$; prevslat=$$;
					 $$->nextslat=0;}}
	|   subscr			{$$ = $1;}
	|   osubscr			{$$ = $1;}
	;

/* AIS: Any expression that isn't an lvalue */
notanlvalue:nlunambig			{$$ = $1;}
	|   osubscr			{$$ = $1;}
	|   unambig SELECT expr		{$$ = cons(SELECT, $1, $3);}
	|   unambig MINGLE expr		{$$ = cons(MINGLE, $1, $3);}
	|   scalar SLAT expr		{NEWFANGLED{$$ = cons(SLAT, $1, $3);
					 opoverused=1; if(!firstslat)
					 firstslat=$$; else
					 prevslat->nextslat=$$; prevslat=$$;
					 $$->nextslat=0;}}
	;

/* AIS: an expr that doesn't start with a unary operator */
limexpr :   limunambig			{$$ = $1;}
	|   limunambig SELECT expr	{$$ = cons(SELECT, $1, $3);}
	|   limunambig MINGLE expr	{$$ = cons(MINGLE, $1, $3);}
	|   scalar SLAT expr		{NEWFANGLED{$$ = cons(SLAT, $1, $3);
					 opoverused=1; if(!firstslat)
					 firstslat=$$; else
					 prevslat->nextslat=$$; prevslat=$$;
					 $$->nextslat=0;}}
	|   subscr			{$$ = $1;}
	|   osubscr			{$$ = $1;}
	;


preftype:   MESH {$$=MESH; } | ONESPOT {$$=ONESPOT;} | TWOSPOT {$$=TWOSPOT;};

/* AIS: unambig split into limunambig (unambigs that don't start with a
	unary operator), nlunambig (unambigs that aren't lvalues),
	lunambig (both), and unambig (the general case) */
lunambig:   constant	{$$ = $1;}
	/* deal with the bizarre unary-op syntax */
	|    preftype UNARY NUMBER %prec UNARYPREC
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = newnode();
		    $$->rval->opcode = $1;
		    if($1 == MESH) {
			    /* enforce the 16-bit constant constraint */
			    if ((unsigned int)$3 > Max_small)
				lose(E017, yylineno, (char *)NULL);
			    $$->rval->constant = intern(MESH, $3);
		    }
		    else {
			$$->rval->constant = intern($1, $3);
		    }
		}

	/* Now deal with the screwy unary-op interaction with grouping */
	/* AIS: Modified to allow for maintenance of the SPARK/EARS stack */
	|    eitherspark UNARY expr CLOSESPARK %prec UNARYPREC
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = $3;
		    DESTACKSPARKEARS;
		}
	|    eitherears UNARY expr CLOSEEARS %prec UNARYPREC
		{
		    $$ = newnode();
		    $$->opcode = $2;
		    $$->rval = $3;
		    DESTACKSPARKEARS;
		}
/* AIS: limexpr, a limited expression that isn't allowed to start with a
   unary operator, is used here to avoid a reduce/reduce conflict. */
	|    eitherspark limexpr CLOSESPARK	{$$ = $2; DESTACKSPARKEARS;}
	|    eitherears limexpr CLOSEEARS	{$$ = $2; DESTACKSPARKEARS;}
	;

limunambig:  lunambig  {$$ = $1;}
	|    scalar    {$$ = $1;}
	;

nlunambig:   lunambig  {$$ = $1;}
	|    UNARY unambig %prec LOWPREC
	     {$$=newnode(); $$->opcode = $1; $$->rval = $2;}
	;

/* AIS: A bit of CLC-INTERCAL compatibility here.
   The syntax now allows any number of unary operators before, and one inside,
   an expression. In ambiguous cases like '&VV#1~#5', the & applies to the
   whole expression (one operator inside is allowed, and inside takes
   precedence), but the Vs apply just to the #1, because only one operator
   inside is allowed. */
unambig :    limunambig {$$ = $1;}
	|    UNARY unambig %prec LOWPREC
	     {$$=newnode(); $$->opcode = $1; $$->rval = $2;}
	;

eitherspark : OPENSPARK ;
	    | CLOSESPARK ;
	    ;

eitherears  : OPENEARS ;
	    | CLOSEEARS ;
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
	else if (tok == DO || tok == PLEASE || tok == LABEL
		 /* AIS */ || tok == MAYBE) {
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
