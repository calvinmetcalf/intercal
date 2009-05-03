/* -*- mode: C; coding: utf-8; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-

    cfunge - a conformant Befunge93/98/08 interpreter in C.
    Copyright (C) 2008 Arvid Norlander <anmaster AT tele2 DOT se>
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

    Note that this code is designed to be linked against a GPLv3 library
    (producing a GPLv3 output), and therefore it is recommended that
    modifications to this code are compatible with GPLv3, although this
    is not a legal requirement.
 */

#ifndef FUNGE_HAD_SRC_FINGERPRINTS_IFFI_H
#define FUNGE_HAD_SRC_FINGERPRINTS_IFFI_H

#include "../../global.h"
#include "../manager.h"

#if !defined(CFUNGE_API_VERSION) || (CFUNGE_API_VERSION > 3)
#  error "This version of IFFI isn't compatible with this cfunge version."
#endif

#ifdef CONCURRENT_FUNGE
#  error "The C-INTERCAL/cfunge external calls interface cannot be used concurrently."
#endif

// We need at least GCC 4.1 for this trick.
#if defined(__GNUC__) && (__GNUC__ >= 4) && !defined(__INTEL_COMPILER)
#  if (__GNUC_MINOR__ >= 1) || (__GNUC__ > 4)
#    define PUBLIC __attribute__((visibility("protected"),externally_visible)) extern
#  else
#    define PUBLIC extern
#  endif
#else
#  define PUBLIC extern
#endif

struct ick_ipposdeltatype {
	long long ix, iy, dx, dy;
};

// Externally-visible functions from IFFI.c
// This is for manager.c
bool finger_IFFI_load(instructionPointer * ip);

// We export these for use by ecto_b98
// Please remember to update the matching extern(s) over there
// if you change here.
PUBLIC void ick_save_ip_pos_delta(struct ick_ipposdeltatype* ippd);
PUBLIC void ick_restore_ip_pos_delta(const struct ick_ipposdeltatype* ippd);
PUBLIC void ick_interpreter_run(void);
PUBLIC void ick_iffi_interpreter_one_iteration(void);

// Implemented in ecto_b98
extern void ick_interpreter_main_loop(void);

// Communication variables with ecto_b98
extern unsigned short ick_iffi_forgetcount;
extern int ick_iffi_nexting;
extern int ick_iffi_comingfrom;
extern int ick_iffi_nextingfrom;
extern int ick_iffi_sucking;
extern int ick_iffi_resuming;
extern uint32_t ick_iffi_linelabel;
extern int ick_iffi_breakloop;
extern int ick_iffi_inmarkmode;

// Communication variables with the compiled .cio file
extern const unsigned char * ick_iffi_befungeString;

// Communication variables with the runtime library
extern int ick_printflow;

// Accessors and mutators for INTERCAL storage
extern void ick_create(const char*, unsigned long);
extern uint16_t ick_getonespot(unsigned short);
extern void ick_setonespot(unsigned short, uint16_t);
extern uint32_t ick_gettwospot(unsigned short);
extern void ick_settwospot(unsigned short, uint32_t);
extern int ick_c_i_width(int);
extern int ick_c_i_isarray(int);
extern unsigned short ick_c_i_varnumber(int);
extern uint32_t ick_c_i_value(int);
extern uint32_t ick_c_i_getvalue(int);
extern void ick_c_i_setvalue(int, uint32_t);

#endif
