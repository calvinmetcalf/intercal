/*****************************************************************************

NAME 
    unravel.c -- multithreading and backtracking support for C-INTERCAL

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

***************************************************************************/

/* Notes about authorship. This entire file was written by Alex Smith (AIS),
   but with reference to an earlier implementation of Threaded INTERCAL written
   by Malcom Ryan. I have not reused any of his C code (two of his INTERCAL
   programs are in the pit), but it was a useful source of inspiration, even
   though I did it a completely different and less efficient way (I think we
   handled mutiple COME FROM recognition in much the same way). This file only
   contains functions necessary for multithreading; some of the code is stored
   in feh.c (stored in strings), or in perpetrate.c (again stored in strings).
   The information about the required syntax is in ick.y and lexer.l.
   A small amount of the multithread code is in ick-wrap.c, although as that
   file is copied almost verbatim (although it's quite a big 'almost') into
   all ick output, it's guarded there by #if...#endif. The whole multithread
   idea started on alt.lang.intercal when it was noticed that multiple COME
   FROMs aiming at the same line were hard to interpret, but I would like
   to thank Malcom Ryan for drafting the Threaded Intercal standard and a
   reference compiler, and for turning the idea into a concrete set of rules.
   (The standard itself carries a note saying that a revision (use of AGAIN)
   was inspired by Kyle Dean, so I'd like to mention that here too, and I also
   revised it (so that ABSTAIN/REINSTATE were preserved through backtracking).)
   In terms of the quality of the code, Malcom Ryan's code was clearly better
   for multithread programs, but required rewriting most of the compiler. As
   clearly it would be unfair to subject the INTERCAL community at large to
   the risk of silent multithreading by default, I decided multithreading
   should be off by default (but ONCE/AGAIN should be legal). So this code is
   designed to interfere as little as possible with non-multithreaded output.
   There are two versions of libick compiled by the Makefile in this
   distribution: libick.a, the non-multithread version, and libickmt.a, the
   multithread version. The only difference between them is this file,
   unravel.c, which is multithread only, so it seemed a good place for this
   comment. To see what the multithreading looks like in the object code,
   see degenerated code or feh.c. */

/* This file also implements Backtracking INTERCAL, which is turned on with
   the same command-line option (-m). This implementation differs from
   Malcom Ryan's standard in that abstention stati are not restored upon
   GOING BACK. As far as I know, this is the only Backtracking INTERCAL
   implementation currently available, although the code depends on the
   multithreading code (which is why -m is neccesary). */

/* Note about control structures: Crazy INTERCAL control structures call for
   crazy C control structures. setjmp/longjmp is probably C's weirdest
   control structure (it's sort of like a time-reversed computed COME FROM).
   I've used them to implement both multithreading and computed COME FROMming.

   Worried that this code might actually be copied by a casual C programmer, I
   have the following warning:

   setjmp/longjmp is worse than goto in terms of spaghettification of code.
   It is strictly to be used only when neccesary, or when writing INTERCAL
   compilers. They also lead to weird portability problems when you don't obey
   all the restrictions put on them by the C standard. For instance,
   if(setjmp(cjb)!=0) puts("Jumped"); is portable, but
   if(setjmp(cjb)) puts("Jumped"); is not, despite seeming to mean exactly the
   same thing. Also, the address of setjmp can't be taken. Semantically,
   setjmp/longjmp lets you do the same crazy things like goto, like jump into
   the middle of a loop, except in this case the loop can be in a different
   function, or even in a different file. unravel.c has no qualms about
   jumping straight into the middle of an if statement in a degenerated C
   program, even if said degenerated C program didn't exist when libickmt.a was
   compiled. */

/* LINTLIBRARY */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#define MULTITHREAD 1

#include "sizes.h"
#include "abcess.h"
#include "lose.h"

int gonebackto; /* Is the choicepoint reached by GOING BACK? */
static int choicing = 0; /* Is a multithread function being used to do
			    backtracking processing? */
jmp_buf btjb; /* The backtracking jmp_buf */
static ickthread* topchoice; /* Top of the choicepoint stack */
extern int lineno; /* Keep track of error-message lines */

extern int printflow; /* from arrgghh.c; a debug option */

