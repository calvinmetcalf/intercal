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
   headers other than setjmp.h and stdint.h. (The printfs are just for
   +printflow debug output and don't effect the code flow.) */

#define ICK_EC 1
#include "ick_lose.h"
#include "config.h"
#include "abcess.h" /* must come before ick_ec.h */
#include "ick_ec.h"
#include <setjmp.h>

/* Checkmode values, for reference:
 * 0: not using checkmode
 * 1: looking for a COME FROM or NEXT FROM, or COME FROM found
 * 2: looking for a label to NEXT to or goto
 * 3: NEXT FROM found
 * 4: no suitable target found
 * 5: do a resume(1) if this reaches ick_allecfuncs
 * 6: starting up, run your init code
 */
int ick_global_checkmode = 0;
/* ick_global_linelabel's marked as volatile to avoid problems with
   optimisation of assignments to it near setjmps. */
unsigned long ick_global_linelabel;
static int ick_forgetamount = 0;
unsigned long ick_global_goto;
void* ick_global_createdata;

extern ick_overop* ick_oo_twospots;

/* Do a CREATEd operator check and call. */
/*@maynotreturn@*/ uint32_t ick_dounop(char* unopstr,
				       uint32_t arg1,
				       uint32_t arg2,
				       int emitlineno,
				       unsigned long vi1,
				       unsigned long vi2,
				       unsigned long vi3,
				       ick_type32 (*og1)(ick_type32),
				       ick_type32 (*og2)(ick_type32),
				       ick_type32 (*og3)(ick_type32),
				       void (*os1)(ick_type32, void(*)()),
				       void (*os2)(ick_type32, void(*)()),
				       void (*os3)(ick_type32, void(*)()),
			/*@observer@*/ const char* errstr)
{
  int st;
  uint32_t rv;
  st = ick_jicmatch(unopstr);
  if(st)
  {
    ick_createdata icd[3];
    icd[0].width=16;         icd[1].width=16;         icd[2].width=16;
    icd[0].isarray=0;        icd[1].isarray=0;        icd[2].isarray=0;
    icd[0].varnumber=1601;   icd[1].varnumber=1602;   icd[2].varnumber=1603;
    icd[0].accessors.get=og1;icd[1].accessors.get=og2;icd[2].accessors.get=og3;
    icd[0].accessors.set=os1;icd[1].accessors.set=os2;icd[2].accessors.set=os3;
    icd[0].value=arg1;       icd[1].value=arg2;       icd[2].value=0;

    ick_stash(ick_TWOSPOT, vi1, ick_twospots+vi1, ick_oo_twospots);
    ick_stash(ick_TWOSPOT, vi2, ick_twospots+vi2, ick_oo_twospots);
    ick_stash(ick_TWOSPOT, vi3, ick_twospots+vi3, ick_oo_twospots);
    ick_oo_twospots[vi1]=icd[0].accessors;
    ick_oo_twospots[vi2]=icd[1].accessors;
    ick_oo_twospots[vi3]=icd[2].accessors;
    ick_global_createdata=icd;
    ick_dogoto(st, emitlineno, 1);
    rv = og3(ick_twospots[vi3]);
    ick_retrieve(ick_twospots+vi1, ick_TWOSPOT, vi1,
		 ick_twoforget[vi1], ick_oo_twospots);
    ick_retrieve(ick_twospots+vi2, ick_TWOSPOT, vi2,
		 ick_twoforget[vi2], ick_oo_twospots);
    ick_retrieve(ick_twospots+vi3, ick_TWOSPOT, vi3,
		 ick_twoforget[vi3], ick_oo_twospots);
  }
  else
    ick_lose(IE000, emitlineno, errstr);
  return rv;
}

/* Do a NEXT or goto. Gotos don't work with an empty NEXT stack, but that's
   trivial to correct by doing a NEXT at the start of the program. */
/*@maynotreturn@*/ void ick_dogoto(unsigned long linelabel,
				   int emitlineno, int isnext)
{
  /* OK, so auto is never necessary, but the point here is that this is being
     stored on the stack deliberately so that it can be recalled if this
     procedure is longjmped to. */
  auto volatile int nextlevel = ick_nextindex;
  if(ick_printflow&&linelabel<=65535&&isnext)
    fprintf(stderr,"[next:%lu]",linelabel);
  ick_global_checkmode = 2; /* look for linelabels */
  ick_global_linelabel = linelabel;
  /* If there was a FORGET earlier, implement it now, by removing the relevant
     NEXT stack entries. Unfortunately, we can't remove them from the main C
     stack if we care about the return address of this NEXT. If this is a GOTO
     rather than a NEXT, it's safe to go back to the top NEXT on the NEXT stack
     and redo it with a different target (because the return address will still
     be correct), and all FORGETs work using an implied GOTO. (COME FROMs work
     by telling the suckpoint to GOTO them.) */
  if(!isnext)
  {
    if(ick_printflow&&ick_forgetamount)
      fprintf(stderr,"[forget:%d]",ick_forgetamount);
    else if(ick_printflow&&linelabel<=65535)
      fprintf(stderr,"[goto:%lu]",linelabel);
    nextlevel = ick_nextindex -= ick_forgetamount + 1;
    ick_forgetamount = 0;
    if(ick_nextindex < 0) ick_nextindex = 0;
    longjmp(ick_next_jmpbufs[ick_nextindex],2);
  }
  else if(ick_nextindex==81)
    ick_lose(IE123, emitlineno, (const char*)NULL);
  /* longjmp return codes: 1 = resume, 2 = redo to a different target */
  if(setjmp(ick_next_jmpbufs[ick_nextindex])==1)
  {
    /* Returning from the next. Clean up the next stack, just in
       case the callee didn't. */
    ick_nextindex = nextlevel;
    ick_global_checkmode=0;
    return;
  }
  ick_nextindex = nextlevel; /* in case we were longjmped to */
  ++ick_nextindex;
  ick_allecfuncs();
  /* If the checkmode is 4, we didn't find a target. */
  if(ick_global_checkmode == 4)
    ick_lose(IE129, emitlineno, (const char*) NULL);
  /* Otherwise, the function called return(). */
  ick_doresume(1,emitlineno);
}

/* Schedule a FORGET. The actual FORGET is performed at the next GOTO,
   which should happen immediately. */
void ick_scheduleforget(unsigned short amount)
{
  ick_forgetamount += amount;
  if(ick_forgetamount >= ick_nextindex)
    ick_forgetamount = ick_nextindex-1;
}

/* Resume to a previous NEXT stack entry. */
/*@noreturn@*/ void ick_doresume(unsigned short amount, int emitlineno)
{
  if(ick_printflow) fprintf(stderr,"[resume:%hu]",amount);
  if(ick_forgetamount) ick_lose(IE778, emitlineno, (const char *)NULL);
  if(!amount) ick_lose(IE621, emitlineno, (const char *)NULL);
  ick_nextindex -= amount;
  if(ick_nextindex < 1) /* the very first NEXT can't be RESUMEd to */
    ick_lose(IE632, emitlineno, (const char *)NULL);
  longjmp(ick_next_jmpbufs[ick_nextindex],1);
}

/* Run any ick_startup blocks. */
void ick_runstartups(void)
{
  ick_global_checkmode = 6;
  ick_allecfuncs();
  ick_global_checkmode = 0;
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
  /* If this was a COME FROM, goto it. */
  if(ick_global_checkmode == 1)
  {
    if(ick_printflow) fprintf(stderr,"[comefrom:%lu]",ick_global_linelabel);
    ick_dogoto(ick_global_goto,-1,0); /* GOTO */
  }
  else if(ick_global_checkmode == 3)
  {
    if(ick_printflow) fprintf(stderr,"[nextfrom:%lu]",ick_global_linelabel);
    ick_dogoto(ick_global_goto,-1,1); /* NEXT */
    return; /* we were RESUMEd to */
  }
  /* This line should be unreachable. */
  ick_lose(IE778, -1, (const char *)NULL);
}

uint16_t ick_getonespot(unsigned short extername)
{
  int i=-1;
  while(++i,1)
  {
    if(ick_ec_vars[i].ick_ec_vartype==ICK_EC_VARS_END) break;
    if(ick_ec_vars[i].ick_ec_vartype==ick_ONESPOT)
      if(ick_ec_vars[i].ick_ec_extername==extername)
	return ick_onespots[ick_ec_vars[i].ick_ec_intername];
  }
  ick_lose(IE200,-1,(const char*)NULL);
}

void ick_setonespot(unsigned short extername, uint16_t value)
{
  int i=-1;
  while(++i,1)
  {
    if(ick_ec_vars[i].ick_ec_vartype==ICK_EC_VARS_END) break;
    if(ick_ec_vars[i].ick_ec_vartype==ick_ONESPOT)
      if(ick_ec_vars[i].ick_ec_extername==extername)
      {
	if(ick_oneforget[ick_ec_vars[i].ick_ec_intername]) return;
	ick_onespots[ick_ec_vars[i].ick_ec_intername]=value;
	return;
      }
  }
  ick_lose(IE200,-1,(const char*)NULL);
}

uint32_t ick_gettwospot(unsigned short extername)
{
  int i=-1;
  while(++i,1)
  {
    if(ick_ec_vars[i].ick_ec_vartype==ICK_EC_VARS_END) break;
    if(ick_ec_vars[i].ick_ec_vartype==ick_TWOSPOT)
      if(ick_ec_vars[i].ick_ec_extername==extername)
	return ick_twospots[ick_ec_vars[i].ick_ec_intername];
  }
  ick_lose(IE200,-1,(const char*)NULL);
}

void ick_settwospot(unsigned short extername, uint32_t value)
{
  int i=-1;
  while(++i,1)
  {
    if(ick_ec_vars[i].ick_ec_vartype==ICK_EC_VARS_END) break;
    if(ick_ec_vars[i].ick_ec_vartype==ick_TWOSPOT)
      if(ick_ec_vars[i].ick_ec_extername==extername)
      {
	if(ick_twoforget[ick_ec_vars[i].ick_ec_intername]) return;
	ick_twospots[ick_ec_vars[i].ick_ec_intername]=value;
	return;
      }
  }
  ick_lose(IE200,-1,(const char*)NULL);
}

/* Register a CREATE target. This is just a wrapper for
   ick_registercreation that gets around scoping problems. */
void ick_create(const char* sig, unsigned long target)
{
  ick_registercreation(sig, target);
}

/* Accessor and mutator functions for CREATE data */
int ick_c_i_width(int argpos)
{
  return ((ick_createdata*)ick_global_createdata)[argpos].width;
}
int ick_c_i_isarray(int argpos)
{
  return ((ick_createdata*)ick_global_createdata)[argpos].isarray;
}
unsigned short ick_c_i_varnumber(int argpos)
{
  return ((ick_createdata*)ick_global_createdata)[argpos].varnumber;
}
uint32_t ick_c_i_value(int argpos)
{
  return (uint32_t)(((ick_createdata*)ick_global_createdata)[argpos].value);
}
uint32_t ick_c_i_getvalue(int argpos)
{
  if(!(((ick_createdata*)ick_global_createdata)[argpos].accessors.get))
    return ick_c_i_value(argpos);
  return ((ick_createdata*)ick_global_createdata)[argpos].accessors.
    get(ick_c_i_value(argpos));
}
void ick_c_i_setvalue(int argpos, uint32_t newval)
{
  if(((ick_createdata*)ick_global_createdata)[argpos].accessors.set)
    ((ick_createdata*)ick_global_createdata)[argpos].accessors.
      set(newval,NULL);
}
