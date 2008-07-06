/* -*- mode: C; coding: utf-8; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * cfunge - a conformant Befunge93/98/08 interpreter in C.
 * Copyright (C) 2008 Arvid Norlander <anmaster AT tele2 DOT se>
 * Copyright (C) 2008 Alex Smith
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at the proxy's option) any later version. Arvid Norlander is a
 * proxy who can decide which future versions of the GNU General Public
 * License can be used.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HAD_SRC_FINGERPRINTS_IFFI_H
#define _HAD_SRC_FINGERPRINTS_IFFI_H

#include "../../global.h"
#include "../manager.h"

#ifdef CONCURRENT_FUNGE
#error The C-INTERCAL/cfunge external calls interface cannot be used concurrently
#endif

struct ick_ipposdeltatype
{
  long long ix, iy, dx, dy;
};

bool FingerIFFIload(instructionPointer * ip);

void ick_InterpreterMainLoop(void);

extern unsigned short ick_iffi_forgetcount;
extern int ick_iffi_nexting;
extern int ick_iffi_comingfrom;
extern int ick_iffi_nextingfrom;
extern int ick_iffi_sucking;
extern int ick_iffi_resuming;
extern uint32_t ick_iffi_linelabel;
extern int ick_iffi_breakloop;
extern int ick_iffi_inmarkmode;

extern const char * ick_iffi_befungeString;
extern int ick_printflow;

#endif
