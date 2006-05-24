/****************************************************************************

NAME
    yuk.c -- C-INTERCAL debugger and profiler code, linked into programs.

DESCRIPTION
    File linked into programs as a debugger or profiler.

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

****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

#define YUKDEBUG 1

#include "yuk.h"
#include "lose.h"
#include "sizes.h"
#include "abcess.h"

#if YPTIMERTYPE == 4
#include <sys/times.h>
#endif

extern signed char onewatch[];
extern signed char twowatch[];
extern type16      oneold[];
extern type32      twoold[];

int yuklines = 0;
int yukloop = 0;
int yukcommands = 0; /* these 3 lines because externs must be defined
			somewhere */
/* Global variable storage types:
   static: limited to the file
   unadorned: defining an extern
   extern: defined elsewhere (unless initialised) */

static char buf[20];

static sig_atomic_t singlestep = 1; /* if 0, run until a breakpoint */
static sig_atomic_t writelines = 1; /* whether to display executed lines onscreen */
static int breakpoints[80]; /* initialised to all 0s. Breakpoint locations */
static int nbreakpoints = 1; /* how many breakpoints we have */
static int monitors[80]; /* monitors give a message when program flow passes them */
static int nmonitors = 0; 
static int untilnext = -1; /* NEXTING level to break the program at */
static int firstrun = 1; /* first time an interactive command point is reached */
static int yukerrcmdg = -1; /* the aboff that indicates an error in the 'g' command */

static yptimer tickcount; /* yptimer of last run */
static int lastaboff = 0; /* last value of aboff */

void handlesigint(int i)
{ /* this is a signal handler, so can't do much */
  singlestep = 1;
  writelines = 1;
  (void) i; /* we're ignoring our argument */
}

#if YPTIMERTYPE==1 || YPTIMERTYPE==2
yptimer yukgettimeofday()
{
  static struct timeval tp;
  yptimer temp;
  gettimeofday(&tp,0);
  temp=(yptimer)tp.tv_usec +
    (yptimer)tp.tv_sec * (yptimer)1000000LU;
  /* here we make use of unsigned wraparound. In the case
     YPTIMERTYPE == 1, it seems quite likely that we're going
     to wraparound, but because everything is cast to the
     unsigned integral type yptimer, we get a value that will
     wraparound in such a way that - will give us the correct
     time interval. */
  return temp;
}
#elif YPTIMERTYPE == 4
yptimer yuktimes()
{
  static struct tms tp;
  times(&tp);
  return tp.tms_utime + tp.tms_stime;
}
#endif /* YPTIMERTYPE 4 */

void yukterm(void)
{
  int i,lastline,thisline,inrow;
  yptimer avgtime,avgtime2;
  if(yukopts==2) puts("Program ended without error.");
  if(!(yukopts&1)) return;
  /* Print profiling information */
  puts("Profiling information saved to \"yuk.out\".");
  freopen("yuk.out","w",stdout);
  i=-1;
  lastline=-1;
  while(++i<yukcommands)
  {
    thisline=lineofaboff[i];
    if(thisline==lastline) inrow++; else
    {
      inrow=1;
      printf("%5d:\t%s\n",thisline,textlines[thisline]);
    }
    lastline=thisline;
    avgtime=ypexecount[i]+ypabscount[i];
    if(avgtime) avgtime=ypexectime[i]/avgtime;
    avgtime2=0;
    if(ypexecount[i]) avgtime2=ypexectime[i]/ypexecount[i];
    printf("C%d: Time%4" YPTIMERTFORMAT ".%0" YPTIMERTFORMATD
	   ", Avg%2" YPTIMERTFORMAT ".%0" YPTIMERTFORMATD ", "
	   "Avg Exec%2" YPTIMERTFORMAT ".%0" YPTIMERTFORMATD
	   ", Exec%8" YPCOUNTERTFORMAT ", Abs%8" YPCOUNTERTFORMAT "\n",
	   inrow,ypexectime[i]/YPTIMERSCALE, ypexectime[i]%YPTIMERSCALE,
	   avgtime/YPTIMERSCALE, avgtime%YPTIMERSCALE,
	   avgtime2/YPTIMERSCALE, avgtime2%YPTIMERSCALE,
	   ypexecount[i],ypabscount[i]);
  }
}

