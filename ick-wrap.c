$L
/* $A.c -- generated C-code file for INTERCAL program $A.i */

/* This code is explicitly *not* GPLed.  Use, abuse, and redistribute freely */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>

$M
#include "fiddle.h"
#include "abcess.h"
#include "lose.h"
$K

#define OK(d) !abstained[d]

extern int printflow;

int lineno;

jmp_buf cjb;
int ccfc;
int skipto;

int oldabstain;
int abstained[$B]$C;
$D
$E

int main(int argc, char *argv[])
{
    parseargs(argc,argv);

    skipto = 0;

    next = calloc(80, sizeof *next);
$N
    
#ifdef YUK
    yuklines = $J;
    yukcommands = $B;
#endif
    /* set seed for random error generation */
#ifdef USG
    srand48(time((long *)0) + getpid());
#else
    srand(time((long *)0));
#endif /* UNIX */

#if MULTITHREAD == 1
    ickmtinit();
#endif

    /* set up stash storage */
    stashinit();

    $F
      
      /* degenerated code */
 ick_restart: 
    ;
    
$G

#ifdef YUK
    if(yukloop) goto ick_restart; 
#endif
    lose(E633, $J, (char *)0);

$H
    return 0;
}

/* Generated code for $A.i ends here */
