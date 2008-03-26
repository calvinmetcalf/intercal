/* uncommon.h -- declarations for uncommon.c and C files linked to it */

#include "config.h"

/*@null@*/ /*@dependent@*/ extern FILE* ick_debfopen(/*@observer@*/ char*, /*@observer@*/ char*);
/*@null@*/ /*@dependent@*/ extern FILE* ick_findandfopen(/*@observer@*/ char*, /*@observer@*/ char*, /*@observer@*/ char*, /*@observer@*/ char*);
/*@observer@*/ /*@null@*/ extern char* ick_findandtestopen(/*@observer@*/ char*, /*@observer@*/ char*, /*@observer@*/ char*, /*@observer@*/ char*);
/*@null@*/ /*@dependent@*/ extern FILE* ick_findandfreopen(/*@observer@*/ char*, /*@observer@*/ char*, /*@observer@*/ char*, /*@observer@*/ char*, FILE*);
