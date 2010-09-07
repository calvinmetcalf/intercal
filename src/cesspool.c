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
#include <limits.h>
#include <assert.h>

#include "config.h"
#ifdef HAVE_STDARG_H
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
#include "ick_lose.h"

#include "numerals.c"

/* AIS: because BUFSIZ could theoretically be too large for an int... */
#if BUFSIZ < INT_MAX
#define INTBUFSIZ (int)BUFSIZ
#else
#define INTBUFSIZ (int)INT_MAX
#endif

/* and likewise, so that we can compare things to INT_MAX */
#ifndef min
#define min(x,y) ((x)>(y)?(y):(x))
#endif

/* and likewise, define SIZE_MAX */
#ifndef SIZE_MAX
#ifdef ULLONG_MAX
#define SIZE_MAX (sizeof(unsigned short    )==sizeof(size_t)?(size_t) USHRT_MAX : \
		  sizeof(unsigned int      )==sizeof(size_t)?(size_t)  UINT_MAX : \
		  sizeof(unsigned long     )==sizeof(size_t)?(size_t) ULONG_MAX : \
		  sizeof(unsigned long long)==sizeof(size_t)?(size_t)ULLONG_MAX : 0)
#else
#define SIZE_MAX (sizeof(unsigned short    )==sizeof(size_t)?(size_t) USHRT_MAX : \
		  sizeof(unsigned int      )==sizeof(size_t)?(size_t)  UINT_MAX : \
		  sizeof(unsigned long     )==sizeof(size_t)?(size_t) ULONG_MAX : 0)
#endif
#endif

/* AIS: These will be set to stdin/stdout at the first opportunity,
   which is not necessarily here. The annotations are to tell splint
   that we know what we're doing here with the assignments; the
   set to stdin/stdout will be done whenever it's necessary.
*/
/*@null@*/ FILE* ick_cesspoolin =0;
/*@null@*/ FILE* ick_cesspoolout=0;

/* AIS: To keep ld happy. This shouldn't ever actually get used, but
 * give it a sane value just in case it does. (This is referenced by
 * clc-cset.c, but due to the linking-in of the character sets
 * themselves the reference should never be used.) */
/*@observer@*/ /*@dependent@*/ const char* ick_datadir=".";

/**********************************************************************
 *
 * The following functions manipulate the nexting stack
 *
 *********************************************************************/

unsigned* ick_next; /* AIS: now allocated by ick-wrap.c */
/*@null@*/ jmp_buf* ick_next_jmpbufs = NULL; /* AIS: for ick_ec, if needed */
int ick_nextindex = 0;

static int ick_clcsem = 0; /* AIS */

void ick_pushnext(unsigned n)
{
    if (ick_nextindex < ick_MAXNEXT)
	ick_next[ick_nextindex++] = n;
    else
	ick_lose(IE123, ick_lineno, (const char *)NULL);
}

unsigned int ick_popnext(unsigned int n)
{
    ick_nextindex -= n;
    if (ick_nextindex < 0) {
	ick_nextindex = 0;
	return (unsigned int)-1;
    }
    return ick_next[ick_nextindex];
}

/* AIS: This is not the ick_resume in ick_ec.h, which is a macro and therefore
   technically speaking doesn't clash with this function as the header file
   ick_ec.h isn't included. */
unsigned int ick_resume(unsigned int n)
{
    if (n == 0)
    {
	ick_lose(IE621, ick_lineno, (const char *)NULL);
	/*@-unreachable@*/ return 0; /*@=unreachable@*/
    }
    else if ((n = ick_popnext(n)) == (unsigned int)-1)
    {
	ick_lose(IE632, ick_lineno, (const char *)NULL);
	/*@-unreachable@*/ return 0; /*@=unreachable@*/
    }
    return(n);
}

/**********************************************************************
 *
 * The following functions implement the INTERCAL I/O model
 *
 *********************************************************************/

