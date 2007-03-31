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
#include <signal.h>
#include "lose.h"

int coreonerr = 0; /* AIS */

int checkforbugs; /* AIS */

void lose(char *m, int n, char *s)
{
    (void) fflush(stdout); /* AIS: To stop stdout getting muddled with stderr*/
    (void) fprintf(stderr,
		   "ICL%c%c%cI\t",
		   m[0], m[1], m[2]);
    if (s)
	(void) fprintf(stderr, m + 4, s, n);
    else
	(void) fprintf(stderr, m + 4, n);
    (void) fprintf(stderr, "        CORRECT SOURCE AND RESUBNIT\n");
    if(atoi(m)==778&&coreonerr) /* AIS */
    {
      /* AIS: Dump core. */
      raise(SIGABRT);
    }
    exit(atoi(m));
}

/* AIS: This function reports potential bugs. It's paraphrased from lose. */
void lwarn(char *m, int n, char *s)
{
  if(!checkforbugs) return; /* Don't report a potential bug without -l */
  (void) fflush(stdout);
  (void) fprintf(stderr,
		 "ICL%c%c%cW\t",
		 m[0],m[1],m[2]);
  if (s)
    (void) fprintf(stderr, m + 4, s, n);
  else if(m[0]!='2'||m[1]!='7'||m[2]!='8')
    (void) fprintf(stderr, m + 4, n);
  else
    (void) fprintf(stderr, m + 4);
  (void) fprintf(stderr, "        RECONSIDER SOURCE AND RESUBNIT\n\n");
  /* Don't exit. This is not any error except one not causing immediate
     termination of program execution. */
}

/* lose.c ends here */
