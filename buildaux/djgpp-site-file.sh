#! /bin/sh
# Based on version 1.3 of the DJGPP site file by Mark E.

echo "Setting up configure for C-INTERCAL on DJGPP..." >&2

# Sanity check
if test "x${DJDIR}" = "x"; then
  echo "Please set DJDIR to your main DJGPP installation directory." >&2
  exit 1
fi

# The root of the DJGPP tree serves as the default prefix.
# Allow for cases where a top-level Cygnus/Red Hat-style configure script
# calls Autoconf configure scripts in subdirectories.
if test "x$ac_default_prefix" = "x/usr/local"; then
ac_default_prefix="/dev/env/DJDIR"
fi

# This is required for config.status script to be run, since
# ./configure runs it by invoking ${CONFIG_SHELL-/bin/sh}

export CONFIG_SHELL='/dev/env/DJDIR/bin/sh.exe'
export SHELL=${CONFIG_SHELL}

# $INSTALL must be an absolute path name, otherwise config.status
# will try to prepend ./ and ../ to it when it goes into subdirs.

export INSTALL=${INSTALL='/dev/env/DJDIR/bin/ginstall -c'}

# Help out libtool
export ac_exeext=".exe"
export lt_cv_sys_max_cmd_len=12288

# Variables to help DJGPP tools be detected.
# For C-INTERCAL, this is set by hand, as autodetect gets it wrong.
export PATH_SEPARATOR=:
export ac_executable_extensions=.exe

# DJGPP gcc is a bit ambiguous about extensions; choose the right one.
export ac_cv_objext=o
export ac_cv_exeext=.exe

export TEST_FINDS_EXE=Y
