/* ick_lose.h --- error message strings */

/*
 * Note: these error message texts, with one exception, are direct from
 * the Princeton compiler (INTERCAL-72) sources (transmitted by Don Woods).
 * The one exception is IE632, which in INTERCAL-72 had the error message
 *	PROGRAM ATTEMPTED TO EXIT WITHOUT ERROR MESSAGE
 * ESR's "THE NEXT STACK HAS RUPTURED!..." has been retained on the grounds
 * that it is more obscure and much funnier.  For context, find a copy of
 * Joe Haldeman's SF short story "A !Tangled Web", ick_first published in
 * Analog magazine in September 1981 and later anthologized in the author's
 * "Infinite Dreams" (Ace 1985).
 */
/* An undecodable statement has been encountered in the course of execution. */
#define IE000 "000 %s\n\
	ON THE WAY TO %d\n"
/* An expression contains a syntax error. */
#define IE017 "017 DO YOU EXPECT ME TO FIGURE THIS OUT?\n\
	ON THE WAY TO %d\n"
/* Improper use has been made of statement identifiers. */
#define IE079 "079 PROGRAMMER IS INSUFFICIENTLY POLITE\n\
	ON THE WAY TO %d\n"
/* Improper use has been made of statement identifiers. */
#define IE099 "099 PROGRAMMER IS OVERLY POLITE\n\
	ON THE WAY TO %d\n"
/* Program has attempted 80 levels of NEXTing */
#define IE123 "123 PROGRAM HAS DISAPPEARED INTO THE BLACK LAGOON\n\
	ON THE WAY TO %d\n"
/* Program has attempted to transfer to a non-existent line label */
#define IE129 "129 PROGRAM HAS GOTTEN LOST\n\
	ON THE WAY TO WHO KNOWS WHERE\n"
/* An ABSTAIN or REINSTATE statement references a non-existent line label */
#define IE139 "139 I WASN'T PLANNING TO GO THERE ANYWAY\n\
	ON THE WAY TO %d\n"
/* A line label has been multiply defined. */
#define IE182 "182 YOU MUST LIKE THIS LABEL A LOT!\n\
	ON THE WAY TO %d\n"
/* An invalid line label has been encountered. */
#define IE197 "197 SO!  65535 LABELS AREN'T ENOUGH FOR YOU?\n\
	ON THE WAY TO %d\n"
/* An expression involves an unidentified variable. */
#define IE200 "200 NOTHING VENTURED, NOTHING GAINED\n\
	ON THE WAY TO %d\n"
/* An attempt has been made to give an array a dimension of zero. */
#define IE240 "240 ERROR HANDLER PRINTED SNIDE REMARK\n\
	ON THE WAY TO %d\n"
/* Invalid dimensioning information was supplied
 * in defining or using an array. */
#define IE241 "241 VARIABLES MAY NOT BE STORED IN WEST HYPERSPACE\n\
	ON THE WAY TO %d\n"
/* A 32-bit value has been assigned to a 16-bit variable. */
#define IE275 "275 DON'T BYTE OFF MORE THAN YOU CAN CHEW\n\
	ON THE WAY TO %d\n"
/* A retrieval has been attempted for an unSTASHed value. */
#define IE436 "436 THROW STICK BEFORE RETRIEVING!\n\
	ON THE WAY TO %d\n"
/* A WRITE IN statement or interleave ($) operation
 * has produced value requiring over 32 bits to represent. */
#define IE533 "533 YOU WANT MAYBE WE SHOULD IMPLEMENT 64-BIT VARIABLES?\n\
	ON THE WAY TO %d\n"
/* Insufficient data. (raised by reading past EOF) */
#define IE562 "562 I DO NOT COMPUTE\n\
	ON THE WAY TO %d\n"
/* Input data is invalid. */
#define IE579 "579 WHAT BASE AND/OR LANGUAGE INCLUDES %s?\n\
	ON THE WAY TO %d\n"
/* The expression of a RESUME statement evaluated to #0. */
#define IE621 "621 ERROR TYPE 621 ENCOUNTERED\n\
	ON THE WAY TO %d\n"
/* Program execution terminated via a RESUME statement instead of GIVE UP. */
#define IE632 "632 THE NEXT STACK RUPTURES.  ALL DIE.  OH, THE EMBARRASSMENT!\n\
	ON THE WAY TO %d\n"
/* Execution has passed beyond the last statement of the program. */
#define IE633 "633 PROGRAM FELL OFF THE EDGE\n\
	ON THE WAY TO THE NEW WORLD\n"
/* A compiler error has occurred (see section 8.1). */
#define IE774 "774 RANDOM COMPILER BUG\n\
	ON THE WAY TO %d\n"
/* An unexplainable compiler error has occurred (see J. Lyon or D. Woods). */
#define IE778 "778 UNEXPLAINED COMPILER BUG\n\
	ON THE WAY TO %d\n"

