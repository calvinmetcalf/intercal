/*****************************************************************************

NAME
    oil.y -- compiler for Optimizer Idiom Language files

LICENSE TERMS
    Copyright (C) 2007 Alex Smith

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

***************************************************************************/

%{

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ick_bool.h"

/* Define strdup if it isn't available */
#ifndef HAVE_STRDUP
char* strdup(const char* s)
{
  char* t=malloc(strlen(s)+1);
  if(t) strcpy(t,s);
  return t;
}
#endif

/*
#define YYDEBUG 1
int yydebug=1;
*/

/* Each semantic value represents either a constraint on a node that needs to
   be true for the optimization template to match, or part of the replacement
   for the optimization template. */
struct ickstype
{
  unsigned depth;     /* The number of child nodes this is from the root */
  unsigned long path; /* Bits from the end of this number represent the
			 sequence of lvals (=0) or rvals (=1) this is from the
			 root; depth bits from the end are relevant, and the
			 bit nearest the root is least significant */
  const char* condition; /* Points to a constant string; the condition to generate
			 on that node. Could also point to something the lexer
			 mallocs, but then the lexer has to keep track of being
			 able to free it again. If this is NULL, it means that
			 there isn't a condition other than maybe mustbemesh */
  const char* nodetypename; /* The name of the type of node this condition matches,
		         or NULL for a LEXERLEAF */
  bool usec;        /* Whether to append the value of c to the condition */
  bool mustbemesh;  /* Must this node be a mesh or mesh32? */
  bool cxneeded;    /* True means calculate c and x for the node and the
			 condition is on those; false means append 'condition'
			 to the node itself to form the condition and c and x
			 aren't needed */
  bool width32;     /* Generate a 32-bit node? */
  bool ublo;        /* Is this a UBLO (if set, generate conditions
			 to check width)? */
  unsigned long c;    /* The value to append to the condition */
  int replnum;        /* Number of this group for replacements */
  struct ickstype *n1;/* n1 and n2 are pointers to other conditions that */
  struct ickstype *n2;/* also have to be satisified */
};

#define YYSTYPE YYSTYPE

#define MAXOPTNAMELEN 64
char optname[MAXOPTNAMELEN]="undefined";
int optnumber = 0;

typedef struct ickstype *YYSTYPE;

void splitend(void);
void splitstart(void);

void treedepthup(YYSTYPE, bool);
void treefree(YYSTYPE);
void gennodepath(unsigned, unsigned long);
bool treeshapecond(YYSTYPE, bool);
YYSTYPE treenscheck(YYSTYPE, YYSTYPE, int);
void treecxcond(YYSTYPE);
void treerepcount(YYSTYPE, int*);
void treerepgen(YYSTYPE, YYSTYPE*, int*);

int countgetchar(void);
int countungetc(int, FILE*);
int cgccol;
int cgcrow;

/* #defines for chaining together template expressions; here, s is the type
   of expression (e.g. select, bitwise and, unary and) that's chaining the
   expressions together and n is the nonterminal that's the rval */
#define BINARYEXPR(s,m,l,r,w) do{ \
                          m=malloc(sizeof(struct ickstype)); \
                          m->n1=l; \
                          m->n2=r; \
                          m->usec=0; \
			  m->condition="->opcode==" s; \
                          m->nodetypename=s; \
                          m->mustbemesh=0; \
                          m->cxneeded=0; \
			  m->depth=0; \
			  m->path=0; \
                          m->replnum=0; \
                          m->width32=w; \
                          m->ublo=0; \
			  treedepthup(m->n1,0); \
			  treedepthup(m->n2,1); \
                          } while(0)

