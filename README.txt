                         Release Notes for C-INTERCAL

                                  DISCREDITS

This package is an implementation of the language INTERCAL designed by Don
Woods and James Lyon, who have since spent more than twenty years trying to
live it down.

This implementation was created by Eric S. Raymond <esr@snark.thyrsus.com>
during a fit of lunacy from which he has since mostly recovered.  It has been
considerably hacked upon and improved by Steve Swales, Michael Ernst, Louis
Howell, Brian Raiter, Alex Smith, Joris Huizer and others.  For detailed
credits see NEWS.

                                     FILES

The compiler itself is in the src directory.  Setting a symbol in the makefile
will allow you to install it and its libraries and support files in a directory
of your choice.  The doc directory contains (surprise!)  documentation, and pit
contains various examples of INTERCAL source code, writhing in agony.

The files included are:

README.txt              -- this file
BUGS.txt                -- current bug and to-do list
NEWS.txt                -- info on features and fixes new in this release
COPYING.txt             -- copyright and licence information
MANIFEST.txt            -- a list of all files in the distribution

etc/intercal.el         -- elisp for editing INTERCAL code under GNU Emacs

doc/READ.ME             -- historical notes on the docs
doc/ick.txi             -- the Revamped Manual sources, a newer manual
doc/fdl-1-2.txi         -- the GNU FDL version 1.2 in Texinfo form
doc/ick.txt             -- the Revamped Manual in plain text form
doc/ick.inf             -- the Revamped Manual in Info
doc/ickstyle.css	-- styling rules for the Revamped Manual in HTML
doc/tidy.cfg		-- HTML tidying rules for the Revamped Manual
doc/fix83.pl            -- script to fixup the HTML output for 8.3 systems
doc/intercal.mm         -- the old manual in groffable form
doc/Makefile            -- makefile to build old and new manuals
doc/THEORY.txt          -- some notes on the internals of the INTERCAL compiler
doc/chipspec.txt        -- plans for an INTERCAL-based microprocessor
doc/ick.1               -- man page for ick
doc/convickt.1          -- man page for convickt

