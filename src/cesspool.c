/*****************************************************************************

NAME
    cesspool.c -- storage management and runtime support for INTERCAL

LICENSE TERMS
    Copyright (C) 1996 Eric S. Raymond

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

***************************************************************************/
/* LINTLIBRARY */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

/* AIS: Make this work with DJGPP's stdarg.h */
#ifdef __DJGPP__
#define _POSIX_SOURCE
#endif

#ifdef _POSIX_SOURCE
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "sizes.h"
/* AIS: To avoid a separate overloaded/nonoverloaded library, we import
   the overloading defines here and ignore them if we don't need them. */
#define MULTITHREAD 0
#define OVEROPUSED 1
#include "abcess.h"
#include "lose.h"

#include "numerals.c"

/* AIS: These will be set to stdin/stdout at the first opportunity,
   which is not necessarily here. */
FILE* cesspoolin=0;
FILE* cesspoolout=0;

/* AIS: To keep ld happy. This shouldn't ever actually get used, but
 * give it a sane value just in case it does. (This is referenced by
 * clc-cset.c, but due to the linking-in of the character sets
 * themselves the reference should never be used.) */
char* datadir=".";

/**********************************************************************
 *
 * The following functions manipulate the nexting stack
 *
 *********************************************************************/

int* next; /* AIS: now allocated by ick-wrap.c */
int nextindex = 0;

