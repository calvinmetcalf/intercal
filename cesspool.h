/* cesspool.h -- interface to INTERCAL storage manager and runtime support */

typedef int bool;
#define TRUE	1
#define FALSE	0

extern unsigned int aget(), pin(), roll(), popnext(), resume();
extern void pushnext(), resize(), stashinit(), stash(), retrieve(), aput();
extern void clockface(), pout();

extern int lineno;

#define ONESPOT	0
#define TWOSPOT	1
#define TAIL	2
#define HYBRID	3

/* cesspool.h ends here */
