/*
 * fiddle.c -- functions that implement the five INTERCAL operators
 *
 * We link these to the compiler, too, in order to do constant folding
 */

#include "sizes.h"

unsigned int mingle(r, s)
register unsigned int r, s;
{
  if (Base == 2) {
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
    unsigned int i, result = 0, fac = 1;
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

unsigned int iselect(r, s)
register unsigned int r, s;
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
    unsigned int i, j, result = 0, fac, digit, ofac = 1;
    for (j = Base - 1 ; j > 0 ; j--) {
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

unsigned int whirl(len,p,n)
unsigned int len,p,n;
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

unsigned int and16(n)
unsigned int n;
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

unsigned int or16(n)
unsigned int n;
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

unsigned int whirl16(p,n)
unsigned int p,n;
{
  return whirl(Small_digits,p,n);
}

unsigned int and32(n)
unsigned int n;
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

unsigned int or32(n)
unsigned int n;
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

unsigned int whirl32(p,n)
unsigned int p,n;
{
  return whirl(Large_digits,p,n);
}

unsigned int xor(len,n)
unsigned int len,n;
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

unsigned int xor16(n)
unsigned int n;
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

unsigned int xor32(n)
unsigned int n;
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

unsigned int fin(len,n)
unsigned int len,n;
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

unsigned int fin16(n)
unsigned int n;
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

unsigned int fin32(n)
unsigned int n;
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

/* fiddle.c */
