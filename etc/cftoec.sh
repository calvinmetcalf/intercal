#! /bin/sh
# This shell-script takes one argument, the path to a source
# distribution of cfunge, and compiles that source distribution into a
# library (libick_ecto_b98.a) suitable for linking to C-INTERCAL
# programs. The cfunge distribution itself is not modified; all changes
# are made on a copy.

die() { echo "Error: $1"; exit 1; }

if [ -z "$1" ] ||  [ -z "$2" ]; then
    echo "Usage: $0 ick-source-path cfunge-path"
    exit 1
fi

# test x$1 != x || (
#     echo Please give the path to the root of a cfunge source
#     echo distribution as an argument to this script.
#     exit 1
# ) || exit 1

ICKSRC_PATH="$1"
CFUNGE_PATH="$2"
BUILD_PATH="$(pwd)"

ICKSRC_PATH=`(cd $ICKSRC_PATH; pwd)` || (
    echo Please give the path to the root of C-INTERCAL source
    echo distribution as the first argument to this script\; the directory
    echo or file you gave does not exist.
    exit 1
) || exit 1

CFUNGE_PATH=`(cd $CFUNGE_PATH; pwd)` || (
    echo Please give the path to the root of a cfunge source
    echo distribution as the second argument to this script\; the directory
    echo or file you gave does not exist.
    exit 1
) || exit 1

test -f "$CFUNGE_PATH/src/interpreter.c" || (
    echo Please give the path to the root of a cfunge source
    echo distribution as the second argument to this script.
    echo You gave $CFUNGE_PATH, but it does not appear to be a cfunge
    echo source distribution.
    exit 1
) || exit 1

test -f "$ICKSRC_PATH/etc/IFFI.spec" || (
    echo Please give the path to the root of the C-INTERCAL source
    echo distribution as the first argument of this script.
    echo You gave $ICKSRC_PATH, but it does not appear to be a C-INTERCAL
    echo source distribution.
    exit 1
) || exit 1

echo Compiling cfunge into a library suitable for use with C-INTERCAL:

rm -rf cftoec_temp || die "Failed to remove any old temp directory"
mkdir -p cftoec_temp || die "Failed to create temp directory"
cd cftoec_temp  || die "Failed to change directory to temp directory"

# Select compiler
if [ -z "$CC" ]; then
    CC=cc
fi

CFLAGS="-std=c99 -DUSE32 -DDISABLE_GC -DFUNGE_EXTERNAL_LIBRARY -DCFUN_IS_IFFI -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED -DFUNGE_OLD_HANDPRINT=0x43464649 '-DFUNGE_NEW_HANDPRINT=\"http://example.com/\"'"
# If GCC 4.1 or later is used add:
#   -O1 -combine -fwhole-program -fvisibility=hidden.
# At least -O1 needed or -fwhole-program have no effect. Which is needed for
# this trick.
if "$CC" --version 2>&1 | grep -q GCC; then
    GCC_VERSION="$("$CC" --version 2>&1 | awk '/[^ ]+ \(GCC\)/ {print $3}')"
    GCC_MAJOR="$(echo "$GCC_VERSION" | cut -d. -f 1)"
    GCC_MINOR="$(echo "$GCC_VERSION" | cut -d. -f 2)"
    # We need GCC 4.1 or later.
    if [ "$GCC_MAJOR" -eq 4 ]; then
        if [ "$GCC_MINOR" -ge 1 ]; then
            IS_GCC=yes
        fi
    elif [ "$GCC_MAJOR" -gt 4 ]; then
        IS_GCC=yes
    fi
fi
if [ "$IS_GCC" ]; then
    CFLAGS="$CFLAGS -O1 -combine -fwhole-program -fvisibility=hidden"
fi
# Allow optional -O2 or such:
if [ "$CFUNGE_CFLAGS" ]; then
    CFLAGS="$CFLAGS $CFUNGE_CFLAGS"
fi
# Please enable this when making changes. Requested by the cfunge author.
#CFLAGS="$CFLAGS -Wall -Wextra -pedantic -Wpointer-arith -Wimplicit -Wnested-externs -Wcast-align -Wcast-qual -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wparentheses -Wshadow -Wundef -Wpacked -Wfloat-equal -Wstrict-aliasing=2 -Wformat=2 -Wmissing-noreturn -Wmissing-format-attribute -Wdeclaration-after-statement -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wwrite-strings"

echo "   " Duplicating cfunge source tree so that it can be modified...
cp -r $CFUNGE_PATH/src $CFUNGE_PATH/lib $CFUNGE_PATH/tools .

# Comment these lines back in if patching is needed
# echo "   " Patching cfunge source tree for use with C-INTERCAL...
# patch -p0 < ../cfunge.patch

echo "   " Adding the IFFI fingerprint...
cp -r "$ICKSRC_PATH/etc/IFFI" "$ICKSRC_PATH/etc/IFFI.spec" src/fingerprints

echo "   " Regenerating fingerprint lists...
tools/gen_fprint_list.sh > /dev/null || die "Failed to regen list."

# Trust the filenames that are in the cfunge directory tree; if the user's
# trying to trick themselves into running this on something that looks like
# the cfunge directory tree but which has spaces or quotes in the filenames,
# I really can't be bothered to save them from themself. Dump all the resulting
# object files in this temp directory.
echo "   " Compiling source...
if [ "$IS_GCC" ]; then
    FILES="$(find . -name '*.c' -printf "%p ")"
    # Pipe needed due to messy double quoting.
    # It is impossible to use -o -c and multiple files all at once. GCC errors
    # if you try. However the *.o will be equal to one of the source filenames
    # but placed in the current directory and with .c replaced with .o
    echo "$CC $CFLAGS -c $FILES" | sh
else
    find . -name '*.c' -printf "$CC $CFLAGS -c %p -o %f.o\n" | sh
fi

# Remove any existing library or ar would just add to the existing one.
rm -f ../libick_ecto_b98.a

echo "   " Creating library...
ar cr ../libick_ecto_b98.a *.o
ranlib ../libick_ecto_b98.a

echo "   " Cleaning up...
rm -rf *
cd ..
rm -rf cftoec_temp

echo Finished, unless there were errors.