/**********************************************************************
 *
 * This functions deal with choicepoints, which are implemented as
 * ickthread objects. choicepoint creates a choicepoint or marks a
 * choicepoint as stale; choiceahead eliminates a choicepoint;
 * choiceback eliminates a stale choicepoint or returns to a fresh
 * choicepoint. 
 *
 *********************************************************************/

void choicepoint(void)
{
  ickthread* oldprev, *oldtc;
  if(gonebackto)
  { /* Create a stale choicepoint */
    if(printflow) printf("(back)");
    oldtc = topchoice;
    topchoice = (ickthread*) malloc(sizeof(ickthread));
    if(!topchoice) lose(E991, lineno, (char*) NULL);
    topchoice->next = oldtc;
    topchoice->stale = 1;
    topchoice->refcount = 1; /* At the moment, this is the only
				thread looking at this choicepoint */
    return;
  }
  else
  { /* Create a new choicepoint */
    if(printflow) printf("(maybe)");
    oldprev = ickmt_prev;
    choicing = 1;
    multicome1(lineno,btjb); /* Duplicate data */
    choicing = 0;
    oldprev->next = ickmt_cur;
    ickmt_prev->choicepoint = topchoice;
    topchoice = ickmt_prev;
    ickmt_prev = oldprev;
    topchoice->stale = 0;
    topchoice->refcount = 1;
    /* So in other words, we've duplicated the current execution
       environment, except for the choicepoint stack, changed the
       duplicate from a thread into a choicepoint, and pushed it
       on top of the choicepoint stack. Its pc is the point in the
       degenerated program where choicepoint was called. */
  }
}

void choiceahead(void)
{
  ickthread* tempthread;
  jmp_buf fakepc;

  if(printflow) printf("(ahead)");
  
  if(!topchoice) lose(E404, lineno, (char*) NULL); /* That's what E404's for */
  /* GO AHEAD with multithreading requires care. The choicepoint should only
     be removed from this thread. topchoice = topchoice->next almost works, but
     is a memory leak. So most of this is garbage-collection. */

  /* If other threads are using the choicepoint, don't free it. */
  if(topchoice->refcount > 1)
  {
    topchoice->refcount--;
    topchoice = topchoice->choicepoint;
    return;
  }

  /* The top choicepoint is not being used by other threads; free it. */

  /* Freeing a stale choicepoint (which contains no data) is easy. */
  if(topchoice->stale)
  {
    tempthread = topchoice;
    topchoice = topchoice->choicepoint;
    free(tempthread);
    return;
  }
  
  /* This code works by converting topchoice back to a thread and placing it
     just before the current thread, and then killing it. First, the data from
     this thread, apart from the choicepoint stack, must be saved. */
  choicing = 1;
  if(setjmp(fakepc) == 0)
  {
    memcpy(ickmt_cur->pc,fakepc,sizeof(jmp_buf));
    nextthread(fakepc, -1, 5);
  }
  choicing = 0;
  /* That's saved the current thread's data. Now to convert topchoice to a
     thread. */
  tempthread = topchoice->choicepoint;
  ickmt_prev->next = topchoice;
  topchoice->next = ickmt_cur;
  ickmt_cur = topchoice;
  topchoice = tempthread;
  /* Let's load the backtracking data... */
  choicing = 1;
  if(setjmp(fakepc) == 0)
  {
    memcpy(ickmt_cur->pc,fakepc,sizeof(jmp_buf));
    nextthread(fakepc, -1, 6);
  }
  /* only to destroy it! Mwahahahahah! */
  if(setjmp(fakepc) == 0)
  {
    memcpy(ickmt_cur->next->pc,fakepc,sizeof(jmp_buf));
    killthread();
  }
  choicing = 0;
  /* So we've reloaded the original current thread, the original previous
     thread is still correct, topchoice has become topchoice->choicepoint,
     and the original topchoice has disappeared. Mission accomplished. */
}

