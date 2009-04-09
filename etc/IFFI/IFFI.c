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

#ifdef HAVE_clock_gettime
#  include <time.h>
#else
#  include <sys/time.h>
#endif

static instructionPointer *iffiIP = NULL;

static bool firstload = true;

// Communication functions with the ecto_b98 expansion library
void ick_save_ip_pos_delta(struct ick_ipposdeltatype* ippd)
{
	ippd->ix = iffiIP->position.x;
	ippd->iy = iffiIP->position.y;
	ippd->dx = iffiIP->delta.x;
	ippd->dy = iffiIP->delta.y;
}

void ick_restore_ip_pos_delta(const struct ick_ipposdeltatype* ippd)
{
	iffiIP->position.x = ippd->ix;
	iffiIP->position.y = ippd->iy;
	iffiIP->delta.x = ippd->dx;
	iffiIP->delta.y = ippd->dy;
}

void ick_interpreter_run(void)
{
	if (!fungespace_create()) {
		perror("Couldn't create funge space!?");
		exit(EXIT_FAILURE);
	}
#if (CFUNGE_API_VERSION < 2)
	fungespace_load_string(ick_iffi_befungeString);
#else
	fungespace_load_string(ick_iffi_befungeString, strlen((const char*)ick_iffi_befungeString));
#endif
	iffiIP = ip_create();
	if (iffiIP == NULL) {
		perror("Couldn't create instruction pointer!?");
		exit(EXIT_FAILURE);
	}
	{
#ifdef HAVE_clock_gettime
		struct timespec tv;
		if (clock_gettime(CLOCK_REALTIME, &tv)) {
			perror("Couldn't get time of day?!");
			exit(EXIT_FAILURE);
		}
		// Set up randomness
		srandom(tv.tv_nsec);
#else
		struct timeval tv;
		if (gettimeofday(&tv, NULL)) {
			perror("Couldn't get time of day?!");
			exit(EXIT_FAILURE);
		}
		// Set up randomness
		srandom(tv.tv_usec);
#endif
	}
	if(ick_printflow) setting_trace_level=9;
	ick_interpreter_main_loop();
}

void ick_iffi_interpreter_one_iteration(void)
{
	funge_cell opcode;
	opcode = fungespace_get(&iffiIP->position);

	if (setting_trace_level > 8) {
		fprintf(stderr, "x=%" FUNGECELLPRI " y=%" FUNGECELLPRI
				": %c (%" FUNGECELLPRI ")\n",
				iffiIP->position.x, iffiIP->position.y, (char)opcode, opcode);
		stack_print_top(iffiIP->stack);
	} else if (setting_trace_level > 3) {
		fprintf(stderr, "x=%" FUNGECELLPRI " y=%" FUNGECELLPRI
				": %c (%" FUNGECELLPRI ")\n",
				iffiIP->position.x, iffiIP->position.y, (char)opcode, opcode);
	} else if (setting_trace_level > 2)
		fprintf(stderr, "%c", (char)opcode);

	execute_instruction(opcode, iffiIP);
	if (iffiIP->needMove)
		ip_forward(iffiIP);
	else
		iffiIP->needMove = true;
}

// A - CREATE a new INTERCAL instruction
static void finger_IFFI_create(instructionPointer * ip)
{
    // arguments: line number on TOS, signature as 0gnirts beneath it
	funge_cell l = stack_pop(ip->stack);
	unsigned char * restrict str = stack_pop_string(ip->stack, NULL);
	ick_create((const char*)str, l);
}

// C - In markmode COME FROM the top of stack
static void finger_IFFI_come_from(instructionPointer * ip)
{
	funge_cell l;

	l = stack_pop(ip->stack);

	if (ick_iffi_inmarkmode) {
		ick_iffi_breakloop = 1;
		ick_iffi_linelabel = l;
		ick_iffi_comingfrom = 1;
	}
}

// D - Push information about a CREATED instruction argument
static void finger_IFFI_create_data(instructionPointer * ip)
{
	// Arguments: argument's index (0-based) on TOS
	// Return: the following values (from bottom to top):
	// The argument's data type, in bits
	// Whether the argument is an array variable
	// 0 if the argument is not a variable, or its number if it is
	// The argument's value at the time the CREATED instruction was called
	// The argument's value now (same as previous if -a was not used)
	funge_cell i;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	i = stack_pop(ip->stack);

	stack_push(ip->stack, ick_c_i_width(i));
	stack_push(ip->stack, ick_c_i_isarray(i));
	stack_push(ip->stack, ick_c_i_varnumber(i));
	stack_push(ip->stack, ick_c_i_value(i));
	stack_push(ip->stack, ick_c_i_getvalue(i));
}

