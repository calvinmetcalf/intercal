/****************************************************************************

NAME
   perpetrate.c -- main routine for C-INTERCAL compiler.

DESCRIPTION
   This is where all the dirty work begins and ends.

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
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "ick.h"
#include "feh.h"
#include "y.tab.h"
#include "sizes.h"
#include "lose.h"

#ifndef ICKINCLUDEDIR
#define ICKINCLUDEDIR "/usr/local/include"
#endif
#ifndef ICKLIBDIR
#define ICKLIBDIR "/usr/local/share"
#endif
#ifndef CC
#define CC "gcc"
#endif

#ifdef USE_YYRESTART
/* function supplied by lex */
extern void yyrestart(FILE*);
#endif /* USE_YYRESTART */

/* function created by yacc */
extern int yyparse(void);

int yydebug;

/* compilation options */
bool compile_only; 	/* just compile into C, don't run the linker */
bool traditional;	/* insist on strict INTERCAL-72 conformance */
bool nocompilerbug;	/* disable error E774 */

static bool dooptimize;	/* do optimizations? (controlled by -O) */
static bool clockface;	/* set up output to do IIII for IV */

#define SKELETON	"ick-wrap.c"
#define SYSLIB          "pit/lib/syslib"

/* numeric base defaults, exported to other files */

#define DEFAULT_BASE 2
#define DEFAULT_SMALL_DIGITS 16
#define DEFAULT_LARGE_DIGITS 32
#define DEFAULT_MAX_SMALL 0xffffL
#define DEFAULT_MAX_LARGE 0xffffffffL

int Base;
int Small_digits;
int Large_digits;
unsigned int Max_small;
unsigned int Max_large;

int lineno;	/* after yyparse, this is the total number of statements */

/* currently supported numeric bases, not exported */
static int maxbase = 7;
static int smallsizes[8] = {0, 0, 16, 10, 8, 6, 6, 5};
static unsigned int maxsmalls[8] =
  {0, 0, 65535, 59048, 65535, 15624, 46655, 16806};

static char *compiler;

atom *oblist = NULL, *obdex;
int obcount = 0;
int nonespots, ntwospots, ntails, nhybrids;

tuple *tuples = NULL;
int tuplecount = 0;

static void abend(int signim)
{
    lose(E778, yylineno, (char *)NULL);
    (void) signim;
}

static void print_usage(char *prog, char *options)
{
    fprintf(stderr,"Usage: %s [-%s] <file> [<file> ... ]\n",prog,options);
    fprintf(stderr,"\t-b\t:reduce the probability of E774 to zero\n");
    fprintf(stderr,"\t-c\t:compile INTERCAL to C, but don't compile C\n");
    fprintf(stderr,"\t-d\t:print debugging information (implies -c)\n");
    fprintf(stderr,"\t-t\t:traditional mode, accept only INTERCAL-72\n");
    fprintf(stderr,"\t-C\t:clockface output (e.g. use IIII instead of IV)\n");
    fprintf(stderr,"\t-O\t:attempt to optimize generated code\n");
    fprintf(stderr,"\t<file>\tINTERCAL source file(s) (use extension .i)\n");
    fprintf(stderr,"\t\teach file produces a separate output program.\n");
}

