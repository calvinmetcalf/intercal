/* $A.c -- generated C-code file for INTERCAL program $A.i */

#include <cesspool.h>
#include <lose.h>

extern unsigned int mingle(), iselect();
extern unsigned int and16(), or16(), xor16(), and32(), or32(), xor32();
extern unsigned int whirl16(), whirl32(), fin16(), fin32();

int lineno;

static int abstained[$B]$C;
$D
$E

main(argc,argv)
int argc;
char **argv;
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
    lose(E633, $B+1, (char *)0);

    /* generated switch for resumes */
 top:
    switch(skipto)
    {
    $H
    }
}

/* Generated code for $A.i ends here */
