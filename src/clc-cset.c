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
  first byte in a pair, followed by the second); each bit of the input
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
extern char* clc_cset_atari;
extern char* clc_cset_baudot;
extern char* clc_cset_ebcdic;
extern char* clc_cset_latin1;

static char* clc_cset_ptr=0;

/* Fake that we're reading hardcoded characters from a file. This
 * method of doing it is obviously not thread-safe. */
int clc_cset_hardcoderead(FILE* ignored)
{
  (void) ignored;
  return *clc_cset_ptr++;
}

struct cset
{
  unsigned char set[4096]; /* allow up to 12 bits of data+shifts */
  int setlen;
  int shifts;
  char setname[9]; /* 8.3 filenames are enforced! */
  char bitorder[16];
  int nbytes;
};

/* In particular, this initialises the setnames to the null string,
 * and clears nbytes. Both of these are used to determine whether a
 * cset is valid or not. */
static struct cset cset_recent[NCSETRECENT]={{{0},0,0,{0},{0},0}};
static int csetow=0; /* which cset to overwrite next */

/* For help finding files */
extern char* globalargv0;
extern char* datadir;

static void clc_cset_load(struct cset* cs, char* fname)
{
  FILE* in;
  char buf[13]; /* enough for an 8.3 filename */
  int i,j,c;
  int (*ipf)(FILE*);
  /* Avoid buffer-overflow attacks. */
  if(strlen(fname)>8) return;
  /* If clc_cset_atari is non-null, then don't read from disk. */
  if(clc_cset_atari)
  {
    /* If the character sets have been hardcoded, only accept
     * hardcoded chararacter sets. */
    clc_cset_ptr=0;
    if(!strcmp(fname,"atari")) clc_cset_ptr=clc_cset_atari;
    if(!strcmp(fname,"baudot")) clc_cset_ptr=clc_cset_baudot;
    if(!strcmp(fname,"ebcdic")) clc_cset_ptr=clc_cset_ebcdic;
    if(!strcmp(fname,"latin1")) clc_cset_ptr=clc_cset_latin1;
    if(!clc_cset_ptr) return; /* not a hardcoded charset */
    in=0;
    ipf=clc_cset_hardcoderead;
  }
  else
  {
    sprintf(buf,"%s.bin",fname);
    if(!(in=findandfopen(buf,datadir,"rb",globalargv0))) return;
    ipf=fgetc;
  }
  /* First row: setlen */
  cs->setlen=0;
  for(;;)
  {
    /* The input is definitely in ASCII, even if the C program isn't,
       which is why numeric codes are used. */
    c=ipf(in);
    if(c==EOF) {if(in) fclose(in); return;} /* error or EOF */
    if(c<48||c>57) break;
    cs->setlen*=10;
    cs->setlen+=c-48;
  }
  if(c!=10) {if(in) fclose(in); return;}
  /* Second row: shifts. This can be from 1 to 9. */
  c=ipf(in);
  if(c<49||c>57) {if(in) fclose(in); return;}
  cs->shifts=c-48;
  if(ipf(in)!=10) {if(in) fclose(in); return;}
  /* Third row: byte order. */
  i=0;
  while(((c=ipf(in)))>96&&i<16) cs->bitorder[i++]=c;
  /* Sanity check; that it is a whole number of bytes, that the input
   * format is correct, and that there are at most 4096 bytes of data
   * total. */
  if(c!=10||i%8||!i||cs->setlen*cs->shifts>4096) return;
  /* i/8 is now the number of bytes, but don't set that yet in case
   * there's an error later. */
  /* Rest of file: the bytes themselves. */
  j=0;
  while(j<cs->setlen*cs->shifts)
    if((cs->set[j++]=c=ipf(in)),c==EOF&&in) {if(in) fclose(in); return;}
  if(in) fclose(in);
  /* Now set the name and number of bytes, indicating a successful
   * load. */
  cs->nbytes=i/8;
  strcpy(cs->setname,fname);
}