#define UNARYEXPR(s,m,r,w,u) do{ \
                          m=malloc(sizeof(struct ickstype)); \
                          m->n1=0; \
                          m->n2=r; \
                          m->usec=0; \
			  m->condition="->opcode==" s; \
                          m->nodetypename=s; \
                          m->mustbemesh=0; \
                          m->cxneeded=0; \
			  m->depth=0; \
			  m->path=0; \
                          m->replnum=0; \
                          m->width32=w; \
                          m->ublo=u; \
			  treedepthup(m->n2,1); \
                        } while(0)

/* Error handling and lexing */
int yylex(void);
int yyerror(char const *);

/* Split the output file */
#define SPLITMAX 20
int splitcount=SPLITMAX;
int filenumber=0;
bool inloop=0;
%}

/* Various conditions can come out from the lexer. The most common is a char,
   which represents that C or INTERCAL operation in a template. Non-character
   tokens are used for the various possible leaves in a template, though:
   .1 A 16-bit expression (number 1, for replacements)
   :1 A 32-bit expression (number 1, for replacements)
   _1 An expression of any width (again, number 1 for replacements)
   #1 A constant with the value 1
   #{x==1}3 A constant which equals 1 (number 3, for replacements)
   Note that in the last example, the expression is written strictly in C;
   for instance,
   #{ick_iselect(x,x)==1}4
   would select a constant that's a power of 2 (ick_iselect is the C name for the
   INTERCAL operation 'select').
   .{c&0xfffffffe==0}5 A 16-bit expression (number 5, for replacements) which
   has been analysed to not possibly have any bits other than the least
   significant set (c is here the list of all potentially set bits)
   All of these come out as correctly set LEXERLEAFs.

   Expressions with identical numbers must be node-for-node identical, except
   for number 0 (which is like _ in Prolog, it means 'can be anything and
   disregard the value). This holds true even if they have different sigils.
   Expressions can use each other's c and x values with the notation c5, x4,
   and so on; replacement numbers are limited to 1 digit.

   The other things that can come out from the lexer are sparks, ears, and
   parentheses (any mix, we're not fussy). */

%token LEXERLEAF

%%

input: /**/ | input optimization ;

optimization: template '-' '>' replacement
{
  static YYSTYPE tempmem[10];
  static int replcount[10];
  /* Handle splitting the file. */
  if(splitcount) splitcount--;
  if(!splitcount && !inloop)
  {
    splitcount=SPLITMAX;
    splitend();
    ++filenumber;
    splitstart();
  }
  /* This is where we actually generate the optimizer code. */
  /* Tree-shape and is-constant conditions */
  printf("  checknodeactbits(np);\n");
  if(treeshapecond($1,1)) printf("  if(1"); printf(")\n  do\n  {\n");
  /* Nodesame and cxdata conditions */
  {
    int i=10;
    YYSTYPE temp;
    while(--i)
    {
      temp=treenscheck($1,0,i);
      if(temp)
      {
	printf("    x%d=np",i);
	gennodepath(temp->depth,temp->path);
	printf("->constant; c%d=np",i);
	gennodepath(temp->depth,temp->path);
	printf("->optdata;\n");
      }
      tempmem[i]=temp;
      replcount[i]=0; /* we need to zero this somewhere, may as well be here */
    }
    treecxcond($1);
  }
  /* If we reach this point in the generated code, we have an optimizer
     template match. */
  printf("    OPTING(%s_%d);\n",optname,++optnumber);

  /* We now need to replace np with its replacement. This is done by creating
     a new nodetree, copying across tempmem'd nodes where necessary, and then
     substituting one for the other. (This is an inefficient but general way
     to do this.) One special case is needed; because pointers into the root
     node need to continue pointing there, the temporary node tp is copied
     member-for-member and then freed again. The root width can change (this
     is a deviation from previous code), in order to prevent a bug where the
     new root happens to be a unary. (This means we can get a 16-bit unary
     applied to 32-bit data; but the optimiser is meant to ensure that this is
     not problematic.) */
  printf("    tp=newnode();\n");
  treerepcount($4,replcount);
  treerepgen($4,tempmem,replcount);
  printf("    nodefree(np->lval); nodefree(np->rval);\n");
  printf("    *np=*tp; free(tp);\n");
  printf("  } while(0);\n\n");
  /* Free the template and replacement now they're finished being used. */
  treefree($1);
  treefree($4);
}
|      '<' LEXERLEAF '-' LEXERLEAF
{
  if(!$2->mustbemesh||!$4->mustbemesh)
  {
    yyerror("syntax error in <#..#()->()> construct");
    free($2); free($4);
    YYERROR;
  }
  printf("  r=%luLU; while(r<=%luLU) {\n",$2->c,$4->c);
  free($2); free($4);
  inloop=1;
}
|      '>' {printf("  r++;\n  }\n"); inloop=0;};

