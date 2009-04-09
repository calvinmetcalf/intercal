/* yuk.h - header file for yuk.c and debugging versions of ick-wrap.c */
/* Copyright (C) Alex Smith 2006. See yuk.c for copyright conditions.
   This file is GPL'd, and so can be freely redistributed but has
   absolutely no warranty. */

#define YUK(t,u) yukline(t,u)
#define YUKTERM yukterm()

#define YUKEND 5

/*@-exportlocal@*/ /* because these are used by generated programs */
extern void yukline(int,int);
extern void yukterm(void);

extern int ick_abstained[];
extern int yukopts;
extern int ick_lineno;
extern int yuklines;
extern int yukcommands;
/*@-incondefs@*/ /* because it's a different textlines to the one in ick.h */
extern char *textlines[];
/*@=incondefs@*/
extern int lineofaboff[];
extern char *yukexplain[];

extern int yukloop;
/*@=exportlocal@*/

/* Defines that change the functions used for profiling. */

/* Define the following to the timing function you want for profiling, or leave
   it undef to use the value computed by config.sh. */
#undef YPTIMERTYPE
/* 0 to use clock(), 1 to use gettimeofday() and long,
   2 to use gettimeofday() and long long, 3 to use gethrtime()
   4 to use times(), 5 to use clock_gettime */
/* Note: On many systems, 0's resolution is too low to produce any output
	 1 and 2 produce the same output; use 2 if your system can handle
	 long long because the overflow is dealt with more simply.
         3 is a system-specific function. If there are more system-specific
         functions around that return more accurate times than the others
         used here, it would improve the profiler on those systems.
         According to the DJGPP documentation, it's impossible to get times
         with decent accuracy under DOS. DJGPP implements timer types 0, 1,
         and 2, so it will 'work' with the default value of 2, but the results
         you get will be basically meaningless as the base information can't
         be accurately obtained (the resolution is slightly worse than 50ms,
         which is far too slow for profiling). */
#ifndef YPTIMERTIME
# ifdef HAVE_GETHRTIME
#  define YPTIMERTYPE 3
# else
#  if defined(HAVE_CLOCK_GETTIME) && SIZEOF_LONG_LONG_INT + 0 > 0
#   define YPTIMERTYPE 5
#  else
#   ifdef HAVE_GETTIMEOFDAY
/* Allow for an erroneous blank value for SIZEOF_LONG_LONG_INT. */
#    if SIZEOF_LONG_LONG_INT + 0 > 0
#     define YPTIMERTYPE 2
#    else
#     define YPTIMERTYPE 1
#    endif
#   else
#    define YPTIMERTYPE 0
#   endif
#  endif
# endif
#endif


#if YPTIMERTYPE == 0
#define YPTIMERTFORMAT "lu"
#define YPCOUNTERTFORMAT "lu"
#define YPTIMERTFORMATD "6" YPTIMERTFORMAT
#define YPTIMERSCALE CLOCKS_PER_SEC
#define YPGETTIME clock()
typedef unsigned long yptimer;
typedef unsigned long ypcounter;

#elif YPTIMERTYPE == 1
#define YPTIMERTFORMAT "lu"
#define YPCOUNTERTFORMAT "lu"
#define YPTIMERTFORMATD "6" YPTIMERTFORMAT
#define YPTIMERSCALE 1000000LU
#define YPGETTIME yukgettimeofday()
typedef unsigned long yptimer;
typedef unsigned long ypcounter;

#elif YPTIMERTYPE == 2
#define YPTIMERTFORMAT "llu"
#define YPCOUNTERTFORMAT "lu"
#define YPTIMERTFORMATD "6" YPTIMERTFORMAT
#define YPTIMERSCALE 1000000LU
#define YPGETTIME yukgettimeofday()
typedef unsigned long long yptimer;
typedef unsigned long ypcounter;

#elif YPTIMERTYPE == 3
#define YPTIMERTFORMAT "llu"
#define YPCOUNTERTFORMAT "lu"
#define YPTIMERTFORMATD "9" YPTIMERTFORMAT
#define YPTIMERSCALE 1000000000LLU
#define YPGETTIME gethrtime()
typedef unsigned long long yptimer;
typedef unsigned long ypcounter;

#elif YPTIMERTYPE == 4
#define YPTIMERTFORMAT "lu"
#define YPCOUNTERTFORMAT "lu"
#define YPTIMERTFORMATD "6" YPTIMERTFORMAT
#define YPTIMERSCALE CLK_TCK
#define YPGETTIME yuktimes()
typedef unsigned long yptimer;
typedef unsigned long ypcounter;

#elif YPTIMERTYPE == 5
#define YPTIMERTFORMAT "llu"
#define YPCOUNTERTFORMAT "lu"
#define YPTIMERTFORMATD "9" YPTIMERTFORMAT
#define YPTIMERSCALE 1000000000LLU
#define YPGETTIME yukclock_gettime()
typedef unsigned long long yptimer;
typedef unsigned long ypcounter;

#else
#error Invalid YPTIMERTYPE in yuk.h

#endif /* YPTIMERTYPE cases */

extern yptimer ypexectime[];
extern ypcounter ypexecount[];
extern ypcounter ypabscount[];

typedef struct yukvar_tag
{
  int vartype;
  int extername;
  int intername;
} yukvar;

extern yukvar yukvars[];

/*@-exportlocal@*/ /* this needs to be global */
extern char** globalargv;
extern int globalargc;
/*@=exportlocal@*/

/* Give our own definition of sig_atomic_t for systems that don't have
   it. char ought to be atomic, on most systems (especially as we don't
   touch anything but the bottom byte). */
#if SIZEOF_SIG_ATOMIC_T + 0 == 0
#undef sig_atomic_t
typedef char sig_atomic_t;
#endif
