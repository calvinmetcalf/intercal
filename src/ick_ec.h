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
   inside the C program back to inside the C program), ick_checksuckpoint will
   longjmp back to a previous incarnation of itself if doing so eliminates no
   NEXT stack entries. This trick doesn't work with FORGET, though; instead
   the stray C stack entries are eliminated at the next RESUME past them;
   also, all but one of the entries can be eliminated at each line label if
   there are more than one of them.
*/

#define ICK_EC_FUNC_START(id)			  \
ICK_EC_PP_0(id)  	  		          \
void id(void)					  \
{						  \
  void* ick_local_rv = 0;			  \
  int ick_local_checkmode = ick_global_checkmode; \
  jmp_buf ick_local_jmpbuf;			  \
  setjmp(ick_local_jmpbuf);			  \
  if(ick_global_checkmode==2)			  \
  {						  \
    goto ick_l2_ICK_EC_PP_2;			  \
  }						  \
  else if(ick_global_checkmode==1 ||		  \
	  ick_global_checkmode==3)		  \
  {						  \
    goto ick_l1_ICK_EC_PP_1;			  \
  }						  \
  else

#define ick_linelabel(expr) ick_labeledblock(expr,0)

#define ick_labeledblock(expr,block)			    \
  do {							    \
    if(ick_forgetamount > 1)				    \
    {							    \
      ick_global_stackcleanup=1;			    \
      ick_donext(expr,-1,1);				    \
    }							    \
    if(0)						    \
    {							    \
    ick_l2_ICK_EC_PP_2: ;				    \
      if(ick_global_linelabel != (expr) || (expr) > 65535)  \
	goto ick_l2_ICK_EC_PP_2;			    \
      ick_global_checkmode = 0;				    \
      if(ick_global_stackcleanup)			    \
	ick_forget(1);					    \
      ick_global_stackcleanup = 0;			    \
    }							    \
    block ;						    \
    ick_checksuckpoint(expr);				    \
  }							    \
  while(0)

#define ick_comefrom(expr)					 \
  if(0)								 \
  {								 \
  ick_l1_ICK_EC_PP_1: ;						 \
    if(ick_global_linelabel == (expr) && (expr) <= 65535)	 \
    {								 \
      if(ick_global_goto) ick_multicome0(-1,ick_local_jmpbuf);	 \
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
      if(ick_global_goto) ick_multicome0(-1,ick_local_jmpbuf);	 \
      ick_global_goto = ICK_EC_PP_3;				 \
      ick_global_checkmode = 3;					 \
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
ick_l2_ICK_EC_PP_2: return;			\
}

#define ick_next(label) do{			\
    if((label)<=65535)				\
      ick_donext((label),-1,0);			\
  } while(0)

#define ick_resume(amount) ick_doresume((amount),-1)

#define ick_return_or_resume() do{		\
    ick_global_checkmode=5;			\
    return;					\
  } while(0)

/*@maynotreturn@*/ void ick_donext(unsigned long, int, int);
void ick_forget(unsigned short amount);
/*@noreturn@*/ void ick_doresume(unsigned short amount, int emitlineno);
/*@maynortreturn@*/ void ick_checksuckpoint(unsigned long linelabel);

void ick_allecfuncs(void); /* in generated program */

extern int ick_global_checkmode;
extern int ick_global_stackcleanup;
extern unsigned long ick_global_linelabel;
extern unsigned long ick_global_goto;
extern int ick_forgetamount;
