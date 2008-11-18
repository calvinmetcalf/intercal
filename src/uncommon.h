/* uncommon.h -- declarations for uncommon.c and C files linked to it */

#include "config.h"

/*@null@*/ /*@dependent@*/ extern FILE* ick_debfopen(/*@observer@*/ const char*, /*@observer@*/ const char*);
/*@null@*/ /*@dependent@*/ extern FILE* ick_findandfopen(/*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*);
/*@observer@*/ /*@null@*/ extern const char* ick_findandtestopen(/*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*);
/*@null@*/ /*@dependent@*/ extern FILE* ick_findandfreopen(/*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, FILE*);
