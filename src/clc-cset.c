/*****************************************************************************

NAME
    clc-cset.c -- CLC-INTERCAL character set support for C-INTERCAL

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

/*
  The input is read from files such as latin1.bin and ebcdic.bin.
  These contain three lines of text data that specify the length of
  the character set (number of characters in each shift state), the
  number of shift states, and the bit order of the input. The bit
  order can either be 8 characters long (msb down to lsb) or 16 (the
  ick_first byte in a pair, followed by the second); each bit of the input
  is transferred to the corresponding bit in a binary table that
  follows (a for the lsb, b for the second least significant bit, up
  to l for the 12th least significant bit; at most 12 significant bits
  are allowed), or x for a don't care on the bit. The table that
  follows is the relevant character codes in Latin-1 (which is used as
  the interconversion language); if shift states are used, they're
  represented by character codes 1, 2, 3, etc.. Invalid characters are
  represented by character code 0, and changed to nulls on output. The
  order of the bytes is 'Latin-1 for this set's char 0 in shift state
  1', 'Latin-1 for this set's char 0 in shift state 2', ..., 'Latin-1
  for this set's char 1 in shift state 1', and so on.

  Note that as the character set feature is designed to mirror
  CLC-INTERCAL's, I've sprinkled a bit of idiomatic Perl throughout
  the code. This is quite rare, though, as it has to be also legal in C.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uncommon.h"
#define NCSETRECENT 8

/* Sometimes we want to link the character set files to the program
 * rather than reading them from disk; in this case, these extern
 * variables will be set non-null by object files invented
 * specifically for the purpose. */
extern /*@null@*/ const char* ick_clc_cset_atari;
extern /*@null@*/ const char* ick_clc_cset_baudot;
extern /*@null@*/ const char* ick_clc_cset_ebcdic;
extern /*@null@*/ const char* ick_clc_cset_latin1;

static /*@null@*/ const char* ick_clc_cset_ptr=0;

/* Fake that we're reading hardcoded characters from a file. This
 * method of doing it is obviously not thread-safe. */
static int ick_clc_cset_hardcoderead(FILE* ignored)
{
  /*@-noeffect@*/
  (void) ignored;
  /*@=noeffect@*/
  return (int)*ick_clc_cset_ptr++;
}

struct cset
{
  unsigned char set[4096]; /* allow up to 12 bits of data+shifts */
  unsigned short setlen;
  int shifts;
  char setname[9]; /* 8.3 filenames are enforced! */
  char bitorder[16];
  int nbytes;
};

/* In particular, this initialises the setnames to the null string,
 * and clears nbytes. Both of these are used to determine whether a
 * cset is valid or not. */
/*@-initallelements@*/ /*@-type@*/
static struct cset ick_cset_recent[NCSETRECENT]={{{0},0,0,{0},{0},0}};
/*@=initallelements@*/ /*@=type@*/
static int ick_csetow=0; /* which cset to overwrite ick_next */

/* For help finding files */
/*@observer@*/ extern char* ick_globalargv0;
/*@observer@*/ extern const char* ick_datadir;

