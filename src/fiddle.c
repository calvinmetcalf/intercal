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
#include "lose.h"
#include <stdio.h>

unsigned int mingle(register unsigned int r, register unsigned int s)
{
  if (Base == 2) {
    if (r>0xffff || s>0xffff)
      lose(E533, lineno, (char *)NULL);
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
    for (i = 0 ; i < Small_digits ; i++) {
      result += fac * (s % Base);
      s /= Base;
      fac *= Base;
      result += fac * (r % Base);
      r /= Base;
      fac *= Base;
    }
    return result;
  }
}

unsigned int iselect(register unsigned int r, register unsigned int s)
{
  if (Base == 2) {
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
    for (j = Base - 1 ; j > 0 ; j--) {
      int i;
      fac = 1;
      for (i = 0; i < Large_digits ; i++) {
	if ((s / fac) % Base == j) {
	  digit = (r / fac) % Base;
	  if (digit)
	    result += ofac * (digit > j ? digit : j);
	  ofac *= Base;
	}
	fac *= Base;
      }
    }
    return result;
  }
}

static unsigned int whirl(unsigned int len, unsigned int p, unsigned int n)
{
  unsigned int i, fac = 1, result = 0, d1, d2, dsave;
  d1 = n % Base;
  dsave = d1;
  for (i = 1 ; i <= len ; i++) {
    d2 = d1;
    d1 = (i < len) ? (n /= Base, n % Base) : dsave;
    if (d1 <= p)
      result += fac * ((d2 < d1 || d2 > p) ? d1 : d2);
    else
      result += fac * ((d2 < d1 && d2 > p) ? d1 : d2);
    fac *= Base;
  }
  return result;
}

unsigned int and16(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m & n);
  }
  else {
    return whirl(Small_digits,0,n);
  }
}

unsigned int or16(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m | n);
  }
  else {
    return whirl(Small_digits,Base-1,n);
  }
}

unsigned int whirl16(unsigned int p, unsigned int n)
{
  return whirl(Small_digits,p,n);
}

unsigned int and32(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m & n);
  }
  else {
    return whirl(Large_digits,0,n);
  }
}

unsigned int or32(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m | n);
  }
  else {
    return whirl(Large_digits,Base-1,n);
  }
}

unsigned int whirl32(unsigned int p, unsigned int n)
{
  return whirl(Large_digits,p,n);
}

unsigned int xor(unsigned int len, unsigned int n)
{
  unsigned int i, fac = 1, result = 0, d1, d2, dsave;
  d1 = n % Base;
  dsave = d1;
  for (i = 1 ; i <= len ; i++) {
    d2 = d1;
    d1 = (i < len) ? (n /= Base, n % Base) : dsave;
    result += fac * ((Base + d1 - d2) % Base);
    fac *= Base;
  }
  return result;
}

unsigned int xor16(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m ^ n);
  }
  else {
    return xor(Small_digits,n);
  }
}

unsigned int xor32(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m ^ n);
  }
  else {
    return xor(Large_digits,n);
  }
}

static unsigned int fin(unsigned int len, unsigned int n)
{
  unsigned int i, fac = 1, result = 0, d1, d2, dsave;
  d1 = n % Base;
  dsave = d1;
  for (i = 1 ; i <= len ; i++) {
    d2 = d1;
    d1 = (i < len) ? (n /= Base, n % Base) : dsave;
    result += fac * ((d1 + d2) % Base);
    fac *= Base;
  }
  return result;
}

unsigned int fin16(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x8000;
    return(m ^ n);
  }
  else {
    return fin(Small_digits,n);
  }
}

unsigned int fin32(unsigned int n)
{
  if (Base == 2) {
    unsigned int m = (n >> 1);
    if (n & 1)
      m |= 0x80000000;
    return(m ^ n);
  }
  else {
    return fin(Large_digits,n);
  }
}

/* AIS: Reversed operations, for operand overloading */

static unsigned int rotleft16(unsigned int n)
{
  return !!(n&0x8000)|((n&0x7FFF)<<1);
}
static unsigned int rotleft32(unsigned int n)
{
  return !!(n&0x80000000)|((n&0x7FFFFFFF)<<1);
}

/* For the time being, just work out the answer in binary, and test using
   the base-whatever operation. This means that there'll nearly always be
   a failure in reversing in bases other than 2. */
unsigned int rev_or16(unsigned int n)
{
  if(or16(rotleft16(and16(n)))==n) return rotleft16(and16(n));
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_or32(unsigned int n)
{
  if(or32(rotleft32(and32(n)))==n) return rotleft32(and32(n));
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_and16(unsigned int n)
{
  if(and16(rotleft16(or16(n)))==n) return rotleft16(or16(n));
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_and32(unsigned int n)
{
  if(and32(rotleft32(or32(n)))==n) return rotleft32(or32(n));
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_xor16(unsigned int n)
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
  if(xor16(a)==n) return a;
  lose(E277, lineno, (char*) NULL);
  return 0;
}


unsigned int rev_xor32(unsigned int n)
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
  if(xor32(a)==n) return a;
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_fin16(unsigned int n)
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
  if(fin16(a)==n) return a;
  lose(E277, lineno, (char*) NULL);
  return 0;
}


unsigned int rev_fin32(unsigned int n)
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
  if(fin32(a)==n) return a;
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_whirl16(unsigned int p, unsigned int n)
{
  /* Only reverse if all digits are the same. */
  if(whirl16(p,n)==n) return n;
  lose(E277, lineno, (char*) NULL);
  return 0;
}

unsigned int rev_whirl32(unsigned int p, unsigned int n)
{
  /* Only reverse if all digits are the same. */
  if(whirl32(p,n)==n) return n;
  lose(E277, lineno, (char*) NULL);
  return 0;
}

/* fiddle.c */
