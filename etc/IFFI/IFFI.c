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

#include "IFFI.h"
#include "../../stack.h"
#include "../../settings.h"
#include "../../interpreter.h"

static instructionPointer *iffiIP = NULL;

static bool firstload = true;

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
	fungeCell opcode;
	opcode = FungeSpaceGet(&iffiIP->position);

	if (SettingTraceLevel > 8) {
		fprintf(stderr, "x=%" FUNGECELLPRI " y=%" FUNGECELLPRI
				": %c (%" FUNGECELLPRI ")\n",
				iffiIP->position.x, iffiIP->position.y, (char)opcode, opcode);
		PrintStackTop(iffiIP->stack);
	} else if (SettingTraceLevel > 3) {
		fprintf(stderr, "x=%" FUNGECELLPRI " y=%" FUNGECELLPRI
				": %c (%" FUNGECELLPRI ")\n",
				iffiIP->position.x, iffiIP->position.y, (char)opcode, opcode);
	} else if (SettingTraceLevel > 2)
		fprintf(stderr, "%c", (char)opcode);

	ExecuteInstruction(opcode, iffiIP);
	if (iffiIP->needMove)
		ipForward(iffiIP, 1);
	else
		iffiIP->needMove = true;
}

// A - CREATE a new INTERCAL instruction
static void FingerIFFIcreate(instructionPointer * ip)
{
    // arguments: line number on TOS, signature as 0gnirts beneath it
	fungeCell l = StackPop(ip->stack);
	char * restrict str = StackPopString(ip->stack);
	ick_create(str, l);
}

// C - In markmode COME FROM the top of stack
static void FingerIFFIcomeFrom(instructionPointer * ip)
{
	fungeCell l;

	l = StackPop(ip->stack);

	if (ick_iffi_inmarkmode) {
		ick_iffi_breakloop = 1;
		ick_iffi_linelabel = l;
		ick_iffi_comingfrom = 1;
	}
}

// D - Push information about a CREATED instruction argument
static void FingerIFFIcreateData(instructionPointer * ip)
{
	// Arguments: argument's index (0-based) on TOS
	// Return: the following values (from bottom to top):
	// The argument's data type, in bits
	// Whether the argument is an array variable
	// 0 if the argument is not a variable, or its number if it is
	// The argument's value at the time the CREATED instruction was called
	// The argument's value now (same as previous if -a was not used)
	fungeCell i;

	if (firstload) {
		ipReverse(ip);
		return;
	}

	i = StackPop(ip->stack);

	StackPush(ip->stack, ick_c_i_width(i));
	StackPush(ip->stack, ick_c_i_isarray(i));
	StackPush(ip->stack, ick_c_i_varnumber(i));
	StackPush(ip->stack, ick_c_i_value(i));
	StackPush(ip->stack, ick_c_i_getvalue(i));
}

// F - FORGET NEXT stack entries equal to top of stack
static void FingerIFFIforget(instructionPointer * ip)
{
	fungeCell f;

	if (firstload) {
		ipReverse(ip);
		return;
	}

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ipReverse(ip);
		return;
	}

	f = StackPop(ip->stack);

	if (f > 81 || f < 0) f = 81;

	ick_iffi_forgetcount = f;
}

// G - Get the value of an INTERCAL scalar variable
static void FingerIFFIvarGet(instructionPointer * ip)
{
	// arguments: var number on TOS
	// var numbers are positive for onespot, negative for twospot
	// return: the value of the variable

	fungeCell v;

	if (firstload) {
		ipReverse(ip);
		return;
	}

	v = StackPop(ip->stack);

	if (v == 0) {
		ipReverse(ip);
	} else if (v > 0) {
		StackPush(ip->stack, ick_getonespot(v));
	} else {
		StackPush(ip->stack, ick_gettwospot(-v));
	}
}

// L - Use top of stack as a line label for this point
static void FingerIFFIlabel(instructionPointer * ip)
{
	fungeCell l;

	if (firstload) {
		ipReverse(ip);
		return;
	}

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
	fungeCell l;

	if (firstload) {
		ipReverse(ip);
		return;
	}

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
	fungeCell f;

	if (firstload) {
		ipReverse(ip);
		return;
	}

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

// S - Set the value of an INTERCAL scalar variable
static void FingerIFFIvarSet(instructionPointer * ip)
{
	// arguments: var number on TOS, new value beneath it
	// var numbers are positive for onespot, negative for twospot
	// return: the value of the variable

	fungeCell v, d;

	if (firstload) {
		ipReverse(ip);
		return;
	}

	v = StackPop(ip->stack);
	d = StackPop(ip->stack);

	if (v == 0) {
		ipReverse(ip);
	} else if (v > 0) {
		ick_setonespot(v, d);
	} else {
		ick_settwospot(-v, d);
	}
}

// V - Assign to a CREATEd instruction argument
static void FingerIFFIargSet(instructionPointer * ip)
{
	// arguments: 0-based argument index on TOS, new value beneath it
	// note that this is a NOP unless -a was used when compiling
	fungeCell i, d;

	if (firstload) {
		ipReverse(ip);
		return;
	}

	i = StackPop(ip->stack);
	d = StackPop(ip->stack);

	ick_c_i_setvalue(i, d);
}

// X - In markmode NEXT FROM the top of stack
static void FingerIFFInextFrom(instructionPointer * ip)
{
	fungeCell l;

	l = StackPop(ip->stack);

	if (ick_iffi_inmarkmode) {
		ick_iffi_breakloop = 1;
		ick_iffi_linelabel = l;
		ick_iffi_nextingfrom = 1;
	}
}

// Y - Marks the end of initialisation
static void FingerIFFIyield(instructionPointer * ip)
{
	ick_iffi_breakloop = firstload;
	if (! firstload)
		ipReverse(ip);
	firstload = false;
}

bool FingerIFFIload(instructionPointer * ip)
{
	ManagerAddOpcode(IFFI,  'A', create)
	ManagerAddOpcode(IFFI,  'C', comeFrom)
	ManagerAddOpcode(IFFI,  'D', createData)
	ManagerAddOpcode(IFFI,  'F', forget)
	ManagerAddOpcode(IFFI,  'G', varGet)
	ManagerAddOpcode(IFFI,  'L', label)
	ManagerAddOpcode(IFFI,  'M', marker)
	ManagerAddOpcode(IFFI,  'N', next)
	ManagerAddOpcode(IFFI,  'R', resume)
	ManagerAddOpcode(IFFI,  'S', varSet)
	ManagerAddOpcode(IFFI,  'V', argSet)
	ManagerAddOpcode(IFFI,  'X', nextFrom)
	ManagerAddOpcode(IFFI,  'Y', yield)
	return true;
}
