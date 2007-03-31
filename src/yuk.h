/* yuk.h - header file for yuk.c and debugging versions of ick-wrap.c */
/* Copyright (C) Alex Smith 2006. See yuk.c for copyright conditions.
   This file is GPL'd, and so can be freely redistributed but has
   absolutely no warranty. */

#include <time.h>
#define YUK(t,u) yukline(t,u)
#define YUKTERM yukterm()

#define YUKEND 5

extern void yukline(int,int);
extern void yukterm(void);

extern int abstained[];
extern int yukopts;
extern int lineno;
extern int yuklines;
extern int yukcommands;
extern char *textlines[];
extern int lineofaboff[];
extern char *yukexplain[];

extern int yukloop;

/* Defines that change the functions used for profiling */
#define YPTIMERTYPE 2
/* 0 to use clock(), 1 to use gettimeofday() and long,
   2 to use gettimeofday() and long long, 3 to use gethrtime()
   4 to use times() */
/* Note: On many systems, 0's resolution is too low to produce any output
	 1 and 2 produce the same output; use 2 if your system can handle
	 long long because the overflow is dealt with more simply
         3 is a system-specific function. If there are more system-specific
         functions around that return more accurate times than the others
         used here, it would improve the profiler on those systems. */

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