template: expr3 ;

expr3: '(' expr2 ')' {$$=$2;}
|      '"' expr2 '"' {$$=$2;}
|      '\'' expr2 '\'' {$$=$2;}

expr: expr3
|     LEXERLEAF ;

expr2: expr '$' expr {BINARYEXPR("MINGLE",$$,$1,$3,1);}
|      expr '~' expr {BINARYEXPR("SELECT",$$,$1,$3,1);}
|      expr '~' '1' '6' expr {BINARYEXPR("SELECT",$$,$1,$5,0);}
|      expr '~' '3' '2' expr {BINARYEXPR("SELECT",$$,$1,$5,1);}
|      '&' '1' '6' expr {UNARYEXPR("AND",$$,$4,0,1);}
|      '&' '3' '2' expr {UNARYEXPR("AND",$$,$4,1,1);}
|      'V' '1' '6' expr {UNARYEXPR("OR",$$,$4,0,1);}
|      'V' '3' '2' expr {UNARYEXPR("OR",$$,$4,1,1);}
|      '?' '1' '6' expr {UNARYEXPR("XOR",$$,$4,0,1);}
|      '?' '3' '2' expr {UNARYEXPR("XOR",$$,$4,1,1);}
|      '^' '1' '6' expr {UNARYEXPR("FIN",$$,$4,0,1);}
|      '^' '3' '2' expr {UNARYEXPR("FIN",$$,$4,1,1);}
|      '@' '1' '6' expr {UNARYEXPR("WHIRL",$$,$4,0,1);}
|      '@' '2' '1' '6' expr {UNARYEXPR("WHIRL2",$$,$5,0,1);}
|      '@' '3' '1' '6' expr {UNARYEXPR("WHIRL3",$$,$5,0,1);}
|      '@' '4' '1' '6' expr {UNARYEXPR("WHIRL4",$$,$5,0,1);}
|      '@' '5' '1' '6' expr {UNARYEXPR("WHIRL5",$$,$5,0,1);}
|      '@' '3' '2' expr {UNARYEXPR("WHIRL",$$,$4,1,1);}
|      '@' '2' '3' '2' expr {UNARYEXPR("WHIRL2",$$,$5,1,1);}
|      '@' '3' '3' '2' expr {UNARYEXPR("WHIRL3",$$,$5,1,1);}
|      '@' '4' '3' '2' expr {UNARYEXPR("WHIRL4",$$,$5,1,1);}
|      '@' '5' '3' '2' expr {UNARYEXPR("WHIRL5",$$,$5,1,1);}
|      expr '&' expr {BINARYEXPR("C_AND",$$,$1,$3,1);}
|      expr '&' '1' '6' expr {BINARYEXPR("C_AND",$$,$1,$5,0);}
|      expr '&' '3' '2' expr {BINARYEXPR("C_AND",$$,$1,$5,1);}
|      expr '|' expr {BINARYEXPR("C_OR",$$,$1,$3,1);}
|      expr '|' '1' '6' expr {BINARYEXPR("C_OR",$$,$1,$5,0);}
|      expr '|' '3' '2' expr {BINARYEXPR("C_OR",$$,$1,$5,1);}
|      expr '^' expr {BINARYEXPR("C_XOR",$$,$1,$3,1);}
|      expr '^' '1' '6' expr {BINARYEXPR("C_XOR",$$,$1,$5,0);}
|      expr '^' '3' '2' expr {BINARYEXPR("C_XOR",$$,$1,$5,1);}
|      expr '+' expr {BINARYEXPR("C_PLUS",$$,$1,$3,1);}
|      expr '+' '1' '6' expr {BINARYEXPR("C_PLUS",$$,$1,$5,0);}
|      expr '+' '3' '2' expr {BINARYEXPR("C_PLUS",$$,$1,$5,1);}
|      expr '-' expr {BINARYEXPR("C_MINUS",$$,$1,$3,1);}
|      expr '-' '1' '6' expr {BINARYEXPR("C_MINUS",$$,$1,$5,0);}
|      expr '-' '3' '2' expr {BINARYEXPR("C_MINUS",$$,$1,$5,1);}
|      expr '*' expr {BINARYEXPR("C_TIMES",$$,$1,$3,1);}
|      expr '*' '1' '6' expr {BINARYEXPR("C_TIMES",$$,$1,$5,0);}
|      expr '*' '3' '2' expr {BINARYEXPR("C_TIMES",$$,$1,$5,1);}
|      expr '/' expr {BINARYEXPR("C_DIVIDEBY",$$,$1,$3,1);}
|      expr '/' '1' '6' expr {BINARYEXPR("C_DIVIDEBY",$$,$1,$5,0);}
|      expr '/' '3' '2' expr {BINARYEXPR("C_DIVIDEBY",$$,$1,$5,1);}
|      expr '%' expr {BINARYEXPR("C_MODULUS",$$,$1,$3,1);}
|      expr '%' '1' '6' expr {BINARYEXPR("C_MODULUS",$$,$1,$5,0);}
|      expr '%' '3' '2' expr {BINARYEXPR("C_MODULUS",$$,$1,$5,1);}
|      expr '>' expr {BINARYEXPR("C_GREATER",$$,$1,$3,1);}
|      expr '>' '1' '6' expr {BINARYEXPR("C_GREATER",$$,$1,$5,0);}
|      expr '>' '3' '2' expr {BINARYEXPR("C_GREATER",$$,$1,$5,1);}
|      expr '<' expr {BINARYEXPR("C_LESS",$$,$1,$3,1);}
|      expr '<' '1' '6' expr {BINARYEXPR("C_LESS",$$,$1,$5,0);}
|      expr '<' '3' '2' expr {BINARYEXPR("C_LESS",$$,$1,$5,1);}
|      '~' '1' '6' expr {UNARYEXPR("C_NOT",$$,$4,0,1);}
|      '~' '3' '2' expr {UNARYEXPR("C_NOT",$$,$4,1,1);}
|      expr '!' '=' expr {BINARYEXPR("C_NOTEQUAL",$$,$1,$4,0);}
|      expr '!' '=' '1' '6' expr {BINARYEXPR("C_NOTEQUAL",$$,$1,$6,0);}
|      expr '!' '=' '3' '2' expr {BINARYEXPR("C_NOTEQUAL",$$,$1,$6,1);}
|      expr '=' '=' expr {BINARYEXPR("C_ISEQUAL",$$,$1,$4,0);}
|      expr '=' '=' '1' '6' expr {BINARYEXPR("C_ISEQUAL",$$,$1,$6,0);}
|      expr '=' '=' '3' '2' expr {BINARYEXPR("C_ISEQUAL",$$,$1,$6,1);}
|      expr '&' '&' expr {BINARYEXPR("C_LOGICALAND",$$,$1,$4,0);}
|      expr '&' '&' '1' '6' expr {BINARYEXPR("C_LOGICALAND",$$,$1,$6,0);}
|      expr '&' '&' '3' '2' expr {BINARYEXPR("C_LOGICALAND",$$,$1,$6,1);}
|      expr '|' '|' expr {BINARYEXPR("C_LOGICALOR",$$,$1,$4,0);}
|      expr '|' '|' '1' '6' expr {BINARYEXPR("C_LOGICALOR",$$,$1,$6,0);}
|      expr '|' '|' '3' '2' expr {BINARYEXPR("C_LOGICALOR",$$,$1,$6,1);}
|      expr '>' '>' expr {BINARYEXPR("C_RSHIFTBY",$$,$1,$4,1);}
|      expr '>' '>' '1' '6' expr {BINARYEXPR("C_RSHIFTBY",$$,$1,$6,0);}
|      expr '>' '>' '3' '2' expr {BINARYEXPR("C_RSHIFTBY",$$,$1,$6,1);}
|      expr '<' '<' expr {BINARYEXPR("C_LSHIFTBY",$$,$1,$4,1);}
|      expr '<' '<' '1' '6' expr {BINARYEXPR("C_LSHIFTBY",$$,$1,$6,0);}
|      expr '<' '<' '3' '2' expr {BINARYEXPR("C_LSHIFTBY",$$,$1,$6,1);}
|      '!' expr {UNARYEXPR("C_LOGICALNOT",$$,$2,0,0);}
|      '!' '1' '6' expr {UNARYEXPR("C_LOGICALNOT",$$,$4,0,0);}
|      '!' '3' '2' expr {UNARYEXPR("C_LOGICALNOT",$$,$4,1,0);}
|      expr ;

