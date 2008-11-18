/*
 * fiddle.c -- functions that implement the five INTERCAL operators
 *
 * We link these to the compiler, too, in order to do constant folding
 *
LICENSE TERMS
    Copyright (C) 1996 Eric S. Raymond

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

#include "fiddle.h"
#include "sizes.h"
#include "ick_lose.h"
#include <stdio.h>

unsigned int ick_mingle(register unsigned int r, register unsigned int s)
{
  if (ick_Base == 2) {
    if (r>0xffff || s>0xffff)
      ick_lose(IE533, ick_lineno, (const char *)NULL);
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
  else {
    unsigned int result = 0, fac = 1;
    int i;
    for (i = 0 ; i < ick_Small_digits ; i++) {
      result += fac * (s % ick_Base);
      s /= ick_Base;
      fac *= ick_Base;
      result += fac * (r % ick_Base);
      r /= ick_Base;
      fac *= ick_Base;
    }
    return result;
  }
}

unsigned int ick_iselect(register unsigned int r, register unsigned int s)
{
  if (ick_Base == 2) {
    register unsigned int i = 1, t = 0;
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
    return(t);
  }
  else {
    unsigned int j, result = 0, fac, digit, ofac = 1;
    for (j = (unsigned)ick_Base - 1 ; j > 0 ; j--) {
      int i;
      fac = 1;
      for (i = 0; i < ick_Large_digits ; i++) {
	if ((s / fac) % ick_Base == j) {
	  digit = (r / fac) % ick_Base;
	  if (digit)
	    result += ofac * (digit > j ? digit : j);
	  ofac *= ick_Base;
	}
	fac *= ick_Base;
      }
    }
    return result;
  }
}

static unsigned int ick_whirl(unsigned int len, unsigned int p, unsigned int n)
{
  unsigned int i, fac = 1, result = 0, d1, d2, dsave;
  d1 = n % ick_Base;
  dsave = d1;
  for (i = 1 ; i <= len ; i++) {
    d2 = d1;
    d1 = (i < len) ? (n /= ick_Base, n % ick_Base) : dsave;
    if (d1 <= p)
      result += fac * ((d2 < d1 || d2 > p) ? d1 : d2);
    else
      result += fac * ((d2 < d1 && d2 > p) ? d1 : d2);
    fac *= ick_Base;
  }
  return result;
}

unsigned int ick_and16(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m & n);
  }
  else {
    return ick_whirl((unsigned)ick_Small_digits,0,n);
  }
}

unsigned int ick_or16(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m | n);
  }
  else {
    return ick_whirl((unsigned)ick_Small_digits,(unsigned)ick_Base-1,n);
  }
}

unsigned int ick_whirl16(unsigned int p, unsigned int n)
{
  return ick_whirl((unsigned)ick_Small_digits,p,n);
}

unsigned int ick_and32(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m & n);
  }
  else {
    return ick_whirl((unsigned)ick_Large_digits,0,n);
  }
}

unsigned int ick_or32(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m | n);
  }
  else {
    return ick_whirl((unsigned)ick_Large_digits,(unsigned)ick_Base-1,n);
  }
}

unsigned int ick_whirl32(unsigned int p, unsigned int n)
{
  return ick_whirl((unsigned)ick_Large_digits,p,n);
}

static unsigned int ick_xor(unsigned int len, unsigned int n)
{
  unsigned int i, fac = 1, result = 0, d1, d2, dsave;
  d1 = n % ick_Base;
  dsave = d1;
  for (i = 1 ; i <= len ; i++) {
    d2 = d1;
    d1 = (i < len) ? (n /= ick_Base, n % ick_Base) : dsave;
    result += fac * ((ick_Base + d1 - d2) % ick_Base);
    fac *= ick_Base;
  }
  return result;
}

unsigned int ick_xor16(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m ^ n);
  }
  else {
    return ick_xor((unsigned)ick_Small_digits,n);
  }
}

unsigned int ick_xor32(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m ^ n);
  }
  else {
    return ick_xor((unsigned)ick_Large_digits,n);
  }
}

static unsigned int ick_fin(unsigned int len, unsigned int n)
{
  unsigned int i, fac = 1, result = 0, d1, d2, dsave;
  d1 = n % ick_Base;
  dsave = d1;
  for (i = 1 ; i <= len ; i++) {
    d2 = d1;
    d1 = (i < len) ? (n /= ick_Base, n % ick_Base) : dsave;
    result += fac * ((d1 + d2) % ick_Base);
    fac *= ick_Base;
  }
  return result;
}

unsigned int ick_fin16(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m ^ n);
  }
  else {
    return ick_fin((unsigned)ick_Small_digits,n);
  }
}

unsigned int ick_fin32(unsigned int n)
{
  if (ick_Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m ^ n);
  }
  else {
    return ick_fin((unsigned)ick_Large_digits,n);
  }
}

/* AIS: Reversed operations, for operand overloading */