int main(int argc, char *argv[])
{
    extern int	optind;		/* set by getopt */
    char	buf[BUFSIZ], buf2[BUFSIZ], *chp, *strrchr();
    tuple	*tp;
    atom	*op;
    int		c, i;
    char	*includedir, *libdir, *getenv();
    FILE	*ifp, *ofp;
    int		maxabstain, nextcount, bugline;
    bool        needsyslib, firstfile;

    if (!(includedir = getenv("ICKINCLUDEDIR")))
      includedir = ICKINCLUDEDIR;
    if (!(libdir = getenv("ICKLIBDIR")))
      libdir = ICKLIBDIR;
    if (!(compiler = getenv("CC")))
      compiler = CC;

    while ((c = getopt(argc, argv, "bcdtOC@")) != EOF)
    {
	switch (c)
	{
	case 'b':
	    nocompilerbug = TRUE;
	    break;

	case 'c':
	    compile_only = TRUE;
	    break;

	case 'd':
	    yydebug = compile_only = TRUE;
	    break;

	case 'C':
	    clockface = TRUE;
	    break;

	case 't':
	    traditional = TRUE;
	    break;

	case 'O':
	    dooptimize = TRUE;
	    break;

	case '?':
	default:
	case '@':
	    print_usage(argv[0],"bcdtCO");
	    exit(1);
	    break;
	}
    }

    (void) signal(SIGSEGV, abend);
#ifdef SIGBUS
    (void) signal(SIGBUS, abend);
#endif /* SIGBUS */

    if (!nocompilerbug) {
#ifdef USG
	srand48(time(NULL) + getpid());
#else
	srand(time(NULL));
#endif /* UNIX */
    }

    (void) sprintf(buf2,"%s/%s",libdir,SKELETON);

    /* now substitute in tokens in the skeleton */
    if ((ifp = fopen(buf2, "r")) == (FILE *)NULL)
	lose(E999, 1, (char *)NULL);
    buf[strlen(buf) - 2] = '\0';

    for (firstfile = TRUE; optind < argc; optind++, firstfile = FALSE)
    {
	if (freopen(argv[optind], "r", stdin) == (FILE *)NULL)
	    lose(E777, 1, (char *)NULL);
	else
	{
	    /* strip off the file extension */
	    if(!(chp = strrchr(argv[optind],'.')))
	    {
		lose(E998, 1, (char *)NULL);
	    }
	    *chp++ = '\0';

	    /* wwp: reset the base variables to defaults, because if the  */
	    /* sourcefile has extension .i they will not be reset in the  */
	    /* following chunk of code. but i don't want to modify the    */
	    /* following chunk of code because i think it is very clever; */
	    /* grabs the base on the first pass, then validates the rest  */
	    /* of the extension on the second.                            */
	    Base = DEFAULT_BASE;
	    Small_digits = DEFAULT_SMALL_DIGITS;
	    Large_digits = DEFAULT_LARGE_DIGITS;
	    Max_small = DEFAULT_MAX_SMALL;
	    Max_large = DEFAULT_MAX_LARGE;

	    /* determine the file type from the extension */
	    while (strcmp(chp,"i"))
	    {
		Base = strtol(chp,&chp,10);
		if (Base < 2 || Base > maxbase)
		    lose(E998, 1, (char *)NULL);
		else if (traditional && Base != 2)
		    lose(E111, 1, (char *)NULL);
		Small_digits = smallsizes[Base];
		Large_digits = 2 * Small_digits;
		Max_small = maxsmalls[Base];
		if (Max_small == 0xffff)
		    Max_large = 0xffffffff;
		else
		    Max_large = (Max_small + 1) * (Max_small + 1) - 1;
	    }

	    /* zero out tuple and oblist storage */
	    treset();
	    politesse = 0;

	    /* reset the lex/yacc environment */
	    if (!firstfile)
	    {
#ifdef NEED_YYRESTART
	      yyrestart(stdin);
#endif /* NEED_YYRESTART */
#ifdef YYLINENO_BY_HAND
	      yylineno = 1;
#endif /* YYLINENO_BY_HAND */
	    }

	    /* compile tuples from current input source */
	    yyparse();	

	    /*
	     * Miss Manners lives.
	     */
	    if (lineno > 2)
	    {
		if (politesse == 0 || (lineno - 1) / politesse >= 5)
		    lose(E079, yylineno, (char *)NULL);
		else if (lineno / politesse < 3)
		    lose(E099, yylineno, (char *)NULL);
	    }

	    /*
	     * check if we need to magically include the system library
	     */
	    needsyslib = FALSE;
	    for (tp = tuples; tp->type; tp++) {
	      /*
	       * If some label in the (1000)-(2000) range is defined,
	       * then clearly the syslib is already there, so we
	       * can stop searching and won't need the syslib.
	       */
	      if (tp->label >= 1000 && tp->label <= 1999) {
		needsyslib = FALSE;
		break;
	      }
	      /*
	       * If some label in the (1000)-(2000) range is being
	       * called, we might need the system library.
	       */
	      if (tp->type == NEXT && tp->u.target >= 1000 &&
		  tp->u.target <= 1999)
		needsyslib = TRUE;
	    }
	    if ( needsyslib ) {
	      if (Base == 2)
		(void) sprintf(buf2, "%s/%s.i", libdir, SYSLIB);
	      else
		(void) sprintf(buf2, "%s/%s.%di", libdir, SYSLIB, Base);
	      if ( freopen(buf2, "r", stdin) == (FILE*) NULL ) {
		lose(E127, 1, (char*) NULL);
	      }
#ifdef USE_YYRESTART
	      yyrestart(stdin);
#endif /* USE_YYRESTART */
	      yyparse();
	    }

	    /* 
	     * Now propagate type information up the expression tree.
	     * We need to do this because the unary-logical operations
	     * are sensitive to the type widths of their operands, so
	     * we have to generate different code depending on the
	     * deducible type of the operand.
	     */
	    for (tp = tuples; tp->type; tp++)
		if (tp->type == GETS || tp->type == RESIZE
                    || tp->type == WRITE_IN || tp->type == READ_OUT
		    || tp->type == FORGET || tp->type == RESUME)
		    typecast(tp->u.node);

	    codecheck();	/* check for compile-time errors */

	    /* perform optimizations */
	    if (dooptimize)
		for (tp = tuples; tp->type; tp++)
		    if (tp->type == GETS || tp->type == RESIZE
			|| tp->type == FORGET || tp->type == RESUME)
			optimize(tp->u.node);

	    /* decide if and where to place the compiler bug */
#ifdef USG
	    if (!nocompilerbug && lrand48() % 10 == 0)
		bugline = (int)(lrand48() % lineno);
#else
	    if (!nocompilerbug && rand() % 10 == 0)
		bugline = rand() % lineno;
#endif
	    else
		bugline = -1;

	    /* set up the generated C output file name */
	    (void) strcpy(buf, argv[optind]);
	    (void) strcat(buf, ".c");
	    if ((ofp = fopen(buf, "w")) == (FILE *)NULL)
		lose(E888, 1, (char *)NULL);
	    
	    fseek(ifp,0L,0);	/* rewind skeleton file */

	    while ((c = fgetc(ifp)) != EOF)
		if (c != '$')
		    (void) fputc(c, ofp);
	        else switch(fgetc(ifp))
		{
		case 'A':	/* source name stem */
		    (void) fputs(argv[optind], ofp);
		    break;

		case 'B':	/* # of statements */
		    (void) fprintf(ofp, "%d", lineno);
		    break;

		case 'C':	/* initial abstentions */
		    maxabstain = 0;
		    for (tp = tuples; tp->type; tp++)
			if (tp->exechance <= 0 && tp - tuples + 1 > maxabstain)
			    maxabstain = tp - tuples + 1;
		    if (maxabstain)
		    {
			(void) fprintf(ofp, " = {");
			for (tp = tuples; tp < tuples + maxabstain; tp++)
			    if (tp->exechance > 0)
				(void) fprintf(ofp, "0, ");
			    else {
				(void) fprintf(ofp, "1, ");
				tp->exechance = -tp->exechance;
			    }
			(void) fprintf(ofp, "}");
		    }
		    break;

		case 'D':	/* linetypes array for abstention handling */
		    maxabstain = 0;
		    for (tp = tuples; tp->type; tp++)
			if (tp->type == ENABLE || tp->type == DISABLE)
			    maxabstain++;
		    if (maxabstain)
		    {
			(void) fprintf(ofp, "#define UNKNOWN\t\t0\n");
			i = 0;
			for (; i < (int)(sizeof(enablers)/sizeof(char *)); i++)
			    (void) fprintf(ofp,
					   "#define %s\t%d\n",
					   enablers[i], i+1);

			(void) fprintf(ofp, "int linetype[] = {\n");
			for (tp = tuples; tp->type; tp++)
			    if (tp->type >= GETS && tp->type <= COME_FROM)
				(void) fprintf(ofp,
					       "    %s,\n",
					       enablers[tp->type - GETS]);
			    else
				(void) fprintf(ofp, "    UNKNOWN,\n");
			(void) fprintf(ofp, "};\n");
		    }
		    break;

		case 'E':	/* extern to intern map */
		    (void) fprintf(ofp,"int Base = %d;\n",Base);
		    (void) fprintf(ofp,"int Small_digits = %d;\n",
				   Small_digits);
		    (void) fprintf(ofp,"int Large_digits = %d;\n",
				   Large_digits);
		    (void) fprintf(ofp,"unsigned int Max_small = 0x%x;\n",
				   Max_small);
		    (void) fprintf(ofp,"unsigned int Max_large = 0x%x;\n",
				   Max_large);
		    if (nonespots)
		    {
			(void) fprintf(ofp,
				       "static type16 onespots[%d];\n",
				       nonespots);
			(void) fprintf(ofp,
				       "static bool oneforget[%d];\n",
				       nonespots);
		    }
		    if (ntwospots)
		    {
			(void) fprintf(ofp,
				       "static type32 twospots[%d];\n",
				       ntwospots);
			(void) fprintf(ofp,
				       "static bool twoforget[%d];\n",
				       ntwospots);
		    }
		    if (ntails)
		    {
			(void) fprintf(ofp,
				       "static array tails[%d];\n",
				       ntails);
			(void) fprintf(ofp,
				       "static bool tailforget[%d];\n",
				       ntails);
		    }
		    if (nhybrids)
		    {
			(void) fprintf(ofp,
				       "static array hybrids[%d];\n",
				       nhybrids);
			(void) fprintf(ofp,
				       "static bool hyforget[%d];\n",
				       nhybrids);
		    }
		    if (yydebug || compile_only)
			for (op = oblist; op < obdex; op++)
			    (void) fprintf(ofp, " /* %s %d -> %d */\n",
					   nameof(op->type, vartypes),
					   op->extindex,
					   op->intindex);
		    break;

		case 'F':	/* set clockface option? */
		    if (clockface)
			(void) fprintf(ofp, "clockface(TRUE);");
		    break;

		case 'G':	/* degenerated code */
		    for (tp = tuples, i = 0; tp->type; tp++, i++)
		    {
			emit(tp, ofp);
			if (i == bugline)
			    (void) fprintf(ofp, "    lose(E774, lineno, "
						"(char *)NULL);\n");
		    }
		    break;

		case 'H':	/* dispatching for resumes */
		    nextcount = 0;
		    for (tp = tuples; tp->type; tp++)
			if (tp->type == NEXT)
			    nextcount++;
		    if (nextcount)
		    {
			(void) fputs("/* generated switch for resumes */",ofp);
			(void) fputs("top:\n    switch(skipto)\n    {\n", ofp);
			for (tp = tuples; tp->type; tp++)
			    if (tp->type == NEXT)
				(void) fprintf(ofp,
					       "\tcase %d: goto N%d; break;\n",
					       (int)(tp-tuples+1), (int)(tp-tuples+1));
			(void) fprintf(ofp, "    }");
		    }
		    break;

		case 'J':	/* # of source file lines */
		    (void) fprintf(ofp, "%d", yylineno);
		    break;
		}

	    (void) fclose(ofp);

	    /* OK, now sic the C compiler on the results */
	    if (!compile_only)
	    {
		(void) sprintf(buf2,
			       "%s %s -I%s -L%s -lick -o %s",
			       compiler, buf, includedir, libdir,
			       argv[optind]);
		(void) system(buf2);
		(void) unlink(buf);
	    }
	}
    }
    (void) fclose(ifp);
    return 0;
}

/* perpetrate.c ends here */
