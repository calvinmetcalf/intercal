/*
 * uncommon.c -- functions used by ick, convickt, yuk and compiled programs
 *
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
 */

#include <stdio.h>
#include <string.h>
#include "uncommon.h"

/* Options that might affect this */
int ick_printfopens=0;

/*@dependent@*/ /*@null@*/ FILE* ick_debfopen(/*@observer@*/ char* fname,
					      /*@observer@*/ char* mode)
{
  FILE* t;
  if(ick_printfopens) fprintf(stderr,"Trying to open '%s'...\n",fname);
  t=fopen(fname,mode);
  if(ick_printfopens && t != NULL) fprintf(stderr,"Success!\n");
  if(ick_printfopens && t == NULL) fprintf(stderr,"Failed!\n");
  return t;
}

/* This function looks for the skeleton and syslib, searching first the
   path they should be in, then the current directory, then argv[0]'s
   directory (if one was given). This function avoids possible buffer
   overflows, instead truncating filenames (and if that manages to find them,
   I'll be incredibly surprised). It also tries argv[0]/../lib and
   argv[0]/../include (where they are when running without installing). */
/*@dependent@*/ /*@null@*/ FILE* ick_findandfopen(/*@observer@*/ char* file,
						  /*@observer@*/ char* guessdir,
						  /*@observer@*/ char* mode,
						  /*@observer@*/ char* argv0)
{
  static char buf2[BUFSIZ];
  /*@observer@*/ static char *fileiter;
  size_t i = 0, j;
  FILE* ret;
  while(*guessdir != '\0' && i<BUFSIZ-2) buf2[i++] = *guessdir++;
  buf2[i++] = '/';
  fileiter = file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = '\0';
  ret = ick_debfopen(buf2,mode); /* where it ought to be */
  if(ret) return ret;
  ret = ick_debfopen(file,mode); /* current dir */
  if(ret) return ret;
  if(!strchr(argv0,'/')&&
     !strchr(argv0,'\\')) return NULL; /* argv[0] has no dir specified */
  i = j = 0;
  while(*argv0 != '\0' && i<BUFSIZ-2)
  {
    buf2[i++] = *argv0++;
    if(*argv0=='/') j = i;
    if(*argv0=='\\') j = i;
  }
  i = j + 1;
  fileiter=file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = '\0';
  ret = ick_debfopen(buf2,mode); /* argv[0]'s dir */
  if(ret) return ret;
  i = j + 1;
  fileiter="../lib/"; /* correct for POSIX and DJGPP */
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]='\0';
  ret = ick_debfopen(buf2,mode); /* argv[0]/../lib/ */
  if(ret) return ret;
  i = j + 1;
  fileiter="../include/"; /* correct for POSIX and DJGPP */
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]='\0';
  ret = ick_debfopen(buf2,mode); /* argv[0]/../include/ */
  if(ret) return ret;
  return NULL; /* just return 0 if even this failed */
}

/* AIS: The same, looking for an executable */
/*@observer@*/ /*@null@*/ char* ick_findandtestopen(/*@observer@*/ char* file,
						    /*@observer@*/ char* guessdir,
						    /*@observer@*/ char* mode,
						    /*@observer@*/ char* argv0)
{
  static char buf2[BUFSIZ];
  /*@observer@*/ static char *fileiter;
  size_t i = 0, j;
  FILE* ret;
  while(*guessdir != '\0' && i<BUFSIZ-2) buf2[i++] = *guessdir++;
  buf2[i++] = '/';
  fileiter = file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = '\0';
  ret = ick_debfopen(buf2,mode); /* where it ought to be */
  if(ret) {(void) fclose(ret); return buf2;}
  ret = ick_debfopen(file,mode); /* current dir */
  if(ret) {(void) fclose(ret); return file;}
  if(!strchr(argv0,'/')&&
     !strchr(argv0,'\\')) return 0; /* argv[0] has no dir specified */
  i = j = 0;
  while(*argv0 != '\0' && i<BUFSIZ-2)
  {
    buf2[i++] = *argv0++;
    if(*argv0=='/') j = i;
    if(*argv0=='\\') j = i;
  }
  i = j + 1;
  fileiter=file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i] = '\0';
  ret = ick_debfopen(buf2,mode); /* argv[0]'s dir */
  if(ret) {(void) fclose(ret); return buf2;}
  i = j + 1;
  fileiter="../lib/"; /* correct for POSIX and DJGPP */
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]='\0';
  ret = ick_debfopen(buf2,mode); /* argv[0]/../lib/ */
  if(ret) {(void) fclose(ret); return buf2;}
    i = j + 1;
  fileiter="../include/"; /* correct for POSIX and DJGPP */
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  fileiter=file;
  while(*fileiter != '\0' && i<BUFSIZ-1) buf2[i++] = *fileiter++;
  buf2[i]='\0';
  ret = ick_debfopen(buf2,mode); /* argv[0]/../include/ */
  if(ret) {(void) fclose(ret); return buf2;}
  return 0; /* just return 0 if even this failed */
}

/* AIS: The same thing, but with freopen */
/*@dependent@*/ /*@null@*/ FILE* ick_findandfreopen(/*@observer@*/ char* file,
						    /*@observer@*/ char* guessdir,
						    /*@observer@*/ char* mode,
						    /*@observer@*/ char* argv0,
						    FILE* over)
{
  char* s=ick_findandtestopen(file,guessdir,mode,argv0);
  if(s != NULL)
    return freopen(s,mode,over);
  else
    return NULL;
}
