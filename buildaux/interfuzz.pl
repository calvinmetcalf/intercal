#
# NAME
#     interfuzz.pl -- C-INTERCAL optimiser fuzz-tester
#
# LICENSE TERMS
#     Copyright (C) 2010 Alex Smith
#
#     This program is free software; you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation; either version 2 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program; if not, write to the Free Software
#     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

use strict;
use warnings;
use File::Temp qw/tempdir/;
use File::Spec;

use constant fuzzamount => 1000;

# We generate random INTERCAL expressions as follows: start with a
# placeholder _1, then repeatedly replace the placeholder with a more
# complicated expression, which is either a primitive equation (_1~_2,
# _V1, etc) or the LHS of an idiom (in order to increase the chance of
# finding idiom bugs). An OIL-style syntax is used internally in order
# to make things easier, it's translated into INTERCAL at the end
# (with any C operators replaced by INTERCAL definitions of them). The
# placeholders are eventually changed to random constants (50% chance)
# or to random variables in the set .123, except that every
# expression contains at least one non-constant variable. Random
# constants have a 50% chance of being completely random, and a 50%
# chance of being a number somehow significant to INTERCAL idioms.
# Instead of the OIL-like _1, we use _[1] in case we end up with more
# than 9 placeholders.

sub random_constant {
    my $bitshift = int(rand(30));
    $bitshift == 23 and $bitshift = 1; # 2 and 3 are significant
    $bitshift > 23 and $bitshift = 0;  # but not as significant as 0 and 1
    $bitshift > 16 and $bitshift = 16; # or as 65535
    my $constant = 1 << $bitshift;
    $constant > 65535 || (rand) > 0.5 and $constant--;
    return '#'.$constant;
}

sub random_operand {
    my $bitwidth = shift;
    my $whichoperand = rand;
    $whichoperand < 0.5 and return '.'.(int(rand(3))+1);
    if ($bitwidth == 16) {
        $whichoperand < 0.75 and return '#'.int(rand(65536));
        return random_constant;
    } else {
        $whichoperand < 0.75 and
            return '(#'.int(rand(65536)).'$#'.int(rand(65536)).')';
        return '('.random_constant.'$'.random_constant.')';
    }
}

sub oil_expression {
    my $expr = ':[1]';
    local $_;
    while ((rand) < 0.9 || $expr eq ':[1]') {
        my @placeholders = ($expr =~ /[.:]\[\d+\]/g);
        my $unused_placeholder = 1;
        /[.:]\[(\d+)\]/ and $1 >= $unused_placeholder
            and $unused_placeholder = $1 + 1 for @placeholders;
        my $replaced_placeholder = $placeholders[int(rand(@placeholders))];
        my $whichexpr = (rand) * 0.35; # todo: idioms from file
        my $newexpr;
        if ($replaced_placeholder =~ /:/) {{
            $whichexpr < 0.08  and $newexpr = '(:~:)', last;
            $whichexpr < 0.2  and $newexpr = '(.$.)', last;
            $whichexpr < 0.25 and $newexpr = '?:', last;
            $whichexpr < 0.3  and $newexpr = 'V:', last;
            $whichexpr < 0.35 and $newexpr = '&:', last;
        }} else {{
            $whichexpr < 0.08 and $newexpr = '(.~.)', last;
            $whichexpr < 0.2  and $newexpr = '(:~.)', last;
            $whichexpr < 0.25 and $newexpr = '?.', last;
            $whichexpr < 0.3  and $newexpr = 'V.', last;
            $whichexpr < 0.35 and $newexpr = '&.', last;
        }}
        $newexpr =~ s/([.:])/$1."[".($unused_placeholder++)."]"/eg;
        # Use {} as maybe-brackets; we don't know if $newexpr needs parens yet.
        $expr =~ s/\Q$replaced_placeholder\E/{$newexpr}/g;
        $expr =~ s/([?&V])\{([?&V][^{}]+)\}/$1($2)/g;
        $expr =~ s/[{}]//g;
    }
    my $newop;
    $newop = random_operand(16), $expr =~ s/\Q$_\E/$newop/g
        for ($expr =~ /.\[\d+\]/g);
    $newop = random_operand(32), $expr =~ s/\Q$_\E/$newop/g
        for ($expr =~ /:\[\d+\]/g);
    $expr =~ /[.:]/ or goto &oil_expression;
    return $expr;
}

