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
   in feh2.c (stored in strings), or in perpet.c (again stored in strings).
   The information about the required syntax is in parser.y and lexer.l.
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
   see degenerated code or feh2.c. */

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
   if(setjmp(ick_cjb)!=0) puts("Jumped"); is portable, but
   if(setjmp(ick_cjb)) puts("Jumped"); is not, despite seeming to mean exactly the
   same thing. Also, the address of setjmp can't be taken. Semantically,
   setjmp/longjmp lets you do the same crazy things like goto, like jump into
   the middle of a loop, except in this case the loop can be in a different
   function, or even in a different file. unravel.c has no qualms about
   jumping straight into the middle of an if statement in a degenerated C
   program, even if said degenerated C program didn't exist when libickmt.a was
   compiled. */

/* LINTLIBRARY */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>

#define MULTITHREAD 1

#include "sizes.h"
#include "abcess.h"
#include "ick_lose.h"

int gonebackto; /* Is the choicepoint reached by GOING BACK? */
static int choicing = 0; /* Is a multithread function being used to do
			    backtracking processing? */
jmp_buf btjb; /* The backtracking jmp_buf */
/* About the annotation 'dependent': topchoice is really 'owned', but the owner keeps
   on changing. So Splint gets less confused if we simply never tell it who the owner
   is. */
/*@null@*/ /*@dependent@*/ static ickthread* topchoice; /* Top of the choicepoint stack */
extern int ick_lineno; /* Keep track of error-message lines */

extern int ick_printflow; /* from arrgghh.c; a debug option */

int weaving=0; /* Weave newly created threads? */

/* Printflow debugging output */
static void fluputs(const char* s)
{
  fprintf(stderr,"%s",s);
  (void) fflush(stderr);
}

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
  int oldweave;
  if(gonebackto)
  { /* Create a stale choicepoint */
    if(ick_printflow) fluputs("(back)");
    oldtc = topchoice;
    /* Suppress the onlytrans warning because we're allocating a member
       of a linked list, which confuses Splint no matter what the list
       is flagged as (because malloc returns 'only' data, but yet that
       data has to be pointed to by other 'only' data in the same list). */
    /*@-onlytrans@*/
    topchoice = (ickthread*) malloc(sizeof(ickthread));
    /*@=onlytrans@*/
    if(!topchoice) ick_lose(IE991, ick_lineno, (const char*) NULL);
    topchoice->choicepoint = oldtc;
    topchoice->stale = 1;
    topchoice->refcount = 1; /* At the moment, this is the only
				thread looking at this choicepoint */
    topchoice->dsi=topchoice;
    topchoice->usesthis=0;
    /* topchoice needn't be completely defined if it's stale */
    /*@-compdef@*/
    return;
    /*@=compdef@*/
  }
  else
  { /* Create a new choicepoint */
    if(ick_printflow) fluputs("(maybe)");
    oldprev = ickmt_prev;
    choicing = 1;
    oldweave = weaving;
    weaving = 0;
    (void) multicome1(ick_lineno,btjb); /* Duplicate data */
    weaving = oldweave;
    choicing = 0;
    oldprev->ick_next = ickmt_cur;
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

  if(!topchoice) ick_lose(IE404, ick_lineno, (const char*) NULL); /* That's what IE404's for */
  /* GO AHEAD with multithreading requires care. The choicepoint should only
     be removed from this thread. topchoice = topchoice->ick_next almost works, but
     is a memory leak. So most of this is garbage-collection. */

  /* If other threads are using the choicepoint, don't free it. */
  if(topchoice->refcount > 1)
  {
    if(ick_printflow) fluputs("(refahead)");
    topchoice->refcount--;
    topchoice = topchoice->choicepoint;
    return;
  }

  /* The top choicepoint is not being used by other threads; free it. */

  /* Freeing a stale choicepoint (which contains no data) is easy. */
  if(topchoice->stale)
  {
    if(ick_printflow) fluputs("(destale)");
    tempthread = topchoice;
    topchoice = topchoice->choicepoint;
    /*@-dependenttrans@*/ /* because it's really owned, not dependent */
    free(tempthread);
    /*@=dependenttrans@*/
    return;
  }

  if(ick_printflow) fluputs("(ahead)");

  /* This code works by converting topchoice back to a thread and placing it
     just before the current thread, and then killing it. First, the data from
     this thread, apart from the choicepoint stack, must be saved. */
  choicing = 1;
  if(setjmp(fakepc) == 0)
  {
    memcpy((void*)ickmt_cur->pc,(const void*)fakepc,sizeof(jmp_buf));
    nextthread(fakepc, -1, 5);
  }
  choicing = 0;
  /* That's saved the current thread's data. Now to convert topchoice to a
     thread. */
  tempthread = topchoice->choicepoint;
  ickmt_prev->ick_next = topchoice;
  topchoice->ick_next = ickmt_cur;
  ickmt_cur = topchoice;
  topchoice = tempthread;
  /* Let's load the backtracking data... */
  choicing = 1;
  if(setjmp(fakepc) == 0)
  {
    memcpy((void *)ickmt_cur->pc,(const void *)fakepc,sizeof(jmp_buf));
    nextthread(fakepc, -1, 6);
  }
  /* only to destroy it! Mwahahahahah! */
  if(setjmp(fakepc) == 0)
  {
    memcpy((void *)ickmt_cur->ick_next->pc,(const void *)fakepc,sizeof(jmp_buf));
    killthread();
  }
  choicing = 0;
  /* So we've reloaded the original current thread, the original previous
     thread is still correct, topchoice has become topchoice->choicepoint,
     and the original topchoice has disappeared. Mission accomplished. */
}