unsigned int ick_pin(void)
{
    char		buf[INTBUFSIZ], *cp;
    unsigned int	result = 0;
    size_t n;

    assert(buf != NULL); /* AIS: splint seems unable of figuring this
			    out for itself */

    if(!ick_cesspoolin) ick_cesspoolin=stdin; /* AIS */

    if (fgets(buf, INTBUFSIZ, ick_cesspoolin) == (char *)NULL)
	ick_lose(IE562, ick_lineno, (const char *)NULL);
    n = strlen(buf) - 1;
    if (n > 0 && buf[n-1] == '\r')
	--n;
    buf[n] = '\0';

    if(ick_wimp_mode) {
	result = (unsigned int)strtoul(buf, (char **)NULL, 10);
	n = 1;
    }
    else
    {
	for(n=0,cp = strtok(buf, " ");cp;cp = strtok((char *)NULL, " "),n++)
	{
	    int	digit = -1;
	    const numeral	*np;

	    for (np = ick_numerals; np < ick_numerals + sizeof(ick_numerals)/sizeof(numeral); np++)
		if (strcmp(np->name, cp) == 0)
		{
		    digit = np->value;
		    break;
		}

	    if (digit == -1)
		ick_lose(IE579, ick_lineno, cp);

	    if (result < 429496729 || (result == 429496729 && digit < 6))
		result = result * 10 + digit;
	    else
		ick_lose(IE533, ick_lineno, (const char *)NULL);
	}
    }
    if (!n)
	ick_lose(IE562, ick_lineno, (const char *)NULL);
    if (result > (unsigned int)ick_Max_large)
	ick_lose(IE533, ick_lineno, (const char *)NULL);
    return(result);
}

/**********************************************************************
 *
 * Butchered Roman ick_numerals implemented by
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
 * The ick_first column tells how many of the succeeding columns are used.
 * The other columns refer to the columns of br_equiv and br_overbar.
 */
static int ick_br_trans[10][5] =
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

static void butcher(unsigned long val, /*@out@*/ char *result)
{
    int i, j;
    int digitsig, digitval;
    char res[MAXROMANS], ovb[MAXROMANS];

    /* We need FOUR columns because of the odd way that M and I interact. */
    static const char br_equiv[MAXDIGITS][4] =
    {
	{'I', 'I', 'V', 'X'}, {'X', 'X', 'L', 'C'},
	{'C', 'C', 'D', 'M'}, {'M', 'I', 'V', 'X'},
	{'X', 'X', 'L', 'C'}, {'C', 'C', 'D', 'M'},
	{'M', 'i', 'v', 'x'}, {'x', 'x', 'l', 'c'},
	{'c', 'c', 'd', 'm'}, {'m', 'i', 'v', 'x'},
    };

    static const char br_overbar[MAXDIGITS][4] =
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
	res[MAXROMANS-1] = '\0';
	ovb[MAXROMANS-1] = '\0';
	i = MAXROMANS-1;

	/* the significance of the current digit is 10 ** digitsig */
	for (digitsig = 0; (digitsig < MAXDIGITS) && (val > 0); digitsig++)
	{
	  digitval = (int)(val % 10);
	    for (j = ick_br_trans[digitval][0]; j > 0; j--)
	    {
		/* printf("In j loop: %d %d\n", j, i); */
		res[--i] = br_equiv[digitsig][ick_br_trans[digitval][j]];
		ovb[i] = br_overbar[digitsig][ick_br_trans[digitval][j]];
	    }
	    val = val / 10;
	}

	j = i;
	while ((*result++ = ovb[j++]) != '\0')
	    continue;
	*--result = '\n';

	j = i;
	while ((*++result = res[j++]) != '\0')
	    continue;
/* Final newline will be added by puts.
	*result++ = '\n';
*/
	*result = '\0';
    }
}

void ick_clockface(bool mode)
/* enable or disable ick_clockface mode (output IIII instead of IV) */
{
    if (mode)
    {
	/* ick_clockface mode */
	ick_br_trans[4][0] = 4;
	ick_br_trans[4][1] = 0;
	ick_br_trans[4][2] = 0;
    }
    else
    {
	/* normal mode */
	ick_br_trans[4][0] = 2;
	ick_br_trans[4][1] = 1;
	ick_br_trans[4][2] = 2;
    }
}

void ick_setclcsemantics(bool mode) /* AIS: CLC-INTERCAL semantics mode? */
{
  ick_clcsem=mode;
}

