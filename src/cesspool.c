/* cesspool.c -- storage management and runtime support for INTERCAL */
/* LINTLIBRARY */
#include <stdio.h>
#include <stdlib.h>
#include <varargs.h>
#include "sizes.h"
#include "abcess.h"
#include "lose.h"

#include "numerals.c"

/**********************************************************************
 *
 * The following functions manipulate the nexting stack
 *
 *********************************************************************/

#define MAXNEXT	80

int next[MAXNEXT];
int nextindex = 0;

void pushnext(int n)
{
    if (nextindex < MAXNEXT)
	next[nextindex++] = n;
    else
	lose(E123, lineno, (char *)NULL);
}

unsigned int popnext(int n)
{
    nextindex -= n;
    if (nextindex < 0) {
	nextindex = 0;
	return (unsigned int)-1;
    }
    return(next[nextindex]);
}

unsigned int resume(unsigned int n)
{
    if (n == 0)
    {
	lose(E621, lineno, (char *)NULL);
	return 0;
    }
    else if ((n = popnext(n)) == (unsigned int)-1)
    {
	lose(E632, lineno, (char *)NULL);
	return 0;
    }
    return(n);
}

/**********************************************************************
 *
 * The following functions implement the INTERCAL I/O model
 *
 *********************************************************************/

unsigned int pin(void)
{
    char		buf[BUFSIZ], *cp, *strtok();
    unsigned int	result = 0;
    int n;
    extern int wimp_mode;

    if (fgets(buf, BUFSIZ, stdin) == (char *)NULL)
	lose(E562, lineno, (char *)NULL);
    buf[strlen(buf)-1] = '\0';

    if(wimp_mode) {
	result = atoi(buf);
	n = 1;
    }
    else
    {
	for(n=0,cp = strtok(buf, " ");cp;cp = strtok((char *)NULL, " "),n++)
	{
	    int	digit = -1;
	    numeral	*np;

	    for (np = numerals; np < numerals + sizeof(numerals)/sizeof(numeral); np++)
		if (strcmp(np->name, cp) == 0)
		{
		    digit = np->value;
		    break;
		}

	    if (digit == -1)
		lose(E579, lineno, cp);

	    if (result < 429496729 || (result == 429496729 && digit < 6))
		result = result * 10 + digit;
	    else
		lose(E533, lineno, (char *)NULL);
	}
    }
    if (!n)
	lose(E562, lineno, (char *)NULL);
    if (result > (unsigned int)Max_large)
	lose(E533, lineno, (char *)NULL);
    return(result);
}

/**********************************************************************
 *
 * Butchered Roman numerals implemented by
 * Michael Ernst, mernst@theory.lcs.mit.edu. May 7, 1990
 *
 * The INTERCAL manual hints that 3999 should translate to MMMIM
 * (compare MMMCMXCIX) without specifying what the translation is.
 * That may be a typo; in any case, this implementation isn't that
 * butchered.
 *
 *********************************************************************/

#define MAXDIGITS	10		/* max base 10 digits */
#define MAXROMANS	(MAXDIGITS*4+1)	/* max chars in translation */

/*
 * The first column tells how many of the succeeding columns are used.
 * The other columns refer to the columns of br_equiv and br_overbar.
 */
