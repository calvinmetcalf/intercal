/* $A.c -- generated C-code file for INTERCAL program $A.i */

/* This code is explicitly *not* GPLed.  Use, abuse, and redistribute freely */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fiddle.h>
#include <abcess.h>
#include <lose.h>

int lineno;

static int abstained[$B]$C;
$D
$E

int main(int argc, char *argv[])
{
    int skipto = 0;

    parseargs(argc,argv);

    /* set seed for random error generation */
#ifdef USG
    srand48(time((long *)0) + getpid());
#else
    srand(time((long *)0));
#endif /* UNIX */

    /* set up stash storage */
    stashinit();

    $F
    /* degenerated code */
$G
    lose(E633, $J, (char *)0);

$H
    return 0;
}

/* Generated code for $A.i ends here */
