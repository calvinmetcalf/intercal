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
 * Similarly for maximum number of variable stashes
 */
#define MAXSTASHES		100

typedef struct node_t
{
    int			opcode;		/* operator or type code */
    unsigned int	constant;	/* constant data attached */
    int			width;		/* is this 32-bit data? */
    struct node_t	*lval, *rval;	/* attached expression nodes */
} node;

/* unary-op types -- values for token of type UNARY */
#define AND	0
#define OR	1
#define XOR	2

typedef struct tuple_t
{
    unsigned int	label;		/* label # of this statement */
    unsigned int	comefrom;	/* if NZ, a COME FROM touches this */
    unsigned int	exechance;	/* chance of execution */
    unsigned int	type;		/* statement type */
#define SPLATTERED	-1		/* it's undecodeable */
    union
    {
	unsigned int	target;		/* for NEXT statements */
	node		*node;		/* attached expression node(s) */
    } u;
} tuple;

/* the lexical analyzer keeps copies of the source logical lines */
extern char *textlines[MAXLINES];
extern int yylineno;

/* ick.h ends here */
