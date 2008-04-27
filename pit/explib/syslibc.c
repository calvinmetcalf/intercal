/*
 * The INTERCAL system library, optimised version in C.
 *
 * This is provided as an example of how to write a C program
 * that links to INTERCAL programs, to demonstrate the external
 * call code.
 *
 * Written originally by Alex Smith, and released to the public
 * domain. This library comes with NO WARRANTY.
 */

#include <ick_ec.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* This identifier name breaches the namespace rule that ick_ must be
   avoided, but this library has to work with both other people's
   INTERCAL programs and other people's C programs. So I've chosen an
   identifier name which breaches the rules (so that it doesn't clash
   with other people's C programs) but isn't used anywhere in the
   compiler, and so doesn't clash with it either.
*/

static void errout(int routine, char* msg)
{
  fprintf(stderr,"C-INTERCAL system library: (%d): %s\n", routine, msg);
  exit(EXIT_FAILURE);
}

ICK_EC_FUNC_START(ick_my_custom_syslib)
{
  register uint16_t os1, os2;
  register uint32_t ts1, ts2;
  static int seededyet = 0;

  ick_linelabel(1000);
  os1=ick_getonespot(1);
  os2=ick_getonespot(2);
  if(0xffff-os1<os2) errout(1000, "onespot overflow");
  ick_setonespot(3,os1+os2);
  ick_resume(1);
  return; /* so the compiler knows the next line can't be reached from here */

  ick_linelabel(1009);
  os1=ick_getonespot(1);
  os2=ick_getonespot(2);
  ick_setonespot(4,1);
  if(0xffff-os1<os2) ick_setonespot(4,2);
  ick_setonespot(3,os1+os2);
  ick_resume(1);
  return;

  ick_linelabel(1010);
  ick_setonespot(3,ick_getonespot(1)-ick_getonespot(2));
  ick_resume(1);
  return;

  ick_linelabel(1020);
  ick_setonespot(1,ick_getonespot(1)+1U);
  ick_resume(1);
  return;

  ick_linelabel(1030);
  os1=ick_getonespot(1);
  os2=ick_getonespot(2);
  if(os1&&0xffff/os1<os2) errout(1030, "onespot overflow");
  ick_setonespot(3,os1*os2);
  ick_resume(1);
  return;

  ick_linelabel(1039);
  os1=ick_getonespot(1);
  os2=ick_getonespot(2);
  ick_setonespot(4,1);
  if(os1&&0xffff/os1<os2) ick_setonespot(4,2);
  ick_setonespot(3,os1*os2);
  ick_resume(1);
  return;

  ick_linelabel(1040);
  os2=ick_getonespot(2);
  if(!os2) errout(1040, "division by zero");
  ick_setonespot(3,ick_getonespot(1)/os2);
  ick_resume(1);
  return;

  ick_linelabel(1050);
  os1=ick_getonespot(1);
  ts1=ick_gettwospot(1);
  if(!os1) errout(1050, "division by zero");
  ts1/=os1;
  if(ts1>0xffffLU) errout(1050, "onespot overflow");
  ick_setonespot(2,(uint16_t)ts1);
  ick_resume(1);
  return;

  ick_linelabel(1500);
  ts1=ick_gettwospot(1);
  ts2=ick_gettwospot(2);
  if(0xffffffffLU-ts1<ts2) errout(1500, "twospot overflow");
  ick_settwospot(3,ts1+ts2);
  ick_resume(1);
  return;

  ick_linelabel(1509);
  ts1=ick_gettwospot(1);
  ts2=ick_gettwospot(2);
  ick_setonespot(4,1);
  if(0xffffffffLU-ts1<ts2) ick_setonespot(4,2);
  ick_settwospot(3,ts1+ts2);
  ick_resume(1);
  return;

  ick_linelabel(1510);
  ick_settwospot(3,ick_gettwospot(1)-ick_gettwospot(2));
  ick_resume(1);
  return;

  ick_linelabel(1520);
  ick_settwospot(1,(((uint32_t)ick_getonespot(1))<<16)+ick_getonespot(2));
  ick_resume(1);
  return;

  ick_linelabel(1530);
  ick_settwospot(1,ick_getonespot(1)*ick_getonespot(2));
  ick_resume(1);
  return;

  ick_linelabel(1540);
  ts1=ick_gettwospot(1);
  ts2=ick_gettwospot(2);
  if(ts1&&0xfffffffflu/ts1<ts2) errout(1540, "twospot overflow");
  ick_settwospot(3,ts1*ts2);
  ick_resume(1);
  return;

  ick_linelabel(1549);
  ts1=ick_gettwospot(1);
  ts2=ick_gettwospot(2);
  ick_setonespot(4,1);
  if(ts1&&0xfffffffflu/ts1<ts2) ick_setonespot(4,2);
  ick_settwospot(3,ts1*ts2);
  ick_resume(1);
  return;

  ick_linelabel(1550);
  ts2=ick_gettwospot(2);
  if(!ts2) errout(1550, "division by zero");
  ick_settwospot(3,ick_gettwospot(1)/ts2);
  ick_resume(1);
  return;

  ick_linelabel(1900);
  if(!seededyet) srand(time(0));
  seededyet=1;
  ick_setonespot(1,rand()/(1+RAND_MAX/65536));
  ick_resume(1);
  return;

  ick_linelabel(1910);
  /* Here, we use the same algorithm as the original INTERCAL,
     so as to produce similarly-distributed results.

     If we add together 12 uniform random variables in the range #0 to
     (.1 / 12), then the resulting random variable has a mean of (.1 /
     2), and a variance of 12 times the original variance, which is
     (((.1 * .1) / 144) / 12), giving a final variance of ((.1 * .1) /
     144) and a final standard deviation of .1 / 12. */
  if(!seededyet) srand(time(0));
  seededyet=1;
  os1=ick_getonespot(1);
  os2 =rand()/(RAND_MAX/((os1   )/12));
  os2+=rand()/(RAND_MAX/((os1+ 1)/12));
  os2+=rand()/(RAND_MAX/((os1+ 2)/12));
  os2+=rand()/(RAND_MAX/((os1+ 3)/12));
  os2+=rand()/(RAND_MAX/((os1+ 4)/12));
  os2+=rand()/(RAND_MAX/((os1+ 5)/12));
  os2+=rand()/(RAND_MAX/((os1+ 6)/12));
  os2+=rand()/(RAND_MAX/((os1+ 7)/12));
  os2+=rand()/(RAND_MAX/((os1+ 8)/12));
  os2+=rand()/(RAND_MAX/((os1+ 9)/12));
  os2+=rand()/(RAND_MAX/((os1+10)/12));
  os2+=rand()/(RAND_MAX/((os1+11)/12));
  ick_setonespot(2,os2);
  ick_resume(1);
  return;

  /* Several existing INTERCAL programs rely on the routine (1001),
     which is undocumented and therefore shouldn't be used. So as not
     to break those programs, here it is: */
  ick_linelabel(1001);
  ick_resume(ick_getonespot(5));
  return;

}
ICK_EC_FUNC_END
