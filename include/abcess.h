/* abcess.h -- functions used by compiled INTERCAL programs */

/* AIS note: This header file now serves two purposes. For non-multithread
   programs, it serves as headers to the degenerated code and many support
   functions. For multithread programs, this header file adds extra functions
   only when compiling the degenerated code and unravel.c, and holds its
   previous contents during other files. */

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#ifndef ICKBOOL_DEFINED
/*@-redef@*/
typedef int ick_bool;
/*@=redef@*/
#define ICKBOOL_DEFINED
#endif

#define ick_TRUE	1
#define ick_FALSE	0

#define ick_ONESPOT	0
#define ick_TWOSPOT	1
#define ick_TAIL	2
#define ick_HYBRID	3

#define ick_MAXNEXT 80 /* AIS: Moved from cesspool.c */

/* the following two types must both promote to unsigned int in expressions */

typedef unsigned short ick_type16;
typedef unsigned int   ick_type32;

typedef struct ick_array_t
{
  unsigned int rank;
  size_t *dims;
  union
  {
    ick_type16 *tail;
    ick_type32 *hybrid;
  } data;
} ick_array;


/* AIS: For operand overloading, a more complicated data type is needed. */
typedef struct ick_overop_t
{
  ick_type32 (*get)(ick_type32);
  void (*set)(ick_type32, void(*)());
} ick_overop;

/* AIS: Moved from cesspool.c */
typedef struct ick_stashbox_t     /* this is a save-stack element */
{
  unsigned int type;	      /* variable type */
  unsigned int index;       /* variable's index within the type */
  union		      /* the data itself */
  {
    ick_type16	onespot;
    ick_type32	twospot;
    ick_array	*a;
  } save;
  /*@null@*/ /*@dependent@*/ struct ick_stashbox_t *ick_next;  /* pointer to next-older ick_stashbox */
  ick_overop overloadinfo; /* AIS: overloading info is stashed too, in a
			    non-overloaded program (ignored otherwise) */
} ick_stashbox;

/* AIS: files to take input and output from */
/*@null@*/ extern FILE* ick_cesspoolin;
/*@null@*/ extern FILE* ick_cesspoolout;

/* defined in cesspool.c */
extern void ick_pushnext(unsigned n);
extern unsigned int ick_popnext(unsigned n);
extern unsigned int ick_resume(unsigned n);
extern unsigned int ick_pin(void);
extern void ick_clockface(ick_bool mode);
extern void ick_setclcsemantics(ick_bool mode); /* AIS */
extern void ick_pout(unsigned int val);
extern void ick_binin(unsigned int type, ick_array *a, ick_bool forget);
extern void ick_binout(unsigned int type, ick_array *a);
extern unsigned int ick_assign(char *dest, unsigned int type, ick_bool forget,
			   unsigned int value);

/* AIS: yuk, unravel and ick_ec need these */
extern unsigned* ick_next;
/*@null@*/ extern jmp_buf* ick_next_jmpbufs;
extern int ick_nextindex;
extern /*@null@*/ ick_stashbox *ick_first;

/* AIS: Implement the +ick_mystery command line option. */
extern unsigned long ick_mysteryc;
extern int ick_mystery;
#define ick_MYSTERYLINE if(ick_mystery && ick_mysteryc++ > 4000000000LU) exit(42);

/* AIS: More command-line options */
extern int ick_wimp_mode;
extern int ick_instapipe;

/* AIS: Handle multiple COME FROMs aiming at the same line */
extern int ick_multicome0(int errlineno, jmp_buf pc);

#ifdef _POSIX_SOURCE
/*@dependent@*/ extern void *ick_aref(unsigned int type, ...);
extern void ick_resize(unsigned int type, ...);
#else
/*@dependent@*/ extern void *ick_aref();
extern void ick_resize();
#endif

extern void ick_stashinit(void);
/* AIS: Added mentions of oo. This is set to 0 in a non-overloaded program. */
extern void ick_stash(unsigned int type, unsigned int index, void *from, ick_overop* oo);
extern void ick_retrieve(void *to, unsigned int type, unsigned int index,
			 ick_bool forget, ick_overop* oo);