void ick_pout(unsigned int val)
/* output in `butchered' Roman ick_numerals; see manual, part 4.4.13 */
{
    char	result[2*MAXROMANS+1];

    if(!ick_cesspoolout) ick_cesspoolout=stdout; /* AIS */

    if(ick_wimp_mode) {
	(void) fprintf(ick_cesspoolout,"%u\n",val);
    }
    else {
	butcher(val, result);
	(void) fprintf(ick_cesspoolout,"%s\n",result);
    }
    (void) fflush(ick_cesspoolout);
}

/**********************************************************************
 *
 * AIS: CLC-INTERCAL bitwise I/O, only used in CLC-INTERCAL semantics
 * mode. The I/O is done in extended Baudot for a tail ick_array, or in
 * mingled form for a hybrid ick_array; for the Baudot, we rely on
 * clc-cset.c and on the Baudot and Latin-1 character sets that are
 * linked to libick.a (or libickmt.a), so the final executable doesn't
 * reference the compiler's libraries. clc-cset.c is designed to
 * handle this all transparently, though, so we don't have to worry
 * about the details. I wrote the next two functions.
 *
 **********************************************************************/

/* AIS: From clc-cset.c */
extern int ick_clc_cset_convert(const char* in, /*@partial@*/ char* out, const char* incset,
				const char* outcset, int padstyle, size_t outsize,
				/*@null@*/ FILE* errsto);

static void clcbinin(unsigned int type, ick_array *a, bool forget)
{
  size_t i;
  int ti;
  char* buf, *tempcp;
  /* Allocating one byte per element in the ick_array must be enough,
   * because the Baudot version cannot possibly be shorter than the
   * original Latin-1, plus one for the terminating NUL. There is one
   * potential problem, which is that fgets takes an int for the
   * number of bytes to read, so we cap the number of bytes to read
   * at INT_MAX and hope that's enough. */
  i=a->dims[0]; /* we already know that there's 1 dim only */
  if(SIZE_MAX/6-2<=a->dims[0])
    ick_lose(IE252, ick_lineno, (const char*)NULL); /* size is too large */
  if(!i) i=1;
  buf=malloc(i+1);
  if(!buf) ick_lose(IE252, ick_lineno, (const char*)NULL);
  if(!ick_cesspoolin) ick_cesspoolin=stdin;
  if(!fgets(buf,(int)(min(a->dims[0],(size_t)INT_MAX)),ick_cesspoolin))
    strcpy(buf,"\n"); /* EOF inputs the null string in CLC-INTERCAL */
  tempcp=strrchr(buf,'\n'); /* still working in ASCII at this point */
  if(!tempcp) /* input too long for the ick_array is an error */
  {
    free(buf);
    ick_lose(IE241, ick_lineno, (const char*)NULL);
  }
  *tempcp='\0'; /* chomp the final newline */
  tempcp=malloc(6*i+12); /* to be on the safe side, even though
			  * Baudot doesn't use 16-byte chars */
  if(!tempcp) ick_lose(IE252, ick_lineno, (const char*)NULL);
  /* Zero the ick_array now. */
  i=a->dims[0];
  if(!forget) while(i--)
		if(type==ick_TAIL)
		  a->data.tail[i]=0;
		else
		  a->data.hybrid[i]=0;
  ti=ick_clc_cset_convert(buf,tempcp,"latin1","baudot",2,6*a->dims[0]+12,(FILE*)0);
  /* Negative ti ought to be impossible here; check anyway, and cause
   * an internal error if it has happened. */
  if(ti<0) ick_lose(IE778, ick_lineno, (const char*)NULL);
  i=(size_t)ti;
  if(i>a->dims[0]) ick_lose(IE241, ick_lineno, (const char*)0);
  if(!forget) while(i--)
		if(type==ick_TAIL)
		  a->data.tail[i]=(ick_type16)tempcp[i]+
		    (ick_type16)((rand()%256)*256);
		else
		  a->data.hybrid[i]=(ick_type32)tempcp[i]+
		    (ick_type32)((rand()%256)*256);
  free(tempcp);
  free(buf);
}