void choiceback(void)
{
  if(!topchoice) lose(E404, lineno, (char *) NULL);
  if(topchoice->stale) {if(printflow) printf("(back=)"); choiceahead(); return;}
  /* That's two simple cases out of the way (at least once choiceahead's
     been implemented). What we need to do to backtrack is to change
     topchoice to a thread after the current thread (rather than before
     as in the previous two functions), and then kill the current thread.
     (It amuses me that destroying a choicepoint, as in choiceahead(),
     is more complicated than either visiting a choicepoint or creating a
     choicepoint. That's how much work it can take to avoid a memory leak.)
     In this case, choiceback won't return in the normal fashion. */
  if(topchoice->refcount > 1)
  {
    /* The Threaded INTERCAL standard states that if other threads are using
       the choicepoint, a GO BACK should cause the thread it's in to be
       killed. (Of course, if it's stale, it should just have been removed.)*/
    killthread();
    return;
  }
  topchoice->next = ickmt_cur->next;
  ickmt_cur->next = topchoice;
  topchoice = topchoice->choicepoint;
  if(ickmt_cur==ickmt_prev) ickmt_prev = ickmt_cur->next;
  choicing = 2; /* Tells killthread to set it back to 0 */
  killthread();
}

/**********************************************************************
 *
 * This function is called when two COME FROMs reference the same
 * line at runtime. multicome1 is used in a multithread
 * program; it forks the program. For multicome0, see cesspool.c.
 *
 *********************************************************************/

