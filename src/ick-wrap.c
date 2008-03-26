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
#include "ick_lose.h"
$K

#ifdef ICK_EC
#include "ick_ec.h"
void ick_main(void);
#endif

#define ICKABSTAINED(d) ick_abstained[d]
#define ICKSTASH(a,b,c,d) ick_stash(a, b, c+b, d)
#define ICKRETRIEVE(a,b,c,d,e) ick_retrieve(a+b, c, b, d[b], e)
#define ICKIGNORE(a,b,c) a[b]

extern int ick_printflow;

int ick_lineno;

jmp_buf ick_cjb;
int ick_ccfc;
unsigned ick_skipto=0;
$O
char* ick_globalargv0;
int ick_oldabstain;
int ick_abstained[$B]$C;
$D
$E
$P
int main(int argc, char *argv[])
{
#ifndef YUK
    ick_parseargs(argc,argv);
#endif

    ick_skipto = 0;

    ick_next = calloc(80, sizeof *ick_next);
#ifdef ICK_EC
    ick_next_jmpbufs = malloc(80 * sizeof *ick_next_jmpbufs);
#endif
$N
    ick_globalargv0=argv[0];
#ifdef YUK
    yuklines = $J;
    yukcommands = $B;
    globalargv = argv;
    globalargc = argc;
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
    ick_stashinit();

    $F

      /* degenerated code */
#ifdef ICK_EC
    ick_main();
}

ICK_EC_FUNC_START(ick_main)
{
#endif
 ick_restart:
 top:
    switch(ick_skipto)
    {
    case 0:
      $G
	}

#ifdef YUK
    if(yukloop) goto ick_restart;
#endif
    ick_lose(IE633, $J, (char *)0);

    $H
#ifndef ICK_EC
    return 0;
#else
    return;
#endif
}
#ifdef ICK_EC
ICK_EC_FUNC_END
#endif
$Q
/* Generated code for $A.i ends here */