void choiceback(void)
{
  if(!topchoice) ick_lose(IE404, ick_lineno, (const char *) NULL);
  if(topchoice->stale)
  {
    if(ick_printflow) fluputs("(back=)");
    choiceahead();
    return;
  }
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
    if(ick_printflow) fluputs("(desplit)");
    killthread();
    return;
  }
  topchoice->ick_next = ickmt_cur->ick_next;
  ickmt_cur->ick_next = topchoice;
  topchoice = topchoice->choicepoint;
  if(ickmt_cur==ickmt_prev) ickmt_prev = ickmt_cur->ick_next;
  choicing = 2; /* Tells killthread to set it back to 0 */
  killthread();
}

/**********************************************************************
 *
 * This function is called when two COME FROMs reference the same
 * line at runtime. multicome1 is used in a multithread
 * program; it forks the program. For ick_multicome0, see cesspool.c.
 *
 *********************************************************************/

int multicome1(int errlineno, jmp_buf pc)
{
  /* Make a new thread just before the current one. Fake a PC in the
     current thread within this function, change to the new thread, then
     call nextthread. The upshot of all this is that all this thread's
     data is stored in the new thread's state. Then, we have to copy
     all this thread's current data to new locations. */
  ickthread* newthread;
  jmp_buf fakepc;
  ick_stashbox *isb, *isb2, *isbprev;
  void* tvp;
  ick_array* a;
  int prod, i, j;
  newthread = malloc(sizeof(ickthread));
  if(!newthread) ick_lose(IE991, errlineno, (const char *) NULL);
  ickmt_prev->ick_next = newthread;
  newthread->ick_next = ickmt_cur;
  ickmt_cur = newthread;
  newthread->dsi=newthread;
  newthread->usesthis=0;

  if(ick_printflow && !choicing) fluputs("(fork)");

  if(setjmp(fakepc) == 0)
  {
    memcpy((void *)newthread->ick_next->pc,(const void *)fakepc,sizeof(jmp_buf));
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

  if(!weaving)
  {
    /* duplicate variables, forget indicators */
    i = onespotcount;
    tvp = ick_onespots;
    ick_onespots = malloc(i * sizeof *ick_onespots);
    if(!ick_onespots) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_onespots, tvp, i * sizeof *ick_onespots);
    if(ick_oo_onespots)
    {
      tvp = ick_oo_onespots;
      ick_oo_onespots = malloc(i * sizeof *ick_oo_onespots);
      if(!ick_oo_onespots) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(ick_oo_onespots, tvp, i * sizeof *ick_oo_onespots);
    }
    i = twospotcount;
    tvp = ick_twospots;
    ick_twospots = malloc(i * sizeof *ick_twospots);
    if(!ick_twospots) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_twospots, tvp, i * sizeof *ick_twospots);
    if(ick_oo_twospots)
    {
      tvp = ick_oo_twospots;
      ick_oo_twospots = malloc(i * sizeof *ick_oo_twospots);
      if(!ick_oo_twospots) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(ick_oo_twospots, tvp, i * sizeof *ick_oo_twospots);
    }
    i = tailcount;
    tvp = ick_tails;
    ick_tails = malloc(i * sizeof *ick_tails);
    if(!ick_tails) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_tails, tvp, i * sizeof *ick_tails);
    i = hybridcount;
    tvp = ick_hybrids;
    ick_hybrids = malloc(i * sizeof *ick_hybrids);
    if(!ick_hybrids) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_hybrids, tvp, i * sizeof *ick_hybrids);
    i = onespotcount;
    tvp = ick_oneforget;
    ick_oneforget = malloc(i * sizeof *ick_oneforget);
    if(!ick_oneforget) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_oneforget, tvp, i * sizeof *ick_oneforget);
    i = twospotcount;
    tvp = ick_twoforget;
    ick_twoforget = malloc(i * sizeof *ick_twoforget);
    if(!ick_twoforget) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_twoforget, tvp, i * sizeof *ick_twoforget);
    i = tailcount;
    tvp = ick_tailforget;
    ick_tailforget = malloc(i * sizeof *ick_tailforget);
    if(!ick_tailforget) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_tailforget, tvp, i * sizeof *ick_tailforget);
    i = hybridcount;
    tvp = ick_hyforget;
    ick_hyforget = malloc(i * sizeof *ick_hyforget);
    if(!ick_hyforget) ick_lose(IE991, errlineno, (const char *) NULL);
    memcpy(ick_hyforget, tvp, i * sizeof *ick_hyforget);

  /* duplicate data stored in arrays */
    j = tailcount;
    tvp = NULL;
    while(j--)
    {
      a = ick_tails+j; /* &(ick_tails[j]) */
      if(!a->rank||!a->dims) continue; /* don't duplicate a deallocated ick_array */
      tvp = a->dims;
      /* Much of this code is paraphrased from the ick_stashbox-copying code below,
	 which was in turn paraphrased from a section in cesspool.c I didn't
	 write. So any errors in this code are probably mine, but the algorithm
	 isn't. */
      a->dims = malloc(a->rank * sizeof *(a->dims));
      if(a->dims == NULL) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(a->dims, tvp, a->rank * sizeof *(a->dims));
      prod = (int)!!a->rank;
      i = (int)a->rank;
      while(i--) prod *= a->dims[i];
      /*@-mustfreeonly@*/ /* how on earth did a->dims become only? */
      tvp = a->data.tail;
      /*@=mustfreeonly@*/
      a->data.tail = malloc(prod * sizeof(ick_type16));
      if(!a->data.tail) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(a->data.tail, tvp, prod * sizeof(ick_type16));
      /*@-mustfreeonly@*/ /* likewise. This isn't only, honest! */
      tvp = NULL;
      /*@=mustfreeonly@*/
    }
    j = hybridcount;
    while(j--)
    {
      a = ick_hybrids+j; /* &(ick_hybrids[j]) */
      if(!a->rank||!a->dims) continue; /* don't duplicate a deallocated ick_array */
      tvp = a->dims;
      /* Much of this code is paraphrased from the ick_stashbox-copying code below,
	 which was in turn paraphrased from a section in cesspool.c I didn't
	 write. So any errors in this code are probably mine, but the algorithm
	 isn't. */
      a->dims = malloc(a->rank * sizeof(*(a->dims)));
      if(!a->dims) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(a->dims, tvp, a->rank * sizeof(*(a->dims)));
      prod = (int)!!a->rank;
      i = (int)a->rank;
      while(i--) prod *= a->dims[i];
      /*@-mustfreeonly@*/
      tvp = a->data.hybrid;
      /*@=mustfreeonly@*/
      a->data.hybrid = malloc(prod * sizeof(ick_type32));
      if(!a->data.hybrid) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(a->data.hybrid, tvp, prod * sizeof(ick_type32));
      /*@-mustfreeonly@*/
      tvp=NULL;
      /*@=mustfreeonly@*/
    }

    /* duplicate ick_stashbox */
    isb2 = ick_first;
    isbprev = (ick_stashbox*)NULL;
    while(isb2)
    {
      isb = (ick_stashbox*)malloc(sizeof(ick_stashbox));
      if(!isb) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(isb,isb2,sizeof(ick_stashbox));
      if(isbprev) isbprev->ick_next = isb;
      isbprev = isb;
      if(isb2==ick_first) ick_first = isb; /* change ick_first only the ick_first time round */
      if(isb->type == ick_ONESPOT || isb->type == ick_TWOSPOT)
      { /* all copying already done */
	isb2 = isb->ick_next;
	continue;
      }
      /* Copy the stashed ick_array. Much of this code is paraphrased from some
	 code in cesspool.c. In fact, it's the same, with a few idioms added
	 and variable names changed. So, although it's in this file, I can't
	 take the credit for it. */
      isb->save.a = (ick_array*)malloc(sizeof(ick_array));
      if(!isb->save.a) ick_lose(IE991, errlineno, (const char *) NULL);
      assert(isb2 != NULL); /* we already said that in the while condition,
			       so it's surprising that Splint needs this hint */
      isb->save.a->rank = isb2->save.a->rank;
      isb->save.a->dims = malloc(isb2->save.a->rank *
				 sizeof(*(isb2->save.a->dims)));
      if(!isb->save.a->dims) ick_lose(IE991, errlineno, (const char *) NULL);
      memcpy(isb->save.a->dims, isb2->save.a->dims,
	     isb2->save.a->rank * sizeof(*(isb2->save.a->dims)));
      prod = (int)!!isb2->save.a->rank;
      /* I use this idiom often enough in the code produced by my optimizer that I
	 may as well use it here. */
      i = (int)isb2->save.a->rank;
      while(i--) prod *= isb2->save.a->dims[i];
      if(isb2->type == ick_TAIL)
      {
	isb->save.a->data.tail = (ick_type16*)malloc(prod * sizeof(ick_type16));
	if(!isb->save.a->data.tail) ick_lose(IE991, errlineno, (const char *) NULL);
	memcpy(isb->save.a->data.tail, isb2->save.a->data.tail,
	       prod * sizeof(ick_type16));
      }
      else
      {
	isb->save.a->data.hybrid = (ick_type32*)malloc(prod * sizeof(ick_type32));
	if(!isb->save.a->data.hybrid) ick_lose(IE991, errlineno, (const char *) NULL);
	memcpy(isb->save.a->data.hybrid, isb2->save.a->data.hybrid,
	       prod * sizeof(ick_type32));
      }
      isb2 = isb->ick_next;
    }
  }
  else /* we are weaving, reference the old current thread */
  {
    ickthread* tempthread;
    if(ick_printflow) fluputs("(weave)");
    /* Sanity check to make sure that the threads are arranged correctly */
    if(newthread->ick_next!=ickmt_cur) ick_lose(IE778, errlineno, (const char *) NULL);
    tempthread=newthread->ick_next; /* the old current thread */
    while(tempthread->usesthis) tempthread=tempthread->usesthis;
    newthread->dsi=tempthread->dsi;
    tempthread->usesthis=newthread;
  }
  /* duplicate NEXT stack */
  tvp = ick_next;
  ick_next = malloc(ick_MAXNEXT * sizeof *ick_next);
  if(!ick_next) ick_lose(IE991, errlineno, (const char *) NULL);
  memcpy(ick_next, tvp, ick_MAXNEXT * sizeof *ick_next);

  /* allow for multithreading with choicepoints on the stack */
  if(!choicing && topchoice != NULL)
  {
    ickthread* icktp = topchoice;
    while(icktp)
    {
      icktp->refcount++;
      icktp = icktp->choicepoint; /* ick_next choicepoint on the stack */
    }
    /* The old thread's and new thread's choicepoint stack share
       memory. This is pretty much necessary to handle backtracking
       past a fork correctly. */
  }

  /* The nullstate annotation is because Splint doesn't notice that all
     the mallocs are guarded by lines that error out permanently if they
     fail. The mustfreefresh annotation is because Splint doesn't realise
     that I've stored newthread in a pointer buried in the linked list of
     threads, and so Splint assumes that it's memory-leaking. */
  /*@-nullstate@*/ /*@-mustfreefresh@*/
  return 1; /* Tell the degenerated program to look for yet
	       another COME FROM */
  /*@=nullstate@*/ /*@=mustfreefresh@*/
}