static unsigned int ick_rotleft16(unsigned int n)
{
  return !!(n&0x8000)|((n&0x7FFF)<<1);
}
static unsigned int ick_rotleft32(unsigned int n)
{
  return !!(n&0x80000000)|((n&0x7FFFFFFF)<<1);
}

/* For the time being, just work out the answer in binary, and test using
   the base-whatever operation. This means that there'll nearly always be
   a failure in reversing in bases other than 2. */
unsigned int ick_rev_or16(unsigned int n)
{
  if(ick_or16(ick_rotleft16(ick_and16(n)))==n) return ick_rotleft16(ick_and16(n));
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_or32(unsigned int n)
{
  if(ick_or32(ick_rotleft32(ick_and32(n)))==n) return ick_rotleft32(ick_and32(n));
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_and16(unsigned int n)
{
  if(ick_and16(ick_rotleft16(ick_or16(n)))==n) return ick_rotleft16(ick_or16(n));
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_and32(unsigned int n)
{
  if(ick_and32(ick_rotleft32(ick_or32(n)))==n) return ick_rotleft32(ick_or32(n));
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_xor16(unsigned int n)
{
  unsigned int a=0, l=1, t=0;
  while(l<=0x4000)
  {
    if(n&l)
      t^=1;
    if(t)
      a+=l*2;
    l*=2;
  }
  if(ick_xor16(a)==n) return a;
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}


unsigned int ick_rev_xor32(unsigned int n)
{
  unsigned int a=0, l=1, t=0;
  while(l<=0x4000000)
  {
    if(n&l)
      t^=1;
    if(t)
      a+=l*2;
    l*=2;
  }
  if(ick_xor32(a)==n) return a;
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_fin16(unsigned int n)
{
  unsigned int a=0, l=1, t=0;
  while(l<=0x4000)
  {
    if(n&l)
      t^=1;
    if(t)
      a+=l*2;
    l*=2;
  }
  if(ick_fin16(a)==n) return a;
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}


unsigned int ick_rev_fin32(unsigned int n)
{
  unsigned int a=0, l=1, t=0;
  while(l<=0x4000000)
  {
    if(n&l)
      t^=1;
    if(t)
      a+=l*2;
    l*=2;
  }
  if(ick_fin32(a)==n) return a;
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_whirl16(unsigned int p, unsigned int n)
{
  /* Only reverse if all digits are the same. */
  if(ick_whirl16(p,n)==n) return n;
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

unsigned int ick_rev_whirl32(unsigned int p, unsigned int n)
{
  /* Only reverse if all digits are the same. */
  if(ick_whirl32(p,n)==n) return n;
  ick_lose(IE277, ick_lineno, (const char*) NULL);
  /*@-unreachable@*/ return 0; /*@=unreachable@*/
}

/* AIS: Some helper functions for the optimizer, only working in base 2 */

unsigned int ick_xselx(unsigned int x)
{
  register unsigned int r=0;
  if(ick_Base != 2) ick_lose(IE778, ick_lineno, (const char*) NULL);
  while(x) {if(x&1) r=(r<<1)|1; x>>=1;}
  return r;
}

unsigned int ick_setbitcount(unsigned int x)
{
  register unsigned int r=0;
  while(x) {if(x&1) r++; x>>=1;}
  return r;
}

unsigned int ick_smudgeright(unsigned int x)
{
  x=x|(x>>1);
  x=x|(x>>2);
  x=x|(x>>4);
  x=x|(x>>8);
  x=x|(x>>18);
  return x;
}

unsigned int ick_smudgeleft(unsigned int x)
{
  x=x|(x<<1);
  x=x|(x<<2);
  x=x|(x<<4);
  x=x|(x<<8);
  x=x|(x<<18);
  return x;
}

/* fiddle.c */
