/* cesspool.h -- interface to INTERCAL storage manager and runtime support */

#include <malloc.h>

typedef int bool;
#define TRUE	1
#define FALSE	0

extern char *aref();
extern unsigned int aget(), pin(), roll(), popnext(), resume(), assign();
extern void pushnext(), resize(), stashinit(), stash(), retrieve(), aput();
extern void clockface(), pout();

extern int lineno;

#define ONESPOT	0
#define TWOSPOT	1
#define TAIL	2
#define HYBRID	3

/* the following two types must both promote to unsigned int in expressions */

typedef unsigned short type16;
typedef unsigned int   type32;

typedef struct array_t
{
  int rank;
  unsigned int *dims;
  union
  {
    type16 *tail;
    type32 *hybrid;
  } data;
} array;

typedef struct
{
    char *name;
    int	value;
}
numeral;

extern numeral numerals[];

/* cesspool.h ends here */