src/abcess.h            -- interface to functions linked with programs
src/arrgghh.c           -- option argument parsing for generated programs
src/bin2c.c             -- creates C code representations of binary files
src/cesspool.c          -- the INTERCAL runtime support code
src/clc-cset.c          -- compatibility code for CLC-INTERCAL character sets
src/csetstub.c          -- small file to allow convickt to link
src/configdj.h          -- config.h substitute for DJGPP without config.sh
src/convickt.c          -- command line character set converter
src/coopt.sh            -- script to help out the -F optimizer (POSIX only)
src/dekludge.c          -- INTERCAL optimizer wrapper and utility functions
src/feh.h               -- interface to INTERCAL-to-C code generator
src/feh2.c              -- INTERCAL-to-C code generator
src/fiddle.[ch]         -- the INTERCAL operators
src/ick-wrap.c          -- the driver for generated C-from-INTERCAL code
src/ick.h               -- compilation types and defines
src/ick_ec.[ch]		-- external call support for INTERCAL
src/ick_lose.[ch]       -- INTERCAL compile- and run-time error handling
src/idiotism.oil        -- list of INTERCAL idioms to optimize
src/lexer.l             -- the lexical analyzer specification (in LEX)
src/numerals.c          -- numerals table for INTERCAL input
src/oil.y               -- compiler to translate idiotism.oil into C
src/oil.h               -- name mangling for OIL files
src/parser.y            -- the grammar specification (in YACC)
src/perpet.c            -- main routine for compiler
src/pick1.h             -- template for compiler-dependent PIC C headers
src/pick2.h             -- runtime support for compiled PIC-INTERCAL
src/pickwrap.c          -- the driver for generated PIC C-from-INTERCAL code
src/sizes.h             -- variables defining the numeric base
src/uncommon.[ch]       -- code common to the compiler and debugger
src/unravel.c           -- multithreading functions
src/yuk.[ch]            -- INTERCAL debugger and profiler
src/*.bin               -- character set definitions for clc-cset and convickt

temp/parser.[ch]        -- Prebuilt parser (for DJGPP systems without bison)
temp/lexer.c            -- Prebuilt lexer (for DJGPP systems without flex)
temp/oil.c              -- Prebuilt OIL compiler (again, for bisonless systems)

makeick.bat             -- script to compile under DJGPP
districk.bat            -- script to remove files not distributed (DJGPP)
cleanick.bat            -- script to remove all non-source files (DJGPP)
etc/ctrlmfix.bat        -- script to remove ^Ms that may have got in by mistake
makeick.sh              --\
districk.sh             --|the same as above, but in sh format
cleanick.sh             --|(use .bat under command.com/cmd, and .sh under bash)
etc/ctrlmfix.sh         --/

etc/cftoec.sh         	-- script to convert cfunge to a library (see below)
etc/IFFI*               -- input to that script
etc/cfunge.patch        -- more input to that script
etc/temp                -- temporary directory used by that script

config.sh               -- GNU Autoconf script that generates a makefile
                           from Makefile.in
ickspec.in              -- Input to config.sh
config.ac               -- Source from which config.sh was generated
configh.in              -- Source from which temp/config.h is generated
Makefile.in             -- makefile input for the INTERCAL compiler

bin/*                   -- Directory that holds binaries after the build
include/*               -- Directory that holds header files after the build
lib/*                   -- Directory that holds library files after the build

pit/*                   -- sample INTERCAL code; see pit/CATALOG
pit/explib/*            -- source code for expansion libraries
pit/lib/syslib.*i       -- source code for the system libraries

                              HOW TO GET STARTED

First, note that there is a much fuller manual than this file available. For
full information about C-INTERCAL, including extensive installation and usage
instructions, see that one; run `info -f doc/ick.inf', or failing that read the
plain text version at doc/ick.txt. (You can build a variety of other formats as
well, such as HTML and PDF, using the Makefile in the doc directory.) After
C-INTERCAL is installed, the manual is also available with just `info ick' for
information about running ick, the compiler, or `info C-INTERCAL' for the
available information about ick and C-INTERCAL.  (If you installed C-INTERCAL
in a directory that did not contain the main Info documentation tree, instead
`info ick' will do what `info C-INTERCAL' should do, and `info C-INTERCAL'
will not work at all.)

You want a man page? Man pages are for wimps. (However, Debian wrote one
anyway, and it's available in the distribution as ick.1.) To compile an
INTERCAL program `foo.i' to executable code, just do

        ick foo.i

There's a -c option that leaves the generated `foo.c' in place for inspection
(suppressing compilation to machine code), a -d option that enables verbose
parse reporting from the yacc/bison parser, a -t option that requires strict
INTERCAL-72 compliance (rejecting COME FROM and the extensions for bases other
than two), a -b option that disables the INTERCAL-72 random-bug feature, and an
-O option that enables the (hah!) optimizer (which can be invoked at higher
levels with -Of or -F).  Invoking ick -?  prints a usage message.  The -m
option enables multithreading extensions. If you need to debug code, -g
generates the C code in such a way that it can be debugged with a C
debugger. -h, -H, and -hH generate debugging output from the optimizer, which
is useful if you need to understand an INTERCAL program, and -l turns on
warnings (far too many warnings!). If ick itself needs debugging, the -U option
will cause it to dump core on E778. Other than that, yer on yer own.

-t has been mentioned above as a compatibility option; -X is also permitted,
and causes the program to treat the ambiguous characters ? and @ as having the
meanings that they have in CLC-INTERCAL rather than their traditional
C-INTERCAL meanings, and also uses the CLC-INTERCAL meaning for some commands
(such as an ABSTAIN aiming at a GIVE UP command's line label).

Another switch affects C-INTERCAL's runtime behavior.  The `-C' option forces
output in "clockface" mode, for superstitious users who believe writing "IV"
upside-down offends IVPITER and would rather see IIII.

Every INTERCAL runtime also accepts certain options at runtime (code by Steve
Swales).  These include [+/-]help, [+/-]traditional, and [+/-]wimpmode.  The
help option (with either + or -) triggers a 'usage' message. The +traditional
option is presently a no-op. Also available are [+/-] printflow, which gives
information that may or may not be helpful for debugging a program, and [+/-]
mystery, which is deliberately undocumented.

Steve writes: "The wimpmode option is the most interesting. I found myself
always running my test programs with filters on both ends to work around the
'nifty' INTERCAL number representations. This was so painful that I decided it
would be LESS painful (and a lot less code) if I added a 'wimp' option.  With
the +wimpmode option, the user is subjected to a humiliating message about what
a wimp he or she is to use this mode, but after that is allowed to use
conventional numerical notation.  While such a mode doubtless violates to some
extent the INTERCAL philosophy, the fact that a 'unbutcher' command has been
posted clearly indicates the need for it. Anyway... if you don't like it, don't
use it... the default is -wimpmode (i.e. NOT wimp mode)."

                              SPREADING THE BLAME

There is an INTERCAL Resource Page at http://www.catb.org/intercal

The latest version of INTERCAL is also kept available at the Retrocomputing
Museun, http://www.catb.org/retro.

(Note that the above paragraph appears to be out of date; last I checked, that
version wasn't the most recent. Looking through the alt.lang.intercal archives
is usually the best way to find out what the latest version of C-INTERCAL (and
you'll find out how to get its rival too, for that matter) is nowadays; they
tend to pop up in various unusual places, so giving a URL might be misleading.
Note also that contact details, and even who to contact, often end up out of
date; the newsgroup is also a good way to find out who to contact.

As of when I wrote this, the current homepage for C-INTERCAL and CLC-INTERCAL
is http://intercal.freeshell.org.)

There is, in addition, an occasionally active USENET newsgroup devoted to the
language: alt.lang.intercal.

                           NOTES ON THE POSIX BUILD

The configure script in this distribution is called 'config.sh', to be kind to
people trying to build under DOS, and also just because we like being
different. (You can build and install C-INTERCAL using the familiar routine of
config.sh, then make, then make install.) C-INTERCAL should run fine without
being installed, as long as you give a full absolute or relative pathname to
its executable, and don't move things from the directories they were compiled
into. The compilation must happen in-place; several empty directories are
distributed with C-INTERCAL specifically for being compiled into.

I have come across lexes whose default limits for various things aren't high
enough to handle INTERCAL's various lexing conundrums. In this case, pay
attention to the error messages they give you, and alter the source file
src/lexer.l accordingly. (Note that previously you might also have had to
change some defines at the top of ick.h to allow for features of your version
of lex; this is no longer required, as one of the defines is now detected by
config.sh and the source files were refactored to make the other moot.)

If you are using gcc in an non-ANSI environment, such as SunOS, it is strongly
recommended that you add "-Wno-unused -Wimplicit" to the gcc command in the
makefile. If you are in an ANSI environment and are using the current versions
of gcc, flex, and bison, C-INTERCAL should build with no warnings (except some
from rm saying it can't find files, which is not a problem; if installing for
a second or subsequent time, you may also get a harmless warning from mkdir
explaining that the directory it's trying to make already exists, and you can
ignore that too; also, it's been known for gcc to warn about things that bison
or flex generated and so aren't our fault).

If you get an error about 'gettimeofday()', or if you have a more accurate
timing function available, there's a whole section about profiling in yuk.h
that you can change to affect the accuracy of the timings. (Option 0 is pure
ANSI C, which can be used as a fallback to allow the debugging library to
compile if you have nothing better.) Note that the configure script now tries
to detect what timing functions you have available; it's pretty good at
finding ANSI and POSIX functions, but at the moment I only have one
nonstandard one there (gethrtime, which was on one proprietary UNIX system I
tested on); feel free to add code to call other proprietary functions in much
the same way if they're more accurate than gettimeofday() or POSIX
alternatives on some system.

                           NOTES ON THE DJGPP BUILD

This distribution needs prebuilding; you can use the makeick.bat or makeick.sh
file provided to compile the program if you can't compile using the usual
POSIX methods via configure and make. (Unlike some previous releases of
C-INTERCAL, the configure/make process can work under DJGPP; invoke the
configure script as 'sh config.sh' (without the sparks), and run make as
normal. However, this requires that you've installed quite a few utilities in
addition to just gcc, including at least bash, perl, sed, tr and make, but
possibly other things I missed.) Once the implementation has been built, you
can install it by adding the \bin subdirectory to your PATH environment
variable (at either end), or by using 'make install' (again without the
sparks) if you got config.sh and make to work (which will copy the INTERCAL
compiler into your main DJGPP installation directories). The temporary output
from bison and flex is included in the distribution (for those people who
don't have bison and flex, makeick will give error messages, but succeed
anyway using the prebuilt versions). The districk.bat file will remove all
generated files that weren't in the distribution, and the cleanick.bat file
will remove all files that aren't needed for a build (including the temporary
bison/flex output). Note that makeick.bat and makeick.sh produce
DJGPP-specific output; and if you're running DJGPP under bash under Windows
XP, you may need to either set your default command interpreter for batch
files to cmd rather than command.com (by setting your SHELL environment
variable), or use some better workaround that I don't know about. (command.com
seems to have problems with the PATH environment variable under Windows XP
when running under bash.) Also try to make sure that you use the .sh versions
of the makeick.* scripts when running under bash or a similar ported shell,
and the .bat versions when running under a DOS shell.

Some distributions of DJGPP have a bug where they complain that they can't find
'm4.exe' (or maybe my PATH is still wrong...). You can solve this problem by
copying m4.exe into the temp\ subdirectory before running makeick.bat.

(P.S. It was my PATH that was wrong. Still, if you made the same mistake as me,
you may want to try the same fix.)

Some features (for instance, -F) are sometimes unavailable under DJGPP (they
are documented, but have no effect); note that if you do compile a program
under DJGPP using -F, the resulting program may not run if invoked using a
command interpreter other than bash (because -F sometimes produces a shell
script with a .exe extension, rather than a DOS-format executable file). Note
also that the profiler is pretty much useless under DOS; this is a problem
inherent in DOS, in that it doesn't provide timings to any decent resolution
(it was worse than 50ms last I checked, and the DJGPP documentation agrees
with me on this; INTERCAL may be traditionally considered to be slow, but it
isn't that slow, especially with the optimiser enabled).

You can get a copy of DJGPP at <http://www.delorie.com/djgpp>.

If you release your own version of C-INTERCAL and have edited it on DJGPP,
please run etc/ctrlmfix.bat (or sh) from the main installation directory before
releasing it; this will remove any trailing control-M characters that may have
got into the files by mistake.

                        NOTES ON COMPILING PIC-INTERCAL

PIC C compilers tend to differ substantially in the way they handle various
language constructs. As a result, this compiler will only compile to C (not to
hex or asm) when compiling a PIC-INTERCAL program. There are two header files,
pick1.h and pick2.h in the /src directory; these need to be placed somewhere
your compiler can find them, and pick1.h will need to be modified to contain
compiler-specific data (like the names of data types and the syntax for
specifying the device type, fuses, and various commands).

	    NOTES ON THE C-INTERCAL / CFUNGE EXTERNAL CALLS SYSTEM

Code is provided to allow linking of Funge-98 programs to INTERCAL programs;
however, this requires a Funge-98 interpreter, which is not present in this
distribution. Therefore, in order to do this, a Funge-98 interpreter must
first be downloaded and prepared for use with C-INTERCAL. At present, code is
only available for linking with 'cfunge', a Funge-98 interpreter written in
C. You can get the latest sources for that interpreter from the Internet via
the bzr version-control system by running this command:

bzr branch http://rage.kuonet.org/~anmaster/bzr/cfunge

Once you have the source code to cfunge, you can compile it into a library
suitable for use by C-INTERCAL by changing to the /etc directory of the
C-INTERCAL distribution, and running the cftoec.sh shell script in that
directory with the path to the cfunge distribution as an argument. The script
will make a copy of cfunge, modify it as required, and then place the
resulting library (libick_ecto_b98.a) in the /lib directory of the C-INTERCAL
distribution; installing (or reinstalling) C-INTERCAL after that will copy
that library into the correct location in your filesystem.

For information on actually using this system once you've set it up, consult
the Revamped Manual.

                                   LICENSING

C-INTERCAL is now distributed under the General Public License, except for the
C skeleton file which is explicitly *not* GPLed in order to avoid the (possibly
mythical) "license virus" effect.  See the file COPYING.txt for details on the
General Public License.

                                  APOCALYPSE

INTERCAL is fully Y2K-compliant.  Indeed, it is Y2K-obsequious, and loves
nothing better than to be punished by relays of leather-clad and spike-heeled
calendricists.  Bite me and go buy your own shotgun.
