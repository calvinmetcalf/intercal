/****************************************************************************

NAME
   perpet.c -- main routine for C-INTERCAL compiler.

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
#include "parser.h"
#include "sizes.h"
#include "lose.h"

#ifndef ICKINCLUDEDIR
#define ICKINCLUDEDIR "/usr/local/include"
#endif
#ifndef ICKLIBDIR
#define ICKLIBDIR "/usr/local/share"
#endif
#ifndef ICKBINDIR
#define ICKBINDIR "/usr/local/bin"
#endif
#ifndef CC
#define CC "gcc"
#endif

#define ARGSTRING "bcdfghlmptuvyCFHOPUX@"

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
int yukdebug;           /* AIS: Use the yuk debugger. */
int yukprofile;         /* AIS: Use the yuk profiler. */
extern int coreonerr;   /* AIS: Dump core on E778. (defined in lose.c) */
int multithread;        /* AIS: Allow multithreading and backtracking. */
int variableconstants;  /* AIS: Allow anything on the left of an assignment. */
int cdebug;             /* AIS: Pass -g to our C compiler, and leave C code. */
int optdebug;           /* AIS: Debug the optimizer. Value is 0, 1, 2, or 3. */
int flowoptimize;       /* AIS: Do flow optimizations (in INTERCAL!). */
int coopt;              /* AIS: The constant-output optimization. This should
			   mean that INTERCAL will beat any other language at
			   many benchmark programs (!) */
int printfopens;        /* AIS: Print messages whenever attempting to open a
			   file */
extern int checkforbugs;/* AIS: Try to find possible bugs in the source code */
int pickcompile;        /* AIS: Compile for PIC? */
int clclex;             /* AIS: 1 means use CLC-INTERCAL meanings for @, ? */
int outtostdout;        /* AIS: Output on stdout rather than the output file */

/* AIS: Autodetected compilation options */
int compucomecount=0;   /* Computed COME FROM count */
int compucomesused=0;   /* Are computed COME FROMs used? */
int gerucomesused=0;    /* Is COME FROM gerund used? */
int nextfromsused=0;    /* Is NEXT FROM used? */
int opoverused=0;       /* Is operand overloading used? */
node* firstslat=0;      /* The first slat expression in the program */
node* prevslat=0;       /* The last slat expression used so far */

static bool dooptimize;	/* do optimizations? (controlled by -O) */
static bool clockface;	/* set up output to do IIII for IV */

#define SKELETON  "ick-wrap.c"
#define PSKELETON "pickwrap.c"
#define SYSLIB    "syslib"

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
int nmeshes; /* AIS */

tuple *tuples = NULL;
int tuplecount = 0;

tuple *optuple = NULL; /* AIS: Tuple being optimized */

extern assoc varstores[]; /* AIS: Need to know this for PIC compilation */

static int myfgetc(FILE* in)
{
  char c;
  fread(&c,1,1,in);
  if(feof(in)) return EOF;
  return (int)c;
}

static FILE* debfopen(char* fname, char* mode)
{
  FILE* t;
  if(printfopens) fprintf(stderr,"Trying to open '%s'...\n",fname);
  t=fopen(fname,mode);
  if(printfopens&&t) fprintf(stderr,"Success!\n");
  if(printfopens&&!t) fprintf(stderr,"Failed!\n");
  return t;
}

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
    fprintf(stderr,"\t-d\t:print yacc debugging information (implies -c)\n");
    fprintf(stderr,"\t-t\t:traditional mode, accept only INTERCAL-72\n");
    fprintf(stderr,"\t-C\t:clockface output (e.g. use IIII instead of IV)\n");
    fprintf(stderr,"\t-O\t:optimize expresssions in generated code\n");
    /* AIS: Changed the help message for the previous line (because the
       function of -O has changed). I wrote the next group of options. */
    fprintf(stderr,"\t-f\t:optimize control flow in generated code "
	    "(prevents -yp)\n");
    fprintf(stderr,"\t-F\t:optimize everything in generated code for\n"
	    "\t\tspeed, regardless of how slow the compiler becomes or how\n"
	    "\t\tlarge the object file becomes. Implies -fO,\n"
	    "\t\tprevents -cdghpyH\n");
    fprintf(stderr,"\t-h\t:print optimizer debugging information "
	    "(implies -cO)\n");
    fprintf(stderr,"\t-H\t:print verbose optimizer debugging information "
	    "(implies -cO)\n");
    fprintf(stderr,"\t-hH\t:print optimizer debugging information in a\n"
	    "\t\tdifferent form (implies -cO)\n");
    fprintf(stderr,"\t-y\t:run the yuk debugger on the code (prevents -fm)\n");
    fprintf(stderr,"\t-p\t:run the yuk profiler on the code (prevents -fm)\n");
    fprintf(stderr,"\t-m\t:allow multithreading and backtracking "
	    "(prevents -yp)\n");
    fprintf(stderr,"\t-v\t:allow anything on the left of an assignment. This "
	    "is required\n\t\tif you want operand overloading. "
	    "(prevents -fFOP)\n");
    fprintf(stderr,"\t-P\t:compile PIC-INTERCAL rather than INTERCAL\n");
    fprintf(stderr,"\t-o\t:output to stdout rather than .c (implies -c)");
    fprintf(stderr,"\t-X\t:interpret ? and @ with CLC-INTERCAL meanings\n");
    fprintf(stderr,"\t-u\t:print a message whenever the compiler tries to "
	    "open a file\n");
    fprintf(stderr,"\t-U\t:dump core on E778 rather than printing an error\n");
    fprintf(stderr,"\t-g\t:compile to both debuggable executable and C\n");
    fprintf(stderr,"\t-l\t:attempt to report likely bugs "
	    "and nonportabilities (implies -O)\n");
    /* AIS: End of options I added. */
    fprintf(stderr,"\t<file>\tINTERCAL source file(s) (use extension .i)\n");
    fprintf(stderr,"\t\teach file produces a separate output program.\n");
}

