/*
 * lose.c -- report INTERCAL compile- or run-time error
 */
/*LINTLIBRARY*/
#include <stdio.h>

void lose(m, n, s)
char	*m;
int	n;
char	*s;
{
    (void) fprintf(stderr,
		   "ICL%c%c%cI\t",
		   m[0], m[1], m[2]);
    if (s)
	(void) fprintf(stderr, m + 4, s, n);
    else
	(void) fprintf(stderr, m + 4, n);
    (void) fprintf(stderr, "        CORRECT SOURCE AND RESUBNIT\n");
    exit(atoi(m));
}

/* lose.c ends here */


