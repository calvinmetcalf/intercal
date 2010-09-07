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

extern int ick_clc_cset_convert(const char* in, /*@partial@*/ char* out, const char* incset,
				const char* outcset, int padstyle, size_t outsize,
				/*@null@*/ FILE* errsto);

/* For communication with clc_cset.c */
/*@observer@*/ char* ick_globalargv0;
/*@-redef@*/ /* it's never the case that both definitions are used at once */
/*@observer@*/ /*@dependent@*/ const char* ick_datadir;
/*@=redef@*/
/* We want to read the latest versions of the character sets from disk. */
/*@null@*/ const char* ick_clc_cset_atari=0;
/*@null@*/ const char* ick_clc_cset_baudot=0;
/*@null@*/ const char* ick_clc_cset_ebcdic=0;
/*@null@*/ const char* ick_clc_cset_latin1=0;

/* In case we don't have vsnprintf. */
void /*@noreturn@*/ ick_lose(const char *m, int n, /*@null@*/ const char *s)
{
  (void) m;
  (void) n;
  (void) s;
  abort();
}

/*@-redef@*/ /* because only one main is used at a time */
int main(int argc, char** argv)
/*@=redef@*/
{
  size_t allocsize=100;
  char* in;
  char* out;
  size_t i=0;
  int ti;
  int c;
  int padding=-1;
  ick_globalargv0=argv[0];
  srand((unsigned)time(NULL));
  if(argc<3||argc>5)
  {
    fprintf(stderr,"Usage: convickt informat outformat [padding [arrayname]]\n");
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
    fprintf(stderr,"If arrayname is given, which must be a legal name for\n");
    fprintf(stderr,"a tail array, the output will instead be a portable\n");
    fprintf(stderr,"INTERCAL program that stores the required byte pattern\n");
    fprintf(stderr,"in that array.\n");
    return 0;
  }
  if(argc>=4&&!strcmp(argv[3],"printable")) padding=1;
  if(argc>=4&&!strcmp(argv[3],"zero")) padding=0;
  if(argc>=4&&!strcmp(argv[3],"random")) padding=2;
  if(argc>=4&&padding==-1)
  {
    fprintf(stderr,"Error: padding value not recognized.\n");
    return 1;
  }
  if(!(ick_datadir=getenv("ICKDATADIR")))
    ick_datadir=ICKDATADIR;
  in=malloc(allocsize);
  if(!in) {perror("Error: Memory allocation failure"); return 0;}
  while((c=getchar())!=EOF)
  {
    in[i++]=(char)c;
    if(i+1>=allocsize)
    {
      char* temp=in;
      allocsize*=2;
      in=realloc(in,allocsize);
      if(!in)
      {
	perror("Error: Memory allocation failure");
	/* Annotation, because in hasn't been freed here; that's what the
	   error return from realloc means. */
	/*@-usereleased@*/
	if(temp) free(temp);
	/*@=usereleased@*/
	return 0;
      }
    }
  }
  in[i]='\0';
  out=malloc(allocsize*6+6);
  if(!out)
  {
    perror("Error: Memory allocation failure");
    free(in);
    return 0;
  }
  ti=ick_clc_cset_convert(in,out,argv[1],argv[2],padding,allocsize*6+6,stderr);
  if(ti>=0&&argc<5) (void) fwrite(out,1,(size_t)ti,stdout);
  else if(ti>=0&&argc==5)
  {
    int pleasedelay=2;
    printf("\tDO %s <- #%d\n",argv[4],ti);
    while(ti--)
    {
      printf("\t");
      if(!--pleasedelay) {printf("PLEASE "); pleasedelay=4;}
      printf("DO %s SUB #%d <- #%d\n",argv[4],ti+1,(int)(out[ti]));
    }
  }
  free(in);
  free(out);
  return ti<0;
}