replacement: expr3;

%%

#define MAXTOFREE 1000

char* tofree[MAXTOFREE]={0};
int tfi=0;

int yylex(void)
{
  int c;
  unsigned long acc;

  /* Whitespace is completely insignificant here, even inside && and other
     two-character operators. Just to be different, though, it /is/ significant
     inside constructs like .1 and #{1}2; in such cases, it isn't allowed. */
  c=countgetchar();
  while(isspace(c)) c=countgetchar();
  while(c==';'||c=='[')
  {
    /* Comments go from a semicolon/hybrid to the end of the line. */
    if(c==';')
    {
      c=countgetchar();
      while(c!='\n') c=countgetchar();
      while(isspace(c)) c=countgetchar();
    }
    /* Square brackets set the name for optimizations. */
    if(c=='[')
    {
      int i=0;
      c=countgetchar();
      while(c!=']')
      {
	optname[i++]=c;
	c=countgetchar();
	if(i==MAXOPTNAMELEN-1) {i=0; yyerror("optimization name too long");}
      }
      optnumber=0;
      optname[i]=0;
      c=countgetchar();
      while(isspace(c)) c=countgetchar();
    }
  }
  if(c==EOF) return 0;
  switch(c)
  {
  case '#':
    c=countgetchar();
    if(c!='{')
    {
      acc=0;
      while(isdigit(c))
      {
	acc*=10;
	acc+=(c-'0');
	c=countgetchar();
      }
      yylval=malloc(sizeof(struct ickstype));
      yylval->depth=0;
      yylval->path=0;
      yylval->condition="->constant==";
      yylval->nodetypename=0;
      yylval->usec=1;
      yylval->mustbemesh=1;
      yylval->cxneeded=0;
      yylval->c=acc;
      yylval->replnum=0;
      yylval->n1=0;
      yylval->n2=0;
      yylval->ublo=0;
      yylval->width32=1; /* generate MESH32 not MESH; we can still AND16 it,
			    etc., if necessary */
      countungetc(c, stdin);
      return LEXERLEAF;
    }
    countungetc(c, stdin);
    c='#';
    /* fall through */
  case '_':
  case ':':
  case '.':
    yylval=malloc(sizeof(struct ickstype));
    yylval->depth=0;
    yylval->path=0;
    yylval->condition=0; /* _ or # */
    yylval->width32=1; /* should never matter, but you never
			  know... */
    yylval->ublo=0;
    if(c==':') yylval->condition="->width==32";
    if(c=='.') {yylval->condition="->width==16"; yylval->width32=0;}
    yylval->nodetypename=0;
    yylval->usec=0;
    yylval->mustbemesh=c=='#';
    yylval->cxneeded=0;
    c=countgetchar();
    if(c=='{')
    {
      /* Create a new node to hold the c/x condition */
      yylval->n1=malloc(sizeof(struct ickstype));
      yylval->n1->depth=0;
      yylval->n1->path=0;
      {
	static char buf[512];
	int bi=0;
	c=countgetchar();
	while(c!='}')
	{
	  buf[bi++]=c;
	  if(bi==511) {yyerror("{quoted} string too long"); bi=0;}
	  c=countgetchar();
	}
	buf[bi]=0;
	yylval->n1->condition=tofree[tfi++]=strdup(buf);
	if(tfi==MAXTOFREE) {yyerror("Too many {quoted} strings"); tfi--;}
	c=countgetchar();
      }
      yylval->n1->nodetypename=0;
      yylval->n1->usec=0;
      yylval->n1->mustbemesh=0;
      yylval->n1->cxneeded=1;
      yylval->n1->n1=0;
      yylval->n1->n2=0;
      yylval->n1->width32=yylval->width32;
      yylval->n1->ublo=0;
    }
    else yylval->n1=0;
    yylval->replnum=0;
    if(yylval->n1) yylval->n1->replnum=c-'0'; else yylval->replnum=c-'0';
    yylval->n2=0;
    return LEXERLEAF;
  default:
    return c;
  }
}