void yukline(int aboff,int emitlineno)
{ /* this runs every time a source line is encountered */
  int i;
  int broken; /* hit a breakpoint, monitorpoint, viewbreak, until */
  int keeplooping;
  int templine;
  int tempcmd;
  int temp;
  yptimer temptick;
  char* text=textlines[lineofaboff[aboff]];
  if(!yukopts) return;
  if(yukopts & 1)
  { /* profile */
    temptick=YPGETTIME;
    if(lastaboff) ypexectime[lastaboff]+=temptick-tickcount;
    tickcount=temptick;
    if(abstained[aboff]) ypabscount[aboff]++;
    else ypexecount[aboff]++;
    lastaboff=aboff;
  }
  if(yukopts & 2)
  { /* debug */
    if(firstrun)
    {
      /* GNU GPL requires a copyright notice at this point */
      puts("yuk debugger Copyright (C) 2006 Alex Smith.");
      puts("The yuk debugger is covered by the GNU GPL and can");
      puts("be redistributed freely, but comes with ABSOLUTELY");
      puts("NO WARRANTY. For more information, type *<RET>.\n");
      puts("For help on yuk, type ?<RET>.\n");
    }
    i=nbreakpoints;
    broken=0;
    while(i--) broken|=breakpoints[i]==lineofaboff[aboff];
    if(yukloop&&broken&&*breakpoints!=lineofaboff[aboff]) broken=0;
    if(broken)
    {
      if(*breakpoints!=lineofaboff[aboff])
	printf("Breakpoint hit at line %d:\n",lineofaboff[aboff]);
      singlestep=1;
    }
    else
    {
      i=nmonitors;
      while(i--) broken|=monitors[i]==lineofaboff[aboff];
      if(yukloop) broken=0;
      if(broken) printf("Command flowed past line %d:\n",lineofaboff[aboff]);
    }
    if(nextindex <= untilnext)
    {
      broken = 1;
      singlestep = 1;
    }
    if(!broken&&yukerrcmdg==aboff)
    {
      singlestep = 1;
      puts("There are no commands on that line.");
      /* To the user, nothing will have happened but the error message! */
    }
    i=-1;
    while(++i,1)
    {
      if(yukvars[i].vartype==YUKEND) break;
      if(yukvars[i].vartype==ONESPOT)
      {
	if(onewatch[yukvars[i].intername])
	{
	  if(onespots[yukvars[i].intername]!=oneold[yukvars[i].intername]&&
	    onewatch[yukvars[i].intername]>1)
	  {
	    oneold[yukvars[i].intername]=onespots[yukvars[i].intername];
	    if(onewatch[yukvars[i].intername]==2||!onespots[yukvars[i].intername])
	    {
	      printf(onewatch[yukvars[i].intername]==2?
		     "Variable .%d changed.\n":"Variable .%d became 0.\n",
		     yukvars[i].extername);
	      broken=1; singlestep=1;
	    }
	  }
	  if(writelines||broken)
	  {
	    printf(".%d is:\n",yukvars[i].extername);
	    pout(onespots[yukvars[i].intername]);
	  }
	}
      }
      if(yukvars[i].vartype==TWOSPOT)
      {
	if(twowatch[yukvars[i].intername])
	{
	  if(twospots[yukvars[i].intername]!=twoold[yukvars[i].intername]&&
	    twowatch[yukvars[i].intername]>1)
	  {
	    twoold[yukvars[i].intername]=twospots[yukvars[i].intername];
	    if(twowatch[yukvars[i].intername]==2||!twospots[yukvars[i].intername])
	    {
	      printf(twowatch[yukvars[i].intername]==2?
		     "Variable :%d changed.\n":"Variable :%d became 0.\n",
		     yukvars[i].extername);
	      broken=1; singlestep=1;
	    }
	  }
	  if(writelines||broken)
	  {
	    printf(":%d is:\n",yukvars[i].extername);
	    pout(twospots[yukvars[i].intername]);
	  }
	}
      }
    }    
    if(writelines||broken)
    {
      /* write line that we're on */
      printf("%5d:\t%s\n",lineofaboff[aboff],text);
      /* write command within line that we're on */
      tempcmd=aboff;
      while(tempcmd&&lineofaboff[--tempcmd]==lineofaboff[aboff]);
      printf("On C%d: Abstained %d time%s\n",aboff?aboff-tempcmd:1,
	     abstained[aboff]-yukloop,abstained[aboff]-yukloop==1?".":"s.");
    }
    if(singlestep)
    {
      signal(SIGINT,handlesigint); /* placing this line here means that a rapid
				      ^C^C will terminate the program, if it's
				      stuck in a loop somewhere */
      keeplooping = 1;
      breakpoints[0] = 0; /* breakpoints[0] goes whenever a breakpoint is hit */
      untilnext = -1;
      if(yukloop)
      { /* reverse the abstentions that g caused */
	i = -1;
	while(++i<yukcommands) abstained[i]--;
      }
      yukloop = 0;
      yukerrcmdg = -1;
      do
      {
	printf("yuk007 "); /* this is our prompt, a sort of reverse
			      INTERCAL version of % */
	fgets(buf,20,stdin); /* input a command */
	if(!strchr(buf,'\n'))
	{
	  lose(E810,emitlineno,(char*)NULL);
	}
	templine=0;
	switch(*buf)
	{
	case '?':
	case '@':
	  puts("a<line>\tabstain once from all non-abstained commands on <line>");
	  puts("b<line>\tset breakpoint at <line>");
	  puts("c\tcontinue execution until a breakpoint is reached");
	  puts("d<line>\tdelete breakpoint at <line>");
	  puts("e<line>\texplain the main expression on <line>");
	  puts("f<line>\tstop producing messages when commands on <line> are run");
	  puts("g<line>\tchange currently executing command to the first command");
	  puts("\ton <line> or the next command if already on <line>");
	  puts("h\tlist 10 lines either side of the current line");
	  puts("i<var>\tignore a variable");
	  puts("j<var>\tremember a variable");
	  puts("k\tcontinue until we RESUME back to the current nexting level");
	  puts("\t(that is, step unless we are on a NEXT, in which case execute");
	  puts("\tuntil a RESUME or FORGET back to the same or smaller NEXT stack)");
	  puts("l<line>\tlist 10 lines either side of <line>");
	  puts("m<line>\tproduce a message every time a command on <line> is run");
	  puts("n\tshow the NEXT stack");
	  puts("o\tcontinue until we RESUME/FORGET below the current nexting level");
	  puts("\tie until the NEXT stack becomes smaller than it is at present");
	  puts("p\tdisplay the values of all onespot and twospot variables");
	  puts("q\tabort execution");
	  puts("r<line>\treinstate once all abstained commands on <line>");
	  puts("s\tstep to the next line of code");
	  puts("t\tcontinue until a breakpoint, displaying all lines executed");
	  puts("u<line>\texecute until just before <line> is reached");
	  puts("v<var>\tshow value of variable every time a command is printed");
	  puts("w\tshow the current line and current command");
	  puts("x<var>\tremove a variable view, breakchange, or breakzero");
	  puts("y<var>\tview variable every displayed line, and break on change");
	  puts("z<var>\tview variable every displayed line, and break on zero");
	  puts("<var>\tview the value of a onespot or twospot variable");
	  puts("<<var>\tset the value of a onespot or twospot variable");
	  puts("*\tview the GNU General Public Licence");
	  puts("?\tview this help screen");
	  puts("@\tview this help screen");
	  puts("Line numbers refer to lines of source code, not line labels.");
	  puts("Listings have (Axxxxxx) at the start of each line: this shows");
	  puts("the abstention status of each command on that line.");
	  puts("The values of variables must be input in proper INTERCAL");
	  puts("notation (i.e. ONE TWO THREE), and are output as butchered");
	  puts("Roman numerals.");
	  puts("You can press ^C to break an executing program.");
	  break;
	case 'q':
	  exit(0);
	  break;
	case 'n':
	  i=nextindex;
	  if(!i)
	  {
	    puts("The NEXT stack is empty.");
	  }
	  else
	  {
	    puts("Commands NEXTED from:");
	    while(i--)
	    {
	      /* write NEXT line */
	      printf("%5d:\t%s\n",lineofaboff[next[i]-1],
		     textlines[lineofaboff[next[i]-1]]);
	      /* write NEXT command within line */
	      tempcmd=next[i];
	      while(tempcmd&&lineofaboff[--tempcmd]==lineofaboff[next[i]-1]);
	      printf("NEXTED from command C%d: Abstained %d time%s\n",next[i]-1?
		     next[i]-1-tempcmd:1,abstained[next[i]-1],
		     abstained[next[i]-1]==1?".":"s.");
    	    }
	  }
	  break;
	case 'w':
	  /* write line that we're on */
	  printf("%5d:\t%s\n",lineofaboff[aboff],text);
	  /* write command within line that we're on */
	  tempcmd=aboff;
	  while(tempcmd&&lineofaboff[--tempcmd]==lineofaboff[aboff]);
	  printf("On C%d: Abstained %d time%s\n",aboff?aboff-tempcmd:1,abstained[aboff],
	     abstained[aboff]==1?".":"s.");
	  break;
	case 'p':
	  i=-1;
	  while(++i,1)
	  {
	    if(yukvars[i].vartype==YUKEND) break;
	    if(yukvars[i].vartype==ONESPOT)
	    {
	      printf("Variable .%d is:\n",yukvars[i].extername);
	      pout(onespots[yukvars[i].intername]);
	    }
	    if(yukvars[i].vartype==TWOSPOT)
	    {
	      printf("Variable :%d is:\n",yukvars[i].extername);
	      pout(twospots[yukvars[i].intername]);
	    }
	  }
	  break;
	case '.':
	case ':':
	  sscanf(buf+1,"%d",&templine);
	  if(templine<1)
	  {
	    puts("Don't know which variable you mean.");
	    break;
	  }
	  i=-1;
	  temp=0;
	  while(++i,!temp)
	  {
	    if(yukvars[i].vartype==YUKEND) break;
	    if((*buf=='.'&&yukvars[i].vartype==ONESPOT)||
	       (*buf==':'&&yukvars[i].vartype==TWOSPOT))
	    {
	      if(yukvars[i].extername==templine)
	      {
		temp=1;
		if(yukvars[i].vartype==ONESPOT)
		{
		  pout(onespots[yukvars[i].intername]);
		  puts(oneforget[yukvars[i].intername]?
		       "This variable is currently ignored.":
		       "This variable is currently remembered.");
		}
		if(yukvars[i].vartype==TWOSPOT)
		{
		  pout(twospots[yukvars[i].intername]);
		  puts(twoforget[yukvars[i].intername]?
		       "This variable is currently ignored.":
		       "This variable is currently remembered.");
		}
	      }
	    }
	  }
	  if(temp) break;
	  puts("That variable is not in the program.");
	  break;
	case 'v':
	case 'x':
	case 'y':
	case 'z':
	  if(buf[1]!='.'&&buf[1]!=':')
	  {
	    puts("This command only works on onespot and twospot variables.");
	    break;
	  }
	  sscanf(buf+2,"%d",&templine);
	  if(templine<1)
	  {
	    puts("Don't know which variable you mean.");
	    break;
	  }
	  i=-1;
	  temp=0;
	  while(++i,!temp)
	  {
	    if(yukvars[i].vartype==YUKEND) break;
	    if(yukvars[i].extername==templine)
	    {
	      if(buf[1]=='.'&&yukvars[i].vartype==ONESPOT)
	      {
		if(*buf=='v') onewatch[yukvars[i].intername]=1;
		if(*buf=='x') onewatch[yukvars[i].intername]=0;
		if(*buf=='y') onewatch[yukvars[i].intername]=2;
		if(*buf=='z') onewatch[yukvars[i].intername]=3;
		oneold[yukvars[i].intername]=onespots[yukvars[i].intername];
		temp=1;
	      }
	      if(buf[1]==':'&&yukvars[i].vartype==TWOSPOT)
	      {
		if(*buf=='v') twowatch[yukvars[i].intername]=1;
		if(*buf=='x') twowatch[yukvars[i].intername]=0;
		if(*buf=='y') twowatch[yukvars[i].intername]=2;
		if(*buf=='z') twowatch[yukvars[i].intername]=3;
		twoold[yukvars[i].intername]=twospots[yukvars[i].intername];
		temp=1;
	      }
	    }
	  }
	  if(!temp)
	  {
	    puts("That variable is not in the program.");
	    break;
	  }
	  if(*buf=='v') puts("Set a normal variable view.");
	  if(*buf=='x') puts("Removed all views from that variable.");
	  if(*buf=='y') puts("Set a breakchange variable view.");
	  if(*buf=='z') puts("Set a breakzero variable view.");
	  break;
	case 'i':
	case 'j':
	  if(buf[1]!='.'&&buf[1]!=':'&&buf[1]!=','&&buf[1]!=';')
	  {
	    puts("That isn't a real sort of variable.");
	    break;
	  }
	  sscanf(buf+2,"%d",&templine);
	  if(templine<1)
	  {
	    puts("Don't know which variable you mean.");
	    break;
	  }
	  i=-1;
	  temp=0;
	  while(++i,!temp)
	  {
	    if(yukvars[i].vartype==YUKEND) break;
	    if((buf[1]=='.'&&yukvars[i].vartype==ONESPOT)||
	       (buf[1]==':'&&yukvars[i].vartype==TWOSPOT)||
	       (buf[1]==','&&yukvars[i].vartype==TAIL)||
	       (buf[1]==';'&&yukvars[i].vartype==HYBRID))
	    {
	      if(yukvars[i].extername==templine)
	      {
		temp=1;
		if(yukvars[i].vartype==ONESPOT)
		  oneforget[yukvars[i].intername]=*buf=='i';
		if(yukvars[i].vartype==TWOSPOT)
		  twoforget[yukvars[i].intername]=*buf=='i';
		if(yukvars[i].vartype==TAIL)
		  tailforget[yukvars[i].intername]=*buf=='i';
		if(yukvars[i].vartype==HYBRID)
		  hyforget[yukvars[i].intername]=*buf=='i';		
	      }
	    }
	  }
	  if(temp)
	  {
	    if(*buf=='i')
	      puts("Variable ignored.");
	    else
	      puts("Variable remembered.");
	    break;
	  }
	  puts("That variable is not in the program.");
	  break;
	  	case '<':
	  if(buf[1]!='.'&&buf[1]!=':')
	  {
	    puts("You cannot set that sort of variable (if it exists at all).");
	    break;
	  }
	  sscanf(buf+2,"%d",&templine);
	  if(templine<1)
	  {
	    puts("Don't know which variable you mean.");
	    break;
	  }
	  i=-1;
	  temp=0;
	  while(++i,!temp)
	  {
	    if(yukvars[i].vartype==YUKEND) break;
	    if((buf[1]=='.'&&yukvars[i].vartype==ONESPOT)||
	       (buf[1]==':'&&yukvars[i].vartype==TWOSPOT))
	    {
	      if(yukvars[i].extername==templine)
	      {
		temp=1;
		if(yukvars[i].vartype==ONESPOT)
		  onespots[yukvars[i].intername]=pin();
		if(yukvars[i].vartype==TWOSPOT)
		  twospots[yukvars[i].intername]=pin();
		/* note that when debugging, you can set an
		   ignored variable */
	      }
	    }
	  }
	  if(temp) break;
	  puts("That variable is not in the program.");
	  break;
	case 'g':
	  sscanf(buf+1,"%d",&templine);
	  if(!templine)
	  {
	    puts("Don't know which line you mean.");
	    break;
	  }
	  breakpoints[0] = templine;
	  yukloop = 1;
	  /* This is implemented by incrementing all ABSTAIN counts,
	     even the normally immutable ones on GIVE UP lines, setting
	     a temporary breakpoint ([0]) on the line given, and running the
	     program. When the breakpoint is hit singlestep will see that
	     yukloop is set (its purpose is to cause the program to go back
	     to the start when it reaches the end) and decrement all ABSTAIN
	     counts, putting the commands back the way they were. We set an error
	     breakpoint on this line in case the user is trying to jump to a
	     line with no commands (although this debugger command is called
	     'g', would I dare to describe this as a GOTO?) */
	  i = -1;
	  while(++i<yukcommands) abstained[i]++;
	  yukerrcmdg = aboff;
	  singlestep = 0;
	  writelines = 0;
	  keeplooping = 0; /* to break out of the loop in the debugger! */
	  break;
	case 'a':
	case 'r':
	  sscanf(buf+1,"%d",&templine);
	  if(!templine)
	  {
	    puts("Don't know which line you mean.");
	    break;
	  }
	  i=-1;
	  tempcmd=1;
	  while(++i<yukcommands)
	  {
	    if(lineofaboff[i]==templine)
	    {
	      temp=abstained[i];
	      if(*buf=='a') if(!temp) abstained[i]=1;
	      if(*buf=='r') if(temp) abstained[i]--;
	      printf("Command %d on line %d was abstained %d time%s, "
		     "now abstained %d time%s.\n",tempcmd,templine,
		     temp,temp==1?"":"s",abstained[i],
		     abstained[i]==1?"":"s");
	      tempcmd++;
	    }
	  }
	  if(tempcmd==1) puts("No commands start on this line.");
	  break;
	case 'k':
	case 'o':
	  untilnext=nextindex-(*buf=='o');
	  /* no break;! */
	case 'c':
	  singlestep=0;
	  writelines=0;
	  keeplooping=0;
	  break;
	case 'b':
	  sscanf(buf+1,"%d",&templine);
	  if(templine)
	  {
	    printf("Breakpoint set at line %d.\n",templine);
	    breakpoints[nbreakpoints++]=templine;
	    if(nbreakpoints>=80) lose(E811,emitlineno,(char*)NULL);
	  }
	  else
	    puts("Don't know which line you mean.");
	  break;
	case 'd':
	  sscanf(buf+1,"%d",&templine);
	  if(templine)
	  {
	    printf("All breakpoints removed from line %d.\n",templine);
	    i=nbreakpoints;
	    while(i--) if(templine==breakpoints[i])
	    {
	      memmove(breakpoints+i,breakpoints+i+1,sizeof(int)*(nbreakpoints-i));
	      nbreakpoints--;
	    }
	  }
	  else
	    puts("Don't know which line you mean.");
	  break;
	case 'm':
	  sscanf(buf+1,"%d",&templine);
	  if(templine)
	  {
	    printf("Monitor set at line %d.\n",templine);
	    monitors[nmonitors++]=templine;
	    if(nmonitors>=80) lose(E811,emitlineno,(char*)NULL);
	  }
	  else
	    puts("Don't know which line you mean.");
	  break;
	case 'f':
	  sscanf(buf+1,"%d",&templine);
	  if(templine)
	  {
	    printf("All monitors removed from line %d.\n",templine);
	    i=nmonitors;
	    while(i--) if(templine==monitors[i])
	    {
	      memmove(monitors+i,monitors+i+1,sizeof(int)*(nmonitors-i));
	      nmonitors--;
	    }
	  }
	  else
	    puts("Don't know which line you mean.");
	  break;	  
	case 's':
	  singlestep=1;
	  writelines=1;
	  keeplooping=0;
	  break;
	case 't':
	  singlestep=0;
	  writelines=1;
	  keeplooping=0;
	  break;
	case 'u':
	  sscanf(buf+1,"%d",&templine);
	  if(templine)
	  {
	    breakpoints[0]=templine;
	    singlestep=0;
	    writelines=0;
	    keeplooping=0;
	  }
	  else
	    puts("Don't know which line you mean.");
	  break;
	case 'e':
	  sscanf(buf+1,"%d",&templine);
	  if(!templine)
	  {
	    puts("Don't know which line you mean.");
	    break;
	  }
	  tempcmd=-1;
	  temp=0;
	  i=0;
	  while(++tempcmd<yukcommands)
	  {
	    if(lineofaboff[tempcmd]==templine)
	    {
	      if(!yukexplain[tempcmd]) ++i;
	      else
	      {
		printf("C%d: Expression is %s\n",++i,yukexplain[tempcmd]);
		temp++;
	      }
	    }
	  }
	  if(!temp) puts("No expressions on that line.");
	  break;
	case 'l':
	  sscanf(buf+1,"%d",&templine);
	  if(!templine)
	  {
	    puts("Don't know which line you mean.");
	    break;
	  }
	  /* no break;! */
	case 'h':
	  if(!templine) templine=lineofaboff[aboff];
	  templine-=10;
	  if(templine+21>=yuklines) templine=yuklines-22;
	  if(templine<1) templine=1;
	  i=templine;
	  while(i<templine+21&&i<yuklines)
	  {
	    buf[6]=0;
	    buf[0]=buf[1]=buf[2]=buf[3]=buf[4]=buf[5]=' ';
	    tempcmd=-1;
	    temp=0;
	    while(++tempcmd<yukcommands)
	    {
	      if(lineofaboff[tempcmd]==i)
	      {
		buf[temp++]='0'+abstained[tempcmd];
		if(abstained[tempcmd]>9) buf[temp-1]='!';
		if(temp==6) break;
	      }
	    }
	    printf("(A%s)%5d:\t%s\n",buf,i,textlines[i]);
	    i++;
	  }
	  break;
	case '*':
	  system("more < " YUKCOPYLOC); /* display the GNU GPL copying conditions */
	  break;
	default:
	  puts("Not sure what you mean. Try typing ?<RET>.");
	  break;
	}
      } while(keeplooping);
    }
  }
  firstrun=0;
}
