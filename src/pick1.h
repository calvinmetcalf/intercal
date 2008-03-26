/*
 * pick1.h - Compiler-dependent defines for PIC compilers
 *
 * This file allows porting of PIC-INTERCAL output files to different
 * compilers. Most compilers will require this file to be modified;
 * as written at the moment, it's ANSI C that invokes undefined
 * behaviour (in a way that might plausibly produce correct
 * results).
 *
LICENSE TERMS
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

/* Place any headers, config fuse selection statements, and device
   statements your compiler needs here. */
#include <stdint.h>

/* Define these constants to unsigned data types of the correct
   bit width. (If your compiler doesn't use 1 bit variables, you
   could substitute an 8 bit variable, but that would take up
   extra RAM, which may or may not drive the program over the PIC's
   RAM limit.) */
#define ICK_INT32 uint32_t
#define ICK_INT16 uint16_t
#define ICK_INT8 uint8_t
#define ICK_INT1 uint1_t

/* Define this to an error-handling procedure. You should ignore the
   arguments, as they won't make too much sense in this context.
   This should block the program; the SLEEP command is one possibility,
   somehow getting the PIC to disconnect its own power supply and signal
   an error is another. */
#define ick_lose(a,b,c) for(;;)

/* PORTA, PORTB, TRISA, and TRISB must be variables which, when read or
   modified, read or modify the corresponding location in the PIC's RAM;
   these are often at locations 0x05, 0x06, 0x85, and 0x86 respectively.
   Place the code to do this here, if it isn't added by the header files
   above. See the section below if your compiler doesn't recognize the
   volatile keyword, but this code is otherwise correct. */
#define PORTA *(volatile ICK_INT8*)0x05
#define PORTB *(volatile ICK_INT8*)0x06
#define TRISA *(volatile ICK_INT8*)0x85
#define TRISB *(volatile ICK_INT8*)0x86

/* This is a hack for compilers that don't interpret volatile correctly.
   The uncommented code assumes your compiler knows how to work it
   correctly; use the replacement below if it doesn't. */
#define seq(a) a
/* You may need to use the commented-out code instead if your compiler
   is confused about volatile.
 ICK_INT8 seq(ICK_INT8 n)
 {
   return n;
 }
*/

void pickinit()
{
   /* Place any initialization code that is needed here. */
}

/* If your implementation doesn't use function pointers, delete this and
   don't write any code that uses operator-overloading. */
typedef struct ick_overop_t
{
  ICK_INT32 (*get)(ICK_INT32);
  void (*set)(ICK_INT32);
} ick_overop;

#define ick_type16 ICK_INT16
#define ick_type32 ICK_INT32
