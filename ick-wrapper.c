/* $A.c -- generated C-code file for INTERCAL program $A.i */

#include <cesspool.h>

extern unsigned int interleave(), select();
extern unsigned int and16(), or16(), xor16(), and32(), or32(), xor32();

int lineno;

static int abstained[$B]$C;
$D

main()
{
    /* declare and initialize gerund variables */
    int	calculating = TRUE;
    int	nexting = TRUE;
    int	forgetting = TRUE;
    int	resuming = TRUE;
    int	stashing = TRUE;
    int	retrieving = TRUE;
    int	ignoring = TRUE;
    int	remembering = TRUE;
    int	abstaining = TRUE;
    int	reinstating = TRUE;
    int	readingout = TRUE;
    int	writingin = TRUE;
    int comingfrom = TRUE;

    int skipto = 0;

    /* set seed for random error generation */
#ifdef USG
    srand48(time((long *)0) + getpid());
#else
    srand(time((long *)0));
#endif /* UNIX */

    /* set up stash storage */
    stashinit();

    $E

top:
    /* degenerated code */
$F
    lose($B+1,"633 Execution has passed beyond the last statment of the program.");
}

