dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl Yes, that is a valid email address. If your mailer doesn't support nested
dnl comments, then get a better mailer.
AC_INIT([C-INTERCAL],[0.29],[[ais523(524\)x\((525)y)@bham.ac.uk]],[ick])
AC_CONFIG_SRCDIR([src/abcess.h])
AC_CONFIG_HEADER([config.h:src/configh.in])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign dist-bzip2 dist-lzma tar-pax check-news
		  no-texinfo.tex])

dnl Information about this version of INTERCAL.
PACKAGE_VERSION=0.29

dnl Checks for programs.
AC_PROG_YACC
AC_PROG_CC
AM_PROG_LEX
AC_PROG_RANLIB
AC_PROG_LN_S
AC_CHECK_TOOL([AR],[ar])
AC_SUBST(AR)

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/time.h unistd.h stdarg.h])
AC_CHECK_HEADERS([stdint.h],[HAVE_STDINT_H=1])
AC_SUBST(HAVE_STDINT_H)

dnl Checks for typedefs, structures, and compiler characteristics.
dnl This Autoconf input is compatible with pre-C99 Autoconf, so to
dnl check for long long AC_CHECK_SIZEOF is used to test for a nonzero result,
dnl likewise for _Bool.
AC_C_CONST
AC_C_VOLATILE
AC_CHECK_SIZEOF([long long int],[0])
AC_CHECK_SIZEOF([_Bool],[0])
AC_CHECK_SIZEOF([sig_atomic_t],[0],[[#include <signal.h>]])
AC_HEADER_TIME
AC_EXEEXT
AC_OBJEXT
AC_SYS_INTERPRETER
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_MSG_CHECKING([[whether yyrestart() is needed]])
# Write a lex program that will cause an error if yyrestart exists.
echo '%%
%%
float yyrestart(int, ...);' | $LEX
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "${LEX_OUTPUT_ROOT}.c"
]], [[exit(0);]])],[AC_MSG_RESULT(no)],[AC_DEFINE([NEED_YYRESTART],dnl
[],[Define if yyrestart() is necessary to yyparse a new file.])
AC_MSG_RESULT(yes)])
rm -f ${LEX_OUTPUT_ROOT}.c

if test "$interpval" != "no"; then
AC_DEFINE([HAVE_SYS_INTERPRETER], [], [Define if #! works on your system.])
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

# Use prebuilt files if necessary, after config.h is generated.
# Automake is capable of handling the files in question, but they need to
# be in the directory configure was run in.
if test ! -n "$LEX"; then
  cp $srcdir/prebuilt/lexer.c $ac_pwd/lexer.c
  AC_MSG_WARN([No version of lex was found; using prebuilt lexer.])
  AC_MSG_WARN([Changes to src/lexer.l will be ignored.])
fi
if test ! -n "$YACC"; then
  cp $srcdir/prebuilt/parser.c $ac_pwd/parser.c
  cp $srcdir/prebuilt/parser.h $ac_pwd/parser.h
  cp $srcdir/prebuilt/oil-oil.c $ac_pwd/oil-oil.c
  AC_MSG_WARN([No version of yacc was found; using prebuilt parser.])
  AC_MSG_WARN([Changes to src/parser.y and src/oil.y will be ingored.])
fi

cd $ac_pwd;

# This trick for unsetting variables comes from the autoconf manual

# "|| exit" suppresses any "Segmentation fault" message.
if ( (MAIL=60; unset MAIL) || exit) >/dev/null 2>&1; then
  unset=unset
else
  unset=false
fi

BUILD_MACHINE_ARG="FOO=BAR"
if test ! "x$build_alias" == x;
then
  BUILD_MACHINE_ARG="--build=$build_alias"
fi

AC_OUTPUT(Makefile coopt.sh:src/cooptsh.in, [chmod a+x coopt.sh])
