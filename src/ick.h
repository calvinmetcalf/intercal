/* ick.h -- compilation types for intercal parser */

/* AIS: Changes for DJGPP */
#ifdef __DJGPP__
#define _POSIX_SOURCE
#endif

#ifdef ICKNOSEARCH
#define ICKINCLUDEDIR "."
#define ICKLIBDIR "."
#define ICKBINDIR "."
#define YYDEBUG 1
#endif

/* AIS: This is now detected by autoconf and doesn't need to be set by the
   user. */
#ifdef NEED_YYRESTART
# define USE_YYRESTART
#endif

#define YY_NO_UNPUT

#ifndef ICKBOOL_DEFINED
/*@-redef@*/
typedef int ick_bool;
/*@=redef@*/
#define ICKBOOL_DEFINED
#endif

#define ick_TRUE	1
#define ick_FALSE	0

#define ALLOC_CHUNK	256

/*
 * We choose this value for maximum number of possible variables on
 * the theory that no human mind could grok a more complex INTERCAL
 * program than this and still retain any vestige of sanity.
#define MAXVARS		100
 */

/*
 * Maximum supported statement types; should be equal to (FROM - GETS + 1)
 * AIS: Changed this when I added new statements.
 */
#define MAXTYPES	31

/* AIS: Maximum supported spark/ears nesting, divided by 32. The value given
   allows for 256 nested spark/ears groupings, which should be enough. */
#define SENESTMAX       8

enum onceagain {onceagain_NORMAL, onceagain_ONCE, onceagain_AGAIN}; /* AIS */

typedef struct node_t
{
    int			opcode;		/* operator or type code */
    unsigned long	constant;	/* constant data attached */
    unsigned long       optdata;        /* AIS: Temp used by the optimizer */
    int			width;		/* is this 32-bit data? */
    struct node_t	*lval, *rval;	/* attached expression nodes */
    struct node_t       *nextslat;      /* AIS: The next node with a slat */
} node;

typedef struct tuple_t
{
    unsigned int	label;		/* label # of this statement */
    unsigned int	ncomefrom;	/* AIS: How many noncomputed COME FROMS
					   have this line as a suck-point */
    int         	exechance;	/* chance of execution, initial abstain,
					   (AIS) MAYBE details */
    ick_bool                maybe;          /* AIS: Where MAYBE details go when
					   exechance has been parsed */
    ick_bool                abstainable;    /* AIS: Is it possible for this line to
					   be abstained from? */
    ick_bool                initabstain;    /* AIS: Is this line initially
					   abstained from? */
    ick_bool                nextable;       /* AIS: Can this line be a NEXT
					   target? */
    ick_bool                optversion;     /* AIS: Use an optimized version? (Only
					   set if the optimizer thinks that
					   it's safe.) */
    ick_bool                preproc;        /* AIS: Is this line a nonexistent one
					   that was added to implement a
					   command in the parser? */
    ick_bool                warn112:1;      /* AIS: Should this line produce warning
					   112 during degeneration? */
    ick_bool warn128:1, warn534:1, warn018:1, warn016:1, warn276:1, warn239:1,
      warn622:1; /* AIS: As warn112. The warnings are a bitfield to save space. */
    unsigned int	type;		/* statement type */
    struct
    { /* AIS: Struct, not union needed because ABSTAIN expr FROM (line) has both */
	unsigned int	target;		/* for NEXT statements */
	node		*node;		/* attached expression node(s) */
    } u;
    unsigned int        nexttarget;     /* AIS: The target tuple of a NEXT must
					   also be recorded for optimizef */
    int ick_lineno; 			/* source line for error messages */
    ick_bool sharedline;			/* if NZ, two statements on a line */
    enum onceagain onceagainflag;       /* AIS: ONCE / AGAIN */
    int ppnewtype;                      /* AIS: 'real' type of this line when
					   the preprocessor is used; 0 on all
					   statements but the 'real'
					   statement */
    signed setweave;                    /* AIS: +1 to turn weaving on, -1 to
					   turn it off, before this command */
} tuple;

/* this maps the `external' name of a variable to an internal ick_array index */
typedef struct
{
  int type;
  unsigned long extindex;
  unsigned long intindex;
  int ignorable; /* AIS: Can this variable be IGNOREd? */
  int memloc; /* AIS: Where does a PIC store this in memory? */
}
atom;

typedef struct
{
    int	value;
    char *name;
}
assoc;

/*@null@*/ /*@owned@*/ /*@partial@*/ extern atom *oblist;
/*@null@*/ /*@dependent@*/ /*@partial@*/ extern atom *obdex;
extern int obcount, nonespots, ntwospots, ntails, nhybrids;
extern int nmeshes; /* AIS */

/*@only@*/ extern tuple *tuples;
extern int tuplecount;

/*@dependent@*/ extern tuple *optuple; /* AIS: The tuple currently being optimized */

extern char **enablers;
extern char *enablersm1[MAXTYPES+1];
extern assoc vartypes[];

/* the lexical analyzer keeps copies of the source logical lines */
/*@only@*/ extern char **textlines;
extern int textlinecount;
extern int iyylineno;

/* AIS: These are needed to sort out a grammar near-ambiguity */
extern unsigned long sparkearsstack[SENESTMAX];
extern int sparkearslev;

/* compilation options */
extern ick_bool compile_only;  /* just compile into C, don't run the linker */
extern ick_bool nocompilerbug; /* set possibility of IE774 to zero */
extern int ick_traditional;    /* compile as INTERCAL-72 */
extern int yydebug;        /* print debugging information while parsing */

extern int politesse;


/* AIS: I added these */
extern int yukdebug;       /* debug the code with yuk */
extern int yukprofile;     /* profile the code with yuk */
extern int compucomecount; /* number of computed COME FROMs */
extern int compucomesused; /* are computed COME FROMs used? */
extern int nextfromsused;  /* is NEXT FROM used? */
extern int gerucomesused;  /* is COME FROM gerund used? */
extern int opoverused;     /* is operand overloading used? */
extern int useickec;       /* are external calls used? */
extern int createsused;    /* are CREATEs used? */
/*@null@*/ extern node* firstslat;    /* the ick_first node with a slat */
/*@null@*/ extern node* prevslat;     /* the last node so far with a slat */
extern int multithread;    /* is the program multithreaded? */
extern int variableconstants; /* is any assignment allowed? */
extern int ick_coreonerr;      /* dump core on IE778? */
extern int optdebug;       /* debug the optimizer */
extern int flowoptimize;   /* optimize program flow */
extern int ick_checkforbugs;   /* check for bugs */
extern int coopt;          /* constant-output optimizer */

/* ick.h ends here */