void treedepthup(YYSTYPE v, bool i)
{
  if(!v) return;
  treedepthup(v->n1,i);
  treedepthup(v->n2,i);
  v->depth++;
  v->path<<=1;
  v->path|=i;
  if(v->depth>30) yyerror("Nesting too deep in template or replacement\n");
}

void treefree(YYSTYPE v)
{
  if(!v) return;
  treefree(v->n1);
  treefree(v->n2);
  free(v);
}

void gennodepath(unsigned depth, unsigned long path)
{
  while(depth--)
  {
    if(path&1) printf("->rval"); else printf("->lval");
    path>>=1;
  }
}

bool treeshapecond(YYSTYPE v, bool firstopt)
{
  if(!v) return firstopt;
  /* To prevent possibly dereferencing a null pointer, check the root ick_first */
  if(v->mustbemesh) /* it's a must-be-constant constraint */
  {
    printf(firstopt?"  if((np":" &&\n     (np");
    gennodepath(v->depth,v->path);
    printf("->opcode==MESH || np");
    gennodepath(v->depth,v->path);
    printf("->opcode==MESH32)");
    firstopt=0;
  }
  if(v->condition&&!v->cxneeded) /* it's a tree-shape constraint */
  {
    printf(firstopt?"  if(np":" &&\n     np");
    gennodepath(v->depth,v->path);
    printf("%s",v->condition);
    if(v->usec) printf("%luLU",v->c);
    firstopt=0;
  }
  if(v->ublo) /* generate a width check */
  {
    printf(firstopt?"  if(np":" &&\n     np");
    gennodepath(v->depth,v->path);
    printf("->width==%d",v->width32?32:16);
    firstopt=0;
  }
  firstopt=treeshapecond(v->n1,firstopt);
  return treeshapecond(v->n2,firstopt);
}