int multicome1(int errlineno, jmp_buf pc)
{
  /* Make a new thread just before the current one. Fake a PC in the
     current thread within this function, change to the new thread, then
     call nextthread. The upshot of all this is that all this thread's
     data is stored in the new thread's state. Then, we have to copy
     all this thread's current data to new locations. */
  static ickthread* newthread;
  jmp_buf fakepc;
  stashbox *isb, *isb2, *isbprev;
  void* tvp;
  array* a;
  int prod, i, j;
  newthread = malloc(sizeof(ickthread));
  if(!newthread) lose(E991, errlineno, (char *) NULL);
  ickmt_prev->next = newthread;
  newthread->next = ickmt_cur;
  ickmt_cur = newthread;

  if(printflow && !choicing) printf("(fork)");

  if(setjmp(fakepc) == 0)
  {
    memcpy(newthread->next->pc,fakepc,sizeof(jmp_buf));
    nextthread(pc, -1, 1);
  }
  /* So on the previous line: Save the value of pc as the program
     counter of the new thread, and give a fake value for the
     program value of the current thread (so it returns here,
     not anywhere in the degenerated program). All that remains
     is to duplicate all the data stored through pointers in the 'old'
     thread (nextthread has changed ickmt_cur to point at the 'old'
     thread). The original memory pointed to by these pointers is in
     use storing the values in the 'new' thread, so the 'old' thread
     needs new copies that it can modify independently. */

  /* duplicate variables, forget indicators */
  i = onespotcount;
  tvp = onespots;
  onespots = malloc(i * sizeof *onespots);
  if(!onespots) lose(E991, errlineno, (char *) NULL);
  memcpy(onespots, tvp, i * sizeof *onespots);
  i = twospotcount;
  tvp = twospots;
  twospots = malloc(i * sizeof *twospots);
  if(!twospots) lose(E991, errlineno, (char *) NULL);
  memcpy(twospots, tvp, i * sizeof *twospots);
  i = tailcount;
  tvp = tails;
  tails = malloc(i * sizeof *tails);
  if(!tails) lose(E991, errlineno, (char *) NULL);
  memcpy(tails, tvp, i * sizeof *tails);
  i = hybridcount;
  tvp = hybrids;
  hybrids = malloc(i * sizeof *hybrids);
  if(!hybrids) lose(E991, errlineno, (char *) NULL);
  memcpy(hybrids, tvp, i * sizeof *hybrids);
  i = onespotcount;
  tvp = oneforget;
  oneforget = malloc(i * sizeof *oneforget);
  if(!oneforget) lose(E991, errlineno, (char *) NULL);
  memcpy(oneforget, tvp, i * sizeof *oneforget);
  i = twospotcount;
  tvp = twoforget;
  twoforget = malloc(i * sizeof *twoforget);
  if(!twoforget) lose(E991, errlineno, (char *) NULL);
  memcpy(twoforget, tvp, i * sizeof *twoforget);
  i = tailcount;
  tvp = tailforget;
  tailforget = malloc(i * sizeof *tailforget);
  if(!tailforget) lose(E991, errlineno, (char *) NULL);  
  memcpy(tailforget, tvp, i * sizeof *tailforget);
  i = hybridcount;
  tvp = hyforget;
  hyforget = malloc(i * sizeof *hyforget);
  if(!hyforget) lose(E991, errlineno, (char *) NULL);  
  memcpy(hyforget, tvp, i * sizeof *hyforget);

  /* duplicate NEXT stack */
  tvp = next;
  next = malloc(MAXNEXT * sizeof *next);
  if(!next) lose(E991, errlineno, (char *) NULL);  
  memcpy(next, tvp, MAXNEXT * sizeof *next);

  /* duplicate data stored in arrays */
  j = tailcount;
  while(j--)
  {
    a = tails+j; /* &(tails[j]) */
    if(!a->rank||!a->dims) continue; /* don't duplicate a deallocated array */
    tvp = a->dims;
    /* Much of this code is paraphrased from the stashbox-copying code below,
       which was in turn paraphrased from a section in cesspool.c I didn't
       write. So any errors in this code are probably mine, but the algorithm
       isn't. */
    a->dims = malloc(a->rank * sizeof(unsigned int));
    memcpy(a->dims, tvp, a->rank * sizeof(unsigned int));
    prod = !!a->rank;
    i = a->rank;
    while(i--) prod *= a->dims[i];
    tvp = a->data.tail;
    a->data.tail = malloc(prod * sizeof(type16));
    if(!a->data.tail) lose(E991, errlineno, (char *) NULL);
    memcpy(a->data.tail, tvp, prod * sizeof(type16));
  }
  j = hybridcount;
  while(j--)
  {
    a = hybrids+j; /* &(hybrids[j]) */
    if(!a->rank||!a->dims) continue; /* don't duplicate a deallocated array */
    tvp = a->dims;
    /* Much of this code is paraphrased from the stashbox-copying code below,
       which was in turn paraphrased from a section in cesspool.c I didn't
       write. So any errors in this code are probably mine, but the algorithm
       isn't. */
    a->dims = malloc(a->rank * sizeof(unsigned int));
    memcpy(a->dims, tvp, a->rank * sizeof(unsigned int));
    prod = !!a->rank;
    i = a->rank;
    while(i--) prod *= a->dims[i];
    tvp = a->data.hybrid;
    a->data.hybrid = malloc(prod * sizeof(type32));
    if(!a->data.hybrid) lose(E991, errlineno, (char *) NULL);
    memcpy(a->data.hybrid, tvp, prod * sizeof(type32));
  }

  /* allow for multithreading with choicepoints on the stack */
  if(!choicing && topchoice)
  {
    ickthread* icktp = topchoice;
    while(icktp)
    {
      icktp->refcount++;
      icktp = icktp->choicepoint; /* next choicepoint on the stack */
    }
    /* The old thread's and new thread's choicepoint stack share
       memory. This is pretty much necessary to handle backtracking
       past a fork correctly. */
  }
  
  /* duplicate stashbox */
  isb2 = first;
  isbprev = (stashbox*)NULL;
  while(isb2)
  {
    isb = (stashbox*)malloc(sizeof(stashbox));
    if(!isb) lose(E991, errlineno, (char *) NULL);
    memcpy(isb,isb2,sizeof(stashbox));
    if(isbprev) isbprev->next = isb;
    isbprev = isb;
    if(isb2==first) first = isb; /* change first only the first time round */
    if(isb->type == ONESPOT || isb->type == TWOSPOT)
    { /* all copying already done */
      isb2 = isb->next;
      continue;
    }
    /* Copy the stashed array. Much of this code is paraphrased from some
       code in cesspool.c. In fact, it's the same, with a few idioms added
       and variable names changed. So, although it's in this file, I can't
       take the credit for it. */
    isb->save.a = (array*)malloc(sizeof(array));
    if(!isb->save.a) lose(E991, errlineno, (char *) NULL);
    isb->save.a->rank = isb2->save.a->rank;
    isb->save.a->dims = (unsigned int*)malloc(isb2->save.a->rank *
					      sizeof(unsigned int));
    if(!isb->save.a->dims) lose(E991, errlineno, (char *) NULL);
    memcpy(isb->save.a->dims, isb2->save.a->dims,
	   isb2->save.a->rank * sizeof(unsigned int));
    prod = !!isb2->save.a->rank; /* I use this idiom often enough in the
				    code produced by my optimizer that I
				    may as well use it here. */
    i = isb2->save.a->rank;
    while(i--) prod *= isb2->save.a->dims[i];
    if(isb2->type == TAIL)
    {
      isb->save.a->data.tail = (type16*)malloc(prod * sizeof(type16));
      if(!isb->save.a->data.tail) lose(E991, errlineno, (char *) NULL);
      memcpy(isb->save.a->data.tail, isb2->save.a->data.tail,
	     prod * sizeof(type16));
    }
    else
    {
      isb->save.a->data.hybrid = (type32*)malloc(prod * sizeof(type32));
      if(!isb->save.a->data.hybrid) lose(E991, errlineno, (char *) NULL);
      memcpy(isb->save.a->data.hybrid, isb2->save.a->data.hybrid,
	     prod * sizeof(type32));      
    }
    isb2 = isb->next;
  }
  return 1; /* Tell the degenerated program to look for yet
	       another COME FROM */
}

