#!/bin/perl -w

# Deindent and delink part headings in the TOC, and remove the initial
# H2 entry.

my $f=1;

while(<>)
{
    $f and /<ul>/ and do{$f=0; next;};
    /^<li><a[^>]*>C-INTERCAL / and next;
    s%^<li><a[^>]*>(PART .*?)</a>%</ul><h3>$1</h3><ul>%;
    print;
    last if $_ eq "</div>\n";
}
while(<>) {print;}
