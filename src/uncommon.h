/* uncommon.h -- declarations for uncommon.c and C files linked to it */

#include "config.h"

#include "ick_bool.h"

/*@null@*/ /*@dependent@*/ extern FILE* ick_debfopen(/*@observer@*/ const char*, /*@observer@*/ const char*);
/*@null@*/ /*@dependent@*/ extern FILE* ick_findandfopen(/*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*);
/*@observer@*/ /*@null@*/ extern const char* ick_findandtestopen(/*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*);
/*@null@*/ /*@dependent@*/ extern FILE* ick_findandfreopen(/*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, /*@observer@*/ const char*, FILE*);
extern int ick_snprintf_or_die(/*@out@*/ char *str, size_t size, /*@observer@*/ const char *format, ...)
#ifdef __GNUC__
  __attribute__ ((format(printf, 3, 4)))
#endif
;
