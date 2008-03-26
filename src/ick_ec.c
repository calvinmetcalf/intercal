/*****************************************************************************

NAME
    ick_ec.c -- external call support between C and C-INTERCAL

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

***************************************************************************/

/* Implement the INTERCAL flow control operators in C, except for ABSTAIN
   and REINSTATE. Worryingly, this is freestanding-legal and needs no C
   headers other than setjmp.h. */

#include "ick_lose.h"
#include "abcess.h" /* must come before ick_ec.h */
#include "ick_ec.h"
#include <setjmp.h>

static jmp_buf lastcspjb[ick_MAXNEXT];
static int lastcspjbvalid[ick_MAXNEXT]={0};

/* Checkmode values, for reference:
 * 0: not using checkmode
 * 1: looking for a COME FROM or NEXT FROM, or COME FROM found
 * 2: looking for a label to NEXT to or goto
 * 3: NEXT FROM found
 * 4: no suitable target found
 * 5: do a resume(1) if this reaches ick_allecfuncs
 */
int ick_global_checkmode = 0;
int ick_global_stackcleanup = 0;
/* ick_global_linelabel's marked as volatile to avoid problems with
   optimisation of assignments to it near setjmps. */
unsigned long ick_global_linelabel;
int ick_forgetamount = 0;
unsigned long ick_global_goto;

/* Clean cspjb data. */
static void cleancspjb(void)
{
  int i=ick_MAXNEXT;
  while(i-->ick_nextindex) lastcspjbvalid[i]=0;
}

/* Do a NEXT. */
/*@maynotreturn@*/ void ick_donext(unsigned long linelabel,
				   int emitlineno,
				   int willforget)
{
  /* OK, so auto is never necessary, but the point here is that this is being
     stored on the stack deliberately so that it can be recalled if this
     procedure is longjmped to. */
  auto volatile int nextlevel = ick_nextindex;
  if(ick_printflow&&linelabel<=65535) printf("[next:%lu]",linelabel);
  ick_global_checkmode = 2; /* look for linelabels */
  ick_global_linelabel = linelabel;
  /* If there was a FORGET earlier, implement it now, by removing the relevant
     NEXT stack entries. Unfortunately, we can't remove them from the main C
     stack if we care about the return address of this NEXT. */
  if(ick_forgetamount)
  {
    if(ick_printflow) printf("[doforget:%d]",ick_forgetamount);
    nextlevel = ick_nextindex -= ick_forgetamount;
    ick_forgetamount = 0;
    if(ick_nextindex < 0) ick_nextindex = 0;
    if(willforget) longjmp(ick_next_jmpbufs[ick_nextindex],2);
  }
  /* longjmp return codes: 1 = resume, 2 = clean up stack */
  if(setjmp(ick_next_jmpbufs[ick_nextindex])==1)
  {
    /* Returning from the next. Clean up the next stack, just in
       case the callee didn't. */
    ick_nextindex = nextlevel;
    cleancspjb();
    ick_global_checkmode=0;
    return;
  }
  ick_nextindex = nextlevel; /* in case we were longjmped to */
  cleancspjb();
  ick_next[ick_nextindex++] = 0;
  ick_allecfuncs();
  /* If the checkmode is 4, we didn't find a target. */
  if(ick_global_checkmode == 4)
    ick_lose(IE129, emitlineno, (char*) NULL);
  /* Otherwise, the function called return(). */
  ick_doresume(1,emitlineno);
}

/* Schedule a FORGET. This doesn't alter the NEXT stack immediately;
   instead, the next RESUME or NEXT takes it into account. */
void ick_forget(unsigned short amount)
{
  if(ick_printflow) printf("[forget:%hu]",amount);
  ick_forgetamount += amount;
  if(ick_forgetamount > ick_nextindex)
    ick_forgetamount = ick_nextindex;
}

/* Resume to a previous NEXT stack entry. */
/*@noreturn@*/ void ick_doresume(unsigned short amount, int emitlineno)
{
  if(ick_printflow) printf("[resume:%hu]",amount);
  if(ick_printflow&&ick_forgetamount) printf("[doforget:%d]",ick_forgetamount);
  if(!amount) ick_lose(IE621, emitlineno, (char *)NULL);
  amount += ick_forgetamount;
  ick_forgetamount = 0;
  ick_nextindex -= amount;
  if(ick_nextindex < 0)
    ick_lose(IE632, emitlineno, (char *)NULL);
  longjmp(ick_next_jmpbufs[ick_nextindex],1);
}

/* Check to see if anything tries to steal control from a suckpoint.
 * This allows line labels > 65535, but bear in mind that such high
 * line labels cannot be COME FROM or NEXTed FROM or to by the user.
 * ('High' line labels are used by the implementation to implement
 * this function, but only as goto targets.)
 */
/*@maynotreturn@*/ void ick_checksuckpoint(unsigned long linelabel)
{
  /* Check to see if any suckpoints aim here; if not, return. */
  ick_global_checkmode=1;
  ick_global_goto=0;
  ick_global_linelabel=linelabel;
  ick_allecfuncs();
  if(!ick_global_goto) {ick_global_checkmode=0; return;}
  /* Cut out bits of stack that we'll never need. */
  if(lastcspjbvalid[ick_nextindex]) longjmp(lastcspjb[ick_nextindex],1);
  setjmp(lastcspjb[ick_nextindex]);
  lastcspjbvalid[ick_nextindex]=1;
  /* If this was a COME FROM, goto it. */
  if(ick_global_checkmode == 1)
  {
    if(ick_printflow) printf("[comefrom:%lu]",ick_global_linelabel);
    ick_global_checkmode = 2;
    ick_global_linelabel = ick_global_goto;
    ick_allecfuncs();
  }
  else if(ick_global_checkmode == 3)
  {
    if(ick_printflow) printf("[nextfrom:%lu]",ick_global_linelabel);
    ick_donext(ick_global_goto,-1,0);
    return; /* we were RESUMEd to */
  }
  /* This line should be unreachable. */
  ick_lose(IE778, -1, (char *)NULL);
}