/*@-mustfreefresh@*/
/* because Splint doesn't understand how findandfopen works */
static void ick_clc_cset_load(/*@unique@*/ struct cset* cs, /*@unique@*/ const char* fname)
{
  FILE* in;
  char buf[13]; /* enough for an 8.3 filename */
  int i,j,c;
  int (*ipf)(FILE*);
  /* Avoid buffer-overflow attacks. */
  if(strlen(fname)>8) return;
  /* If ick_clc_cset_atari is non-null, then don't read from disk. */
  if(ick_clc_cset_atari)
  {
    /* If the character sets have been hardcoded, only accept
     * hardcoded chararacter sets. */
    ick_clc_cset_ptr=0;
    if(!strcmp(fname,"atari")) ick_clc_cset_ptr=ick_clc_cset_atari;
    if(!strcmp(fname,"baudot")) ick_clc_cset_ptr=ick_clc_cset_baudot;
    if(!strcmp(fname,"ebcdic")) ick_clc_cset_ptr=ick_clc_cset_ebcdic;
    if(!strcmp(fname,"latin1")) ick_clc_cset_ptr=ick_clc_cset_latin1;
    if(!ick_clc_cset_ptr) return; /* not a hardcoded charset */
    in=(FILE*)0;
    ipf=ick_clc_cset_hardcoderead;
  }
  else
  {
    /* We already checked above that this isn't a buffer overflow. */
    /*@-bufferoverflowhigh@*/
    sprintf(buf,"%s.bin",fname);
    /*@=bufferoverflowhigh@*/
    if(!(in=ick_findandfopen(buf,ick_datadir,"rb",ick_globalargv0))) return;
    ipf=fgetc;
  }
  /* First row: setlen */
  cs->setlen=0;
  do
  {
    /* The input is definitely in ASCII, even if the C program isn't,
       which is why numeric codes are used. */
    /* Here, ipf allows NULL input iff in is actually NULL; this situation
       is impossible to explain with an annotation, so instead just disable
       the warning. */
    /*@-nullpass@*/
    c=ipf(in);
    /*@=nullpass@*/
    if(c==EOF) {if(in) (void)fclose(in); return;}
    if(c<48||c>57) break;
    cs->setlen*=10;
    cs->setlen+=c-48;
  } while(1);
  if(c!=10) {if(in) (void)fclose(in); return;}
  /* Second row: shifts. This can be from 1 to 9. */
  /*@-nullpass@*/
  c=ipf(in);
  /*@=nullpass@*/
  if(c<49||c>57) {if(in) (void)fclose(in); return;}
  cs->shifts=c-48;
  /*@-nullpass@*/
  if(ipf(in)!=10) {if(in) (void)fclose(in); return;}
  /*@=nullpass@*/
  /* Third row: byte order. */
  i=0;
  /*@-nullpass@*/
  while(((c=ipf(in)))>96&&i<16) cs->bitorder[i++]=(char)c;
  /*@=nullpass@*/
  /* Sanity check; that it is a whole number of bytes, that the input
   * format is correct, and that there are at most 4096 bytes of data
   * total. */
  if(c!=10||i%8||!i||cs->setlen*cs->shifts>4096) return;
  /* i/8 is now the number of bytes, but don't set that yet in case
   * there's an error later. */
  /* Rest of file: the bytes themselves. */
  j=0;
  /*@-nullpass@*/
  while(j<cs->setlen*cs->shifts)
    if((cs->set[j++]=(unsigned char)(c=ipf(in))),c==EOF && in != NULL)
    {if(in) (void)fclose(in); return;}
  /*@=nullpass@*/
  if(in) (void) fclose(in);
  /* Now set the name and number of bytes, indicating a successful
   * load. */
  cs->nbytes=i/8;
  strcpy(cs->setname,fname);
}
/*@=mustfreefresh@*/

/* Helper function for fixing bit order in output. */
static void ick_bitencout(char** pop, const struct cset* co,
		      unsigned short val, int padstyle)
{
  unsigned short outword=0;
  int i=co->nbytes*8;
  /*@-shiftnegative@*/ /* i can't go above it's initial value here */
  while(i--)
    if(co->bitorder[i]>'l')
    {
      if((padstyle==1&&(i==1||i==9) && !(outword&(1<<(co->nbytes*8-i)))) ||
	 (padstyle==2&&(rand()%2||!outword)))
	outword |= 1<<(co->nbytes*8-i-1);
    }
    /* Copy the appropriate bit from val to outword. */
    else outword |= (unsigned short)((val>>(co->bitorder[i]-'a'))&1)
	   << (co->nbytes*8-i-1);
  /*@=shiftnegative@*/
  if(co->nbytes==2) *(*pop)++=(char)(outword/256);
  *(*pop)++=(char)(outword%256);
}

/* padstyle is 0 to pad with zeros, 1 to pad to make the output
 * printable characters, or 2 to pad with garbage, avoiding 0s.
 * Return value is the number of characters in the output string,
 * which may contain embedded NULs if the input contained invalid
 * characters. Returns -1 on error. The caller is responsible for
 * making sure that out is big enough, but as a check, no more than
 * outsize-1 characters and a NUL will be written to out. The code is
 * conservative about this; to be safe, make outsize six times as long
 * as the in is (including in's terminal NUL), plus 6. */
