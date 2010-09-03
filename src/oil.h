/* Header file for compiled OIL programs
   Does name mangling, defines macros
   Copyright (C) Alex Smith 2008
   See oil.y for copyright conditions. */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "sizes.h"
#include "ick.h"
#include "parser.h"
#include "fiddle.h"
#include "ick_lose.h"
#include "feh.h"

#define OPTING(x) \
  if(optdebug == 2)			  \
  {					  \
    explexpr(optdebugnode,stderr);	  \
    putc('\n',stderr);			  \
  }					  \
  if(optdebug == 3)			  \
  {					  \
    prexpr(optdebugnode,stderr,0);	  \
    putc('\n',stderr);			  \
  }					  \
  if(optdebug) fprintf(stderr,"[%s]",#x); \
  if(optdebug >= 2) putc('\n',stderr);	  \
  opted = 1;
#define MAYBENEWNODE(n) if(!(n)) (n)=newnode();

#define Base ick_Base
#define Large_digits ick_Large_digits
#define Max_large ick_Max_large
#define Max_small ick_Max_small
#define Small_digits ick_Small_digits
#define and16 ick_and16
#define and32 ick_and32
#define aref ick_aref
#define assign ick_assign
#define binin ick_binin
#define binout ick_binout
#define bitencout ick_bitencout
#define br_trans ick_br_trans
#define cesspoolin ick_cesspoolin
#define cesspoolout ick_cesspoolout
#define checkforbugs ick_checkforbugs
#define clc_cset_atari ick_clc_cset_atari
#define clc_cset_baudot ick_clc_cset_baudot
#define clc_cset_convert ick_clc_cset_convert
#define clc_cset_ebcdic ick_clc_cset_ebcdic
#define clc_cset_hardcoderead ick_clc_cset_hardcoderead
#define clc_cset_latin1 ick_clc_cset_latin1
#define clc_cset_load ick_clc_cset_load
#define clc_cset_ptr ick_clc_cset_ptr
#define clcsem ick_clcsem
#define clcsemantics ick_clcsemantics
#define clockface ick_clockface
#define coreonerr ick_coreonerr
#define cset_recent ick_cset_recent
#define csetow ick_csetow
#define datadir ick_datadir
#define debfopen ick_debfopen
#define fin ick_fin
#define fin16 ick_fin16
#define fin32 ick_fin32
#define findandfopen ick_findandfopen
#define findandfreopen ick_findandfreopen
#define findandtestopen ick_findandtestopen
#define first ick_first
#define globalargv0 ick_globalargv0
#define iselect ick_iselect
#define lineno ick_lineno
#define lose ick_lose
#define lwarn ick_lwarn
#define mingle ick_mingle
#define multicome0 ick_multicome0
#define mystery ick_mystery
#define mysteryc ick_mysteryc
#define next ick_next
#define nextindex ick_nextindex
#define numerals ick_numerals
#define or16 ick_or16
#define or32 ick_or32
#define parseargs ick_parseargs
#define pin ick_pin
#define popnext ick_popnext
#define pout ick_pout
#define printflow ick_printflow
#define printfopens ick_printfopens
#define pushnext ick_pushnext
#define resize ick_resize
#define resume ick_resume
#define retrieve ick_retrieve
#define rev_and16 ick_rev_and16
#define rev_and32 ick_rev_and32
#define rev_fin16 ick_rev_fin16
#define rev_fin32 ick_rev_fin32
#define rev_or16 ick_rev_or16
#define rev_or32 ick_rev_or32
#define rev_whirl16 ick_rev_whirl16
#define rev_whirl32 ick_rev_whirl32
#define rev_xor16 ick_rev_xor16
#define rev_xor32 ick_rev_xor32
#define roll ick_roll
#define rotleft16 ick_rotleft16
#define rotleft32 ick_rotleft32
#define setbitcount ick_setbitcount
#define smudgeleft ick_smudgeleft
#define smudgeright ick_smudgeright
#define stash ick_stash
#define stashinit ick_stashinit
#define traditional ick_traditional
#define whirl ick_whirl
#define whirl16 ick_whirl16
#define whirl32 ick_whirl32
#define wimp_mode ick_wimp_mode
#define xor ick_xor
#define xor16 ick_xor16
#define xor32 ick_xor32
#define xselx ick_xselx