static int br_trans[10][5] =
{
    {0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {2, 0, 0, 0, 0},
    {3, 0, 0, 0, 0},
    {2, 1, 2, 0, 0},        /* or use {4, 0, 0, 0, 0} */
    {1, 2, 0, 0, 0},
    {2, 2, 1, 0, 0},
    {3, 2, 1, 1, 0},
    {4, 2, 1, 1, 1},
    {2, 1, 3, 0, 0}
};

/*
 * butcher places in the string result the "butchered" Roman numeral for val.
 * This string should be printed at the beginning of a line; it spans two
 * lines and already contains newlines.
 *
 * 11/24/91 LHH:  Removed unnecessary final newline.
 */

static void butcher(unsigned long val, char *result)
{
    int i, j;
    int digitsig, digitval;
    char res[MAXROMANS], ovb[MAXROMANS];

    /* We need FOUR columns because of the odd way that M and I interact. */
    static char br_equiv[MAXDIGITS][4] =
    {
	{'I', 'I', 'V', 'X'}, {'X', 'X', 'L', 'C'},
	{'C', 'C', 'D', 'M'}, {'M', 'I', 'V', 'X'},
	{'X', 'X', 'L', 'C'}, {'C', 'C', 'D', 'M'},
	{'M', 'i', 'v', 'x'}, {'x', 'x', 'l', 'c'},
	{'c', 'c', 'd', 'm'}, {'m', 'i', 'v', 'x'},
    };

    static char br_overbar[MAXDIGITS][4] =
    {
	{' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' '},
	{' ', '_', '_', '_'},
	{'_', '_', '_', '_'},
	{'_', '_', '_', '_'},
	{'_', ' ', ' ', ' '},
	{' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' '},
	{' ', '_', '_', '_'},
    };

    if (val == 0)
/* Final newline will be added by puts.
	(void) strcpy(result, "_\n \n");
*/
	(void) strcpy(result, "_\n");
    else
    {
	res[MAXROMANS-1] = 0;
	ovb[MAXROMANS-1] = 0;
	i = MAXROMANS-1;

	/* the significance of the current digit is 10 ** digitsig */
	for (digitsig = 0; (digitsig < MAXDIGITS) && (val > 0); digitsig++)
	{
	    digitval = val % 10;
	    for (j = br_trans[digitval][0]; j > 0; j--)
	    {
		/* printf("In j loop: %d %d\n", j, i); */
		res[--i] = br_equiv[digitsig][br_trans[digitval][j]];
		ovb[i] = br_overbar[digitsig][br_trans[digitval][j]];
	    }
	    val = val / 10;
	}

	j = i;
	while ((*result++ = ovb[j++]))
	    continue;
	*--result = '\n';

	j = i;
	while ((*++result = res[j++]))
	    continue;
/* Final newline will be added by puts.
	*result++ = '\n';
*/
	*result = '\0';
    }
}

void clockface(bool mode)
/* enable or disable clockface mode (output IIII instead of IV) */
{
    if (mode)
    {
	/* clockface mode */
	br_trans[4][0] = 4;
	br_trans[4][1] = 0;
	br_trans[4][2] = 0;
    }
    else
    {
	/* normal mode */
	br_trans[4][0] = 2;
	br_trans[4][1] = 1;
	br_trans[4][2] = 2;
    }
}

void pout(unsigned int val)
/* output in `butchered' Roman numerals; see manual, part 4.4.13 */
{
    char	result[2*MAXROMANS+1];
    extern int wimp_mode;

    if(wimp_mode) {
	printf("%u\n",val);
    }
    else {
	butcher(val, result);
	(void) puts(result);
    }
    fflush(stdout);
}

/**********************************************************************
 *
 * The following two routines implement bitwise I/O.  They assume
 * 8 bit characters, but there's no reason more general versions
 * could not be written.
 *
 *********************************************************************/

void binin(unsigned int type, array *a, bool forget)
{
  static unsigned int lastin = 0;
  int c, v;
  unsigned int i;

  if (a->rank != 1)
    lose(E241, lineno, (char *)NULL);

  for (i = 0 ; i < a->dims[0] ; i++) {
    v = ((c=getchar()) == EOF) ? 256 : (c - lastin) % 256;
    lastin = c;
    if (!forget) {
      if (type == TAIL)
	a->data.tail[i] = v;
      else
	a->data.hybrid[i] = v;
    }
  }
}

void binout(unsigned int type, array *a)
{
  static unsigned int lastout = 0;
  unsigned int i, c;

  if (a->rank != 1)
    lose(E241, lineno, (char *)NULL);

  for (i = 0 ; i < a->dims[0] ; i++) {
    if (type == TAIL)
      c = lastout - a->data.tail[i];
    else
      c = lastout - a->data.hybrid[i];
    lastout = c;
    c = (c & 0x0f) << 4 | (c & 0xf0) >> 4;
    c = (c & 0x33) << 2 | (c & 0xcc) >> 2;
    c = (c & 0x55) << 1 | (c & 0xaa) >> 1;
    putchar(c);
  }
}

/**********************************************************************
 *
 * The following assignment function performs IGNORE and type checks
 *
 *********************************************************************/

unsigned int assign(char *dest, unsigned int type, bool forget,
		    unsigned int value)
{
  unsigned int retval;
  if (type == ONESPOT || type == TAIL) {
    if (value > (unsigned int)Max_small)
      lose(E275, lineno, (char *)NULL);
    if (forget)
      retval = value;
    else {
      retval = *(type16*)dest;
      *(type16*)dest = value;
    }
  }
  else if (type == TWOSPOT || type == HYBRID) {
    if (forget)
      retval = value;
    else {
      retval = *(type32*)dest;
      *(type32*)dest = value;
    }
  }
  return retval;
}

/**********************************************************************
 *
 * The following functions implement the INTERCAL array model
 *
 *********************************************************************/

char *aref(va_alist) va_dcl
/* return a pointer to the array location specified by args */
{
  unsigned int type;
  array *a;
  unsigned int v;
  va_list ap;
  int address = 0;
  unsigned int i;

  va_start(ap);
  type = va_arg(ap, unsigned int);
  a = va_arg(ap, array*);

  if (va_arg(ap, unsigned int) != a->rank)
    lose(E241, lineno, (char *)NULL);

  for (i = 0 ; i < a->rank ; i++) {
    v = va_arg(ap, unsigned int);
    if (v == 0 || v > a->dims[i])
      lose(E241, lineno, (char *)NULL);
    address = address * a->dims[i] + v - 1;
  }

  va_end(ap);

  if (type == TAIL)
    return (char*)&a->data.tail[address];
  else
    return (char*)&a->data.hybrid[address];
}

void resize(va_alist) va_dcl
/* resize an array to the given shape */
{
  unsigned int type;
  array *a;
  bool forget;
  unsigned int i, r, v;
  va_list ap;
  int prod = 1;

  va_start(ap);
  type = va_arg(ap, unsigned int);
  a = va_arg(ap, array*);
  forget = va_arg(ap, bool);

  r = va_arg(ap, unsigned int);
  if (!forget) {
    a->rank = r;
    if (a->dims)
      free((char*)a->dims);
    a->dims = (unsigned int*) malloc(a->rank * sizeof(unsigned int));
    if (a->dims == NULL)
      lose(E241, lineno, (char *)NULL);
  }

  for (i = 0 ; i < r ; i++) {
    v = va_arg(ap, unsigned int);
    if (v == 0)
      lose(E240, lineno, (char *)NULL);
    if (!forget) {
      a->dims[i] = v;
      prod *= v;
    }
  }

  if (!forget) {
    if (type == TAIL) {
      if (a->data.tail)
	free((char *)a->data.tail);
      a->data.tail   = (type16*)malloc(prod * sizeof(type16));
      if (a->data.tail == NULL)
	lose(E241, lineno, (char *)NULL);
    }
    else {
      if (a->data.hybrid)
	free((char *)a->data.hybrid);
      a->data.hybrid = (type32*)malloc(prod * sizeof(type32));
      if (a->data.hybrid == NULL)
	lose(E241, lineno, (char *)NULL);
    }
  }

  va_end(ap);
}

/**********************************************************************
 *
 * The following functions implement save/retrieve
 *
 *********************************************************************/

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
} stashbox;

static stashbox *first;

void stashinit(void)
{
  first = NULL;
}

static stashbox *fetch(unsigned int type, unsigned int index)
/* find a stashed variable in the save stack and extract it */
{
  stashbox **pp = &first, *sp = first;

  while (sp && (sp->type != type || sp->index != index)) {
    pp = &sp->next;
    sp = sp->next;
  }
  if (sp)
    *pp = sp->next;

  return (sp);
}

void stash(unsigned int type, unsigned int index, void *from)
/* stash away the variable's value */
{
  /* create a new stashbox and push it onto the stack */
  stashbox *sp = (stashbox*)malloc(sizeof(stashbox));
  if (sp == NULL) lose(E222, lineno, (char *)NULL);
  sp->next = first;
  first = sp;

  /* store the variable in it */
  sp->type = type;
  sp->index = index;
  if (type == ONESPOT)
    memcpy((char *)&sp->save.onespot, from, sizeof(type16));
  else if (type == TWOSPOT)
    memcpy((char *)&sp->save.twospot, from, sizeof(type32));
  else if (type == TAIL || type == HYBRID) {
    array *a = (array*)from;
    int prod;
    unsigned int i;
    sp->save.a = (array*)malloc(sizeof(array));
    if (sp->save.a == NULL) lose(E222, lineno, (char *)NULL);
    sp->save.a->rank = a->rank;
    sp->save.a->dims = (unsigned int*)malloc(a->rank * sizeof(unsigned int));
    if (sp->save.a->dims == NULL) lose(E222, lineno, (char *)NULL);
    memcpy((char*)sp->save.a->dims, (char*)a->dims,
	   a->rank * sizeof(unsigned int));
    prod = a->rank ? 1 : 0;
    for (i = 0 ; i < a->rank ; i++) {
      prod *= a->dims[i];
    }
    if (type == TAIL) {
      sp->save.a->data.tail =
	(type16*)malloc(prod * sizeof(type16));
      if (sp->save.a->data.tail == NULL) lose(E222, lineno, (char *)NULL);
      memcpy((char *)sp->save.a->data.tail,
	     (char*)a->data.tail, prod * sizeof(type16));
    }
    else {
      sp->save.a->data.hybrid =
	(type32*)malloc(prod * sizeof(type32));
      if (sp->save.a->data.hybrid == NULL) lose(E222, lineno, (char *)NULL);
      memcpy((char *)sp->save.a->data.hybrid,
	     (char*)a->data.hybrid, prod * sizeof(type32));
    }
  }
  return;
}

void retrieve(void *to, int type, unsigned int index, bool forget)
/* restore the value of a variable from the save stack */
{
  stashbox *sp;

  if ((sp = fetch(type, index)) == (stashbox *)NULL)
    lose(E436, lineno, (char *)NULL);
  else if (!forget) {
    if (type == ONESPOT)
      memcpy(to, (char *)&sp->save.onespot, sizeof(type16));
    else if (type == TWOSPOT)
      memcpy(to, (char *)&sp->save.twospot, sizeof(type32));
    else if (type == TAIL || type == HYBRID) {
      array *a = (array*)to;
      if (a->rank) {
	free(a->dims);
	if (type == TAIL)
	  free(a->data.tail);
	else
	  free(a->data.hybrid);
	memcpy(to, (char*)sp->save.a, sizeof(array));
      }
      free(sp->save.a);
    }
  }
  else if (type == TAIL || type == HYBRID) {
    free(sp->save.a->dims);
    if (type == TAIL)
      free(sp->save.a->data.tail);
    else
      free(sp->save.a->data.hybrid);
    free(sp->save.a);
  }
  free(sp);
}

/**********************************************************************
 *
 * The following function is used for random error generation
 *
 *********************************************************************/

unsigned int roll(unsigned int n)
/* return TRUE on n% chance, FALSE otherwise */
{
#ifdef USG
   return((unsigned int)(lrand48() % 100) < n);
#else
   return((unsigned int)(rand() % 100) < n);
#endif /* UNIX */
}

/* cesspool.c ends here */
