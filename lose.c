/*
 * lose.c -- report INTERCAL compile- or run-time error
 */
/*LINTLIBRARY*/
#include <stdio.h>

void lose(n, m)
int	n;
char	*m;
{
    (void) fprintf(stderr,
		   "ICL%c%c%cI %s\n        ON THE WAY TO STATEMENT %04d\n",
		   m[0], m[1], m[2], m+4, n);
    (void) fprintf(stderr, "        CORRECT SOURCE AND RESUBMIT\n");
    exit(atoi(m));
}

/* lose.c ends here */