// F - FORGET NEXT stack entries equal to top of stack
static void finger_IFFI_forget(instructionPointer * ip)
{
	funge_cell f;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ip_reverse(ip);
		return;
	}

	f = stack_pop(ip->stack);

	if (f > 81 || f < 0) f = 81;

	ick_iffi_forgetcount = f;
}

// G - Get the value of an INTERCAL scalar variable
static void finger_IFFI_var_get(instructionPointer * ip)
{
	// arguments: var number on TOS
	// var numbers are positive for onespot, negative for twospot
	// return: the value of the variable

	funge_cell v;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	v = stack_pop(ip->stack);

	if (v == 0) {
		ip_reverse(ip);
	} else if (v > 0) {
		stack_push(ip->stack, ick_getonespot(v));
	} else {
		stack_push(ip->stack, ick_gettwospot(-v));
	}
}

// L - Use top of stack as a line label for this point
static void finger_IFFI_label(instructionPointer * ip)
{
	funge_cell l;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	l = stack_pop(ip->stack);

	ick_iffi_breakloop = 1;
	ick_iffi_linelabel = l;
	ick_iffi_sucking = 1;
}

// M - Marks points where the code can be entered from outside
static void finger_IFFI_marker(instructionPointer * ip)
{
	(void) ip;
	ick_iffi_breakloop = ick_iffi_inmarkmode;
}

// N - Try to NEXT to the line labelled with the top of stack
static void finger_IFFI_next(instructionPointer * ip)
{
	funge_cell l;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ip_reverse(ip);
		return;
	}

	l = stack_pop(ip->stack);

	ick_iffi_breakloop = 1;
	ick_iffi_linelabel = l;
	ick_iffi_nexting = 1;
}

// R - RESUME to the top-of-stackth NEXT stack entry
static void finger_IFFI_resume(instructionPointer * ip)
{
	funge_cell f;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	if (ick_iffi_inmarkmode) { /* this is an error in the user's code */
		ip_reverse(ip);
		return;
	}

	f = stack_pop(ip->stack);

	if (f > 81 || f < 0) f = 81;

	ick_iffi_forgetcount = f;

	ick_iffi_resuming = 1;

	ick_iffi_breakloop = 1;
}

// S - Set the value of an INTERCAL scalar variable
static void finger_IFFI_var_set(instructionPointer * ip)
{
	// arguments: var number on TOS, new value beneath it
	// var numbers are positive for onespot, negative for twospot
	// return: the value of the variable

	funge_cell v, d;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	v = stack_pop(ip->stack);
	d = stack_pop(ip->stack);

	if (v == 0) {
		ip_reverse(ip);
	} else if (v > 0) {
		ick_setonespot(v, d);
	} else {
		ick_settwospot(-v, d);
	}
}

// V - Assign to a CREATEd instruction argument
static void finger_IFFI_arg_set(instructionPointer * ip)
{
	// arguments: 0-based argument index on TOS, new value beneath it
	// note that this is a NOP unless -a was used when compiling
	funge_cell i, d;

	if (firstload) {
		ip_reverse(ip);
		return;
	}

	i = stack_pop(ip->stack);
	d = stack_pop(ip->stack);

	ick_c_i_setvalue(i, d);
}

// X - In markmode NEXT FROM the top of stack
static void finger_IFFI_next_from(instructionPointer * ip)
{
	funge_cell l;

	l = stack_pop(ip->stack);

	if (ick_iffi_inmarkmode) {
		ick_iffi_breakloop = 1;
		ick_iffi_linelabel = l;
		ick_iffi_nextingfrom = 1;
	}
}

// Y - Marks the end of initialisation
static void finger_IFFI_yield(instructionPointer * ip)
{
	ick_iffi_breakloop = firstload;
	if (! firstload)
		ip_reverse(ip);
	firstload = false;
}

bool finger_IFFI_load(instructionPointer * ip)
{
	manager_add_opcode(IFFI,  'A', create)
	manager_add_opcode(IFFI,  'C', come_from)
	manager_add_opcode(IFFI,  'D', create_data)
	manager_add_opcode(IFFI,  'F', forget)
	manager_add_opcode(IFFI,  'G', var_get)
	manager_add_opcode(IFFI,  'L', label)
	manager_add_opcode(IFFI,  'M', marker)
	manager_add_opcode(IFFI,  'N', next)
	manager_add_opcode(IFFI,  'R', resume)
	manager_add_opcode(IFFI,  'S', var_set)
	manager_add_opcode(IFFI,  'V', arg_set)
	manager_add_opcode(IFFI,  'X', next_from)
	manager_add_opcode(IFFI,  'Y', yield)
	return true;
}