sub intercal_expression {
    my $expr = oil_expression;
    # TODO: Translate C to INTERCAL

    # Two changes are needed to allow for INTERCAL's insane syntax:
    # move all unaries forwards one character, and change parens into
    # spark/ears. In theory we could safely change everything to
    # sparks as the location of operands disambiguates, but we'll be
    # good and alternate in order to make life easier on a reader.
    my $spark = 1;
    $expr =~ s/([()])/($spark=!$spark),$spark ^ ($1 eq '(')?"'":'"'/ge;
    $expr =~ s/([?&V]+)([#.'"])/$2$1/g;
    return $expr;
}

# The main program: create a temporary directory, and an INTERCAL program
# in it. We run 1000 expressions, each on 1000 sets of variable values.

print "Generating test data...\n";
my $dir = tempdir( CLEANUP => 1 );
my $fn = File::Spec->catfile($dir,"fuzz");
open my $fh, '>', "$fn.i";
my (@dot1, @dot2, @dot3);
# Store the data in a massive stash.
for (0..(fuzzamount-1)) {
    print $fh "PLEASE .1 <- ", ($dot1[$_]=random_constant);
    print $fh " DO .2 <- #", ($dot2[$_]=int(rand(65536)));
    print $fh " DO .3 <- ", ($dot3[$_]=random_constant);
    print $fh " DO STASH .1 + .2 + .3\n";
}
# The computed ABSTAIN is used as a loop counter.
print $fh "PLEASE ABSTAIN #1000 FROM (3) (1) DO COME FROM (2)\n";
print $fh "DO RETRIEVE .1 + .2 + .3\n";

print "Generating test program...\n";
my $pleaseflop = 0;
my @expressions = ();
for (0..(fuzzamount-1)) {
    $expressions[$_] = intercal_expression;
    $pleaseflop && print $fh "PLEASE ";
    $pleaseflop = !$pleaseflop;
    print $fh "DO :1 <- ", $expressions[$_], " DO READ OUT :1\n";
}
print $fh "(2) DO REINSTATE (3) (3) DO COME FROM (1) PLEASE GIVE UP\n";
close $fh;

print "Running optimiser and compiling to C...\n";
my $optimiser_output = `../build/ick -bOch $fn.i 2>&1`;
my %usedopts = ();
$usedopts{$_}++ for $optimiser_output =~ m/\[(\w+)\]/g;
print "Optimisations seen:\n";
printf '%8d %s'."\n", $usedopts{$_}, $_
    for sort {$usedopts{$b} <=> $usedopts{$a}} keys %usedopts;

print "Compiling optimised program to executable...\n";
open my $prog, '<', "$fn.c";
my $line1 = <$prog>;
close $prog;
# We grab the compile-command that ick places into the generated
# executable, and run it minus any -On option (because gcc is bad at
# optimising large programs, and because we're not fuzzing /it/).
my ($compilecommand) = $line1 =~ /compile-command:"([^"]+)"/;
$compilecommand =~ s/-O\d//;
system $compilecommand;

print "Running optimised program...\n";
my $optimised_output = `$fn`;

print "Compiling program unoptimised...\n";
system "../build/ick -b $fn.i";

print "Running unoptimised program...\n";
my $unoptimised_output = `$fn`;

if ($optimised_output eq $unoptimised_output) {
    print "No errors found!\n";
    exit(0);
}

print "Errors found:\n";
my @o1 = split $/, $unoptimised_output;
my @o2 = split $/, $optimised_output;
my %problemindices = ();
my %problemlines = ();
for my $index (0..$#o1) {
    $o1[$index] eq $o2[$index] and next;
    $problemindices{int($index/2)}=1;
}
for my $index (sort {$a%fuzzamount <=> $b%fuzzamount} keys %problemindices) {
    my $dataline = fuzzamount - 1 - int($index/1000);
    my $ickline = ($index % 1000) + 1;
    $problemlines{$ickline} = 1;
    my $line2 = "Line $ickline (.1 = ". $dot1[$dataline];
    $line2 .= ", .2 = #". $dot2[$dataline];
    $line2 .= ", .3 = ". $dot3[$dataline] . "): u = ";
    my $line1 = ' ' x length $line2;
    $line1 .= $o1[$index*2];
    $line2 .= $o1[$index*2+1] . ", o = ";
    $line1 .= ' ' x ((length $line2) - (length $line1));
    $line1 .= $o2[$index*2];
    $line2 .= $o2[$index*2+1];
    print "$line1\n$line2\n";
}
for my $index (sort {$a <=> $b} keys %problemlines) {
    print "Line $index is ".$expressions[$index-1]."\n";
}

exit(1);