/* AIS: Determine whether an environment variable exists (this is used to
   find a temp directory) */
int isenv(char* e)
{
  char* x=getenv(e);
  return x&&*x;
}

/* AIS: This function looks for the skeleton and syslib, searching first the
   path they should be in, then the current directory, then argv[0]'s
   directory (if one was given). This function avoids possible buffer
   overflows, instead truncating filenames (and if that manages to find them,
   I'll be incredibly surprised). It also tries argv[0]/../lib and
   argv[0]/../include (where they are when running without installing). */
static FILE* findandfopen(char* file, char* guessdir, char* mode, char* argv0)
{
  char buf2[BUFSIZ], *fileiter;
  int i = 0, j;
  FILE* ret;
  while(*guessdir&&i<BUFSIZ-2) buf2[i++] = *guessdir++;
  buf2[i++] = '/';
  fileiter = file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = 0;
  ret = debfopen(buf2,mode); /* where it ought to be */
  if(ret) return ret;
  ret = debfopen(file,mode); /* current dir */
  if(ret) return ret;
  if(!strchr(argv0,'/')&&
     !strchr(argv0,'\\')) return 0; /* argv[0] has no dir specified */
  i = j = 0;
  while(*argv0&&i<BUFSIZ-2)
  {
    buf2[i++] = *argv0++;
    if(*argv0=='/') j = i;
    if(*argv0=='\\') j = i;
  }
  i = j + 1;
  fileiter=file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = 0;
  ret = debfopen(buf2,mode); /* argv[0]'s dir */
  if(ret) return ret;
  i = j + 1;
  fileiter="../lib/"; /* correct for POSIX and DJGPP */
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]=0;
  ret = debfopen(buf2,mode); /* argv[0]/../lib/ */
  if(ret) return ret;
  i = j + 1;
  fileiter="../include/"; /* correct for POSIX and DJGPP */
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]=0;
  ret = debfopen(buf2,mode); /* argv[0]/../include/ */
  if(ret) return ret;
  return 0; /* just return 0 if even this failed */
}

/* AIS: The same, looking for an executable */
static char* findandtestopen(char* file, char* guessdir, char* mode,
			     char* argv0)
{
  static char buf2[BUFSIZ], *fileiter;
  int i = 0, j;
  FILE* ret;
  while(*guessdir&&i<BUFSIZ-2) buf2[i++] = *guessdir++;
  buf2[i++] = '/';
  fileiter = file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = 0;
  ret = debfopen(buf2,mode); /* where it ought to be */
  if(ret) {fclose(ret); return buf2;}
  ret = debfopen(file,mode); /* current dir */
  if(ret) {fclose(ret); return file;}
  if(!strchr(argv0,'/')&&
     !strchr(argv0,'\\')) return 0; /* argv[0] has no dir specified */
  i = j = 0;
  while(*argv0&&i<BUFSIZ-2)
  {
    buf2[i++] = *argv0++;
    if(*argv0=='/') j = i;
    if(*argv0=='\\') j = i;
  }
  i = j + 1;
  fileiter=file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = 0;
  ret = debfopen(buf2,mode); /* argv[0]'s dir */
  if(ret) {fclose(ret); return buf2;}
  i = j + 1;
  fileiter="../lib/"; /* correct for POSIX and DJGPP */
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]=0;
  ret = debfopen(buf2,mode); /* argv[0]/../lib/ */
  if(ret) {fclose(ret); return buf2;}
    i = j + 1;
  fileiter="../include/"; /* correct for POSIX and DJGPP */
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter&&i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]=0;
  ret = debfopen(buf2,mode); /* argv[0]/../include/ */
  if(ret) {fclose(ret); return buf2;}
  return 0; /* just return 0 if even this failed */
}

/* AIS: The same thing, but with freopen */
static FILE* findandfreopen(char* file, char* guessdir, char* mode,
			    char* argv0, FILE* over)
{
  char* s=findandtestopen(file,guessdir,mode,argv0);
  return freopen(s,mode,over);
}

