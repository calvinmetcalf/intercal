/*
 * pick2.h - Runtime library for INTERCAL, for use by PIC C compilers
 *
 * Some of the code here was originally in fiddle.c, some was originally
 * in cesspool.c. The rest is a translation into C of syslib.i.
 *
LICENSE TERMS
    Copyright (C) 1996 Eric S. Raymond
    Copyright (C) 2006 Alex Smith

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

#define ick_resume ick_popnext

ICK_INT32 ick_mingle(ICK_INT16 r, ICK_INT16 s)
{
  r = ((r & 0x0000ff00) << 8) | (r & 0x000000ff);
  r = ((r & 0x00f000f0) << 4) | (r & 0x000f000f);
  r = ((r & 0x0c0c0c0c) << 2) | (r & 0x03030303);
  r = ((r & 0x22222222) << 1) | (r & 0x11111111);
  s = ((s & 0x0000ff00) << 8) | (s & 0x000000ff);
  s = ((s & 0x00f000f0) << 4) | (s & 0x000f000f);
  s = ((s & 0x0c0c0c0c) << 2) | (s & 0x03030303);
  s = ((s & 0x22222222) << 1) | (s & 0x11111111);
  return (r << 1) | s;
}

ICK_INT32 ick_iselect(ICK_INT32 r, ICK_INT32 s)
{
  ICK_INT32 i = 1, t = 0;
  while (s) {
    if (s & i) {
   	t |= r & i;
   	s ^= i;
   	i <<= 1;
    }
    else {
   	s >>= 1;
   	r >>= 1;
    }
  }
  return t;
}

ICK_INT16 ick_and16(ICK_INT16 n)
{
  ICK_INT16 m;
  m = (n >> 1);
  if (n & 1)
  m |= 0x8000;
  return(m & n);
}

ICK_INT16 ick_or16(ICK_INT16 n)
{
  ICK_INT16 m;
  m = (n >> 1);
  if (n & 1)
    m |= 0x8000;
  return(m | n);
}

ICK_INT32 ick_and32(ICK_INT32 n)
{
  ICK_INT32 m;
  m = (n >> 1);
  if (n & 1)
    m |= 0x80000000;
  return(m & n);
}

ICK_INT32 ick_or32(ICK_INT32 n)
{
  ICK_INT32 m;
  m = (n >> 1);
  if (n & 1)
    m |= 0x80000000;
  return(m | n);
}

ICK_INT16 ick_xor16(ICK_INT16 n)
{
  ICK_INT16 m;
  m = (n >> 1);
  if (n & 1)
    m |= 0x8000;
  return(m ^ n);
}

ICK_INT32 ick_xor32(ICK_INT32 n)
{
  ICK_INT32 m;
  m = (n >> 1);
  if (n & 1)
    m |= 0x80000000;
  return(m ^ n);
}

ICK_INT8 ick_nextindex = 0;
ICK_INT16 ick_next[16];

void ick_pushnext(ICK_INT16 n)
{
	ick_next[ick_nextindex++] = n;
}

ICK_INT16 ick_popnext(ICK_INT8 n)
{
    if (ick_nextindex < n) {
   	ick_nextindex = 0;
   	return (ICK_INT16)-1;
    }
    else
      ick_nextindex -= n;
    return(ick_next[ick_nextindex]);
}

/* It's hard to do randomization on a PIC. */
ICK_INT1 ick_roll(ICK_INT8 n)
{
   return n>50;
}

/* Syslib, implemented in C to save ROM space. */
#if defined(ONESPOT1)&&defined(ONESPOT2)&&defined(TWOSPOT1)
void syslibopt1050()
{
   ONESPOT2=0;
   if(ONESPOT1) ONESPOT2=TWOSPOT1/ONESPOT1;
}
void syslibopt1520()
{
   TWOSPOT1=((ICK_INT32)ONESPOT1<<16)|ONESPOT2;
}
#endif /* .1 .2 :1 */
#if defined(TWOSPOT1)&&defined(TWOSPOT2)&&defined(TWOSPOT3)
void syslibopt1550()
{
   TWOSPOT3=0;
   if(TWOSPOT2) TWOSPOT3=TWOSPOT1/TWOSPOT2;
}
void syslipopt1500()
{
   TWOSPOT3=TWOSPOT2+TWOSPOT1;
}
void syslibopt1510()
{
   TWOSPOT3=TWOSPOT1-TWOSPOT2;
}
void syslibopt1530()
{
   TWOSPOT3=TWOSPOT1*TWOSPOT2;
}
void syslibopt1540()
{
   TWOSPOT3=0;
   if(TWOSPOT2) TWOSPOT3=TWOSPOT1/TWOSPOT2;
}
#ifdef TWOSPOT4
void syslibopt1509()
{
   TWOSPOT3=TWOSPOT2+TWOSPOT1;
   TWOSPOT4=1;
   if(TWOSPOT3<TWOSPOT2) TWOSPOT4=2; /* overflow */
}
void syslibopt1539()
{
   TWOSPOT4=1;
   if(TWOSPOT2&&TWOSPOT1>0xffffffff/TWOSPOT2) TWOSPOT4=2;
   TWOSPOT3=TWOSPOT1*TWOSPOT2;
}
#endif /* :4 */
#endif /* :1 :2 :3 */
#if defined(ONESPOT1)&&defined(ONESPOT2)&&defined(ONESPOT3)
void syslipopt1000()
{
   ONESPOT3=ONESPOT2+ONESPOT1;
}
void syslibopt1010()
{
   ONESPOT3=ONESPOT1-ONESPOT2;
}
void syslibopt1030()
{
   ONESPOT3=ONESPOT1*ONESPOT2;
}
void syslibopt1040()
{
   ONESPOT3=0;
   if(ONESPOT2) ONESPOT3=ONESPOT1/ONESPOT2;
}
#ifdef ONESPOT4
void syslibopt1009()
{
   ONESPOT3=ONESPOT2+ONESPOT1;
   ONESPOT4=1;
   if(ONESPOT3<ONESPOT2) ONESPOT4=2; /* overflow */
}
void syslibopt1039()
{
   ONESPOT4=1;
   if(ONESPOT2&&ONESPOT1>0xffff/ONESPOT2) ONESPOT4=2;
   ONESPOT3=ONESPOT1*ONESPOT2;
}
#endif /* .4 */
#endif /* .1 .2 .3 */
#ifdef ONESPOT1
void syslibopt1020()
{
   ONESPOT1++;
}
#endif
