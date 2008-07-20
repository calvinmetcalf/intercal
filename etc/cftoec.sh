#! /bin/sh
# This shell-script takes one argument, the path to a source
# distribution of cfunge, and compiles that source distribution into a
# library (libick_ecto_b98.a) suitable for linking to C-INTERCAL
# programs.

test x$1 != x || (
    echo Please give the path to the root of a cfunge source
    echo distribution as an argument to this script.
    exit 1
) || exit 1

CFUNGEPATH=`realpath $1` || (
    echo Please give the path to the root of a cfunge source
    echo distribution as an argument to this script\; the directory
    echo or file you gave does not exist.
    exit 1
) || exit 1

test -f $CFUNGEPATH/src/interpreter.c || (
    echo Please give the path to the root of a cfunge source
    echo distribution as an argument to this script.
    echo You gave $CFUNGEPATH, but it does not appear to be a cfunge
    echo source distribution.
    exit 1
) || exit 1

test -f IFFI.spec || (
    echo Please run this script in the /etc directory of your
    echo C-INTERCAL distribution.
    exit 1
) || exit 1

echo Compiling cfunge into a library suitable for use with C-INTERCAL:

cd temp

CFLAGS="--std=c99 -DUSE32 -DDISABLE_GC -DFUNGE_EXTERNAL_LIBRARY -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED -DFUNGE_OLD_HANDPRINT=0x43464649 '-DFUNGE_NEW_HANDPRINT=\"http://example.com/\"'"

echo "   " Duplicating cfunge source tree so that it can be modified...
cp -r $CFUNGEPATH/src $CFUNGEPATH/lib .

# Comment these lines back in if patching is needed
# echo "   " Patching cfunge source tree for use with C-INTERCAL...
# patch -p0 < ../cfunge.patch

echo "   " Adding the IFFI fingerprint...
cp -r ../IFFI ../IFFI.spec src/fingerprints

echo "   " Regenerating fingerprint lists...
$CFUNGEPATH/tools/gen_fprint_list.sh > /dev/null

# Trust the filenames that are in the cfunge directory tree; if the user's
# trying to trick themselves into running this on something that looks like
# the cfunge directory tree but which has spaces or quotes in the filenames,
# I really can't be bothered to save them from themself. Dump all the resulting
# object files in this temp directory.
echo "   " Compiling source...
find . -name '*.c' -printf "gcc $CFLAGS -c %p -o %f.o\n" | sh

echo "   " Creating library...
ar cr ../../lib/libick_ecto_b98.a *.o
ranlib ../../lib/libick_ecto_b98.a

echo "   " Cleaning up...
rm -rf *
cd ..

echo Finished, unless there were errors.