int main(int argc, char *argv[])
{
    extern int	optind;		/* set by getopt */
    char	buf[BUFSIZ], buf2[BUFSIZ], *chp; /* AIS: , *strrchr(); */
    tuple	*tp;
    atom	*op;
    int		c, i;
    char	*includedir, *libdir; /* AIS: , *getenv(); */
    char        *bindir, *cooptsh;
    FILE	*ifp, *ofp;
    int		maxabstain, /* nextcount, AIS */ bugline;
    bool        needsyslib, firstfile;
    int         oldstdin; /* AIS: for keeping track of where stdin was */

    if (!(includedir = getenv("ICKINCLUDEDIR")))
      includedir = ICKINCLUDEDIR;
    if (!(libdir = getenv("ICKLIBDIR")))
      libdir = ICKLIBDIR;
    if (!(bindir = getenv("ICKBINDIR"))) /* AIS */
      bindir = ICKBINDIR;    
    if (!(compiler = getenv("CC")))
      compiler = CC;

    while ((c = getopt(argc, argv, ARGSTRING)) != EOF)
    {
	switch (c)
	{
	case 'b':
	    nocompilerbug = TRUE;
	    break;

	case 'c':
	    compile_only = TRUE;
	    /* AIS */ coopt = FALSE;
	    break;

	case 'o': /* AIS */
	  compile_only = TRUE;
	  outtostdout = TRUE;
	  coopt = FALSE;
	  break;

	case 'd':
	    yydebug = compile_only = TRUE;
	    /* AIS */ coopt = FALSE;
	    break;

	case 'C':
	    clockface = TRUE;
	    break;

	case 't':
	    traditional = TRUE;
	    if(multithread) lose(E111, 1, (char*) NULL); /* AIS */
	    if(pickcompile) lose(E111, 1, (char*) NULL); /* AIS */
	    break;

	case 'O':
	    dooptimize = TRUE;
	    variableconstants = FALSE; /* AIS */
	    break;

	case 'f': /* By AIS */
	  flowoptimize = TRUE;
	  yukdebug = yukprofile = FALSE;
	  variableconstants = FALSE;
	  break;
	  
	case 'F': /* By AIS */
	  coopt = flowoptimize = dooptimize = TRUE;
	  variableconstants = FALSE;
	  yukdebug = yukprofile = yydebug = outtostdout =
	    compile_only = cdebug = FALSE;
	  if(pickcompile) lose(E256, 1, (char*) NULL);
	  break;
	  
	case 'h': /* By AIS */
	  optdebug|=1;
	  compile_only=dooptimize=TRUE;
	  coopt=FALSE;
	  break;

	case 'H': /* By AIS */
	  optdebug|=2;
	  compile_only=dooptimize=TRUE;
	  coopt=FALSE;
	  break;
	  
	case 'y': /* By AIS */
	  yukdebug=TRUE;
	  multithread=flowoptimize=coopt=FALSE;
	  break;

	case 'p': /* By AIS */
	  yukprofile=TRUE;
	  multithread=flowoptimize=coopt=FALSE;
	  break;

	case 'm': /* By AIS */
	  multithread=TRUE;
	  yukprofile=FALSE;
	  yukdebug=FALSE;
	  if(traditional) lose(E111, 1, (char*) NULL);
	  break;

	case 'v': /* By AIS */
	  variableconstants=TRUE;
	  dooptimize=FALSE;
	  flowoptimize=FALSE;
	  coopt=FALSE;
	  pickcompile=FALSE;
	  break;
	  
	case 'l': /* By AIS */
	  checkforbugs=TRUE;
	  dooptimize=TRUE;
	  break;

	case 'U': /* By AIS */
	  coreonerr=TRUE;
	  break;

	case 'u': /* By AIS */
	  printfopens=TRUE;
	  break;

	case 'P': /* By AIS */
	  pickcompile=TRUE;
	  multithread=coopt=variableconstants=FALSE;
	  compile_only=TRUE;
	  dooptimize=flowoptimize=TRUE; /* needed for PICs */
	  break;
	  
	case 'X': /* By AIS */
	  clclex=TRUE;
	  break;

	case 'g': /* By AIS */
	  cdebug=TRUE;
	  coopt=FALSE;
	  break;
	    
	case '?':
	default:
	case '@':
	    print_usage(argv[0], ARGSTRING);
	    exit(1);
	    break;
	}
    }

    /* AIS: This has only been able to compile 1 file at a time for ages,
       as far as I can tell, without problems. */
    if(optind+1<argc)
    {
      print_usage(argv[0], ARGSTRING);
      exit(1);
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

    /* AIS: New function for enhanced file-finding */
    if ((ifp = findandfopen(pickcompile?PSKELETON:SKELETON,
			    libdir, "r", argv[0])) == 0)
      lose(E999, 1, (char *)NULL);
	
    /* now substitute in tokens in the skeleton */

    buf[strlen(buf) - 2] = '\0';

    /* AIS: Save the old stdin */
    oldstdin=dup(0);

    for (firstfile = TRUE; optind < argc; optind++, firstfile = FALSE)
    {
      /* AIS: Read as binary to pick up Latin-1 and UTF-8 better */
	if (freopen(argv[optind], "rb", stdin) == (FILE *)NULL)
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
		else if (pickcompile && Base != 2)
		    lose(E256, 1, (char *)NULL); /* AIS */
		Small_digits = smallsizes[Base];
		Large_digits = 2 * Small_digits;
		Max_small = maxsmalls[Base];
		if (Max_small == 0xffff)
		    Max_large = 0xffffffffLU;
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

	    if(variableconstants)
	    {
	      /* AIS: Up to 4 extra meshes may be needed by feh.c. */
	      intern(MESH, 0xFFFFFFFFLU);
	      intern(MESH, 0xFFFFLU);
	      intern(MESH, 0xAAAAAAAALU);
	      intern(MESH, 0x55555555LU);
	    }
	    

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
	    if(!pickcompile) /* AIS: We never need syslib when compiling
				for PIC, because it's preoptimized. */
	    {
	      for (tp = tuples; tp->type; tp++)
	      {
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
	    }
	    if ( needsyslib ) { /* AIS: modified to use findandfreopen */
	      if (Base == 2)    /* see code for opening the skeleton */
		(void) sprintf(buf2, "%s.i", SYSLIB);
	      else
		(void) sprintf(buf2, "%s.%di", SYSLIB, Base);
	      if (findandfreopen(buf2, libdir, "r", argv[0], stdin) == 0)
		lose(E127, 1, (char*) NULL);
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
	    {
		if (tp->type == GETS || tp->type == RESIZE
		    || tp->type == WRITE_IN || tp->type == READ_OUT
		    || tp->type == FROM || tp->type == MANYFROM
		    || tp->type == FORGET || tp->type == RESUME
		    || tp->type == COMPUCOME)
		  typecast(tp->type == MANYFROM ? tp->u.node->lval : tp->u.node);
		if (tp->type == WRITE_IN) coopt = 0; /* AIS: may as well do
							this here */
	    }

	    codecheck();	/* check for compile-time errors */
	    /* AIS: And importantly, sort out line number references */
	    
	    /* perform optimizations */
	    if (dooptimize)
		for (tp = tuples; tp->type; tp++)
		{
		  optuple = tp;
		  if (tp->type == GETS || tp->type == RESIZE
		      || tp->type == FORGET || tp->type == RESUME
		      || tp->type == FROM || tp->type == COMPUCOME)
		    optimize(tp->u.node);
		  if (tp->type == MANYFROM) optimize(tp->u.node->lval);
		} /* AIS: Added FROM and MANYFROM support. */

	    /* AIS: perform flow optimizations */
	    if (flowoptimize) optimizef();

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
	    if(outtostdout) ofp=stdout; /* AIS */
	    else if((ofp = debfopen(buf, "w")) == (FILE *)NULL)
		lose(E888, 1, (char *)NULL);
	    
	    fseek(ifp,0L,0);	/* rewind skeleton file */

	    /* AIS: Before changing argv[0], locate coopt.sh. */
	    cooptsh = findandtestopen("coopt.sh", bindir, "rb", argv[0]);
	    
	    /* AIS: Remove the filename from argv[0], leaving only a directory.
	       If this would leave it blank, change argv[0] to '.'.
	       This is so gcc can find the includes/libraries the same way that
	       findandfreopen does. */
	    if(strchr(argv[0],'/')) *(strrchr(argv[0],'/')) = 0;
	    else strcpy(argv[0],".");
	    
	    (void) sprintf(buf2,
			   "%s %s%s-I%s -I%s -I%s/../include -L%s -L%s -L%s/../lib -O%c -o %s"
#ifdef __DJGPP__
			   ".exe -lick%s%s","",
#else
			   " -lick%s%s",compiler,
#endif
			   buf, yukdebug||yukprofile?" -lyuk ":" ",
			   includedir, argv[0], argv[0], libdir, argv[0], argv[0],
			   coopt?'3':'2', /* AIS: If coopting, optimize as much as possible */
			   argv[optind], multithread?"mt":"", cdebug?" -g":"");
	    /* AIS: Possibly link in the debugger yuk and/or libickmt.a here. */
	    /* AIS: Added -g support. */
	    /* AIS: Added argv[0] to the -I, -L settings. */
	    
	    while ((c = myfgetc(ifp)) != EOF)
		if (c != '$')
		    (void) fputc(c, ofp);
	        else switch(myfgetc(ifp))
		{
		case 'A':	/* source name stem */
		    (void) fputs(argv[optind], ofp);
		    break;

		case 'B':	/* # of statements */
		    (void) fprintf(ofp, "%d", lineno);
		    break;

		case 'C':	/* initial abstentions */
		  /* AIS: Modified to check for coopt, pickcompile */
		    maxabstain = 0;
		    for (tp = tuples; tp->type; tp++)
			if (((tp->exechance <= 0 || tp->exechance >= 101)
			     && tp - tuples + 1 > maxabstain)
			    || coopt || pickcompile)
			    maxabstain = tp - tuples + 1;
		    if (maxabstain)
		    {
			if(!pickcompile) (void) fprintf(ofp, " = {");
			for (tp = tuples; tp < tuples + maxabstain; tp++)
			{
			    if(tp->exechance != 100 && tp->exechance != -100)
			    { /* AIS: The double-oh-seven operator prevents
				 coopt working. However, syslib contains a
				 double-oh-seven. feh.c has checked that that
				 isn't referenced; if it isn't, we can allow
				 one double-oh-seven if syslib was
				 automagically inclulded. */
			      if(needsyslib) needsyslib = 0; else coopt = 0;
			    }
			    if(!pickcompile)
			    {
			      if (tp->exechance > 0)
			      {
				(void) fprintf(ofp, "0, ");
				tp->initabstain=0; /* AIS: -f might not be
						      given, so we can't rely
						      on dekludge.c doing
						      this */
			      }
			      else {
				(void) fprintf(ofp, "1, ");
				tp->exechance = -tp->exechance;
				tp->initabstain=1; /* AIS: As above */
				/* AIS: If the line was abstained, we need to
				   swap ONCEs and AGAINs on it round, to suit
				   the code degenerator. */
				if(tp->onceagainflag == onceagain_ONCE)
				  tp->onceagainflag = onceagain_AGAIN;
				else if(tp->onceagainflag == onceagain_AGAIN)
				  tp->onceagainflag = onceagain_ONCE;
			      }
			      if(tp->exechance >= 101)
			      {
				/* AIS: This line has a MAYBE */
				tp->maybe = 1;
				tp->exechance /= 100;
			      }
			      else tp->maybe = 0;
			    }
			    else /* AIS: hardcoded abstain bits for PICs */
			    {
			      if(!tp->abstainable) continue;
			      if(tp->exechance > 0)
				(void) fprintf(ofp, "ICK_INT1 ICKABSTAINED(%d)=0;\n",tp-tuples);
			      else
				(void) fprintf(ofp, "ICK_INT1 ICKABSTAINED(%d)=1;\n",tp-tuples);				
			    }
			}
			if(!pickcompile) (void) fprintf(ofp, "}");
		    }
		    break;

		case 'D':	/* linetypes array for abstention handling */
		    maxabstain = 0;
		    for (tp = tuples; tp->type; tp++)
			if (tp->type == ENABLE || tp->type == DISABLE || tp->type == MANYFROM)
			    maxabstain++;
		    if (maxabstain || /* AIS */ gerucomesused)
		    {
		      /* AIS: Changed to use enablersm1 */
		      /*(void) fprintf(ofp, "#define UNKNOWN\t\t0\n");*/
			i = 0;
			for (;i < (int)(sizeof(enablersm1)/sizeof(char *));i++)
			    (void) fprintf(ofp,
					   "#define %s\t%d\n",
					   enablersm1[i], i);

			(void) fprintf(ofp, "int linetype[] = {\n");
			for (tp = tuples; tp->type; tp++)
			  if (tp->type >= GETS && tp->type <= FROM) /* AIS: FROM added */
				(void) fprintf(ofp,
					       "    %s,\n",
					       enablers[tp->type - GETS]);
			    else
				(void) fprintf(ofp, "    UNKNOWN,\n");
			(void) fprintf(ofp, "};\n");
		    }
		    break;

		case 'E':	/* extern to intern map */
		  if(!pickcompile)
		  {
		    (void) fprintf(ofp,"int Base = %d;\n",Base);
		    (void) fprintf(ofp,"int Small_digits = %d;\n",
				   Small_digits);
		    (void) fprintf(ofp,"int Large_digits = %d;\n",
				   Large_digits);
		    (void) fprintf(ofp,"unsigned int Max_small = 0x%x;\n",
				   Max_small);
		    (void) fprintf(ofp,"unsigned int Max_large = 0x%x;\n",
				   Max_large);
		    if (yukprofile || yukdebug || multithread)
		    { /* AIS: yuk.c, multithreading require all these to exist */
		      if(!nonespots) nonespots = 1;
		      if(!ntwospots) ntwospots = 1;
		      if(!ntails) ntails = 1;
		      if(!nhybrids) nhybrids = 1;
		    }
		    else if(opoverused)
		    {
		      /* AIS: The operand-overloading code requires onespot and
			 twospot variables to exist. */
		      if(!nonespots) nonespots = 1;
		      if(!ntwospots) ntwospots = 1;
		    }
		    /* AIS:I de-staticed all these so they could be accessed by
		       yuk and cesspool, and added all the mentions of yuk and
		       multithread. Then I changed it so the variables would be
		       allocated dynamically, to speed up multithreading. (It's
		       an O(1) change to the speed of ordinary programs, so I
		       thought I could get away with it. The order is wrt the
		       number of lines in the program. The change is O(n) wrt
		       the number of variables, but again I hope that doesn't
		       matter, and I won't get the entire INTERCAL community
		       angry with me for daring to implement an extension that
		       slows down existing programs.) */
		    if (variableconstants) /* AIS */
		    {
		      int temp=0;
		      (void) fprintf(ofp, "type32 meshes[%d] = {",nmeshes);
		      while(temp<nmeshes)
		      {
			(void) fprintf(ofp, "%luLU, ", varextern(temp,MESH));
			temp++;
		      }
		      (void) fprintf(ofp, "};\n");
		    }
		    
		    if (nonespots)
		    {
			(void) fprintf(ofp,
				       "type16* onespots;\n");
			(void) fprintf(ofp,
				       "bool* oneforget;\n");
			if(yukprofile || yukdebug)
			{
			  (void) fprintf(ofp,
					 "type16 oneold[%d];\n",
					 nonespots);
			  (void) fprintf(ofp,
					 "signed char onewatch[%d];\n",
					 nonespots);
			}
			if(multithread)
			{
			  (void) fprintf(ofp,
					 "int onespotcount = %d;\n",
					 nonespots);
			}
			if(opoverused) /* AIS */
			{
			  int temp=nonespots;
			  (void) fprintf(ofp,
					 "overop oo_onespots[%d];\n",
					 nonespots);
			  while(temp--)
			    (void) fprintf(ofp,
"type32 og1spot%d(type32 t)\n{\n  (void)t;\n  return onespots[%d];\n}\n"
"void os1spot%d(type32 val, void(*f)())\n{\n  (void)f;\n  assign((void*)"
"(onespots+%d), ONESPOT, oneforget[%d], val);\n}\n",temp,temp,temp,temp,temp);
			}
		    }
		    if (ntwospots)
		    {
			(void) fprintf(ofp,
				       "type32* twospots;\n");
			(void) fprintf(ofp,
				       "bool* twoforget;\n");
			if(yukprofile || yukdebug)
			{
			  (void) fprintf(ofp,
					 "type32 twoold[%d];\n",
					 ntwospots);
			  (void) fprintf(ofp,
					 "signed char twowatch[%d];\n",
					 ntwospots);
			}
			if(multithread)
			{
			  (void) fprintf(ofp,
					 "int twospotcount = %d;\n",
					 ntwospots);
			}
			if(opoverused) /* AIS */
			{
			  int temp=ntwospots;
			  (void) fprintf(ofp,
					 "overop oo_twospots[%d];\n",
					 ntwospots);
			  while(temp--)
			    (void) fprintf(ofp,
"type32 og2spot%d(type32 t)\n{\n  (void)t;\n  return twospots[%d];\n}\n"
"void os2spot%d(type32 val, void(*f)())\n{\n  (void)f;\n  assign((void*)"
"(twospots+%d), TWOSPOT, twoforget[%d], val);\n}\n",temp,temp,temp,temp,temp);
			}
		    }
		    if (ntails)
		    {
			(void) fprintf(ofp,
				       "array* tails;\n");
			(void) fprintf(ofp,
				       "bool* tailforget;\n");
			if(multithread)
			{
			  (void) fprintf(ofp,
					 "int tailcount = %d;\n",
					 ntails);
			}
		    }
		    if (nhybrids)
		    {
			(void) fprintf(ofp,
				       "array* hybrids;\n");
			(void) fprintf(ofp,
				       "bool* hyforget;\n");
			if(multithread)
			{
			  (void) fprintf(ofp,
					 "int hybridcount = %d;\n",
					 nhybrids);
			}
		    }
		    if (yydebug || compile_only)
			for (op = oblist; op < obdex; op++)
			  if(op->type!=MESH) /* AIS: Added this check */
			    (void) fprintf(ofp, " /* %s %d -> %d */\n",
					   nameof(op->type, vartypes),
					   op->extindex,
					   op->intindex);
		    if (yukdebug || yukprofile)
		    { /* AIS: drop intern to extern map into the program */
		      (void) fprintf(ofp, "\nyukvar yukvars[]={\n");
		      for (op = oblist; op < obdex; op++)
			if(op->type!=MESH) /* AIS: Added this check */
			  (void) fprintf(ofp,"    {%s,%d,%d},\n",
					 nameof(op->type, vartypes),
					 op->extindex,
					 op->intindex);
		      (void) fprintf(ofp,"    {YUKEND,0,0}};\n");
		    }
		  }
		  else
		  {
		    /* Compiling for PIC */
		    /* Arrays not supported on PICs */
		    if(ntails || nhybrids)
		      lose(E256, yylineno, (char*) NULL);
		    /* and neither are variable constants */
		    if(variableconstants)
		      lose(E256, yylineno, (char*) NULL);
 		    for (op = oblist; op < obdex; op++)
 		    {
 		      (void) fprintf(ofp, " /* %s %d -> %d */\n",
 				     nameof(op->type, vartypes),
 				     op->extindex,
 				     op->intindex);
 		      (void) fprintf(ofp, "#define %s%d %s[%d]\n",
 				     nameof(op->type, vartypes),
 				     op->extindex,
 				     nameof(op->type, varstores),
 				     op->intindex);
 		      if(op->ignorable)
 			(void) fprintf(ofp, "ICK_INT1 ignore%s%d = 0;\n",
 				       nameof(op->type, varstores),
  				       op->intindex);
  		    }
		    (void) fprintf(ofp, "#include \"pick1.h\"\n");
		    if(nonespots)
		    {
		      (void) fprintf(ofp,
				     "ICK_INT16 onespots[%d];\n"
				     "ICK_INT16 onespotsstash[%d];\n",
				     nonespots,
				     nonespots);
		      if(opoverused) /* AIS */
		      {
			int temp=nonespots;
			(void) fprintf(ofp,
				       "overop oo_onespots[%d];\n",
				       nonespots);
			while(temp--)
			  (void) fprintf(ofp,
"type32 og1spot%d(type32 t)\n{\n  (void)t;\n  return onespots[%d];\n}\n"
"void os1spot%d(type32 val,void(*f)())\n{\n  (void)f;\n  if(!ignoreonespots%d)"
" onespots[%d]=val;\n}\n",temp,temp,temp,temp,temp);
		      }
		    }
		    if(ntwospots)
		    {
		      (void) fprintf(ofp,
				     "ICK_INT32 twospots[%d];\n"
				     "ICK_INT32 twospotsstash[%d];\n",
				     ntwospots,
				     ntwospots);
		      if(opoverused) /* AIS */
		      {
			int temp=ntwospots;
			(void) fprintf(ofp,
				       "overop oo_twospots[%d];\n",
				       ntwospots);
			while(temp--)
			  (void) fprintf(ofp,
"type32 og2spot%d(type32 t)\n{\n  (void)t;\n  return twospots[%d];\n}\n"
"void os2spot%d(type32 val,void(*f)())\n{\n  (void)f;\n  if(!ignoretwospots%d)"
" twospots[%d]=val;\n}\n",temp,temp,temp,temp,temp);
		      }		      
		    }
		    (void) fprintf(ofp, "#include \"pick2.h\"\n");
		  }
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

		case 'H':	/* COMPUCOME, and dispatching for resumes */
                 /* AIS: Added COMPUCOME here. This line must be fully guarded
		    to prevent a longjmp to an uninitialised buffer (it's
		    guarded by a lose() in ick-wrap.c.) Also checks for
		    multithread; programs that mix normal and computed COME
		    FROM need to use the same conventions for both, so even
		    if no computed COME FROMs are used, the normal ones need
		    this line so that COME FROMs can be handled consistently.*/
		    if(compucomesused || multithread)
		    {
		      (void) fprintf(ofp, "CCF%d: longjmp(cjb,1);\n",
				     compucomecount);
		    }
#if 0
		    /* AIS: This section is no longer used, because a switch
		       is used inline instead of using gotos into the code. */
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
#endif
		    break;

		case 'J':	/* # of source file lines */
		    (void) fprintf(ofp, "%d", yylineno);
		    break;

		case 'K':       /* AIS: yuk information (or not) */
		  if(yukdebug||yukprofile)
		  {
		    (void) fprintf(ofp, "#include \"yuk.h\"\n\n");
		    (void) fprintf(ofp, "char* textlines[] = {\n");
		    emittextlines(ofp); /* from feh.c */
		    (void) fprintf(ofp, "\"\"};\n\n");
		    (void) fprintf(ofp, "char* yukexplain[] = {\n");
		    for (tp = tuples; tp->type; tp++)
		    {
		      if (tp->type == GETS || tp->type == FORGET || tp->type == RESUME
			  || tp->type == FROM || tp->type == COMPUCOME
			  || tp->type == MANYFROM)
		      {
			(void) fprintf(ofp, "\"");
			explexpr(tp->type == MANYFROM ? tp->u.node->lval :
				 tp->type == GETS ? tp->u.node->rval : tp->u.node, ofp);
			(void) fprintf(ofp, "\",\n");
		      }
		      else (void) fprintf(ofp, "0,");
		    }
		    (void) fprintf(ofp, "0};\n\n");
		    (void) fprintf(ofp, "int lineofaboff[] = {\n");
		    for (tp = tuples; tp->type; tp++)
		    {
		      fprintf(ofp,"%d,",tp->lineno);
		    }
		    (void) fprintf(ofp, "-1};\n\n");
		    (void) fprintf(ofp, "int yukopts = %d;\n", yukprofile+yukdebug*2);
		    (void) fprintf(ofp, "yptimer ypexectime[%d];\n", lineno);
		    (void) fprintf(ofp, "ypcounter ypexecount[%d];\n",lineno);
		    (void) fprintf(ofp, "ypcounter ypabscount[%d];\n",lineno);
		  }
		  break;

		case 'L': /* AIS: increase Emacs compatibility */
		  (void) fprintf(ofp,
				 "/* -*- mode:c; compile-command:\"%s%s%s\" -*- */",
#ifdef __DJGPP__
				 compiler," ",
#else
				 "","",
#endif
				 buf2);
		  break;

		case 'M': /* AIS: place new features defines in program */
		  /* This is needed even in a non-multithread program, to let
		     the header files know it's non-multithread */
		  (void) fprintf(ofp, "#define MULTITHREAD %d\n", multithread);
		  /* Likewise, to let the header files know whether it
		     overloads operands (I don't think this is used at
		     the moment, though) */
		  (void) fprintf(ofp, "#define OPOVERUSED %d\n",opoverused);
		  break;

		case 'N':	/* allocate variables */
		    /* AIS:I de-staticed all these so they could be accessed by
		       yuk and cesspool, and added all the mentions of yuk and
		       multithread. Then I changed it so the variables would be
		       allocated dynamically, to speed up multithreading (it's
		       an O(1) change to the speed of ordinary programs, so I
		       thought I could get away with it). At this point, the
		       'E' case must already have been done. calloc sets all
		       the integer values to 0, as before. In the case of
		       arrays, it will not zero pointers, but the number-of-
		       dimensions value will become 0, which can serve as a
		       'deallocated' flag. */
		    if (nonespots)
		    {
		      if(!pickcompile) /* AIS */
		      {
			(void) fprintf(ofp,
				       "    onespots = calloc("
				       "%d, sizeof *onespots);\n",
				       nonespots);
			(void) fprintf(ofp,
				       "    oneforget = calloc("
				       "%d, sizeof *oneforget);\n",
				       nonespots);
		      }
		      if(opoverused)
		      {
			int temp=nonespots;
			while(temp--)
			  (void) fprintf(ofp,
"    oo_onespots[%d].get=og1spot%d;\n    oo_onespots[%d].set=os1spot%d;\n",
					 temp,temp,temp,temp);
		      }
		    }
		    if (ntwospots)
		    {
		      if(!pickcompile)
		      {
			(void) fprintf(ofp,
				       "    twospots = calloc("
				       "%d, sizeof *twospots);\n",
				       ntwospots);
			(void) fprintf(ofp,
				       "    twoforget = calloc("
				       "%d, sizeof *twoforget);\n",
				       ntwospots);
		      }
		      if(opoverused)
		      {
			int temp=ntwospots;
			while(temp--)
			  (void) fprintf(ofp,
"    oo_twospots[%d].get=og2spot%d;\n    oo_twospots[%d].set=os2spot%d;\n",
					 temp,temp,temp,temp);
		      }
		    }
		    if (ntails&&!pickcompile)
		    {
			(void) fprintf(ofp,
				       "    tails = calloc("
				       "%d, sizeof *tails);\n",
				       ntails);
			(void) fprintf(ofp,
				       "    tailforget = calloc("
				       "%d, sizeof *tailforget);\n",
				       ntails);
		    }
		    if (nhybrids&&!pickcompile)
		    {
			(void) fprintf(ofp,
				       "    hybrids = calloc("
				       "%d, sizeof *hybrids);\n",
				       nhybrids);
			(void) fprintf(ofp,
				       "    hyforget = calloc("
				       "%d, sizeof *hyforget);\n",
				       nhybrids);
		    }		    
		    break;
		case 'O': /* AIS; for GERUCOME and operand overloading */
		  if(gerucomesused || nextfromsused)
		    fprintf(ofp,"unsigned truelineno = 0;\n");
		  if(opoverused)
		    fprintf(ofp,"%s trueval;\n",
			    pickcompile?"ICK_INT32":"type32");
		  break;
		case 'P': /* AIS: for operand overloading */
		  if(opoverused)
		    emitslatproto(ofp);
		  break;
		case 'Q': /* AIS: for operand overloading */
		  if(opoverused)
		    emitslat(ofp);
		  break;
		}

	    if(!outtostdout) (void) fclose(ofp);

#ifndef __DJGPP__	    
	    /* OK, now sic the C compiler on the results */
	    if (!compile_only&&!yukdebug&&!yukprofile)
	    {
	      /* AIS: buf2 now assigned elsewhere so $L works */
	      (void) system(buf2);
	      /* AIS: no unlink if cdebug */ if(!cdebug) (void) unlink(buf);
	    }
	    else if(!compile_only)
	    { /* AIS: run, then delete all output but yuk.out */
	      /* Note that the output must be deleted for copyright
		 reasons (so as not to GPL a non-GPL file automatically) */
	      (void) system(buf2);
	      dup2(oldstdin,0); /* restore stdin */
	      (void) system(argv[optind]);
	      (void) unlink(buf);
	      (void) unlink(argv[optind]);
	    }
	    if(coopt) /* AIS */
	    {
	      /* The constant-output optimizer is a form of post-processor.
		 IMPORTANT NOTE: This MUST NOT be run if the input program
		 takes any input or is affected in any way by the state of
		 the system, as the degenerated program may be wrong. At the
		 moment, the only INTERCAL command that takes input is
		 WRITE IN. Double-oh-sevens screw this up, too. */
	      if(cooptsh)
	      {
		sprintf(buf2,"sh %s %s", cooptsh, argv[optind]);
		(void) system(buf2); /* replaces the output executable if
					neccesary */
	      }
	    }
#else /* we are using DJGPP */
	    /* OK, now sic the C compiler on the results */
	    if (!compile_only)
	    {
	      /* AIS: buf2 now assigned elsewhere so $L works */
	      /* AIS: This changes somewhat for DJGPP, due to the
		 command-line cap. It creates a temporary file
		 with the arguments needed to give gcc. */
	      FILE* rsp;
	      char* tempfn="ickgcc.rsp"; /*use current dir as temp if needed*/
	      /* Three tries are used to find a temp directory.
		 ICKTEMP is the preferred environment variable to check;
	         if, as expected, this isn't set, try TMPDIR (which DJGPP
	         sets to its own temp directory, at least when running under
                 bash), TEMP and TMP (in that order). DJGPP offers /dev/env
	         as a method of accessing environment variables in filenames.*/
	      if(isenv("TMP")) tempfn="/dev/env/TMP/ickgcc.rsp";
	      if(isenv("TEMP")) tempfn="/dev/env/TEMP/ickgcc.rsp";
	      if(isenv("TMPDIR")) tempfn="/dev/env/TMPDIR/ickgcc.rsp";
	      if(isenv("ICKTEMP")) tempfn="/dev/env/ICKTEMP/ickgcc.rsp";
	      rsp=debfopen("ickgcc.rsp","w");
	      fprintf(rsp,"%s\n",buf2);
	      fclose(rsp);
	      system("gcc @ickgcc.rsp");
	      remove("ickgcc.rsp");
	      if(yukdebug || yukprofile)
	      {
		dup2(oldstdin,0); /* restore stdin */
		sprintf(buf2,"%s.exe",argv[optind]);
		system(buf2);
		remove(buf);
		remove(buf2);
	      }
	      else if(!cdebug)
	      {
		remove(buf);
	      }
	    }
#endif	    
	}
    }
    (void) fclose(ifp);
    (void) close(oldstdin); /* AIS */
    return 0;
}

/* perpet.c ends here */