/* Helper function for fixing bit order in output. */
static void bitencout(char** pop, struct cset* co,
		      unsigned short val, int padstyle)
{
  unsigned short outword=0;
  int i=co->nbytes*8;
  while(i--)
    /* 108 is 'l' in Latin-1 */
    if(co->bitorder[i]>108)
    {
      if((padstyle==1&&(i==1||i==9)&&!(outword&(1<<(co->nbytes*8-i))))||
	 (padstyle==2&&(rand()>RAND_MAX/2||!outword)))
	outword |= 1<<(co->nbytes*8-i-1);
    }
    /* Copy the appropriate bit from val to outword. */
    else outword |= ((val>>(co->bitorder[i]-97))&1)<<(co->nbytes*8-i-1);
  if(co->nbytes==2) *(*pop)++=outword/256;
  *(*pop)++=outword%256;
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
int clc_cset_convert(char* in, char* out, char* incset, char* outcset,
		     int padstyle, int outsize, FILE* errsto)
{
  int ic=-1, oc=-1;
  int i;
  int ssi, sso;
  unsigned short tus, csi;
  char* ip, *op;
  struct cset *csri, *csro;
  int noconvwarn=0;
  int substwarn=0;
  /* First, see if we have a recently-used version of incset or outcset. */
  i=NCSETRECENT;
  while(i--)
  {
    (void)(strcmp(incset,cset_recent[i].setname) || (ic=i));
    (void)(strcmp(outcset,cset_recent[i].setname) || (oc=i));
  }
  /* Find a blank entry to load on top of. */
  if(ic==-1) for(i=NCSETRECENT;i--;) if(!cset_recent[i].nbytes) ic=i;
  if(oc==-1) for(i=NCSETRECENT;i--;) if(!cset_recent[i].nbytes&&i!=ic) oc=i;
  /* Failing that, find any entry to load on top of. */
  (void)(ic==-1 && (cset_recent[ic=csetow++].nbytes=0));
  if(csetow==ic) csetow++;
  csetow%=NCSETRECENT;
  (void)(oc==-1 && (cset_recent[oc=csetow++].nbytes=0));
  csetow%=NCSETRECENT;
  /* If the character set hasn't been loaded, load it now. */
  cset_recent[ic].nbytes || (clc_cset_load(cset_recent+ic,incset),0);
  cset_recent[oc].nbytes || (clc_cset_load(cset_recent+oc,outcset),0);
  csri=cset_recent+ic;
  csro=cset_recent+oc;
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
  while(*ip&&(op-out)<outsize-6)
  {
    tus=(unsigned char)*ip++;
    if(csri->nbytes==2)
    {
      tus*=256;
      tus+=(unsigned char)*ip++;
    }
    i=csri->nbytes*8;
    csi=0;
    while(i--)
    {
      /* 108 is 'l' in Latin-1 */
      if(csri->bitorder[i]>108) continue;
      /* Copy the appropriate bit from tus to csi. */
      csi |= ((tus>>(csri->nbytes*8-i-1))&1)<<(csri->bitorder[i]-97);
    }
    if(csi>csri->setlen)
    {
      bitencout(&op,csro,0,padstyle); /* not in the charset */
      if(!noconvwarn&&errsto)
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
	int sstesting, ssbestsf, ssrecord, j;
	sstesting=csri->shifts+1; ssbestsf=ssrecord=0;
	while(--sstesting)
	{
	  i=sstesting; j=0;
	  while(csri->set[csi+i-1]&&
		csri->set[csi+i-1]!=i&&
		csri->set[csi+i-1]<=csri->shifts)
	  {i=csri->set[csi+i-1]; j++;}
	  if(ssbestsf<j) {ssbestsf=sstesting; ssrecord=j;}
	}
	ssi=ssbestsf;
      }
      csi+=ssi-1;
      tus=csri->set[csi]; /* we now have the Latin-1 conversion! */
      if(tus>=1&&tus<=csri->shifts&&csri->shifts>1)
      {
	/* That wasn't a character, but a shift command. */
	ssi=tus;
	continue;
      }
      /* Look for the character in the output's character
       * set. Preferably we want something in the current shift
       * state, but failing that, any character will do. */
    spacenowtab:
      i=csro->shifts*csro->setlen;
      csi=10000;
      while(i--)
	(void)(csro->set[i]==tus && (csi==10000||csi%csro->shifts!=sso-1)
	       && (csi=i));
      if(csi==10000&&tus==9 /* latin-1 tab */)
      {
	if(!substwarn&&errsto)
	  fprintf(errsto,"Warning: no tab in output character set,"
		  " space was used instead.\n");
	substwarn=1;
	tus=32; /* latin-1 space */
	goto spacenowtab;
      }
      if(csi==10000)
      {
	bitencout(&op,csro,0,padstyle); /* not in the charset */
	if(!noconvwarn&&errsto)
	  fprintf(errsto,"Warning: some characters could not be translated,"
		  " they were replaced with NUL.\n");
	noconvwarn=1;
      }
      else if(csi%csro->shifts==sso-1)
	/* in the right shift state already */
	bitencout(&op,csro,csi/csro->shifts,padstyle);
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
	    if(j-1==csi%csro->shifts) continue;
	    i=csro->setlen;
	    while(i--)
	      if(csro->set[i*csro->shifts+j-1]==csi%csro->shifts+1)
	      {j=-j; break; /* there is one in this set */};
	    j=-j;
	    if(j<0) break;
	  }
	  /* Pick the worst-case if we found one, or otherwise just
	   * any state we aren't in at the moment. */
	  sso=(j<0?-j:csi%csro->shifts);
	  if(!sso) sso=csro->shifts;
	}
	/* Look for the shift code, if there is one. */
	i=csro->setlen;
	while(i--)
	  if(csro->set[i*csro->shifts+sso-1]==csi%csro->shifts+1) break;
	tempi=i*csro->shifts+sso-1;
	if(i==-1)
	{
	  int intershift=-1;
	  /* That didn't work. Look for the shift code in some shift
	   * state other than the one we're aiming for. */
	retry:
	  i=csro->setlen*csro->shifts;
	  while(i--)
	    if(csro->set[i]==csi%csro->shifts+1&&
	       i%csro->shifts!=csi%csro->shifts&&
	       i%csro->shifts+1!=intershift) break;
	  if(i==-1) return -1; /* no way to get into the right state */
	  intershift=i%csro->shifts+1;
	  tempi=i;
	  i=csro->setlen;
	  while(i--)
	    if(csro->set[i*csro->shifts+sso-1]==intershift) break;
	  if(i==-1) goto retry; /* try once more */
	  bitencout(&op,csro,i,padstyle);
	  /* sso=intershift here but we're going to overwrite it
	   * immediately anyway, so no point in the assignment */
	}
	bitencout(&op,csro,tempi/csro->shifts,padstyle);
	bitencout(&op,csro,csi/csro->shifts,padstyle);
	sso=csi%csro->shifts+1;
      }
    }
  }
  *op=0;
  return op-out;
}
