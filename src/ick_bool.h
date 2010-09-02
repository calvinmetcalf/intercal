/*
 * Hide difference between compilers with the C99 bool type and those without.
 * The main reason this is desirable is so splint can do more rigorous checking
 * on bools.
 */

/*@-redef@ -incondefs*/
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#endif
#endif
#ifndef __bool_true_false_are_defined
typedef int bool;
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#endif
/*@=redef =incondefs@*/
