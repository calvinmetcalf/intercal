#!/bin/perl -w

# Deindent and delink part headings in the TOC, and remove the initial
# H2 entry.

my $f=1;
my $g=1;

while(<>)
{
    $f and /<ul>/ and do{$f=0; next;};
    /^<li><a[^>]*>C-INTERCAL / and next;
    s%^<li><a[^>]*>(PART .*?)</a>%($g?($g=0,''):"</ul>")."<h3>$1</h3><ul>"%e;
    last if $_ eq "</div>\n";
    print;
}
while(<>) {print;}
