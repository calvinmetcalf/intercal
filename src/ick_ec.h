/*****************************************************************************

NAME
    ick_ec.h -- external call support between C and C-INTERCAL

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

/* The external calls to C work via ICK_EC_FUNCs; whenever a suckpoint is
   encountered, all ICK_EC_FUNCs are run with ick_global_checkmode set to 1,
   and when a NEXT is called but finds no target in the INTERCAL program, all
   ICK_EC_FUNCs are run with ick_global_checkmode set to 2. COME FROMs and
   NEXT FROMs don't steal control immediately (they know where to steal
   control from using ick_global_linelabel), but instead set ick_global_goto
   to a 'high' line label (one of the internal ones above 65536 which are
   allocated for this sort of purpose, and inside the preprocessor) or error
   if it's set nonzero already. After verifying that exactly one COME or NEXT
   FROM is involved, the INTERCAL program will NEXT to the high line label
   (whether it leaves a NEXT stack entry will depend on whether COME FROM or
   NEXT FROM was used, which is communicated by a NEXT FROM setting the
   checkmode to 3, which is identical to 1 in all other respects).

   To prevent this process running the program out of stack space (which would
   happen with a naive implementation if there were many COME FROMs from
   inside the C program back to inside the C program), all transitions but
   NEXTs are done indirectly, by jumping back inside a relevant invocation of
   ick_dogoto and changing its targets. (So effectively, to GOTO a particular
   destination, you go back in time to the last time you NEXTed - or to
   implement a FORGET, the last-but-n-time you NEXTed - and redo it with a
   different target.)
*/

#if ICK_HAVE_STDINT_H+1 == 2
#include <stdint.h>
#else
/* Ensure that uint32_t, etc, aren't implemented as compatibility macros
   so that we can implement them as typedefs. */
#undef uint32_t
#undef uint16_t
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
#endif

#ifndef ICK_ABCESS_H_INCLUDED
typedef unsigned short ick_type16;
typedef unsigned int   ick_type32;
#endif