/**********************************************************************
 *
 * These functions do the multithreading, using setjmp and longjmp
 * to save the program counter. ickmtinit sets up the ick_first
 * thread, and nextthread changes to the ick_next thread in the
 * sequence. (Note that nextthread rarely actually returns). The
 * code makes each command atomic, so that ONCE and AGAIN appear
 * to the user to be atomic operations.
 *
 *********************************************************************/

/*@partial@*/ /*@dependent@*/ ickthread* ickmt_cur; /* define ickmt_cur */
/*@partial@*/ /*@dependent@*/ ickthread* ickmt_prev; /* define ickmt_prev */

void ickmtinit(void)
{
  /* Splint linked list problems again; the list is marked as 'dependent',
     which is correct for everything except adding and removing items in the
     list. (All annotations are incorrect in some respects.) */
  /*@-onlytrans@*/
  ickmt_cur = malloc(sizeof(ickthread));
  /*@=onlytrans@*/
  if(!ickmt_cur) ick_lose(IE991, 1, (const char *) NULL);
  ickmt_prev = ickmt_cur;
  ickmt_cur->ick_next = ickmt_cur;
  topchoice = (ickthread*) NULL; /* No choicepoints */
  ickmt_cur->dsi=ickmt_cur;
  ickmt_cur->usesthis=0;
}