/*
 * These errors are unique to C-INTERCAL.
 */
/* You tried to use a C-INTERCAL extension with the `ick_traditional' flag on */
#define IE111 "111 COMMUNIST PLOT DETECTED, COMPILER IS SUICIDING\n\
	ON THE WAY TO %d\n"
/* Cannot find the magically included system library */
#define IE127 "127 SAYING 'ABRACADABRA' WITHOUT A MAGIC WAND WON'T DO YOU ANY GOOD\n\
        ON THE WAY TO THE CLOSET\n"
/* Out of stash space */
#define IE222 "222 BUMMER, DUDE!\n\
	ON THE WAY TO %d\n"
/* (AIS) Out of memory during I/O */
#define IE252 "252 I'VE FORGOTTEN WHAT I WAS ABOUT TO SAY\n\
	ON THE WAY TO %d\n"
/* (AIS) Can't reverse an expression in an assignment. This is the
   same error number as in CLC-INTERCAL, but the message is different. */
#define IE277 "277 YOU CAN ONLY DISTORT THE LAWS OF MATHEMATICS SO FAR\n\
        ON THE WAY TO %d\n"
/* (AIS) The spark/ears nesting is too deep. */
#define IE281 "281 THAT MUCH QUOTATION AMOUNTS TO PLAGIARISM\n\
	ON THE WAY TO %d\n"
/* Too many variables. */
#define IE333 "333 YOU CAN'T HAVE EVERYTHING, WHERE WOULD YOU PUT IT?\n\
	ON THE WAY TO %d\n"
/* Out of memory during compilation. */
#define IE345 "345 THAT'S TOO COMPLEX FOR ME TO GRASP\n\
        ON THE WAY TO SOMEWHERE\n"
/* (AIS, from the suggestion by Malcom Ryan)
   GO BACK or GO AHEAD was executed without a choicepoint. */
#define IE404 "404 I'M ALL OUT OF CHOICES!\n\
	ON THE WAY TO %d\n"
/* (AIS) WHILE, MAYBE, GO BACK, or GO AHEAD used without the -m option. */
#define IE405 "405 PROGRAM REJECTED FOR MENTAL HEALTH REASONS\n\
	ON THE WAY TO %d\n"
/* A COME FROM statement references a non-existent line label. */
#define IE444 "444 IT CAME FROM BEYOND SPACE\n\
	ON THE WAY TO %d\n"
/* (AIS) We just buffer-overflowed. (Detecting this /before/ the overflow
   happens is probably more secure, but harder and less fun. As long as we
   don't return from any functions, it's probably safe in theory, but don't
   rely on this. */
#define IE553 "553 BETTER LATE THAN NEVER\n\
	ON THE WAY TO %d\n"
/* More than one COME FROM references the same label. */
#define IE555 "555 FLOW DIAGRAM IS EXCESSIVELY CONNECTED\n\
	ON THE WAY TO %d\n"
/* Too many source lines, or too many input files, or source line is
   too long. */
#define IE666 "666 COMPILER HAS INDIGESTION\n\
	ON THE WAY TO %d\n"
/* No such source file. */
#define IE777 "777 A SOURCE IS A SOURCE, OF COURSE, OF COURSE\n\
	ON THE WAY TO %d\n"
/* Can't open C output file */
#define IE888 "888 I HAVE NO FILE AND I MUST SCREAM\n\
	ON THE WAY TO %d\n"
/* Can't open C skeleton file. */
#define IE999 "999 NO SKELETON IN MY CLOSET, WOE IS ME!\n\
	ON THE WAY TO %d\n"
/* Source file name with invalid extension (use .i or .[3-7]i). */
#define IE998 "998 EXCUSE ME,\n\
	YOU MUST HAVE ME CONFUSED WITH SOME OTHER COMPILER\n"
/* (AIS) File used of a type for which the required libraries aren't
   available. */
#define IE899 "899 HELLO?\n\
	CAN ANYONE GIVE ME A HAND HERE?\n"
/* Illegal possession of a controlled unary operator. */
#define IE997 "997 ILLEGAL POSSESSION OF A CONTROLLED UNARY OPERATOR.\n\
	ON THE WAY TO %d\n"
/* (AIS) Command found after TRY AGAIN. */
#define IE993 "993 I GAVE UP LONG AGO\n\
        ON THE WAY TO %d\n"
/* (AIS) Memory allocation failure during multithreading */
#define IE991 "991 YOU HAVE TOO MUCH ROPE TO HANG YOURSELF\n\
        ON THE WAY TO %d\n"
/* (AIS) Unimplemented feature used. This should never come up, hopefully. */
#define IE995 "995 DO YOU REALLY EXPECT ME TO HAVE IMPLEMENTED THAT?\n\
	ON THE WAY TO %d\n"