extern unsigned int ick_roll(unsigned int n);

/* defined in arrgghh.c */
extern void ick_parseargs(int argc, char **argv);
extern int ick_printflow;

/* AIS: For the CREATE statement */
typedef struct ick_tag_createdata ick_createdata;
struct ick_tag_createdata
{
  int width;           /* 16 or 32 (maybe 0 will be allowed at some point) */
  int isarray;         /* this and the previous determine what vartype it is */
  unsigned short varnumber;
                       /* 0 if not a variable, the var's number if it is */
  ick_overop accessors;/* how to get and set this lvalue, or {0,0} */
  unsigned long value; /* current value of the var or expression */
};
extern void ick_registercreation(char*,unsigned long);
extern unsigned long ick_jicmatch(char*);

/* AIS: Multithreading types and defines */
#if MULTITHREAD != 0
typedef struct tag_ickthread ickthread;
/*@refcounted@*/ struct tag_ickthread
{
  void* varforget[10]; /* holds all four variable types, and forgetting data */
  unsigned* nextstack;
  int nextpointer;
  jmp_buf pc; /* program counter */
  ick_stashbox* sb; /* holds all stash data */
  /*@partial@*/ /*@dependent@*/ ickthread* ick_next;
  /*@null@*/ /*@partial@*/ /*@dependent@*/ ickthread* choicepoint;
  /* the top choicepoint available. Used as a next pointer in the choicepoint
     stack. */
  int stale; /* if this is a choicepoint, whether it's a stale choicepoint. */
  int refcount; /* when Threaded INTERCAL and Backtracking INTERCAL are
		   combined, reference-counting on choicepoints is needed
		   so that backtracking past multithreading is possible.
		   This also allows for garbage-collection of choicepoints.
		   (Luckily, choicepoints cannot refer to themselves, so this
		   mechanism works.) This only applies to ickthreads that are
		   acting as choicepoints, not those acting as threads. */
  int ick_ccfc;  /* number of comefroms currently active */
  long ick_skipto; /* compucome line number */
  jmp_buf ick_cjb; /* keeps track of compucomes */
  /*@partial@*/ /*@dependent@*/ ickthread* dsi; /* which thread's varforget and sb to use */
  /*@null@*/ /*@dependent@*/ ickthread* usesthis; /* for garbage collection purposes */
};
/*@partial@*/ /*@dependent@*/ extern ickthread* ickmt_cur; /* current thread */
/*@partial@*/ /*@dependent@*/ extern ickthread*ickmt_prev; /* previous thread: an optimisation to make
                                                              thread switching O(1), not O(n), with
                                                              respect to the number of threads */
extern int weaving; /* whether to weave newly created threads */
#define NEXTTHREAD if(ick_printflow) printf("[%d:%lx]",ick_lineno,(unsigned long)\
                                                          ickmt_cur); \
                                 if(setjmp(ick_cjb) == 0) \
                                   nextthread(ick_cjb, ick_lineno, 3);

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

extern int ick_oldabstain;
extern int gonebackto;
extern int ick_ccfc;
extern long ick_skipto;
extern jmp_buf btjb;
extern jmp_buf ick_cjb;

#define MULTICOME multicome1
#else
#define MULTICOME ick_multicome0
#endif /* MULTITHREAD */

/* AIS: Used by the debugger, multithread code, external calls */
#if (MULTITHREAD != 0) || (YUKDEBUG != 0) || defined(ICK_EC)
extern ick_type16* ick_onespots;
extern ick_bool* ick_oneforget;
extern ick_type32* ick_twospots;
extern ick_bool* ick_twoforget;
extern ick_array* ick_tails;
extern ick_bool* ick_tailforget;
extern ick_array* ick_hybrids;
extern ick_bool* ick_hyforget;
#if (MULTITHREAD != 0)
/*@null@*/ extern ick_overop* ick_oo_onespots;
/*@null@*/ extern ick_overop* ick_oo_twospots;
#endif
#endif

/* abcess.h ends here */
