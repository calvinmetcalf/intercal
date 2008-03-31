/* Public domain, by Alex Smith.
   An example of CREATing statements from C, in this case an example COMPUNEX
   statement that does a computed NEXT. (Note that newly CREATEd statements
   can't contain consecutive characters that spell an INTERCAL keyword; also,
   I wanted a name that was 8 chars or shorter to serve as a DOS filename.)

   Note that as a CREATEd command, COMPUNEX uses one NEXT stack entry itself,
   meaning the COMPUNEX will take up two NEXT stack entries total. Programs
   that do large RESUMEs will need to take this into account. (Tail call
   optimisation would require some sort of GOTO, but ick_goto isn't documented
   and therefore shouldn't be used, right? */
#include <ick_ec.h>

/* This name starts ick_, but isn't used; in general, the prefix ick_my_ is
   reserved for expansion libraries (as are the line numbers 1600-1699) because
   it doesn't conflict either with the compiler or with any linked-in C
   programs. */
ICK_EC_FUNC_START(ick_my_compunex)
{
  ick_startup( {
      ick_create(".COMPUNEX",1600);
      ick_create(";COMPUNEX",1600);
      ick_create("~COMPUNEX",1600);
    });
  ick_linelabel(1600);
  ick_next(ick_c_value(0));
  ick_resume(1);
}
ICK_EC_FUNC_END