/**********************************************************************
 *
 * These functions do the multithreading, using setjmp and longjmp
 * to save the program counter. ickmtinit sets up the first
 * thread, and nextthread changes to the next thread in the
 * sequence. (Note that nextthread rarely actually returns). The
 * code makes each command atomic, so that ONCE and AGAIN appear
 * to the user to be atomic operations.
 *
 *********************************************************************/

ickthread* ickmt_cur; /* define ickmt_cur */
ickthread* ickmt_prev; /* define ickmt_prev */

void ickmtinit(void)
{
  ickmt_cur = malloc(sizeof(ickthread));
  if(!ickmt_cur) lose(E991, 1, (char *) NULL);
  ickmt_prev = ickmt_cur;
  ickmt_cur->next = ickmt_cur;
  topchoice = (ickthread*) NULL; /* No choicepoints */
}

/* Destroys the current thread, and switches to the next thread.
   If there are no threads left, terminates the program using exit(0). */
void killthread(void)
{
  static jmp_buf dummy;
  stashbox* isb, *isbi;
  int i;

  if(printflow&&!choicing) printf("(kill)");
  
  /* Deallocate the current thread's data */
  i=tailcount;
  while(i--)
  { /* free tail data */
    if(!tails[i].rank||!tails[i].dims) continue; /* already free */
    free(tails[i].dims);
    free(tails[i].data.tail);
  }
  i=hybridcount;
  while(i--)
  { /* free hybrid data */
    if(!hybrids[i].rank||!hybrids[i].dims) continue; /* already free */
    free(hybrids[i].dims);
    free(hybrids[i].data.hybrid);
  }  
  free(onespots); free(twospots); free(tails); free(hybrids);
  free(oneforget); free(twoforget); free(tailforget); free(hyforget);
  free(next); /* Free NEXT stack */
  if(!choicing) while(topchoice) choiceahead();
  /* The above line will mark each of the choicepoints as no longer being
     used by this thread, and free them if neccesary. */
  isbi = first;
  while(isbi) /* Free stash */
  {
    isb=isbi->next;
    if(isbi->type == TAIL || isbi->type == HYBRID)
    {
      free(isbi->save.a->dims);
      if(isbi->type == TAIL)
	free(isbi->save.a->data.tail);
      else
	free(isbi->save.a->data.hybrid);
    }
    free(isbi);
    isbi=isb;
  }
    
  ickmt_prev->next = ickmt_cur->next;
  if(ickmt_cur->next == ickmt_cur)
  {
    free(ickmt_cur);
    exit(0);
  }
  else
  {
    /* We need to run about half of nextthread. So we pass in a 2 for
       flags and tell it to skip the first half. */
    free(ickmt_cur);
    ickmt_cur = ickmt_prev;
    nextthread(dummy, -1, 2); /* dummy is not used by nextthread */
    lose(E778, -1, (char *) NULL); /* nextthread does not return */
  }
}

/* This function does not return in the normal fashion, but by a nonlocal
   goto to a mysterious location, possibly in multicome1 but possibly in
   the degenerated code. From the point of view of libickmt.a, we're going
   to a piece of code that doesn't even exist yet! The general ugliness of
   INTERCAL multithreading pulls out all the stops when it comes to using
   unusual and confusing C control structures. It is important to remember
   that longjmp clobbers all auto variables that have changed since the
   corresponding setjmp. */

