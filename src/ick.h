/* ick.h -- compilation types for intercal parser */

/* Comment this out if your version of lex automatically supplies yylineno. */
#define YYLINENO_BY_HAND

/* Comment this out if your version of lex doesn't use yyrestart(). */
#define USE_YYRESTART

typedef int	bool;
#define TRUE	1
#define FALSE	0

/*
 * We choose this value for maximum number of possible source lines on
 * the theory that no human mind could grok more lines of INTERCAL than
 * this and still retain any vestige of sanity.
 */
#define MAXLINES	1000

/*
 * Similarly for maximum number of variables
 */
#define MAXVARS		100

/*
 * Maximum supported statement types; should be equal to (COME_FROM - GETS + 1)
 */
#define MAXTYPES	17

typedef struct node_t
{
    int			opcode;		/* operator or type code */
    unsigned long	constant;	/* constant data attached */
    int			width;		/* is this 32-bit data? */
    struct node_t	*lval, *rval;	/* attached expression nodes */
} node;

typedef struct tuple_t
{
    unsigned int	label;		/* label # of this statement */
    unsigned int	comefrom;	/* if NZ, a COME FROM touches this */
    int         	exechance;	/* chance of execution */
    unsigned int	type;		/* statement type */
    union
    {
	unsigned int	target;		/* for NEXT statements */
	node		*node;		/* attached expression node(s) */
    } u;
    int lineno; 			/* source line for error messages */
    bool sharedline;			/* if NZ, two statements on a line */
} tuple;

/* this maps the `external' name of a variable to an internal array index */
typedef struct
{
    int	type;
    int extindex;
    int intindex;
}
atom;

typedef struct
{
    int	value;
    char *name;
}
assoc;

extern atom oblist[MAXVARS], *obdex;
extern int nonespots, ntwospots, ntails, nhybrids;

extern tuple tuples[MAXLINES];

extern char *enablers[MAXTYPES];
extern assoc vartypes[];

/* the lexical analyzer keeps copies of the source logical lines */
extern char *textlines[MAXLINES];
extern int yylineno;

/* compilation options */
extern bool compile_only;  /* just compile into C, don't run the linker */
extern bool nocompilerbug; /* set possibility of E774 to zero */
extern int traditional;    /* compile as INTERCAL-72 */
extern int yydebug;        /* print debugging information while parsing */

extern int politesse;

/* ick.h ends here */
