/* lose.h --- error message strings

/*
 * Note: these error message texts, with one exception, are direct from 
 * the Princeton compiler (INTERCAL-72) sources (transmitted by Don Woods).
 * The one exception is E632, which in INTERCAL-72 had the error message
 *	PROGRAM ATTEMPTED TO EXIT WITHOUT ERROR MESSAGE
 * ESR's "THE NEXT STACK HAS RUPTURED!..." has been retained on the grounds
 * that it is more obscure and much funnier.  For context, find a copy of
 * Joe Haldeman's SF short story "A !Tangled Web", first published in 
 * Analog magazine sometime in 1983 and later anthologized in the author's
 * "Infinite Dreams" (Ace 1985).
 */
/* An undecodable statement has been encountered in the course of execution."*/
#define E000 "000 %s\n\
	ON THE WAY TO %d\n"
/* An expression contains a syntax error. */
#define E017 "017 DO YOU EXPECT ME TO FIGURE THIS OUT?\n\
	ON THE WAY TO %d\n"
/* Improper use has been made of statement identifiers. */
#define E079 "079 PROGRAMMER IS INSUFFICIENTLY POLITE\n\
	ON THE WAY TO %d\n"
/* Improper use has been made of statement identifiers. */
#define E099 "099 PROGRAMMER IS OVERLY POLITE\n\
	ON THE WAY TO %d\n"
/* Program has attempted 80 levels of NEXTing */
#define E123 "123 PROGRAM HAS DISAPPEARED INTO THE BLACK LAGOON\n\
	ON THE WAY TO %d\n"
/* Program has attempted to transfer to a non-existent line label */
#define E129 "129 PROGRAM HAS GOTTEN LOST\n\
	ON THE WAY TO WHO KNOWS WHERE\n"
/* An ABSTAIN or REINSTATE statement references a non-existent line label */
#define E139 "139 I WASN'T PLANNING TO GO THERE ANYWAY\n\
	ON THE WAY TO %d\n"
/* A line label has been multiply defined. */
#define E182 "182 YOU MUST LIKE THIS LABEL A LOT!\n\
	ON THE WAY TO %d\n"
/* An invalid line label has been encountered. */
#define E197 "197 SO!  65535 LABELS AREN'T ENOUGH FOR YOU?\n\
	ON THE WAY TO %d\n"
/* An expression involves an unidentified variable. */
#define E200 "200 NOTHING VENTURED, NOTHING GAINED\n\
	ON THE WAY TO %d\n"
/* An attempt has been made to give an array a dimension of zero. */
#define E240 "240 ERROR HANDLER PRINTED SNIDE REMARK\n\
	ON THE WAY TO %d\n"
/* Invalid dimensioning information was supplied
 * in defining or using an array. */
#define E241 "241 VARIABLES MAY NOT BE STORED IN WEST HYPERSPACE\n\
	ON THE WAY TO %d\n"
/* A 32-bit value has been assigned to a 16-bit variable. */
#define E275 "275 DON'T BYTE OFF MORE THAN YOU CAN CHEW\n\
	ON THE WAY TO %d\n"
/* A retrieval has been attempted for an unSTASHed value. */
#define E436 "436 THROW STICK BEFORE RETRIEVING!\n\
	ON THE WAY TO %d\n"
/* A WRITE IN statement or interleave ($) operation
 * has produced value requiring over 32 bits to represent. */
#define E533 "533 YOU WANT MAYBE WE SHOULD IMPLEMENT 64-BIT VARIABLES?\n\
	ON THE WAY TO %d\n"
/* Insufficient data. (raised by reading past EOF) */
#define E562 "562 I DO NOT COMPUTE\n\
	ON THE WAY TO %d\n"
/* Input data is invalid. */
#define E579 "579 WHAT BASE AND/OR LANGUAGE INCLUDES %s?\n\
	ON THE WAY TO %d\n"
/* The expression of a RESUME statement evaluated to #0. */
#define E621 "621 ERROR TYPE 621 ENCOUNTERED\n\
	ON THE WAY TO %d\n"
/* Program execution terminated via a RESUME statement instead of GIVE UP. */
#define E632 "632 THE NEXT STACK RUPTURES.  ALL DIE.  OH, THE EMBARRASSMENT!\n\
	ON THE WAY TO %d\n"
/* Execution has passed beyond the last statement of the program. */
#define E633 "633 PROGRAM FELL OFF THE EDGE\n\n\
	ON THE WAY TO THE NEW WORLD\n"
/* A compiler error has occurred (see section 8.1). */
#define E774 "774 RANDOM COMPILER BUG\n\
	ON THE WAY TO %d\n"
/* An unexplainable compiler error has occurred (see J. Lyon or D. Woods). */
#define E778 "778 UNEXPLAINED COMPILER BUG\n\
	ON THE WAY TO %d\n"

/*
 * These errors are unique to C-INTERCAL.  They ought to be documented in
 * the revised manual.
 */
/* You tried to use a C-INTERCAL extension with the `traditional' flag on */
#define E111 "111 COMMUNIST PLOT DETECTED, COMPILER IS SUICIDING\n\
	ON THE WAY TO %d\n"
/* Cannot find the magically included system library */
#define E127 "127 SAYING 'ABRACADABRA' WITHOUT A MAGIC WAND WON'T DO YOU ANY GOOD\n\
        ON THE WAY TO THE CLOSET\n"
/* Out of stash space */
#define E222 "222 BUMMER, DUDE!\n\
	ON THE WAY TO %d\n"
/* Too many variables. */
#define E333 "333 YOU CAN'T HAVE EVERYTHING, WHERE WOULD YOU PUT IT?\n\
	ON THE WAY TO %d\n"
/* A COME FROM statement references a non-existent line label. */
#define E444 "444 IT CAME FROM BEYOND SPACE\n\
	ON THE WAY TO %d\n"
/* More than one COME FROM references the same label. */
#define E555 "555 FLOW DIAGRAM IS EXCESSIVELY CONNECTED\n\
	ON THE WAY TO %d\n"
/* Too many source lines. */
#define E666 "666 COMPILER HAS INDIGESTION\n\
	ON THE WAY TO %d\n"
/* No such source file. */
#define E777 "777 A SOURCE IS A SOURCE, OF COURSE, OF COURSE\n\
	ON THE WAY TO %d\n"
/* Can't open C output file */
#define E888 "888 I HAVE NO FILE AND I MUST SCREAM\n\
	ON THE WAY TO %d\n"
/* Can't open C skeleton file. */
#define E999 "999 NO SKELETON IN MY CLOSET, WOE IS ME!\n\
	ON THE WAY TO %d\n"
/* Source file name with invalid extension (use .i or .[3-7]i). */
#define E998 "998 EXCUSE ME,\n\
	YOU MUST HAVE ME CONFUSED WITH SOME OTHER COMPILER\n"
/* Illegal possession of a controlled unary operator. */
#define E997 "997 ILLEGAL POSSESSION OF A CONTROLLED UNARY OPERATOR.\n\
	ON THE WAY TO %d\n"

extern void lose();

/* lose.h ends here */