static void clcbinout(unsigned int type, const ick_array* a)
{
  size_t i;
  int ti;
  char* buf, *tempcp;
  if(SIZE_MAX/6-2<=a->dims[0])
    ick_lose(IE252, ick_lineno, (const char*)NULL); /* size is too large */
  buf=malloc(a->dims[0]+1);
  if(!buf) ick_lose(IE252, ick_lineno, (const char*) NULL);
  i=0; tempcp=buf;
  while(i<a->dims[0])
  {
    /* Values above 31 are invalid in Baudot, so cap them at 33 to
       avoid integer wraparound trouble. */
    if(type==ick_TAIL)
      *tempcp=(char)min(33,a->data.tail[i]);
    else
      *tempcp=(char)min(33,a->data.hybrid[i]);
    i++;
    if(*tempcp!='\0')
      tempcp++; /* NULs are ignored for some reason, but
		 * that's the behaviour the CLC-INTERCAL
		 * specs specify */
  }
  *tempcp='\0';
  /* tempcp is definitely overkill here, but the *6+6 rule is being
   * obeyed because that way the code is robust against any future
   * changes in character sets. */
  tempcp=malloc(a->dims[0]*6+12);
  if(!tempcp) ick_lose(IE252, ick_lineno, (const char*) NULL);
  ti=ick_clc_cset_convert(buf,tempcp,"baudot","latin1",0,6*a->dims[0]+12,(FILE*)0);
  if(ti<0) ick_lose(IE778, ick_lineno, (const char*)NULL);
  i=(size_t)ti;
  tempcp[i]='\0';
  /* CLC-INTERCAL bails out on invalid characters. C-INTERCAL uses
   * instead the behaviour of replacing them with character code 26.
   * (This is actually the purpose of character code 26 in ASCII, I
   * think, although this is derived from memory; I don't know of any
   * other system that uses it for this purpose, though, and the
   * ability to confuse Windows with it is worth what might be lost
   * through standards compliance, because Windows nonstandardly
   * treats it as an EOF character.) */
  while(i--) if(tempcp[i] == '\0') tempcp[i]='\x1a';
  if(!ick_cesspoolout) ick_cesspoolout=stdout;
  fprintf(ick_cesspoolout,"%s\n",tempcp);
  (void) fflush(ick_cesspoolout);
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

void ick_binin(unsigned int type, ick_array *a, bool forget)
{
  static int lastin = 0;
  int c, v;
  size_t i;

  if (a->rank != 1)
    ick_lose(IE241, ick_lineno, (const char *)NULL);

  if(!ick_cesspoolin) ick_cesspoolin=stdin; /* AIS */

  if(ick_clcsem) {clcbinin(type, a, forget); return;} /* AIS */

  for (i = 0 ; i < a->dims[0] ; i++) {
    v = ((c=fgetc(ick_cesspoolin)) == EOF) ? 256 :
      ((unsigned)c - lastin) % 256;
    lastin = c;
    if (!forget) {
      if (type == ick_TAIL)
	a->data.tail[i] = (ick_type16) v;
      else
	a->data.hybrid[i] = (ick_type32) v;
    }
  }
}

void ick_binout(unsigned int type, const ick_array *a)
{
  static unsigned int lastout = 0;
  unsigned int c;
  size_t i;

  if (a->rank != 1)
    ick_lose(IE241, ick_lineno, (const char *)NULL);

  if(!ick_cesspoolout) ick_cesspoolout=stdout; /* AIS */

  if(ick_clcsem) {clcbinout(type, a); return;} /* AIS */

  for (i = 0 ; i < a->dims[0] ; i++) {
    if (type == ick_TAIL)
      c = lastout - a->data.tail[i];
    else
      c = lastout - a->data.hybrid[i];
    lastout = c;
    c = (c & 0x0f) << 4 | (c & 0xf0) >> 4;
    c = (c & 0x33) << 2 | (c & 0xcc) >> 2;
    c = (c & 0x55) << 1 | (c & 0xaa) >> 1;
    (void) fputc((int)c,ick_cesspoolout);
    if (c == 10 /* \n in INTERCAL */ || /* AIS */ ick_instapipe)
      (void) fflush(ick_cesspoolout);
  }
}

/**********************************************************************
 *
 * The following assignment function performs IGNORE and type checks
 *
 *********************************************************************/

unsigned int ick_assign(char *dest, unsigned int type, bool forget,
		    unsigned int value)
{
  unsigned int retval = 0;
  if (type == ick_ONESPOT || type == ick_TAIL) {
    if (value > (unsigned int)ick_Max_small)
      ick_lose(IE275, ick_lineno, (const char *)NULL);
    if (forget)
      retval = value;
    else {
      retval = *(ick_type16*)dest;
      *(ick_type16*)dest = (ick_type16) value;
    }
  }
  else if (type == ick_TWOSPOT || type == ick_HYBRID) {
    if (forget)
      retval = value;
    else {
      retval = *(ick_type32*)dest;
      *(ick_type32*)dest = value;
    }
  }
  return retval;
}

/**********************************************************************
 *
 * The following functions implement the INTERCAL ick_array model
 * If HAVE_STDARG_H is defined, stdarg is used, otherwise varargs.
 *
 *********************************************************************/


#ifdef HAVE_STDARG_H
/*@dependent@*/ void *ick_aref(unsigned int type, ...)
#else
/*@dependent@*/ void *ick_aref(va_alist) va_dcl
#endif
/* return a pointer to the ick_array location specified by args */
{
#ifndef HAVE_STDARG_H
  unsigned int type;
#endif
  ick_array *a;
  unsigned int v;
  va_list ap;
  size_t address = 0;
  unsigned int i;

#ifdef HAVE_STDARG_H
  va_start(ap, type);
#else
  va_start(ap);
  type = va_arg(ap, unsigned int);
#endif
  a = va_arg(ap, ick_array*);

  if (va_arg(ap, unsigned int) != a->rank)
    ick_lose(IE241, ick_lineno, (const char *)NULL);

  for (i = 0 ; i < a->rank ; i++) {
    v = va_arg(ap, unsigned int);
    if (v == 0 || (size_t)v > a->dims[i])
      ick_lose(IE241, ick_lineno, (const char *)NULL);
    address = address * a->dims[i] + v - 1;
  }

  va_end(ap);

  if (type == ick_TAIL)
    return (void*)&(a->data.tail[address]);
  else
    return (void*)&(a->data.hybrid[address]);
}

#ifdef HAVE_STDARG_H
void ick_resize(unsigned int type, ...)
#else
void ick_resize(va_alist) va_dcl
#endif
/* ick_resize an ick_array to the given shape */
{
#ifndef HAVE_STDARG_H
  unsigned int type;
#endif
  ick_array *a;
  bool forget;
  unsigned int i, r;
  size_t v;
  va_list ap;
  int prod = 1;

#ifdef HAVE_STDARG_H
 va_start(ap, type);
#else
  va_start(ap);
  type = va_arg(ap, unsigned int);
#endif
  a = va_arg(ap, ick_array*);
#ifdef BOOL_VARARGS_BROKEN
  forget = (bool)va_arg(ap, int);
#else
  forget = va_arg(ap, bool);
#endif

  /* AIS: a->dims is no longer initialised. So initialise it here if
     it isn't already initialised, with an annotation to explain that
     we aren't freeing the old pointer (because it was never malloced in
     the first place and is probably invalid anyway.) */
  /*@-mustfreeonly@*/
  if (!a->rank) a->dims = 0;
  /*@-mustfreeonly@*/

  r = va_arg(ap, unsigned int);
  if (!forget) {
    a->rank = r;
    if (a->dims)
      free((char*)a->dims);
    a->dims = malloc(a->rank * sizeof(*(a->dims)));
    if (a->dims == NULL)
      ick_lose(IE241, ick_lineno, (const char *)NULL);
  }

  for (i = 0 ; i < r ; i++) {
    v = va_arg(ap, size_t);
    if (v == 0)
      ick_lose(IE240, ick_lineno, (const char *)NULL);
    if (!forget) {
      assert(a->dims != NULL); /* AIS: it isn't, because !forget, but
				  splint doesn't know that */
      a->dims[i] = v;
      prod *= v;
    }
  }

  if (!forget) {
    if (type == ick_TAIL) {
      if (a->data.tail)
	free((char *)a->data.tail);
      a->data.tail   = (ick_type16*)malloc(prod * sizeof(ick_type16));
      if (a->data.tail == NULL)
	ick_lose(IE241, ick_lineno, (const char *)NULL);
    }
    else {
      if (a->data.hybrid)
	free((char *)a->data.hybrid);
      a->data.hybrid = (ick_type32*)malloc(prod * sizeof(ick_type32));
      if (a->data.hybrid == NULL)
	ick_lose(IE241, ick_lineno, (const char *)NULL);
    }
  }

  va_end(ap);
}

/**********************************************************************
 *
 * The following functions implement save/retrieve
 *
 *********************************************************************/

/*@null@*/ ick_stashbox *ick_first; /* AIS: made non-static so it can be seen by unravel.c */

void ick_stashinit(void)
{
  ick_first = NULL;
}

static /*@null@*/ ick_stashbox *fetch(unsigned int type, unsigned int index)
/* find a stashed variable in the save stack and extract it */
{
  ick_stashbox **pp = &ick_first, *sp = ick_first;

  while (sp != NULL && (sp->type != type || sp->index != index)) {
    pp = &sp->ick_next;
    sp = sp->ick_next;
  }
  if (sp)
    *pp = sp->ick_next;

  /* The annotation here is because Splint can't figure out that
     ick_first can be modified via pp, and because this function
     is the place where storage marked 'dependent' is initialised
     and deinitialised. */
  /*@-globstate@*/ /*@-dependenttrans@*/
  return (sp);
  /*@=globstate@*/ /*@=dependenttrans@*/
}

void ick_stash(unsigned int type, unsigned int index, void *from, ick_overop* oo)
/* stash away the variable's value */
{
  /*@-nullassign@*/
  ick_overop dummyoo;
  /*@=nullassign@*/
  /* create a new ick_stashbox and push it onto the stack */
  ick_stashbox *sp;
  dummyoo.get = (ick_type32 (*)(ick_type32))NULL;
  dummyoo.set = (void (*)(ick_type32, void(*)()))NULL;
  sp = (ick_stashbox*)malloc(sizeof(ick_stashbox));
  if (sp == NULL) ick_lose(IE222, ick_lineno, (const char *)NULL);
  sp->ick_next = ick_first;
  ick_first = sp;

  /* store the variable in it */
  ick_first->type = type;
  ick_first->index = index;
  if(oo) ick_first->overloadinfo=oo[index]; /* AIS */
  else ick_first->overloadinfo=dummyoo; /* AIS */
  if (type == ick_ONESPOT)
  {
    memcpy(&ick_first->save.onespot, from, sizeof(ick_type16));
  }
  else if (type == ick_TWOSPOT)
    memcpy(&ick_first->save.twospot, from, sizeof(ick_type32));
  else if (type == ick_TAIL || type == ick_HYBRID) {
    ick_array *a = (ick_array*)from;
    int prod;
    unsigned int i;
    ick_first->save.a = (ick_array*)malloc(sizeof(ick_array));
    if (ick_first->save.a == NULL) ick_lose(IE222, ick_lineno, (const char *)NULL);
    ick_first->save.a->rank = a->rank;
    ick_first->save.a->dims = malloc(a->rank * sizeof(*(ick_first->save.a->dims)));
    if (ick_first->save.a->dims == NULL) ick_lose(IE222, ick_lineno, (const char *)NULL);
    memcpy(ick_first->save.a->dims, a->dims,
	   a->rank * sizeof(*(a->dims)));
    prod = a->rank ? 1 : 0;
    for (i = 0 ; i < a->rank ; i++) {
      prod *= a->dims[i];
    }
    if (type == ick_TAIL) {
      ick_first->save.a->data.tail =
	(ick_type16*)malloc(prod * sizeof(ick_type16));
      if (ick_first->save.a->data.tail == NULL) ick_lose(IE222, ick_lineno, (const char *)NULL);
      memcpy(ick_first->save.a->data.tail,
	     a->data.tail, prod * sizeof(ick_type16));
    }
    else {
      ick_first->save.a->data.hybrid =
	(ick_type32*)malloc(prod * sizeof(ick_type32));
      if (ick_first->save.a->data.hybrid == NULL) ick_lose(IE222, ick_lineno, (const char *)NULL);
      memcpy(ick_first->save.a->data.hybrid,
	     a->data.hybrid, prod * sizeof(ick_type32));
    }
  }
  return;
}

void ick_retrieve(void *to, unsigned int type, unsigned int index,
		  bool forget, ick_overop* oo)
/* restore the value of a variable from the save stack */
{
  ick_stashbox *sp;

  if ((sp = fetch(type, index)) == (ick_stashbox *)NULL)
    ick_lose(IE436, ick_lineno, (const char *)NULL);
  else if (!forget) {
    if(oo) oo[index]=sp->overloadinfo; /* AIS */
    if (type == ick_ONESPOT)
      memcpy(to, (const char *)&sp->save.onespot, sizeof(ick_type16));
    else if (type == ick_TWOSPOT)
      memcpy(to, (const char *)&sp->save.twospot, sizeof(ick_type32));
    else if (type == ick_TAIL || type == ick_HYBRID) {
      ick_array *a = (ick_array*)to;
      /*@-branchstate@*/ /* it's a union, so one valid is correct */

      if (a->rank) {
	free(a->dims);
	if (type == ick_TAIL)
	  free(a->data.tail);
	else
	  free(a->data.hybrid);
	memcpy(to, (const char*)sp->save.a, sizeof(ick_array));
      }
      /*@=branchstate@*/
      /* AIS: there isn't a memory leak here, because we memcpyd the
	 pointers elsewhere and so they are yet accessible. You can't
	 expect Splint to figure out what's going on there, though, thus
	 the annotations. */
      /*@-compdestroy@*/
      free(sp->save.a);
      /*@=compdestroy@*/
    }
  }
  else if (type == ick_TAIL || type == ick_HYBRID) {
    free(sp->save.a->dims);
    if (type == ick_TAIL)
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

unsigned int ick_roll(unsigned int n)
/* return true on n% chance, false otherwise */
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
 *      line at runtime. ick_multicome0 is used in a non-multithread
 *      program; it produces an error. For multicome1, see unravel.c.
 *
 *********************************************************************/

int ick_multicome0(int errlineno, jmp_buf pc)
{
  /*@-noeffect@*/
  (void) pc; /* it's ignored by this function */
  /*@=noeffect@*/
  ick_lose(IE555, errlineno, (const char *) NULL);
  /* this line number is quite possibly going to be wildly inaccurate */
  /*@-unreachable@*/
  return 0;
  /*@=unreachable@*/
}

/**********************************************************************
 *
 * AIS: The next two functions are mine, and handle CREATE statements.
 *
 **********************************************************************/

struct ick_jictype
{
  /*@observer@*/ const char* sig; /* a shallow copy of a constant string */
  unsigned long target;
  /*@null@*/ /*@only@*/ struct ick_jictype* next;
};

/*@null@*/ /*@only@*/ static struct ick_jictype* jiclist = NULL;

/* Return a jic entry that matches the requested signature exactly,
   creating one if there isn't one yet. */
static struct ick_jictype* jicextract(/*@observer@*/ const char* sig)
{
  struct ick_jictype* jicptr = jiclist;
  while(jicptr)
  {
    if(strcmp(jicptr->sig,sig)==0) return jicptr;
    if(jicptr->next != NULL) jicptr = jicptr->next;
    else break;
  }
  if(!jicptr)
  {
    jiclist=malloc(sizeof *jiclist);
    jicptr=jiclist;
  }
  else
  {
    jicptr->next=malloc(sizeof *jiclist);
    jicptr=jicptr->next;
  }
  jicptr->next = NULL;
  jicptr->sig = sig;
  jicptr->target = 0;
  return jicptr;
}

void ick_registercreation(const char* sig, unsigned long target)
{
  jicextract(sig)->target=target;
}

unsigned long ick_jicmatch(const char* sig)
{
  return jicextract(sig)->target;
}

/* AIS: Used by the JIC code to error out when attempting to access an array */
ick_type32 ick_ieg277(ick_type32 ignored)
{
  /*@-noeffect@*/
  (void) ignored;
  /*@=noeffect@*/
  ick_lose(IE277, ick_lineno, (const char*) NULL);
}

void ick_ies277(ick_type32 ignored, void(*ignored2)())
{
  /*@-noeffect@*/
  (void) ignored;
  (void) ignored2;
  /*@=noeffect@*/
  ick_lose(IE277, ick_lineno, (const char*) NULL);
}

/* cesspool.c ends here */
