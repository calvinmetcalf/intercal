dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl Yes, that is a valid email address. If your mailer doesn't support nested
dnl comments, then get a better mailer.
AC_INIT([C-INTERCAL],[0.29],[[ais523(524\)x\((525)y)@bham.ac.uk]],[intercal])
AC_CONFIG_SRCDIR([src/perpet.c])
AC_CONFIG_HEADER([config.h:buildaux/configh.in])
AC_CONFIG_AUX_DIR([buildaux])
AM_INIT_AUTOMAKE([foreign dist-bzip2 dist-lzma tar-pax check-news
		  no-texinfo.tex -Wall -Wno-override])

dnl Information about this version of INTERCAL.
PACKAGE_VERSION=0.29

dnl Checks for programs.
AC_PROG_YACC
dnl Don't assume yacc exists just because bison doesn't (wtf autoconf...)
if test "x$YACC" == "xyacc"; then
YACC=
AC_CHECK_PROG([YACC],[yacc])
fi
AC_PROG_CC
AM_PROG_LEX
AC_PROG_RANLIB
AC_PROG_LN_S
AC_CHECK_TOOL([AR],[ar])
AC_SUBST(AR)

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_STDBOOL
if test "x$ac_cv_header_stdbool_h" == "xyes"
then
HAVE_STDBOOL_H=1
fi
AC_CHECK_HEADERS([sys/time.h unistd.h stdarg.h])
AC_CHECK_HEADERS([stdint.h],[HAVE_STDINT_H=1])
AC_SUBST(HAVE_STDINT_H)
AC_SUBST(HAVE_STDBOOL_H)

dnl Checks for typedefs, structures, and compiler characteristics.
dnl This Autoconf input is compatible with pre-C99 Autoconf, so to
dnl check for long long AC_CHECK_SIZEOF is used to test for a nonzero
dnl result.
AC_C_CONST
AC_C_VOLATILE
AC_CHECK_SIZEOF([long long int],[0])
AC_CHECK_SIZEOF([sig_atomic_t],[0],[[#include <signal.h>]])
AC_HEADER_TIME
AC_EXEEXT
AC_OBJEXT
AC_SYS_INTERPRETER
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Some compilers (correctly according to the standard, but dubiously for
# actual use in code) dislike mixing _Bool and varargs.
if test "x$ac_cv_type__Bool" == "xyes"
then
HAVE__BOOL=1
AC_MSG_CHECKING([[whether booleans can be passed via varargs]])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
void test(int x, ...)
#else
#include <varargs.h>
void test(va_alist) va_dcl
#endif
{
  int y,z;
  _Bool b;
  va_list ap;
#ifdef HAVE_STDARG_H
  va_start(ap, x);
  y = x;
#else
  va_start(ap);
  y = va_arg(ap, int);
#endif
  b = va_arg(ap, _Bool);
  z = va_arg(ap, int);
  if(b && x != z) exit(2);
  if(!b && x == z) exit(3);
}
]],[[
_Bool match = (_Bool)1;
_Bool nomatch = (_Bool) 0;
test(12345,match,12345);
test(23456,nomatch,1234);
exit(0);
]])],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)
AC_DEFINE([BOOL_VARARGS_BROKEN],[1],dnl
[Define to 1 if _Bool works but cannot be passed via varargs.])
],[AC_MSG_RESULT([[unknown in a cross-compile, assuming no]])
AC_DEFINE([BOOL_VARARGS_BROKEN],[1],dnl
[Define to 1 if _Bool works but cannot be passed via varargs.])
])
fi

AC_SUBST(HAVE__BOOL)

AC_MSG_CHECKING([[whether yyrestart() is needed]])
# Write a lex program that will cause an error if yyrestart exists.
echo '%%
%%
float yyrestart(int, ...);' | $LEX
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "${LEX_OUTPUT_ROOT}.c"
]], [[exit(0);]])],[AC_MSG_RESULT(no)],[AC_DEFINE([NEED_YYRESTART],dnl
[1],[Define to 1 if yyrestart() is necessary to yyparse a new file.])
AC_MSG_RESULT(yes)])
rm -f ${LEX_OUTPUT_ROOT}.c

