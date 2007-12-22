/*****************************************************************************

NAME
    convickt.c -- translate between various INTERCAL formats

LICENSE TERMS
    Copyright (C) 2007 Alex Smith

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

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int clc_cset_convert(char* in, char* out, char* incset, char* outcset,
		     int padstyle, int outsize, FILE* errsto);

/* For communication with clc_cset.c */
char* globalargv0;
char* datadir;
/* We want to read the latest versions of the character sets from disk. */
char* clc_cset_atari=0;
char* clc_cset_baudot=0;
char* clc_cset_ebcdic=0;
char* clc_cset_latin1=0;

#ifndef ICKDATADIR
#define ICKDATADIR "/usr/local/share/ick-0.27"
#endif

int main(int argc, char** argv)
{
  int allocsize=100;
  char* in;
  char* out;
  int i=0;
  int c;
  int padding=-1;
  globalargv0=argv[0];
  srand(time(NULL));
  if(argc!=3&&argc!=4)
  {
    fprintf(stderr,"Usage: convickt informat outformat [padding]\n");
    fprintf(stderr,"Available formats are:\n");
    fprintf(stderr,"\tPrinceton: latin1, baudot, ebcdic\n");
    fprintf(stderr,"\tAtari: atari (7-bit ASCII)\n");
    fprintf(stderr,"Possible values for padding are:\n");
    fprintf(stderr,"\tprintable: try to leave the output in char range "
	    "32-126\n");
    fprintf(stderr,"\tzero: leave the output with 0s in the high bits\n");
    fprintf(stderr,"\trandom: pad with random data\n");
    fprintf(stderr,"Padding only has an effect on character sets with less\n");
    fprintf(stderr,"than 8 bits used per bytes; default is 'printable'.\n");
    return 0;
  }
  if(argc==4&&!strcmp(argv[3],"printable")) padding=1;
  if(argc==4&&!strcmp(argv[3],"zero")) padding=0;
  if(argc==4&&!strcmp(argv[3],"random")) padding=2;
  if(argc==4&&padding==-1)
  {
    fprintf(stderr,"Error: padding value not recognized.\n");
    return 1;
  }
  if(!(datadir=getenv("ICKDATADIR")))
    datadir=ICKDATADIR;
  in=malloc(allocsize);
  if(!in) {perror("Error: Memory allocation failure"); return 0;}
  while((c=getchar())!=EOF)
  {
    in[i++]=c;
    if(i+1>=allocsize)
    {
      char* temp=in;
      allocsize*=2;
      in=realloc(in,allocsize);
      if(!in)
      {
	perror("Error: Memory allocation failure");
	free(temp);
	return 0;
      }
    }
  }
  in[i]=0;
  out=malloc(allocsize*6+6);
  if(!out)
  {
    perror("Error: Memory allocation failure");
    free(in);
    return 0;
  }
  i=clc_cset_convert(in,out,argv[1],argv[2],padding,allocsize*6+6,stderr);
  if(i>=0) fwrite(out,1,i,stdout);
  free(in);
  free(out);
  return i<0;
}