int ick_clc_cset_convert(const char* in, /*@partial@*/ char* out, const char* incset,
			 const char* outcset, int padstyle, size_t outsize,
			 /*@null@*/ FILE* errsto)
{
  int ic=-1, oc=-1;
  int i;
  int ssi, sso;
  unsigned short tus, csi;
  const char* ip;
  char* op;
  struct cset *csri, *csro;
  int noconvwarn=0;
  int substwarn=0;
  /* First, see if we have a recently-used version of incset or outcset. */
  i=NCSETRECENT;
  while(i--)
  {
    (void)(strcmp(incset,ick_cset_recent[i].setname) || (ic=i));
    (void)(strcmp(outcset,ick_cset_recent[i].setname) || (oc=i));
  }
  /* Find a blank entry to load on top of. */
  if(ic==-1) for(i=NCSETRECENT;i--;) if(!ick_cset_recent[i].nbytes) ic=i;
  if(oc==-1) for(i=NCSETRECENT;i--;) if(!ick_cset_recent[i].nbytes&&i!=ic) oc=i;
  /* Failing that, find any entry to load on top of. */
  (void)(ic==-1 && (ick_cset_recent[ic=ick_csetow++].nbytes=0));
  if(ick_csetow==ic) ick_csetow++;
  ick_csetow%=NCSETRECENT;
  (void)(oc==-1 && (ick_cset_recent[oc=ick_csetow++].nbytes=0));
  ick_csetow%=NCSETRECENT;
  /* If the character set hasn't been loaded, load it now. */
  ick_cset_recent[ic].nbytes || (ick_clc_cset_load(ick_cset_recent+ic,incset),0);
  ick_cset_recent[oc].nbytes || (ick_clc_cset_load(ick_cset_recent+oc,outcset),0);
  csri=ick_cset_recent+ic;
  csro=ick_cset_recent+oc;
  /* If a character set failed to load, bail out. */
  if(!csri->nbytes)
  {
    if(errsto) fprintf(errsto,"Error: Nonexistent input character set.\n");
    return -1;
  }
  if(!csro->nbytes)
  {
    if(errsto) fprintf(errsto,"Error: Nonexistent output character set.\n");
    return -1;
  }
  /* There is no initial shift state. */
  ssi=sso=0;
  csri->shifts==1 && (ssi=1);
  csro->shifts==1 && (sso=1);
  ip=in; op=out;
  while(*ip != '\0' && (size_t)(op-out)<outsize-6)
  {
    tus=(unsigned short)(unsigned char)*ip++;
    if(csri->nbytes==2)
    {
      tus*=256;
      tus+=(unsigned short)(unsigned char)*ip++;
    }
    i=csri->nbytes*8;
    csi=0;
    while(i--)
    {
      if(csri->bitorder[i]>'l') continue;
      /* Copy the appropriate bit from tus to csi. */
      /*@-shiftnegative@*/
      csi |= (unsigned short)((tus>>(csri->nbytes*8-i-1))&1)
	<< (csri->bitorder[i]-'a');
      /*@=shiftnegative@*/
    }
    if(csi>csri->setlen)
    {
      ick_bitencout(&op,csro,0,padstyle); /* not in the charset */
      if(!noconvwarn && errsto != NULL)
	fprintf(errsto,"Warning: some characters could not be translated,"
		" they were replaced with NUL.\n");
      noconvwarn=1;
    }
    else
    {
      /* The more interesting case. */
      csi*=csri->shifts;
      if(!ssi)
      {
	/* We're at the start of a shift-stated string, but not
	   actually in any shift state. There is no general solution
	   here, so use one that works for Baudot: starting in each
	   state in turn, choose the option that takes the longest
	   until it ends up not changing shift state, then perform one
	   shift from that option. */
	int sstesting, ssbestsf, ssrecord, j, k;
	sstesting=csri->shifts+1; ssbestsf=ssrecord=0;
	while(--sstesting)
	{
	  k=sstesting; j=0;
	  while(csri->set[csi+i-1] != (unsigned char)0 &&
		(int)csri->set[csi+i-1]!=k &&
		(int)csri->set[csi+i-1]<=csri->shifts)
	  {k=(int)csri->set[csi+i-1]; j++;}
	  if(ssbestsf<j) {ssbestsf=sstesting; ssrecord=j;}
	}
	ssi=ssbestsf;
      }
      csi+=ssi-1;
      tus=(unsigned short)csri->set[csi]; /* we now have the Latin-1 conversion! */
      if(tus>=1&&tus<=(unsigned short)csri->shifts&&csri->shifts>1)
      {
	/* That wasn't a character, but a shift command. */
	ssi=(int)tus;
	continue;
      }
      /* Look for the character in the output's character
       * set. Preferably we want something in the current shift
       * state, but failing that, any character will do. */
    spacenowtab:
      i=csro->shifts*csro->setlen;
      csi=10000;
      while(i--)
	(void)((unsigned short)csro->set[i]==tus &&
	       (csi==10000 || (int)csi%csro->shifts!=sso-1) &&
	       (csi=(unsigned short)i));
      if(csi==10000&&tus==9 /* latin-1 tab */)
      {
	if(!substwarn && errsto != NULL)
	  fprintf(errsto,"Warning: no tab in output character set,"
		  " space was used instead.\n");
	substwarn=1;
	tus=32; /* latin-1 space */
	goto spacenowtab;
      }
      if(csi==10000)
      {
	ick_bitencout(&op,csro,0,padstyle); /* not in the charset */
	if(!noconvwarn && errsto != NULL)
	  fprintf(errsto,"Warning: some characters could not be translated,"
		  " they were replaced with NUL.\n");
	noconvwarn=1;
      }
      else if((int)(csi%csro->shifts)==(int)sso-1)
	/* in the right shift state already */
	ick_bitencout(&op,csro,(unsigned short)(csi/csro->shifts),padstyle);
      else
      {
	int tempi;
	/* Generate shift codes. If sso isn't 0, generate from where
	 * we are at the moment; if it is 0, generate worse-case
	 * shifts by assuming we're in a shift state that can't shift
	 * to the state we want directly, if possible. */
	if(!sso)
	{
	  int j=csro->shifts+1;
	  while(--j>0)
	  {
	    if(j-1==(int)(csi%csro->shifts)) continue;
	    i=(int)csro->setlen;
	    while(i--)
	      if((int)csro->set[i*csro->shifts+j-1]==csi%csro->shifts+1)
	      {j=-j; break; /* there is one in this set */};
	    j=-j;
	    if(j<0) break;
	  }
	  /* Pick the worst-case if we found one, or otherwise just
	   * any state we aren't in at the moment. */
	  sso=(j<0?-j:(int)(csi%csro->shifts));
	  if(!sso) sso=csro->shifts;
	}
	/* Look for the shift code, if there is one. */
	i=(int)csro->setlen;
	while(i--)
	  if((int)csro->set[i*csro->shifts+sso-1]==csi%csro->shifts+1) break;
	tempi=i*csro->shifts+sso-1;
	if(i==-1)
	{
	  int intershift=-1;
	  /* That didn't work. Look for the shift code in some shift
	   * state other than the one we're aiming for. */
	retry:
	  i=csro->setlen*csro->shifts;
	  while(i--)
	    if((int)csro->set[i]==csi%csro->shifts+1&&
	       i%csro->shifts!=(int)(csi%csro->shifts)&&
	       i%csro->shifts+1!=intershift) break;
	  if(i==-1) return -1; /* no way to get into the right state */
	  intershift=i%csro->shifts+1;
	  tempi=i;
	  i=(int)csro->setlen;
	  while(i--)
	    if((int)csro->set[i*csro->shifts+sso-1]==intershift) break;
	  if(i==-1) goto retry; /* try once more */
	  ick_bitencout(&op,csro,(unsigned short)i,padstyle);
	  /* sso=intershift here but we're going to overwrite it
	   * immediately anyway, so no point in the assignment */
	}
	ick_bitencout(&op,csro,(unsigned short)(tempi/csro->shifts),padstyle);
	ick_bitencout(&op,csro,(unsigned short)(csi/csro->shifts),padstyle);
	sso=csi%csro->shifts+1;
      }
    }
  }
  *op='\0';
  return op-out;
}