YYSTYPE treenscheck(YYSTYPE v, YYSTYPE prev, int replnum)
{
  if(!v) return prev;
  prev=treenscheck(v->n1,prev,replnum);
  prev=treenscheck(v->n2,prev,replnum);
  if(v->replnum!=replnum) return prev;
  if(prev)
  {
    printf("    if(!nodessame(np");
    gennodepath(prev->depth,prev->path);
    printf(",np");
    gennodepath(v->depth,v->path);
    printf(")) break;\n");
  }
  return v;
}

void treecxcond(YYSTYPE v)
{
  if(!v) return;
  if(v->cxneeded&&strcmp(v->condition,"1"))
  {
    if(v->replnum)
      printf("    x=x%d; c=c%d; ",v->replnum,v->replnum);
    else
    {
      printf("    x=np");
      gennodepath(v->depth,v->path);
      printf("->constant;\n    c=np");
      gennodepath(v->depth,v->path);
      printf("->optdata;\n    ");
    }
    printf("if(!(%s)) break;\n",v->condition);
  }
  treecxcond(v->n1);
  treecxcond(v->n2);
}

void treerepcount(YYSTYPE v, int* rc)
{
  if(!v) return;
  if(!(v->nodetypename)&&v->replnum&&!(v->cxneeded)) rc[v->replnum]++;
  treerepcount(v->n1, rc);
  treerepcount(v->n2, rc);
}

