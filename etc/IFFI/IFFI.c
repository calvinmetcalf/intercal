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

#include "IFFI.h"
#include "../../stack.h"
#include "../../settings.h"
#include "../../interpreter.h"

static instructionPointer *iffiIP = NULL;

// Communication functions with the ecto_b98 expansion library
void ick_SaveIPPosDelta(struct ick_ipposdeltatype* ippd)
{
	ippd->ix = iffiIP->position.x;
	ippd->iy = iffiIP->position.y;
	ippd->dx = iffiIP->delta.x;
	ippd->dy = iffiIP->delta.y;
}

void ick_RestoreIPPosDelta(struct ick_ipposdeltatype* ippd)
{
	iffiIP->position.x = ippd->ix;
	iffiIP->position.y = ippd->iy;
	iffiIP->delta.x = ippd->dx;
	iffiIP->delta.y = ippd->dy;
}

void ick_InterpreterRun(void)
{
	if (!FungeSpaceCreate()) {
		perror("Couldn't create funge space!?");
		exit(EXIT_FAILURE);
	}
	FungeSpaceLoadString(ick_iffi_befungeString);
	iffiIP = ipCreate();
	if (iffiIP == NULL) {
		perror("Couldn't create instruction pointer!?");
		exit(EXIT_FAILURE);
	}
	{
		struct timeval tv;
		if (gettimeofday(&tv, NULL)) {
			perror("Couldn't get time of day?!");
			exit(EXIT_FAILURE);
		}
		/* Set up randomness */
		srandom(tv.tv_usec);
	}
	if(ick_printflow) SettingTraceLevel=9;
	ick_InterpreterMainLoop();
}

void ick_iffi_InterpreterOneIteration(void)
{
	FUNGEDATATYPE opcode;
	opcode = FungeSpaceGet(&iffiIP->position);

	if (SettingTraceLevel > 8) {
		fprintf(stderr, "x=%" FUNGEVECTORPRI " y=%" FUNGEVECTORPRI
				": %c (%" FUNGEDATAPRI ")\n",
				iffiIP->position.x, iffiIP->position.y, (char)opcode, opcode);
		PrintStackTop(iffiIP->stack);
	} else if (SettingTraceLevel > 3) {
		fprintf(stderr, "x=%" FUNGEVECTORPRI " y=%" FUNGEVECTORPRI
				": %c (%" FUNGEDATAPRI ")\n",
				iffiIP->position.x, iffiIP->position.y, (char)opcode, opcode);
	} else if (SettingTraceLevel > 2)
		fprintf(stderr, "%c", (char)opcode);

	ExecuteInstruction(opcode, iffiIP);
	if (iffiIP->needMove)
		ipForward(iffiIP, 1);
	else
		iffiIP->needMove = true;
}

// C - In markmode COME FROM the top of stack
static void FingerIFFIcomeFrom(instructionPointer * ip)
{
	FUNGEDATATYPE l;

	l = StackPop(ip->stack);

	if (ick_iffi_inmarkmode) {
		ick_iffi_breakloop = 1;
		ick_iffi_linelabel = l;
		ick_iffi_comingfrom = 1;
	}
}

// F - FORGET NEXT stack entries equal to top of stack
static void FingerIFFIforget(instructionPointer * ip)
{
	FUNGEDATATYPE f;

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ipReverse(ip);
		return;
	}

	f = StackPop(ip->stack);

	if (f > 81 || f < 0) f = 81;

	ick_iffi_forgetcount = f;
}

// L - Use top of stack as a line label for this point
static void FingerIFFIlabel(instructionPointer * ip)
{
	FUNGEDATATYPE l;

	l = StackPop(ip->stack);

	ick_iffi_breakloop = 1;
	ick_iffi_linelabel = l;
	ick_iffi_sucking = 1;
}

// M - Marks points where the code can be entered from outside
static void FingerIFFImarker(instructionPointer * ip)
{
	(void) ip;
	ick_iffi_breakloop = ick_iffi_inmarkmode;
}

// N - Try to NEXT to the line labelled with the top of stack
static void FingerIFFInext(instructionPointer * ip)
{
	FUNGEDATATYPE l;

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ipReverse(ip);
		return;
	}

	l = StackPop(ip->stack);

	ick_iffi_breakloop = 1;
	ick_iffi_linelabel = l;
	ick_iffi_nexting = 1;
}

// R - RESUME to the top-of-stackth NEXT stack entry
static void FingerIFFIresume(instructionPointer * ip)
{
	FUNGEDATATYPE f;

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ipReverse(ip);
		return;
	}

	f = StackPop(ip->stack);

	if (f > 81 || f < 0) f = 81;

	ick_iffi_forgetcount = f;

	ick_iffi_resuming = 1;

	ick_iffi_breakloop = 1;
}

// X - In markmode NEXT FROM the top of stack
static void FingerIFFInextFrom(instructionPointer * ip)
{
	FUNGEDATATYPE l;

	l = StackPop(ip->stack);

	if (ick_iffi_inmarkmode) {
		ick_iffi_breakloop = 1;
		ick_iffi_linelabel = l;
		ick_iffi_nextingfrom = 1;
	}
}

bool FingerIFFIload(instructionPointer * ip)
{
	static int firstload = 1;
	ick_iffi_breakloop = firstload;
	firstload = 0;
	ManagerAddOpcode(IFFI,  'C', comeFrom)
	ManagerAddOpcode(IFFI,  'F', forget)
	ManagerAddOpcode(IFFI,  'L', label)
	ManagerAddOpcode(IFFI,  'M', marker)
	ManagerAddOpcode(IFFI,  'N', next)
	ManagerAddOpcode(IFFI,  'R', resume)
	ManagerAddOpcode(IFFI,  'X', nextFrom)
	return true;
}
