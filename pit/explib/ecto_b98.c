/***************************************************************************

NAME
    ecto_b98.c - glue code for linking .b98 files with .i files

LICENSE TERMS
    Copyright (C) 2008 Alex Smith

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

    Note that this code is designed to be linked against a GPLv3 library
    (producing a GPLv3 output), and therefore it is recommended that
    modifications to this code are compatible with GPLv3, although this
    is not a legal requirement.

***************************************************************************/

/* This is an expansion library designed to link with the Funge-98 interpreter
   cfunge (when compiled as a library); this is glue code which hooks into the
   INTERCAL control structure, and uses functions in glue code which hooks
   into the Funge control structure in cfunge. The Befunge program itself that
   is interpreted is stored in yet another .cio file, and accessed via the
   extern variables ick_iffi_befungeString, ick_iffi_markerposns, and
   ick_iffi_markercount. The name ecto_b98 is magical, and causes this library
   to be included when a .b98 files is encountered on the command line (as
   well as the cfunge library itself, which has the magical name
   libick_ecto_b98.a).

   The library itself works by using label names that have a special meaning
   to ick, along with ICK_EC_FUNCs, to cause a compiled C-INTERCAL program to
   also search in the Befunge program for line labels, COME FROMs, and NEXT
   FROMs, as well as in the INTERCAL program itself. It also allows the
   Befunge program to use INTERCAL control flow structures (even to itself),
   by monitoring flags set in a Funge-98 fingerprint (IFFI, which accounts
   for most of the variable names) and simulating the required structure
   (such as FORGET or NEXT) somewhere where cfunge itself will be unaffected
   and unaware of what is going on.

   In general, INTERCAL control flow structures affect only the position and
   delta of the IP in the Befunge program, not anything else. (In particular,
   the Befunge stack is unaffected.)
 */

#include <stdio.h>
#include <assert.h>
#include <ick_ec.h>

extern int ick_printflow;

struct ick_ipposdeltatype
{
  long long ix, iy, dx, dy;
};

/* This is called from IFFI. Remember to keep matching
   extern over there in sync. */
void ick_interpreter_main_loop(void);

/* This is just a forward decl. */
void ick_iffi_handle_control(void);

/* These are implemented in IFFI. */
extern void ick_interpreter_run(void);
extern void ick_iffi_interpreter_one_iteration(void);
extern void ick_save_ip_pos_delta(struct ick_ipposdeltatype*);
extern void ick_restore_ip_pos_delta(const struct ick_ipposdeltatype*);

unsigned short ick_iffi_forgetcount=0;
int ick_iffi_nexting=0;
int ick_iffi_comingfrom=0;
int ick_iffi_nextingfrom=0;
int ick_iffi_sucking=0;
int ick_iffi_resuming=0;
uint32_t ick_iffi_linelabel=0;
int ick_iffi_breakloop=0; /* becomes 1 when IFFI's loaded or mark-mode ends */
int ick_iffi_inmarkmode=0;

extern const unsigned char* ick_iffi_befungeString;
extern long long ick_iffi_markerposns[][2];
extern int ick_iffi_markercount;

ICK_EC_FUNC_START(ick_interpreter_main_loop)
{
  ick_startup( {ick_interpreter_run();} ); /* could be put anywhere, is put here
					     for convenience */

  /* we enter here when running at startup code and continue until IFFI is loaded */

  if(0)
  {
  ick_l2_ICK_EC_PP_2: /* this will be automatically replaced with a valid label,
			 as will the next occurence of it, by ick when it does
			 linking (i.e. this is not an infinite loop). */
    if(ick_global_linelabel != 0x70001ff1 &&
       ick_global_linelabel != 0x70001ff2)
      goto ick_l2_ICK_EC_PP_2;
    /* If control reaches this point, a COME FROM or NEXT has just completed,
       and the IP already in the correct place. Continue, outside markmode. */
    ick_iffi_nexting = 0;
    ick_iffi_comingfrom = 0;
    ick_iffi_nextingfrom = 0;
    ick_iffi_sucking = 0;
    ick_iffi_inmarkmode = 0;
    ick_global_checkmode = 0;
    /* If a NEXT has just completed, do a suckpoint-check on that line label
       (in case someone COMEs FROM the target of the NEXT). */
    if(ick_global_linelabel == 0x70001ff2)
    {
      auto struct ick_ipposdeltatype ippd;
      ick_save_ip_pos_delta(&ippd);
      ick_iffi_sucking = 0;
      ick_checksuckpoint(ick_iffi_linelabel); /* may not return */
      ick_restore_ip_pos_delta(&ippd);
    }
  }

ick_iffi_iml_rerun:

  ick_iffi_breakloop=0;
  while(!ick_iffi_breakloop)
  {
    ick_iffi_interpreter_one_iteration();
    if (ick_iffi_forgetcount && !ick_iffi_breakloop)
    {
      ick_forget(ick_iffi_forgetcount);
      ick_iffi_forgetcount = 0;
    }
  }
  /* There are several reasons why we could reach this point. Most of them,
     we just fall off the end of the function and whatever called us handles
     the program flow from there. However, if the loop broke due to a RESUME,
     line label or NEXT, we need to handle it. */
  if(ick_iffi_resuming)
  {
    unsigned short iifctemp = ick_iffi_forgetcount;
    ick_iffi_resuming = 0;
    ick_iffi_forgetcount = 0;
    ick_resume(iifctemp); /* never returns */
  }
  if(ick_iffi_nexting)
  {
    /* To handle a NEXT, we need to save the relevant IP information (position
       and delta, which are the only things saved/restored on the NEXT stack)
       onto the C stack. This is done in a struct ick_ipposdatatype, using the
       functions available for such save/restore. */
    auto struct ick_ipposdeltatype ippd;
    ick_save_ip_pos_delta(&ippd);
    ick_iffi_nexting = 0;
    ick_next(ick_iffi_linelabel); /* may not return */
    ick_restore_ip_pos_delta(&ippd);
    goto ick_iffi_iml_rerun; /* continue the main loop if we got here */
  }
  if(ick_iffi_sucking && ! ick_iffi_inmarkmode)
  {
    /* Handle a line-label encountered in normal execution. */
    auto struct ick_ipposdeltatype ippd;
    ick_save_ip_pos_delta(&ippd);
    ick_iffi_sucking = 0;
    ick_checksuckpoint(ick_iffi_linelabel); /* may not return */
    ick_restore_ip_pos_delta(&ippd);
    goto ick_iffi_iml_rerun; /* continue the main loop if we got here */
  }
}
ICK_EC_FUNC_END

