/* lose.h --- error message strings

/*
 * These error messages are documented in the INTERCAL-72 manual.
 */
#define E000 "000 An undecodable statement has been encountered in the course of execution."
#define E017 "017 CAN'T YOU SPELL, YOU HOPELESS DWEEB?\n\
        (An expression contains a syntax error.)"
#define E079 "079 Improper use has been made of statement identifiers."
#define E099 "099 Improper use has been made of statement identifiers."
#define E123 "123 YOUR PROGRAM HAS VANISHED INTO THE BLACK LAGOON!\n\
        (Program has attempted 80 levels of NEXTing.)"
#define E129 "129 Program has attempted to transfer to a non-existent line label."
#define E139 "139 An ABSTAIN or REINSTATE statement references a non-existent line label."
#define E182 "182 A line label has been multiply defined."
#define E197 "197 An invalid line label has been encountered."
#define E200 "200 An expression involves an unidentified variable."
#define E240 "240 An attempt has been made to give an array a dimension of zero."
#define E241 "241 Invalid dimensioning information was supplied in defining or using an array."
#define E275 "275 DON'T BYTE OFF MORE THAN YOU CAN CHEW\n\
        A 32-bit value has been assigned to a 16-bit variable."
#define E436 "436 THROW STICK BEFORE RETRIEVING!\n\
        (A retrieval has been attempted for an unSTASHed value)."
#define E533 "533 A WRITE IN statement or interleave ($) operation has produced value requiring over 32 bits to represent."
#define E562 "562 Insufficient data."
#define E579 "579 Input data is invalid."
#define E621 "621 The expression of a RESUME statement evaluated to #0."
#define E632 "632 THE NEXT STACK RUPTURES. ALL DIE. OH, THE EMBARRASSMENT!\n\
        (Program execution terminated via a RESUME statement instead of GIVE UP)."
#define E633 "633 YOU HAVE FALLEN OFF THE EDGE OF THE WORLD. BYE-BYE!\n\
        (Execution has passed beyond the last statement of the program.)"
#define E774 "774 A compiler error has occurred (see section 8.1)."
#define E778 "778 An unexplainable compiler error has occurred (see J. Lyon or B. Woods)."

/*
 * These errors are unique to C-INTERCAL
 */
#define E222 "222 Out of stash space."
#define E333 "333 Too many variables."
#define E444 "444 A COME FROM statement references a non-existent line label."
#define E555 "555 More than one COME FROM references the same label."
#define E666 "666 Too many source lines."
#define E777 "777 No such source file."
#define E888 "888 Can't open C output file"
#define E999 "999 Can't open C skeleton file."

extern void lose();

/* lose.h ends here */



