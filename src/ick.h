/* ick.h -- compilation types for intercal parser */

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
    int lineno; 			/* for splatters*/
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

extern unsigned int mingle(), iselect();
extern unsigned int and16(), or16(), xor16(), and32(), or32(), xor32();
extern unsigned int whirl16(), whirl32(), fin16(), fin32();

extern int yydebug;
extern int yylineno;
extern int politesse;

/* compilation options */
extern bool compile_only; /* just compile into C, don't run the linker */

extern int lineno;	/* current source line number; lose() uses it */

/* ick.h ends here */