/* Unknown invocation flag */
#define IE990 "990 FLAG ETIQUETTE FAILURE BAD SCOUT NO BISCUIT.\n\
	ON THE WAY TO %d\n"
/* Code generator encountered an unknown opcode in a tuple */
#define IE994 "994 NOCTURNAL EMISSION, PLEASE LAUNDER SHEETS IMMEDIATELY.\n\
	ON THE WAY TO %d\n"
/*
 *  AIS: These errors are specific to PIC-INTERCAL
 */
/* (AIS) Attempted to use an unsupported language feature. */
#define IE256 "256 THAT'S TOO HARD FOR MY TINY BRAIN\n\
	ON THE WAY TO %d\n"

/* (AIS) Attempted to use a PIC feature in a non-PIC program. */
#define IE652 "652 HOW DARE YOU INSULT ME!\n\
        ON THE WAY TO %d\n"

/*
 *  AIS: These errors are generated by the yuk debugger
 */
/* (AIS) fgets' buffer overflowed on debugger comand input. */
#define IE810 "810 ARE ONE-CHARACTER COMMANDS TOO SHORT FOR YOU?\n\
        ON THE WAY TO %d\n"

/* (AIS) Too many breakpoints. */
#define IE811 "811 PROGRAM IS TOO BADLY BROKEN TO RUN\n\
        ON THE WAY TO %d\n"

/*
 * (AIS) Warnings produced by -l.
 */

/* (AIS) Non-INTERCAL-72 identifier used. */
#define W112 "112 THAT RELIES ON THE NEW WORLD ORDER\n\
	ON THE WAY TO %d\n"

/* (AIS) That stupid idiom in syslib was used. */
#define W128 "128 SYSLIB IS OPTIMIZED FOR OBFUSCATION\n\
	ON THE WAY TO %d\n"

/* (AIS) Possibly nonportable unary operator. */
#define W534 "534 KEEP LOOKING AT THE TOP BIT\n\
	ON THE WAY TO %d\n"

/* (AIS) Expression still contains INTERCAL operators after
   optimization. Only in binary, because this nearly always
   happens in the higher bases. Syslib causes quite a lot of
   these. This warning is fine on INTERCAL-like lines, but
   flags a mistake on lines that are meant to be translations
   of C. */
#define W018 "018 THAT WAS MEANT TO BE A JOKE\n\
	ON THE WAY TO %d\n"

/* (AIS) Possible misplaced unary operator. At the moment, ick -l cannot
   detect this condition (so it never comes up). It's meant to detect
   expressions like '?"?.1~#256"$#2'~#3 (from numio.i); in this expression,
   the second what has no effect (it changes only the msb of the immediately
   surrounding expression, which is eventually filtered out by the select
   against #3). But detecting this would probably require code that could
   check which bits of a result were going to be used elsewhere in an
   expression, which is something I haven't written yet (but would make
   a decent optimize_pass3). */
#define W016 "016 DON'T TYPE THAT SO HASTILY\n\
	ON THE WAY TO %d\n"

/* (AIS) Possibly overflowing assignment or ick_mingle. Syslib causes some of
   these too, in complicated situations where the bugcatcher can't figure
   out what's happening, and also in a few blatant statements in the mould
   of DO .3 <- :3, which are quite clearly possible overflows. Strangely
   enough, there's a commented-out section of code in feh.c that suggests
   that someone tried to make this an error (using a more general check
   which would have caught more correct code involving GETS, but none
   involving $). As a middle ground, I've made it an -l warning. */
#define W276 "276 YOU CAN'T EXPECT ME TO CHECK BACK THAT FAR\n\
	ON THE WAY TO %d\n"

/* (AIS) A line will inevitably cause an expression-reversal failure. */
#define W278 "278 FROM A CONTRADICTION, ANYTHING FOLLOWS\n\
        ON THE WAY TO A HUGE DISASTER\n"

/* (AIS) The two following warnings are both compile-time traps for
   near-certain runtime errors. As such, they have similar numbers
   and similar messages. In fact, they're a shameless ripoff of the
   originals, but should serve as a reminder for anyone aware of the
   original messages. */
/* (AIS) Dimension given for an ick_array will come out as #0. */
#define W239 "239 WARNING HANDLER PRINTED SNIDE REMARK\n\
	ON THE WAY TO %d\n"

/* (AIS) RESUME argument will come out as #0. */
#define W622 "622 WARNING TYPE 622 ENCOUNTERED\n\
	ON THE WAY TO %d\n"

extern int ick_lineno;

extern void /*@noreturn@*/ ick_lose(const char *m, int n, /*@null@*/ const char *s)
#ifdef __GNUC__
  __attribute__ ((noreturn))
#endif
;

extern void ick_lwarn(const char *m, int n, /*@null@*/ const char *s);

/* ick_lose.h ends here */