void treerepgen(YYSTYPE v, YYSTYPE* refs, int* rc)
{
  if(!v) return;
  /* We absolutely have to generate the root node ick_first here, because otherwise
     the nodes in question won't exist. */
  if(v->nodetypename) /* Generate an intermediate node */
  {
    printf("    MAYBENEWNODE(tp");
    gennodepath(v->depth,v->path);
    printf(");\n    tp");
    gennodepath(v->depth,v->path);
    printf("->opcode=%s;\n    tp",v->nodetypename);
    gennodepath(v->depth,v->path);
    printf("->width=%d;\n",v->width32?32:16);
    /* optdata will be filled in by checknodeactbits before the ick_next idiom is
       tested; constant is irrelevant, lval and rval are NULL at present and
       will be filled in by later recursions of this function, and I seriously
       hope that nextslat is never filled in by an optimizer idiom. */
  }
  else if(v->replnum&&!(v->cxneeded))
  {
    /* Copy a node from the template. The node ought not to be allocated at
       this point, so we can safely just ick_assign to it with a new malloced
       node. */
    if(refs[v->replnum])
    {
      if(rc[v->replnum]>1||!refs[v->replnum]->depth)
      {
	/* The node actually has to be copied, either because another copy is
	   needed or because it's np itself that's being copied over. */
	rc[v->replnum]--;
	printf("    tp");
	gennodepath(v->depth,v->path);
	printf("=nodecopy(np");
	gennodepath(refs[v->replnum]->depth,refs[v->replnum]->path);
	printf(");\n");
      }
      else
      {
	/* This can be optimized slightly by moving rather than copying,
	   zeroing backlinks so that the node won't be freed. */
	rc[v->replnum]--;
	printf("    tp");
	gennodepath(v->depth,v->path);
	printf("=np");
	gennodepath(refs[v->replnum]->depth,refs[v->replnum]->path);
	printf(";\n    np");
	gennodepath(refs[v->replnum]->depth,refs[v->replnum]->path);
	printf("=0;\n");
      }
    }
    else yyerror("Replacement isn't in the template");
  }
  else if(v->cxneeded)
  {
    /* Generate a constant node based on an expression (#{expr}0). */
    printf("    MAYBENEWNODE(tp");
    gennodepath(v->depth,v->path);
    printf(");\n    tp");
    gennodepath(v->depth,v->path);
    printf("->opcode=MESH32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->width=32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->constant=(%s);\n",v->condition);
  }
  else if(v->mustbemesh&&!v->n1)
  {
    /* Generate a constant node based on a constant (#65535). */
    printf("    MAYBENEWNODE(tp");
    gennodepath(v->depth,v->path);
    printf(");\n    tp");
    gennodepath(v->depth,v->path);
    printf("->opcode=MESH32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->width=32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->constant=(%luLU);\n",v->c);
  }
  else if(v->mustbemesh&&v->n1)
    /* let this node's n1 handle it */ ;
  else
    yyerror("can't figure out how to generate a replacement node\n");
  treerepgen(v->n1,refs,rc);
  treerepgen(v->n2,refs,rc);
}

