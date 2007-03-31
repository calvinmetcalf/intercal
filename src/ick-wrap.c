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

#define ICKABSTAINED(d) abstained[d]
#define ICKSTASH(a,b,c,d) stash(a, b, c+b, d)
#define ICKRETRIEVE(a,b,c,d,e) retrieve(a+b, c, b, d[b], e)
#define ICKIGNORE(a,b,c) a[b]

extern int printflow;

int lineno;

jmp_buf cjb;
int ccfc;
unsigned skipto=0;
$O

int oldabstain;
int abstained[$B]$C;
$D
$E
$P
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
    srand48(time(0) + getpid());
#else
    srand(time(0));
#endif /* UNIX */

#if MULTITHREAD == 1
    ickmtinit();
#endif

    /* set up stash storage */
    stashinit();

    $F
      
      /* degenerated code */
 ick_restart: 
 top:
    switch(skipto)
    {
    case 0:
      $G
	}
    
#ifdef YUK
    if(yukloop) goto ick_restart; 
#endif
    lose(E633, $J, (char *)0);

    $H
    
    return 0;
}
$Q
/* Generated code for $A.i ends here */
