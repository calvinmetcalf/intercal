/* cesspool.c -- storage management and runtime support for INTERCAL */
/* LINTLIBRARY */
#include <stdio.h>
#include "cesspool.h"
#include "lose.h"

/**********************************************************************
 *
 * The following functions manipulate the nexting stack
 *
 *********************************************************************/

#define MAXNEXT	80

int next[MAXNEXT];
int nextindex = 0;

void pushnext(n)
int n;
{
    if (nextindex < MAXNEXT)
	next[nextindex++] = n;
    else
	lose(lineno, E123);
}

unsigned int popnext(n)
int n;
{
    nextindex -= n;
    if (nextindex < 0) {
	nextindex = 0;
	return(-1);
    }
    return(next[nextindex]);
}

unsigned int resume(n)
unsigned int n;
{
    if (n == 0)
	lose(lineno, E621);
    else if ((n = popnext(n)) == -1)
	lose(lineno, E632);
    else
	return(n);
}

/**********************************************************************
 *
 * The following functions implement the INTERCAL I/O model
 *
 *********************************************************************/

unsigned int pin(width)
int width;
{
    char		buf[BUFSIZ], *cp, *strtok();
    unsigned int	result = 0;

    if (fgets(buf, BUFSIZ, stdin) == (char *)NULL)
	lose(lineno, E562);
    buf[strlen(buf)-1] = '\0';

    cp = strtok(buf, " ");
    do {
	int	digit;

	if (strcmp(cp, "OH") == 0 || strcmp(cp, "ZERO") == 0)
	    digit = 0;
	else if (strcmp(cp, "ONE") == 0)
	    digit = 1;
	else if (strcmp(cp, "TWO") == 0)
	    digit = 2;
	else if (strcmp(cp, "THREE") == 0)
	    digit = 3;
	else if (strcmp(cp, "FOUR") == 0)
	    digit = 4;
	else if (strcmp(cp, "FIVE") == 0)
	    digit = 5;
	else if (strcmp(cp, "SIX") == 0)
	    digit = 6;
	else if (strcmp(cp, "SEVEN") == 0)
	    digit = 7;
	else if (strcmp(cp, "EIGHT") == 0)
	    digit = 8;
	else if (strcmp(cp, "NINE") == 0 || strcmp(cp, "NINER") == 0)
	    digit = 9;
	else
	    lose(lineno, E579);

	result = result * 10 + digit;
    } while
	(cp = strtok((char *)NULL, " "));

    if (width == 16 && result > 0xffff)
	lose(lineno, E579);
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
 */

static void butcher(val, result)
unsigned long val;
char	*result;
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
	(void) strcpy(result, "_\n \n");
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
	while (*result++ = ovb[j++])
	    continue;
	*--result = '\n';

	j = i;
	while (*++result = res[j++])
	    continue;
	*result++ = '\n';
	*result = '\0';
    }
}

void clockface(mode)
/* enable or disable clockface mode (output IIII instead of IV) */
bool mode;
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

void pout(val)
/* output in `butchered' Roman numerals; see manual, part 4.4.13 */
unsigned int val;
{
    char	result[2*MAXROMANS+1];

    butcher(val, result);
    (void) puts(result);
}

/**********************************************************************
 *
 * The following functions implement the INTERCAL array model
 *
 *********************************************************************/

/*VARARGS3*/
unsigned int aget(type, index, dim)
/* return the value of the array location given by the 4th-nth args */
unsigned int	type, index;
int		dim;
{
    /*
     * The function may abort if no resize() has been executed for the
     * array, or if any member of the tuple is zero.
     */
}

/*VARARGS4*/
void aput(value, type, index, dim)
/* set the value of an array location given by the 5th-nth args */
unsigned int	value, type, index;
int		dim;
{
    /*
     * The function may abort if no resize() has been executed for the
     * array, or if any member of the tuple is zero.
     */
}

/*VARARGS3*/
void resize(type, index, dim)
/* resize an array to the shape of the 4th-nth args */
unsigned int	type, index;
int		dim;
{
    /*
     * The function may abort if any member of the tuple is zero.
     * If the array being resized exists, it should preserve the contents
     * of whatever sub-volume remains in the resized version.
     */
}

static void arraycopy(type, index, target, source)
/* stash away an array variable's value */
unsigned int	type, index;
char	**target, *source;
{
}

/**********************************************************************
 *
 * The following functions implement save/retrieve
 *
 * If you want a more sophisticated dynamic allocation strategy,
 * go ahead and implement one! *I'm* sure not stopping you...
 *
 *********************************************************************/

#define MAXSAVE	100

typedef struct		/* this is a save-stack element */
{
    int	type;		/* variable type */
    int index;		/* variable's index within the type */
    int depth;		/* depth of stash */
    union		/* the data itself */
    {
	short	onespot;
	int	twospot;
	short	*tail;
	int	*hybrid;
    } save;
}
stashbox;

static stashbox stashes[MAXSAVE];

void stashinit()
{
    memset(stashes, '\0', sizeof(stashes));
}

static stashbox *fetch(type, index)
/* restore the value of a variable from the save stack */
unsigned int	type, index;
{
    stashbox *sp, *tp = (stashbox *)NULL;
    int depthfound = 0;

    for (sp = stashes; sp < stashes + MAXSAVE; sp++)
	if (sp->type == type && sp->index == index && sp->depth > depthfound)
	{
	    depthfound = sp->depth;
	    tp = sp;
	}

    return(tp);
}

void stash(type, index, from)
/* stash away the variable's value */
char		*from;
unsigned int	type, index;
{
    stashbox *sp, *op = fetch(type, index);

    for (sp = stashes; sp < stashes + MAXSAVE; sp++)
	if (sp->type == 0)
	{
	    if (op)
		sp->depth = op->depth + 1;
	    else
		sp->depth = 0;
	    if (type == ONESPOT)
		memcpy((char *)&sp->save.onespot, from,sizeof(unsigned short));
	    else if (type == TWOSPOT)
		memcpy((char *)&sp->save.twospot, from, sizeof(unsigned int));
	    else if (type == TAIL)
		arraycopy((char **)&sp->save.tail, from);
	    else if (type == HYBRID)
		arraycopy((char **)&sp->save.hybrid, from);
	    return;
	}
    lose(E222);
}

void retrieve(to, type, index)
char		**to;
unsigned int	type, index;
{
    stashbox	*sp;

    if ((sp = fetch(type, index)) == (stashbox *)NULL)
	lose(E436);
    else
    {
	if (type == ONESPOT)
	    memcpy(to, (char *)&sp->save.onespot, sizeof(unsigned short));
	else if (type == TWOSPOT)
	    memcpy(to, (char *)&sp->save.twospot, sizeof(unsigned int));
	else if (type == TAIL)
	{
	    free(to);		/* free storage allocated for array copy */
	    *to = (char *)&sp->save.tail;
	}
	else if (type == HYBRID)
	{
	    free(to);		/* free storage allocated for array copy */
	    *to = (char *)&sp->save.hybrid;
	}
	sp->type = 0;	/* mark this stashbox free */
    }
}

/**********************************************************************
 *
 * The following function is used for random error generation
 *
 *********************************************************************/

unsigned int roll(n)
/* return TRUE on n% chance, FALSE otherwise */
unsigned int n;
{
#ifdef USG
   return(lrand48() % 100 < n);
#else
   return(rand() % 100 < n);
#endif /* UNIX */
}

/* cesspool.c ends here */