int yyerror(char const *s)
{
  fprintf(stderr,"Error at (%d:%d): \"%s\"\n",cgcrow,cgccol,s);
  return 0; /* this return value is ignored anyway */
}

static int cgcpushback=0;

int countgetchar(void)
{
  int c=getchar();
  if(cgcpushback) {cgcpushback=0; return c;}
  cgccol++;
  if(c=='\n') {cgccol=0; cgcrow++;}
  return c;
}

int countungetc(int c, FILE* f)
{
  ungetc(c,f);
  cgcpushback=1;
  return c;
}

void splitstart(void)
{
  static char fname[]="oilout00.c";
  FILE *dummy;     /* GCC 4 un-suppressable warning suck */
  if(filenumber>255)
  {
    filenumber=255;
    fprintf(stdout,"Input file too long.\n");
  }
  sprintf(fname,"oilout%02x.c",filenumber);
  dummy = freopen(fname,"w",stdout);
  puts("/* Automatically generated output, edit source and recompile to "
       "change */");
  printf("#include \"oil.h\"\n"
	 "int optimize_pass1_%x(node *np)\n"
	 "{"
	 "  int opted=0;\n"
	 "  unsigned long c,c1,c2,c3,c4,c5,c6,c7,c8,c9;\n"
	 "  unsigned long x,x1,x2,x3,x4,x5,x6,x7,x8,x9,r;\n"
	 "  int tempw;\n"
	 "  node *tp;\n", filenumber);
}

void splitend(void)
{
  /* Disabling warnings about unused variables. gcc will optimize this right
     out, and in any case the raise(SIGSEGV) will be unreachable (but will
     cause a pretty recognizable error because it'll be caught by the handler
     for SIGSEGV and output an internal error, with an obvious debug backtrace
     if -U is used). */
  printf("  c=c1=c2=c3=c4=c5=c6=c7=c8=c9=0;\n"
	 "  x=x1=x2=x3=x4=x5=x6=x7=x8=x9=r=0;\n"
	 "  if(c+c1+c2+c3+c4+c5+c6+c7+c8+c9+r+\n"
	 "     x+x1+x2+x3+x4+x5+x6+x7+x8+x9) raise(SIGSEGV);\n");
  printf("  return opted;\n}\n");
  /* do not close stdout; freopen implicitly closes it anyway, and explicitly
     closing it breaks on DOS */
}

int main(void)
{
  int e,i;
  FILE *dummy;    /* GCC 4 un-suppressible warnings suck */
  /*
    "  if(!np) return 0;\n"
    "  if(np->lval) opted|=optimize_pass1(np->lval);\n"
    "  if(np->rval) opted|=optimize_pass1(np->rval);\n"
  */
  splitstart();
  cgccol=0;
  cgcrow=1;
  e=yyparse();
  while(tfi--) free(tofree[tfi]);
  splitend();
  dummy = freopen("oilout-m.c","w",stdout);
  puts("/* Automatically generated output, edit source and recompile to "
       "change */");
  puts("#include \"config.h\"");
  puts("#include \"ick.h\"");
  i=filenumber+1;
  while(i--) printf("extern int optimize_pass1_%x(node*);\n",i);
  puts("int optimize_pass1(node* np)\n"
       "{\n"
       "  int opted=0;\n"
       "  if(!np) return 0;\n"
       "  if(np->lval) opted|=optimize_pass1(np->lval);\n"
       "  if(np->rval) opted|=optimize_pass1(np->rval);");
  i=filenumber+1;
  while(i--) printf("  opted|=optimize_pass1_%x(np);\n",i);
  puts("  return opted;\n"
       "}");
  return e;
}