ICK_EC_FUNC_START(ick_iffi_handle_control)
{
  static int recursing=0;
  int markerno, dirno, nextcheck;
  struct ick_ipposdeltatype ippd;

  /* The label names here are magical internal identifiers. That means that this
     is not in fact an infinite loop, but is instead a handler for sucking in
     from line labels. The 0x70001ff1 is a magic number used specifically by
     this code. */
  if(0)
  {
  ick_l1_ICK_EC_PP_1:
    if(ick_global_linelabel > 65535)
      goto skipsearch; /* there can't be a match, don't check to avoid recursion */
    nextcheck = 0;
    if(ick_printflow)
      fprintf(stderr,"[cfcheck:%d]\n",(int)ick_global_linelabel);
  }

  if(0)
  {
  ick_l2_ICK_EC_PP_2:
    if(ick_global_linelabel > 65535)
      goto skipsearch; /* there can't be a match, don't check to avoid recursion */
    nextcheck = 1;
    if(ick_printflow)
      fprintf(stderr,"[llcheck:%d]\n",(int)ick_global_linelabel);
  }

  assert(!recursing);

  recursing=1;

  ick_save_ip_pos_delta(&ippd);

  markerno=ick_iffi_markercount;
  while(markerno--)
  {
    dirno=4;
    while(dirno--)
    {
      struct ick_ipposdeltatype tippd;
      tippd.ix=ick_iffi_markerposns[markerno][0];
      tippd.iy=ick_iffi_markerposns[markerno][1];
      tippd.dx=(dirno == 3 ? -1 : dirno == 1 ? 1 : 0);
      tippd.dy=(dirno == 0 ? -1 : dirno == 2 ? 1 : 0);
      if(ick_printflow)
	fprintf(stderr, "Checking for %s at (%lld, %lld) going %c...\n",
		nextcheck ? "line label" : "COME FROM/NEXT FROM",
		tippd.ix, tippd.iy, dirno["^>v<"]);
      tippd.ix += tippd.dx;
      tippd.iy += tippd.dy;
      ick_restore_ip_pos_delta(&tippd);
      ick_iffi_inmarkmode = 1;
      ick_local_checkmode = ick_global_checkmode;
      ick_global_checkmode = 0;
      ick_interpreter_main_loop();
      ick_global_checkmode = ick_local_checkmode;
      ick_iffi_inmarkmode = 0;
      if((ick_iffi_nextingfrom||ick_iffi_comingfrom) &&
	ick_iffi_linelabel == ick_global_linelabel && !nextcheck)
      {
	if(ick_printflow)
	  fprintf(stderr,"%s FROM found!\n",ick_iffi_comingfrom?"COME":"NEXT");
	/* Error out on multiple COME FROM/NEXT FROM with the same target. */
	if(ick_global_goto) ick_lose(ICK_IE555, -1, (char*)0);
	ick_global_goto=0x70001FF1; /* we found a suckpoint */
	if(ick_iffi_nextingfrom) ick_global_checkmode = 3; /* do a next from */
	/* if it's a come-from, the checkmode is already correct */
	ick_save_ip_pos_delta(&ippd);
      }
      if(ick_iffi_sucking && ick_iffi_linelabel == ick_global_linelabel &&
	 nextcheck)
      {
	if(ick_printflow)
	  fprintf(stderr,"Line label found!\n");
	/* Another part of the code is NEXTing to this line label. */
	ick_save_ip_pos_delta(&ippd);
	nextcheck = 2; /* when this loop ends, goto the main loop */
      }
      ick_iffi_nextingfrom = 0;
      ick_iffi_comingfrom = 0;
      ick_iffi_sucking = 0;
    }
  }

  if(ick_printflow) fprintf(stderr,"Checks finished.\n");

  /* Set the IP to what we want it to be on resume, or otherwise return it to
     its original value. (Returning it to its original value is unneccesary
     but harmless.) */
  ick_restore_ip_pos_delta(&ippd);
  recursing=0;
  ick_iffi_linelabel = ick_global_linelabel;
  if(nextcheck == 2)
    ick_dogoto(0x70001FF2,-1,0);
skipsearch:
  if(nextcheck)
    goto ick_l2_ICK_EC_PP_2;
  else
    goto ick_l1_ICK_EC_PP_1;
}
ICK_EC_FUNC_END