if test "$interpval" != "no"; then
AC_DEFINE([HAVE_SYS_INTERPRETER], [1], [Define to 1 if #! works on your system.])
fi
AC_DEFINE_UNQUOTED([EXEEXT],"$EXEEXT",[Extension to use on executable files.])

dnl Checks for library functions.
AC_CHECK_FUNCS(gettimeofday gethrtime strdup strtol strtoul snprintf vsnprintf isatty)
ICK_SAVECFLAGS=$CFLAGS
CFLAGS="$CFLAGS -lrt"
AC_CHECK_FUNCS(clock_gettime)
CFLAGS=$ICK_SAVECFLAGS

dnl Substitutions.
INCLUDE_HIDDEN_FROM_AUTOMAKE="include"
AC_SUBST(PACKAGE_VERSION)
AC_SUBST(INCLUDE_HIDDEN_FROM_AUTOMAKE)

AC_CONFIG_FILES([Makefile:buildaux/Makefile.in coopt.sh:src/cooptsh.in host.mak:buildaux/hostmak.in abcess.h:src/abcessh.in])
AC_OUTPUT()
# After the output, chmod coopt.sh to be executable.
chmod a+x coopt.sh

# Use prebuilt files if necessary, after config.h is generated.
# Automake is capable of handling the files in question, but they need to
# be in the directory configure was run in.
if test ! -n "$LEX" || echo "$LEX" | grep missing > /dev/null 2>&1; then
  cp $srcdir/prebuilt/lexer.c $ac_pwd/lexer.c
  AC_MSG_WARN([No version of lex was found; using prebuilt lexer.])
  AC_MSG_WARN([Changes to src/lexer.l will be ignored.])
fi
if test ! -n "$YACC" || echo "$YACC" | grep missing > /dev/null 2>&1; then
  cp $srcdir/prebuilt/parser.c $ac_pwd/parser.c
  cp $srcdir/prebuilt/parser.h $ac_pwd/parser.h
  cp $srcdir/prebuilt/oil-oil.c $ac_pwd/oil-oil.c
  AC_MSG_WARN([No version of yacc was found; using prebuilt parser.])
  AC_MSG_WARN([Changes to src/parser.y and src/oil.y will be ingored.])
fi

# Configure again, for the build system, if necessary.
# It's simpler the second time round as we don't care about things like
# installation dirs, just about how to do things like run the compiler.
if test "$cross_compiling" != yes; then
  cp config.h bconfig.h
  exit 0
fi

cd $ac_pwd;

# This trick for unsetting variables comes from the autoconf manual

# "|| exit" suppresses any "Segmentation fault" message.
if ( (MAIL=60; unset MAIL) || exit) >/dev/null 2>&1; then
  unset=unset
else
  unset=false
fi

AC_DEFUN([GEN_BUILD_CMD_LINE],[
BUILD_$1_ARG="FOO=BAR"
if test ! "x$[]$1_FOR_BUILD" == x;
then
  BUILD_$1_ARG="$1=$[]$1_FOR_BUILD";
fi
$1=foo
$unset $1 || $1=''])

GEN_BUILD_CMD_LINE(CC)
GEN_BUILD_CMD_LINE(CFLAGS)
GEN_BUILD_CMD_LINE(LDFLAGS)
GEN_BUILD_CMD_LINE(LIBS)
GEN_BUILD_CMD_LINE(CPPFLAGS)
GEN_BUILD_CMD_LINE(CPP)

BUILD_MACHINE_ARG="FOO=BAR"
if test ! "x$build_alias" == x;
then
  BUILD_MACHINE_ARG="--build=$build_alias"
fi

# Run the new configure script.
mv config.status config.status.save
(env -i "PATH=$PATH" $srcdir/buildaux/buildconfig "$BUILD_MACHINE_ARG" "$BUILD_CC_ARG" "$BUILD_CFLAGS_ARG" "$BUILD_LDFLAGS_ARG" "$BUILD_LIBS_ARG" "$BUILD_CPPFLAGS_ARG" "$BUILD_CPP_ARG")
mv config.status config.status.build
mv config.status.save config.status
