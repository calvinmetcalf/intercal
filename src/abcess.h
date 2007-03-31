/* abcess.h -- functions used by compiled INTERCAL programs */

/* AIS note: This header file now serves two purposes. For non-multithread
   programs, it serves as headers to the degenerated code and many support
   functions. For multithread programs, this header file adds extra functions
   only when compiling the degenerated code and unravel.c, and holds its
   previous contents during other files. */

typedef int bool;
#define TRUE	1
#define FALSE	0

#define ONESPOT	0
#define TWOSPOT	1
#define TAIL	2
#define HYBRID	3

#define MAXNEXT 80 /* AIS: Moved from cesspool.c */

/* the following two types must both promote to unsigned int in expressions */

typedef unsigned short type16;
typedef unsigned int   type32;

typedef struct array_t
{
  unsigned int rank;
  unsigned int *dims;
  union
  {
    type16 *tail;
    type32 *hybrid;
  } data;
} array;


/* AIS: For operand overloading, a more complicated data type is needed. */
typedef struct overop_t
{
  type32 (*get)(type32);
  void (*set)(type32, void(*)());
} overop;

/* AIS: Moved from cesspool.c */
typedef struct stashbox_t     /* this is a save-stack element */
{
  unsigned int type;	      /* variable type */
  unsigned int index;       /* variable's index within the type */
  union		      /* the data itself */
  {
    type16	onespot;
    type32	twospot;
    array	*a;
  } save;
  struct stashbox_t *next;  /* pointer to next-older stashbox */
  overop overloadinfo; /* AIS: overloading info is stashed too, in a
			    non-overloaded program (ignored otherwise) */
} stashbox;

/* AIS: files to take input and output from */
extern FILE* cesspoolin;
extern FILE* cesspoolout;

/* defined in cesspool.c */
extern void pushnext(int n);
extern unsigned int popnext(int n);
extern unsigned int resume(unsigned int n);
extern unsigned int pin(void);
extern void clockface(bool mode);
extern void pout(unsigned int val);
extern void binin(unsigned int type, array *a, bool forget);
extern void binout(unsigned int type, array *a);
extern unsigned int assign(char *dest, unsigned int type, bool forget,
			   unsigned int value);

/* AIS: yuk and unravel need these */
extern int* next;
extern int nextindex;
extern stashbox* first;

/* AIS: Implement the +mystery command line option. */
extern unsigned long mysteryc;
extern int mystery;
#define MYSTERYLINE if(mystery && mysteryc++ > 4000000000LU) exit(42);

/* AIS: Handle multiple COME FROMs aiming at the same line */
extern int multicome0(int errlineno, jmp_buf pc);

#ifdef _POSIX_SOURCE
extern char *aref(unsigned int type, ...);
extern void resize(unsigned int type, ...);
#else
extern char *aref();
extern void resize();
#endif

extern void stashinit(void);
/* AIS: Added mentions of oo. This is set to 0 in a non-overloaded program. */
extern void stash(unsigned int type, unsigned int index, void *from, overop* oo);
extern void retrieve(void *to, int type, unsigned int index, bool forget,
		     overop* oo);
extern unsigned int roll(unsigned int n);

/* defined in arrgghh.c */
extern void parseargs(int argc, char **argv);

/* AIS: Multithreading types and defines */
#if MULTITHREAD != 0
typedef struct tag_ickthread ickthread;
struct tag_ickthread
{
  void* varforget[8]; /* holds all four variable types, and forgetting data */
  int* nextstack;
  int nextpointer;
  jmp_buf pc; /* program counter */
  stashbox* sb; /* holds all stash data */
  ickthread* next;
  ickthread* choicepoint; /* the top choicepoint available. Used as a next
			     pointer in the choicepoint stack. */
  int stale; /* if this is a choicepoint, whether it's a stale choicepoint. */
  int refcount; /* when Threaded INTERCAL and Backtracking INTERCAL are
		   combined, reference-counting on choicepoints is needed
		   so that backtracking past multithreading is possible.
		   This also allows for garbage-collection of choicepoints.
		   (Luckily, choicepoints cannot refer to themselves, so this
		   mechanism works.) This only applies to ickthreads that are
		   acting as choicepoints, not those acting as threads. */
  int ccfc;  /* number of comefroms currently active */
  int skipto; /* compucome line number */
  jmp_buf cjb; /* keeps track of compucomes */
};
extern ickthread* ickmt_cur; /* current thread */
extern ickthread* ickmt_prev; /* previous thread: an optimisation to make
			         thread switching O(1), not O(n), with
			         respect to the number of threads */
#define NEXTTHREAD if(printflow) printf("[%d]",lineno); \
                                 if(setjmp(cjb) == 0) \
                                   nextthread(cjb, lineno, 3);

extern void nextthread(jmp_buf pc, int errlineno, int flags);
extern void killthread(void);
extern void ickmtinit(void);
extern int multicome1(int errlineno, jmp_buf pc);
extern void choicepoint(void);
extern void choiceahead(void);
extern void choiceback(void);

/* from ick-wrap.c, declare as extern so they can be accessed by cesspool.c,
   unravel.c */
extern int onespotcount;
extern int twospotcount;
extern int tailcount;
extern int hybridcount;

extern int oldabstain;
extern int gonebackto;
extern int ccfc;
extern unsigned skipto;
extern jmp_buf btjb;
extern jmp_buf cjb;

#define MULTICOME multicome1
#else
#define MULTICOME multicome0
#endif /* MULTITHREAD */

/* AIS: Used by the debugger, multithread code */
#if (MULTITHREAD != 0) || (YUKDEBUG != 0)
extern type16* onespots;
extern bool* oneforget;
extern type32* twospots;
extern bool* twoforget;
extern array* tails;
extern bool* tailforget;
extern array* hybrids;
extern bool* hyforget;
#endif

/* abcess.h ends here */
