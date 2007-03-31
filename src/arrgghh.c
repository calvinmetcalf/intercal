/* 
 *	SYNOPSIS:	parseargs(argc,argv)
 *	FILE :		parseargs.c
 *	AUTHOR :	Steve Swales
 *	DATE:		October 7, 1990
 *	PURPOSE: 	Parse arguments for INTERCAL programs.
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <setjmp.h>
#include "abcess.h"
#include "lose.h"

int traditional = 0;
int wimp_mode = 0;
int printflow = 0; /* AIS */
int mystery = 0; /* AIS */

unsigned long mysteryc = 0; /* AIS */

void
parseargs(int argc, char **argv)
{
    register int i, j;
    static int helpflag = -1;
    static char *flags[] = {
	"help",
	"wimpmode",
	"traditional",
	"printflow", /* AIS */
	"mystery", /* AIS */
      };
    static int *bools[] = {
	&helpflag,
	&wimp_mode,
	&traditional,
	&printflow, /* AIS */
	&mystery, /* AIS */
      };
    static int nflags = (int)(sizeof(flags)/sizeof(flags[0]));

    for(i = 1;i < argc;i++) {
	if(argv[i][0] != '+' && argv[i][0] != '-') {
	    break;
	}
	for(j = 0; j < nflags;j++) {
	    if(0 == strcmp(argv[i]+1,flags[j])) {
		*(bools[j]) = (argv[i][0] == '+');
		break;
	    }
	}
	if(j == nflags) {
	    fprintf(stderr,"%s: can't grok %s\n",argv[0],argv[i]);
	    helpflag = 1;
	}
    }
    if(helpflag != -1) {
	if(!helpflag) {
	    fprintf(stderr,
		    "Once you start messing with INTERCAL... \n");
	    fprintf(stderr,"\t\tthere is no help for you!\n\n");
	    sleep(3);
	}
	fprintf(stderr,"Current flags (and current state) are:\n");
	for(i = 0;i < nflags;i++) {
	    fprintf(stderr,"\t[+/-]%-20.20s\t(%s)\n",flags[i],
		    (*(bools[i]) == 1)?"ON":((*(bools[i]) == 0)?"OFF":"???"));
	}
	fprintf(stderr,"All flags must be preceded by either + or -, which\n");
	fprintf(stderr," usually will mean turn on or turn off something,\n");
	fprintf(stderr," but not always, and not all of them currently\n");
	fprintf(stderr," do anything, but you can switch them on or off\n");
	fprintf(stderr," if you like anyway. Isn't this helpful?\n\n");
	fflush(stderr);
	sleep(3);
	lose(E778, 0, (char *)NULL);
    }
    if(wimp_mode) {
	fprintf(stderr,"How sad... you have selected to run an INTERCAL\n");
	fprintf(stderr,"program in WIMP MODE.\n\n");
	fprintf(stderr,"This means that:\n");
	fprintf(stderr," A) Rather than the nifty input like:\n");
	fprintf(stderr,"\tONE NINER SEVEN TWO OH SIX THREE,\n");
	fprintf(stderr,"    and even niftier output like:\n");
	fprintf(stderr,"\t______\n");
	fprintf(stderr,"\tMCMLXXMMLXIII,\n");
	fprintf(stderr,"    you will have to settle for plain old number\n");
	fprintf(stderr,"    representations like 1972063; and,\n");
	fprintf(stderr," B) You are a WIMP!\n\n");
	fflush(stderr);
    }
}