#define ICK_EC_FUNC_START(id)			  \
ICK_EC_PP_0(id)  	  		          \
void id(void)					  \
{						  \
  void* ick_local_createdata =			  \
    ick_global_createdata;			  \
  int ick_local_checkmode = ick_global_checkmode; \
  if(ick_global_checkmode==6)			  \
  {						  \
    goto ick_l6_ICK_EC_PP_6;			  \
  }						  \
  if(ick_global_checkmode==2)			  \
  {						  \
    goto ick_l2_ICK_EC_PP_2;			  \
  }						  \
  else if(ick_global_checkmode==1 ||		  \
	  ick_global_checkmode==3)		  \
  {						  \
    goto ick_l1_ICK_EC_PP_1;			  \
  }						  \
  ick_local_checkmode=ick_global_checkmode=0;

#define ick_linelabel(expr) ick_labeledblock(expr,0)

#define ick_labeledblock(expr,block)			    \
  do {							    \
    if(0)						    \
    {							    \
    ick_l2_ICK_EC_PP_2: ;				    \
      if(ick_global_linelabel != (expr) || (expr) > 65535)  \
	goto ick_l2_ICK_EC_PP_2;			    \
      ick_global_checkmode = 0;				    \
    }							    \
    block ;						    \
    ick_checksuckpoint(expr);				    \
  }							    \
  while(0)

#define ick_linelabelnosp(expr)				    \
  do {							    \
    if(0)						    \
    {							    \
    ick_l2_ICK_EC_PP_2: ;				    \
      if(ick_global_linelabel != (expr) || (expr) > 65535)  \
	goto ick_l2_ICK_EC_PP_2;			    \
      ick_global_checkmode = 0;				    \
    }							    \
  }							    \
  while(0)

#define ick_forget(amount)			\
  do {						\
    ick_scheduleforget(amount);			\
    ick_dogoto(ICK_EC_PP_3,-1,0);		\
    ick_lose(ICK_IE778, -1, (char*) NULL);	\
    return;					\
  ick_l2_ICK_EC_PP_2: ;				\
    if(ick_global_linelabel != ICK_EC_PP_3)	\
      goto ick_l2_ICK_EC_PP_2;			\
    ick_global_checkmode = 0;			\
  } while(0)


#define ick_startup(block)			\
  if(0)						\
  {						\
  ick_l6_ICK_EC_PP_6:				\
    ick_global_checkmode=0;			\
    block ;					\
    ick_global_checkmode=ick_local_checkmode;	\
    goto ick_l6_ICK_EC_PP_6;			\
  }

#define ick_comefrom(expr)					 \
  if(0)								 \
  {								 \
  ick_l1_ICK_EC_PP_1: ;						 \
    if(ick_global_linelabel == (expr) && (expr) <= 65535)	 \
    {								 \
      if(ick_global_goto) ick_lose(ICK_IE555, -1, (char*)0);	 \
      ick_global_goto = ICK_EC_PP_3;				 \
    }								 \
    goto ick_l1_ICK_EC_PP_1;					 \
  ick_l2_ICK_EC_PP_2: ;						 \
    if(ick_global_linelabel != ICK_EC_PP_3)			 \
      goto ick_l2_ICK_EC_PP_2;					 \
    ick_global_checkmode = 0;					 \
  }

#define ick_comefromif(expr,condition) \
  ick_docomefromif(expr,-1,condition)

#define ick_docomefromif(expr,lbl,condition)	                 \
  if(0)								 \
  {								 \
  ick_l1_ICK_EC_PP_1: ;						 \
    if(ick_global_linelabel == (expr) && (expr) <= 65535)	 \
      if(condition)						 \
      {								 \
	if(ick_global_goto) ick_lose(ICK_IE555, lbl, (char*)0);	 \
	ick_global_goto = ICK_EC_PP_3;				 \
      }								 \
    goto ick_l1_ICK_EC_PP_1;					 \
  ick_l2_ICK_EC_PP_2: ;						 \
    if(ick_global_linelabel != ICK_EC_PP_3)			 \
      goto ick_l2_ICK_EC_PP_2;					 \
    ick_global_checkmode = 0;					 \
  }

#define ick_nextfrom(expr)					 \
  if(0)								 \
  {								 \
  ick_l1_ICK_EC_PP_1: ;						 \
    if(ick_global_linelabel == (expr) && (expr) <= 65535)	 \
    {								 \
      if(ick_global_goto) ick_lose(ICK_IE555, -1, (char*)0);	 \
      ick_global_goto = ICK_EC_PP_3;				 \
      ick_global_checkmode = 3;					 \
    }								 \
    goto ick_l1_ICK_EC_PP_1;					 \
  ick_l2_ICK_EC_PP_2: ;						 \
    if(ick_global_linelabel != ICK_EC_PP_3)			 \
      goto ick_l2_ICK_EC_PP_2;					 \
    ick_global_checkmode = 0;					 \
  }

#define ick_nextfromif(expr,condition) \
  ick_donextfromif(expr,-1,condition)

#define ick_donextfromif(expr,lbl,condition)	                 \
  if(0)								 \
  {								 \
  ick_l1_ICK_EC_PP_1: ;						 \
    if(ick_global_linelabel == (expr) && (expr) <= 65535)	 \
      if(condition)						 \
      {								 \
	if(ick_global_goto) ick_lose(ICK_IE555, lbl, (char*)0);	 \
	ick_global_goto = ICK_EC_PP_3;				 \
	ick_global_checkmode = 3;				 \
      }								 \
    goto ick_l1_ICK_EC_PP_1;					 \
  ick_l2_ICK_EC_PP_2: ;						 \
    if(ick_global_linelabel != ICK_EC_PP_3)			 \
      goto ick_l2_ICK_EC_PP_2;					 \
    ick_global_checkmode = 0;					 \
  }


#define ICK_EC_FUNC_END				\
  if(ick_local_checkmode) ick_resume(1);	\
ick_l1_ICK_EC_PP_1: ;				\
ick_l6_ICK_EC_PP_6: ;				\
ick_l2_ICK_EC_PP_2: return;			\
}