static int clcsem = 0; /* AIS */

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

    if(!cesspoolin) cesspoolin=stdin; /* AIS */

    if (fgets(buf, BUFSIZ, cesspoolin) == (char *)NULL)
	lose(E562, lineno, (char *)NULL);
    n = strlen(buf) - 1;
    if (n > 0 && buf[n-1] == '\r')
	--n;
    buf[n] = '\0';

    if(wimp_mode) {
	result = (unsigned int)strtoul(buf, (char **)NULL, 10);
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

void clcsemantics(bool mode) /* AIS: CLC-INTERCAL semantics mode? */
{
  clcsem=mode;
}

void pout(unsigned int val)
/* output in `butchered' Roman numerals; see manual, part 4.4.13 */
{
    char	result[2*MAXROMANS+1];
    extern int wimp_mode;

    if(!cesspoolout) cesspoolout=stdout; /* AIS */

    if(wimp_mode) {
	(void) fprintf(cesspoolout,"%u\n",val);
    }
    else {
	butcher(val, result);
	(void) fprintf(cesspoolout,"%s\n",result);
    }
    fflush(cesspoolout);
}

/**********************************************************************
 *
 * AIS: CLC-INTERCAL bitwise I/O, only used in CLC-INTERCAL semantics
 * mode. The I/O is done in extended Baudot for a tail array, or in
 * mingled form for a hybrid array; for the Baudot, we rely on
 * clc-cset.c and on the Baudot and Latin-1 character sets that are
 * linked to libick.a (or libickmt.a), so the final executable doesn't
 * reference the compiler's libraries. clc-cset.c is designed to
 * handle this all transparently, though, so we don't have to worry
 * about the details. I wrote the next two functions.
 *
 **********************************************************************/

/* AIS: From clc-cset.c */
extern int clc_cset_convert(char* in, char* out, char* incset,
			    char* outcset, int padstyle, int outsize,
			    FILE* errsto);

static void clcbinin(unsigned int type, array *a, bool forget)
{
  int i;
  char* buf, *tempcp;
  /* Allocating one byte per element in the array must be enough,
   * because the Baudot version cannot possibly be shorter than the
   * original Latin-1, plus one for the terminating NUL. */
  i=a->dims[0]; /* we already know that there's 1 dim only */
  if(!i) i=1;
  buf=malloc(i+1);
  if(!buf) lose(E252, lineno, (char*)NULL);
  if(!fgets(buf,a->dims[0],cesspoolin))
    strcpy(buf,"\n"); /* EOF inputs the null string in CLC-INTERCAL */
  tempcp=strrchr(buf,'\n'); /* still working in ASCII at this point */
  if(!tempcp) /* input too long for the array is an error */
  {
    free(buf);
    lose(E241, lineno, (char*)NULL);
  }
  *tempcp='\0'; /* chomp the final newline */
  tempcp=malloc(6*i+12); /* to be on the safe side, even though
			  * Baudot doesn't use 16-byte chars */
  if(!tempcp) lose(E252, lineno, (char*)NULL);
  /* Zero the array now. */
  i=a->dims[0];
  if(!forget) while(i--)
		if(type==TAIL)
		  a->data.tail[i]=0;
		else
		  a->data.hybrid[i]=0;
  i=clc_cset_convert(buf,tempcp,"latin1","baudot",2,6*a->dims[0]+12,0);
  /* Negative i ought to be impossible here; check anyway, and cause
   * an internal error if it has happened. */
  if(i<0) lose(E778, lineno, (char*)NULL);
  if((unsigned)i>a->dims[0]) lose(E241, lineno, (char*)NULL);
  if(!forget) while(i--)
		if(type==TAIL)
		  a->data.tail[i]=tempcp[i]+(rand()/(RAND_MAX/256))*256;
		else
		  a->data.hybrid[i]=tempcp[i]+(rand()/(RAND_MAX/256))*256;
  free(tempcp);
  free(buf);
}

static void clcbinout(unsigned int type, array* a)
{
  int i;
  char* buf, *tempcp;
  buf=malloc(a->dims[0]+1);
  if(!buf) lose(E252, lineno, (char*) NULL);
  i=0; tempcp=buf;
  while((unsigned)i<a->dims[0])
  {
    if(type==TAIL)
      *tempcp=a->data.tail[i];
    else
      *tempcp=a->data.hybrid[i];
    i++;
    if(*tempcp) tempcp++; /* NULs are ignored for some reason, but
			   * that's the behaviour the CLC-INTERCAL
			   * specs specify */
  }
  *tempcp=0;
  /* tempcp is definitely overkill here, but the *6+6 rule is being
   * obeyed because that way the code is robust against any future
   * changes in character sets. */
  tempcp=malloc(a->dims[0]*6+12);
  if(!tempcp) lose(E252, lineno, (char*) NULL);
  i=clc_cset_convert(buf,tempcp,"baudot","latin1",0,6*a->dims[0]+12,0);
  tempcp[i]=0;
  /* CLC-INTERCAL bails out on invalid characters. C-INTERCAL uses
   * instead the behaviour of replacing them with character code 26.
   * (This is actually the purpose of character code 26 in ASCII, I
   * think, although this is derived from memory; I don't know of any
   * other system that uses it for this purpose, though, and the
   * ability to confuse Windows with it is worth what might be lost
   * through standards compliance.) */
  while(i--) if(!tempcp[i]) tempcp[i]=26;
  fprintf(cesspoolout,"%s\n",tempcp);
  free(tempcp);
  free(buf);
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

  if(!cesspoolin) cesspoolin=stdin; /* AIS */

  if(clcsem) {clcbinin(type, a, forget); return;} /* AIS */

  for (i = 0 ; i < a->dims[0] ; i++) {
    v = ((c=fgetc(cesspoolin)) == EOF) ? 256 : (c - lastin) % 256;
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

  if(!cesspoolout) cesspoolout=stdout; /* AIS */

  if(clcsem) {clcbinout(type, a); return;} /* AIS */

  for (i = 0 ; i < a->dims[0] ; i++) {
    if (type == TAIL)
      c = lastout - a->data.tail[i];
    else
      c = lastout - a->data.hybrid[i];
    lastout = c;
    c = (c & 0x0f) << 4 | (c & 0xf0) >> 4;
    c = (c & 0x33) << 2 | (c & 0xcc) >> 2;
    c = (c & 0x55) << 1 | (c & 0xaa) >> 1;
    fputc(c,cesspoolout);
    if (c == '\n')
      fflush(cesspoolout);
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
  unsigned int retval = 0;
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
 * If _POSIX_SOURCE is defined, stdarg is used, otherwise varargs.
 *
 *********************************************************************/


#ifdef _POSIX_SOURCE
char *aref(unsigned int type, ...)
#else
char *aref(va_alist) va_dcl
#endif
/* return a pointer to the array location specified by args */
{
#ifndef _POSIX_SOURCE
  unsigned int type;
#endif
  array *a;
  unsigned int v;
  va_list ap;
  int address = 0;
  unsigned int i;

#ifdef _POSIX_SOURCE
  va_start(ap, type);
#else
  va_start(ap);
  type = va_arg(ap, unsigned int);
#endif
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

#ifdef _POSIX_SOURCE
void resize(unsigned int type, ...)
#else
void resize(va_alist) va_dcl
#endif
/* resize an array to the given shape */
{
#ifndef _POSIX_SOURCE
  unsigned int type;
#endif
  array *a;
  bool forget;
  unsigned int i, r, v;
  va_list ap;
  int prod = 1;

#ifdef _POSIX_SOURCE
 va_start(ap, type);
#else
  va_start(ap);
  type = va_arg(ap, unsigned int);
#endif
  a = va_arg(ap, array*);
  forget = va_arg(ap, bool);

  if (!a->rank) a->dims = 0; /* AIS: a->dims no longer initialised */

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

stashbox *first; /* AIS: made non-static so it can be seen by unravel.c */

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

void stash(unsigned int type, unsigned int index, void *from, overop* oo)
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
  if(oo) sp->overloadinfo=oo[index]; /* AIS */
  if (type == ONESPOT)
  {
    memcpy((char *)&sp->save.onespot, from, sizeof(type16));
  }
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

void retrieve(void *to, int type, unsigned int index, bool forget, overop* oo)
/* restore the value of a variable from the save stack */
{
  stashbox *sp;

  if ((sp = fetch(type, index)) == (stashbox *)NULL)
    lose(E436, lineno, (char *)NULL);
  else if (!forget) {
    if(oo) oo[index]=sp->overloadinfo; /* AIS */
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
 * The following function is used for random decision making
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

/**********************************************************************
 *
 * AIS: This function is called when two COME FROMs reference the same
 *      line at runtime. multicome0 is used in a non-multithread
 *      program; it produces an error. For multicome1, see unravel.c.
 *
 *********************************************************************/

int multicome0(int errlineno, jmp_buf pc)
{
  (void) pc; /* it's ignored by this function */
  lose(E555, errlineno, (char *) NULL);
  /* this line number is quite possibly going to be wildly inaccurate */
  return 0;
}

/* cesspool.c ends here */
