/*
 *
 * NAME
 *    lose.c -- report INTERCAL compile- or run-time error
 *
LICENSE TERMS
    Copyright (C) 1996 Eric S. Raymond 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*LINTLIBRARY*/
#include <stdio.h>
#include <stdlib.h>
#include "lose.h"

void lose(char *m, int n, char *s)
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


