/*
 * Hide difference between compilers with the C99 bool type and those without.
 * The main reason this is desirable is so splint can do more rigorous checking
 * on bools. This also allows for compilers that define _Bool but not true or
 * false, etc.
 */

/*@-redef@ -incondefs*/
#ifndef __bool_true_false_are_defined
# if HAVE_STDBOOL_H >= 1
#  include <stdbool.h>
# else
#  if !defined(HAVE__BOOL) || HAVE__BOOL < 1
#   if HAVE_STDINT_H >= 1
#    include <stdint.h>
typedef int_fast8_t bool;
#   else
typedef int bool;
#   endif
#  else
typedef _Bool bool;
#  endif
#  define true 1
#  define false 0
#  define __bool_true_false_are_defined 1
# endif
#endif
/*@=redef =incondefs@*/