/* Destroys the current thread, and switches to the ick_next thread.
   If there are no threads left, terminates the program using exit(0). */
void killthread(void)
{
  static jmp_buf dummy;
  ick_stashbox* isb, *isbi;
  int i;

  if(ick_printflow&&!choicing) fluputs("(kill)");

  if(!choicing) while(topchoice) choiceahead();
  /* The above line will mark each of the choicepoints as no longer being
     used by this thread, and free them if neccesary. This has to be done
     ick_first while this thread's pointers are still valid due to the way that
     choiceahead works. */

  /* If this thread uses another, let the other know about the change */
  if(ickmt_cur->dsi!=ickmt_cur)
  {
    ickthread* temp=ickmt_cur->dsi;
    if(ick_printflow) fluputs("(deweave)");
    while(!temp||temp->usesthis!=ickmt_cur)
    {
      if(!temp) ick_lose(IE778, -1, (const char *) NULL);
      temp=temp->usesthis;
    }
    temp->usesthis=ickmt_cur->usesthis;
  }
  /* If this thread is holding data for others, move it somewhere safe */
  if(ickmt_cur->usesthis != NULL && ickmt_cur->usesthis->dsi==ickmt_cur)
  {
    ickthread* newuses=ickmt_cur->usesthis;
    ickthread* temp=ickmt_cur->usesthis;
    if(ick_printflow) fluputs("(shift)");
    while(temp)
    {
      temp->dsi=newuses;
      temp=temp->usesthis;
    }
    ickmt_cur->dsi=newuses; /* so the data will be transferred later */
  }

  if(ickmt_cur->dsi==ickmt_cur)
  {
    /* We aren't storing data for another thread, and we have data of our own,
       or dsi would point somewhere else (either naturally or because it was
       changed higher up). */
    if(ick_printflow) fluputs("(free)");
    /* Deallocate the current thread's data */
    i=tailcount;
    while(i--)
    { /* free tail data */
      if(!ick_tails[i].rank||!ick_tails[i].dims) continue; /* already free */
      free(ick_tails[i].dims);
      free(ick_tails[i].data.tail);
    }
    i=hybridcount;
    while(i--)
    { /* free hybrid data */
      if(!ick_hybrids[i].rank||!ick_hybrids[i].dims) continue; /* already free */
      free(ick_hybrids[i].dims);
      free(ick_hybrids[i].data.hybrid);
    }
    /* unqualifiedtrans because although they aren't always only, they're only at the moment;
       compdestroy because we have just deallocated tail and hybrid data */
    /*@-unqualifiedtrans@*/ /*@-compdestroy@*/
    free(ick_onespots); free(ick_twospots); free(ick_tails); free(ick_hybrids);
    free(ick_oneforget); free(ick_twoforget); free(ick_tailforget); free(ick_hyforget);
    if(ick_oo_onespots) free(ick_oo_onespots); if(ick_oo_twospots) free(ick_oo_twospots);
    /*@=unqualifiedtrans@*/ /*@=compdestroy@*/

    isbi = ick_first;
    while(isbi) /* Free ick_stash */
    {
      isb=isbi->ick_next;
      if(isbi->type == ick_TAIL || isbi->type == ick_HYBRID)
      {
	free(isbi->save.a->dims);
	if(isbi->type == ick_TAIL)
	  free(isbi->save.a->data.tail);
	else
	  free(isbi->save.a->data.hybrid);
      }
      free(isbi);
      isbi=isb;
    }
  }
  else
  {
    /* We still need to save our variables for the benefit of woven threads. */
    /* save variables */
    ickmt_cur->dsi->varforget[0] = ick_onespots;
    ickmt_cur->dsi->varforget[1] = ick_twospots;
    ickmt_cur->dsi->varforget[2] = ick_tails;
    ickmt_cur->dsi->varforget[3] = ick_hybrids;
    ickmt_cur->dsi->varforget[4] = ick_oneforget;
    ickmt_cur->dsi->varforget[5] = ick_twoforget;
    ickmt_cur->dsi->varforget[6] = ick_tailforget;
    ickmt_cur->dsi->varforget[7] = ick_hyforget;
    ickmt_cur->dsi->varforget[8] = ick_oo_onespots;
    ickmt_cur->dsi->varforget[9] = ick_oo_twospots;

    /* save ick_stashbox */
    /*@-unqualifiedtrans@*/ /* the linked list problem again */
    ickmt_cur->dsi->sb = ick_first;
    /*@=unqualifiedtrans@*/
    /*@-branchstate@*/ /* it's reference-counted */
  }
  /*@=branchstate@*/
  /*@-unqualifiedtrans@*/ /* it is only at this point */
  free(ick_next); /* Free NEXT stack */
  /*@=unqualifiedtrans@*/

  ickmt_prev->ick_next = ickmt_cur->ick_next;
  if(ickmt_cur->ick_next == ickmt_cur)
  {
    /*@-dependenttrans@*/ /* because it isn't really dependent */
    free(ickmt_cur);
    /*@=dependenttrans@*/
    exit(0);
  }
  else
  {
    /* We need to run about half of nextthread. So we pass in a 2 for
       flags and tell it to skip the ick_first half. */
    /*@-dependenttrans@*/ /* because it isn't really dependent */
    free(ickmt_cur);
    /*@=dependenttrans@*/
    ickmt_cur = ickmt_prev;
    nextthread(dummy, -1, 2); /* dummy is not used by nextthread */
    ick_lose(IE778, -1, (const char *) NULL); /* nextthread does not return */
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
     flags | 2 means load the ick_next thread.
     flags | 4 means don't change thread. */
  if(errlineno > -1 && ickmt_cur->ick_next == ickmt_cur && !choicing) longjmp(pc,1);
  /* If we only have 1 thread, just continue with it. Otherwise: */

  if(!(flags&1)) goto advancethread;
  /* OK, so I've spaghettified this procedure slightly by using goto
     instead of if. But I was so deep into figuring out setjmp/longjmp,
     a goto seemed crystal-clear by comparison. */

  /* save variables */
  ickmt_cur->dsi->varforget[0] = ick_onespots;
  ickmt_cur->dsi->varforget[1] = ick_twospots;
  ickmt_cur->dsi->varforget[2] = ick_tails;
  ickmt_cur->dsi->varforget[3] = ick_hybrids;
  ickmt_cur->dsi->varforget[4] = ick_oneforget;
  ickmt_cur->dsi->varforget[5] = ick_twoforget;
  ickmt_cur->dsi->varforget[6] = ick_tailforget;
  ickmt_cur->dsi->varforget[7] = ick_hyforget;
  ickmt_cur->dsi->varforget[8] = ick_oo_onespots;
  ickmt_cur->dsi->varforget[9] = ick_oo_twospots;

  /* save NEXT stack */
  /*@-unqualifiedtrans@*/ /* because nextstack isn't only really */
  ickmt_cur->nextstack = ick_next;
  /*@=unqualifiedtrans@*/
  ickmt_cur->nextpointer = ick_nextindex;

  /* save ick_stashbox */
  /*@-unqualifiedtrans@*/ /* ditto */
  ickmt_cur->dsi->sb = ick_first;
  /*@=unqualifiedtrans@*/

  /* save choicepoints */
  if(!choicing) ickmt_cur->choicepoint = topchoice;

  /* save comefrom information */
  memcpy((void*)(ickmt_cur->ick_cjb), (const void*)ick_cjb, sizeof(jmp_buf));
  ickmt_cur->ick_ccfc = ick_ccfc;
  ickmt_cur->ick_skipto = ick_skipto;

  /* save program counter */
  memcpy((void*)(ickmt_cur->pc), (const void*)pc, sizeof(jmp_buf));
  /* And another thing about setjmp/longjmp. A jmp_buf
     acts like a structure that passes itself around by
     reference. However, it cannot be assigned, although
     just about everything else in C can be, although it
     can be copied with memcpy (what I'm doing in the line
     above - remember it passes itself around by reference).
     Generally speaking, it's some sort of ick_array,
     even though some implementors use a 1-element ick_array.
     The exact representation of jmp_buf is one of the most
     implementation-dependent things in C (I've seen both
     a 1-element ick_array of structure and an int[12].) */

 advancethread:
  /* change thread */
  if(!(flags&4))
  {
    ickmt_prev = ickmt_cur;
    ickmt_cur = ickmt_cur->ick_next;
  }

  if(!(flags&2)) goto returnjmp;

  /* load variables */
  ick_onespots    = ickmt_cur->dsi->varforget[0];
  ick_twospots    = ickmt_cur->dsi->varforget[1];
  ick_tails       = ickmt_cur->dsi->varforget[2];
  ick_hybrids     = ickmt_cur->dsi->varforget[3];
  ick_oneforget   = ickmt_cur->dsi->varforget[4];
  ick_twoforget   = ickmt_cur->dsi->varforget[5];
  ick_tailforget  = ickmt_cur->dsi->varforget[6];
  ick_hyforget    = ickmt_cur->dsi->varforget[7];
  ick_oo_onespots = ickmt_cur->dsi->varforget[8];
  ick_oo_twospots = ickmt_cur->dsi->varforget[9];

  /* load NEXT stack */
  /*@-onlytrans@*/ /* the nextstack shouldn't be only */
  ick_next = ickmt_cur->nextstack;
  /*@=onlytrans@*/
  ick_nextindex = ickmt_cur->nextpointer;

  /* load choicepoints */
  if(!choicing) topchoice = ickmt_cur->choicepoint;

  /* load ick_stashbox */
  /*@-onlytrans@*/ /* ditto */
  ick_first = ickmt_cur->dsi->sb;
  /*@=onlytrans@*/

  /* load comefrom information */
  memcpy((void*)ick_cjb, (const void*)(ickmt_cur->ick_cjb), sizeof(jmp_buf));
  ick_ccfc = ickmt_cur->ick_ccfc;
  ick_skipto = ickmt_cur->ick_skipto;

 returnjmp:
  /* return to the new current thread's program counter */
  if(choicing==2) choicing = 0;
  longjmp(ickmt_cur->pc, 1);
}
