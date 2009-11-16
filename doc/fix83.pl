#!/bin/perl

# Change .html to .htm and change all URLs appropriately in the file
# being edited; also fix a bug in makeinfo that sometimes causes
# incorrect anchors to be used in the META REFRESH commands.
#
# This script assumes the output from makeinfo is reasonably correct, so it
# needs to be run on a system that supports filenames longer than 8.3; it
# converts these long filenames into 8.3 form.

my @base32str=qw{0 1 2 3 4 5 6 7 8 9 a b c d e f
		 g h i j k l m n o p q r s t u v};

# Hash a string into 8 5-bit segments, base32 encoded
# The algorithm isn't designed to be secure or anything like that, but simply
# to avoid collisions in what filenames are mapped to. I don't know how well
# it succeeds at this. Note that this can't map to "index" unless the input
# is "index", in which case it always does; this is a special case. The
# hashing is case-insensitive to reduce errors on DOS.
sub hash8x5($)
{
    my $low32sf=0;
    my $high32sf=0;
    my $str=shift;
    $str=lc $str;
    defined($str) or die;
    return "index" if $str eq "index";
    while($str ne '')
    {
	$low32sf ^= !!($low32sf & (1<<19));
	$low32sf &= (1<<19)-1;
	$low32sf <<= 1;
	$low32sf ^= ord $str;
	$low32sf *= 41;
	$high32sf ^= !!($high32sf & (511<<11));
	$high32sf &= (1<<11)-1;
	$high32sf <<= 9;
	$high32sf ^= ord $str;
	$high32sf *= 43;
	$str = substr $str, 1;
    }
    $str = $base32str[$low32sf & 31]; $low32sf >>= 5;
    $str .= $base32str[$low32sf & 31]; $low32sf >>= 5;
    $str .= $base32str[$low32sf & 31]; $low32sf >>= 5;
    $str .= $base32str[$low32sf & 31];
    $str .= $base32str[$high32sf & 31]; $high32sf >>= 5;
    $str .= $base32str[$high32sf & 31]; $high32sf >>= 5;
    $str .= $base32str[$high32sf & 31]; $high32sf >>= 5;
    $str .= $base32str[$high32sf & 31];
    return $str;
}

$/=undef; #read the whole file at a time

open CSSFILE, "<ickstyle.css";
my $css = <CSSFILE>;
close CSSFILE;

while(<>)
{
    /^<meta http-equiv="refresh"/s and do{
	s/\#\%/'#g_t%'/ge;
	s/\%/_00/g;
	s/(url=)([^\">]*?).html([\"\#])/$1 . hash8x5($2) . ".htm" . $3/ge;
    };
    s/(\-\-><\/style>)/$css$1/;
    s/(href=\")([^\">]*?).html([\"\#])/$1 . hash8x5($2) . ".htm" . $3/ge;
    s=^(.*?)\.html$="mv html/".$1.".html html/".hash8x5($1).".htm"=gemi;
    print;
}