/* In this Threaded INTERCAL implementation, data is saved in a linked ring
   of threads. A thread runs for a command, then all the data pointers are
   changed, saving the old ones. This is an improvement on an earlier attempt
   of mine in which the values themselves were copied, but is still less
   efficient than Malcom Ryan's method (which didn't involve copying things
   about except when forking). However, I justify this by saying that it leaves
   as much existing code as possible untouched, which is helpful for single-
   thread compatibility, modifying the code in feh.c without an understanding
   of multithread issues, and because I'm lazy. It also makes the rest of the
   program shorter. */
void nextthread(jmp_buf pc, int errlineno, int flags)
{ /* flags | 1 means save this thread.
     flags | 2 means load the next thread.
     flags | 4 means don't change thread. */
  if(errlineno > -1 && ickmt_cur->next == ickmt_cur && !choicing) longjmp(pc,1);
  /* If we only have 1 thread, just continue with it. Otherwise: */

  if(!(flags&1)) goto advancethread;
  /* OK, so I've spaghettified this procedure slightly by using goto
     instead of if. But I was so deep into figuring out setjmp/longjmp,
     a goto seemed crystal-clear by comparison. */
  
  /* save variables */
  ickmt_cur->varforget[0] = onespots;
  ickmt_cur->varforget[1] = twospots;
  ickmt_cur->varforget[2] = tails;
  ickmt_cur->varforget[3] = hybrids;
  ickmt_cur->varforget[4] = oneforget;
  ickmt_cur->varforget[5] = twoforget;
  ickmt_cur->varforget[6] = tailforget;
  ickmt_cur->varforget[7] = hyforget;

  /* save NEXT stack */
  ickmt_cur->nextstack = next;
  ickmt_cur->nextpointer = nextindex;

  /* save stashbox */
  ickmt_cur->sb = first;

  /* save choicepoints */
  if(!choicing) ickmt_cur->choicepoint = topchoice;

  /* save comefrom information */
  memcpy(ickmt_cur->cjb, cjb, sizeof(jmp_buf));
  ickmt_cur->ccfc = ccfc;
  ickmt_cur->skipto = skipto;
  
  /* save program counter */
  memcpy(ickmt_cur->pc, pc, sizeof(jmp_buf));
  /* And another thing about setjmp/longjmp. A jmp_buf
     acts like a structure that passes itself around by
     reference. However, it cannot be assigned, although
     just about everything else in C can be, although it
     can be copied with memcpy (what I'm doing in the line
     above - remember it passes itself around by reference).
     Generally speaking, it's some sort of array,
     even though some implementors use a 1-element array.
     The exact representation of jmp_buf is one of the most
     implementation-dependent things in C (I've seen both
     a 1-element array of structure and an int[12].) */

 advancethread:
  /* change thread */
  if(!(flags&4))
  {
    ickmt_prev = ickmt_cur;
    ickmt_cur = ickmt_cur->next;
  }

  if(!(flags&2)) goto returnjmp;

  /* load variables */
  onespots   = ickmt_cur->varforget[0];
  twospots   = ickmt_cur->varforget[1];
  tails      = ickmt_cur->varforget[2];
  hybrids    = ickmt_cur->varforget[3];
  oneforget  = ickmt_cur->varforget[4];
  twoforget  = ickmt_cur->varforget[5];
  tailforget = ickmt_cur->varforget[6];
  hyforget   = ickmt_cur->varforget[7];

  /* load NEXT stack */
  next = ickmt_cur->nextstack;
  nextindex = ickmt_cur->nextpointer;

  /* load choicepoints */
  if(!choicing) topchoice = ickmt_cur->choicepoint;
  
  /* load stashbox */
  first = ickmt_cur->sb;

  /* load comefrom information */
  memcpy(cjb, ickmt_cur->cjb, sizeof(jmp_buf));
  ccfc = ickmt_cur->ccfc;
  skipto = ickmt_cur->skipto;

 returnjmp:
  /* return to the new current thread's program counter */
  if(choicing==2) choicing = 0;
  longjmp(ickmt_cur->pc, 1);
}
