/* abcess.h -- functions used by compiled INTERCAL programs */

typedef int bool;
#define TRUE	1
#define FALSE	0

#define ONESPOT	0
#define TWOSPOT	1
#define TAIL	2
#define HYBRID	3

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
extern char *aref();
extern void resize();
extern void stashinit(void);
extern void stash(unsigned int type, unsigned int index, void *from);
extern void retrieve(void *to, int type, unsigned int index, bool forget);
extern unsigned int roll(unsigned int n);

/* defined in arrgghh.c */
extern void parseargs(int argc, char **argv);

/* abcess.h ends here */