#define ick_next(label) do{			\
    if((label)<=65535)				\
      ick_dogoto((label),-1,1);			\
  } while(0)

#define ick_goto(label) do{			\
    if((label)<=65535)				\
      ick_dogoto((label),-1,0);			\
  } while(0)

#define ick_resume(amount) ick_doresume((amount),-1)

#define ick_return_or_resume() do{		\
    if(ick_local_checkmode) ick_doresume(1,-1); \
    return;					\
  } while(0)

/*@maynotreturn@*/ void ick_dogoto(unsigned long, int, int);
void ick_scheduleforget(unsigned short);
/*@noreturn@*/ void ick_doresume(unsigned short, int);
/*@maynotreturn@*/ void ick_checksuckpoint(unsigned long);
void ick_runstartups(void);
/*@maynotreturn@*/ uint32_t ick_dounop(char*, uint32_t, uint32_t, int,
				       unsigned long, unsigned long, unsigned long,
				       ick_type32(*)(ick_type32),
				       ick_type32(*)(ick_type32),
				       ick_type32(*)(ick_type32),
				       void(*)(ick_type32, void(*)()),
				       void(*)(ick_type32, void(*)()),
				       void(*)(ick_type32, void(*)()),
			/*@observer@*/ const char*);

void ick_allecfuncs(void); /* in generated program */

extern int ick_global_checkmode;
extern unsigned long ick_global_linelabel;
extern unsigned long ick_global_goto;
extern void* ick_global_createdata;

/* Variables. */
typedef struct ick_ec_var_tag
{
  int ick_ec_vartype;
  int ick_ec_extername;
  int ick_ec_intername;
} ick_ec_var;

extern ick_ec_var ick_ec_vars[];

#define ICK_EC_VARS_END 5

uint16_t ick_getonespot(unsigned short);
void ick_setonespot(unsigned short, uint16_t);
uint32_t ick_gettwospot(unsigned short);
void ick_settwospot(unsigned short, uint32_t);

void ick_create(const char*, unsigned long);

/* For accessing the arguments to an ick_created command */
int ick_c_i_width(int);
int ick_c_i_isarray(int);
unsigned short ick_c_i_varnumber(int);
uint32_t ick_c_i_value(int);
/* These require -a to work */
uint32_t ick_c_i_getvalue(int);
void ick_c_i_setvalue(int, uint32_t);

#define ick_c_width(a) ick_c_i_width((ick_global_createdata=ick_local_createdata,(a)))
#define ick_c_isarray(a) ick_c_i_isarray((ick_global_createdata=ick_local_createdata,(a)))
#define ick_c_varnumber(a) ick_c_i_varnumber((ick_global_createdata=ick_local_createdata,(a)))
#define ick_c_value(a) ick_c_i_value((ick_global_createdata=ick_local_createdata,(a)))
#define ick_c_getvalue(a) ick_c_i_getvalue((ick_global_createdata=ick_local_createdata,(a)))
#define ick_c_setvalue(a,n) ick_c_i_setvalue((ick_global_createdata=ick_local_createdata,(a)),(n))

/* Fragments of ick_lose.h, that don't impinge on unmangled namespace. */
#define ICK_IE555 "555 FLOW DIAGRAM IS EXCESSIVELY CONNECTED\n\
	ON THE WAY TO %d\n"
#define ICK_IE778 "778 UNEXPLAINED COMPILER BUG\n\
	ON THE WAY TO %d\n"

extern void /*@noreturn@*/ ick_lose(const char *m, int n, /*@null@*/ const char *s)
#ifdef __GNUC__
  __attribute__ ((noreturn))
#endif
;
