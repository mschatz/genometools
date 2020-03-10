#!/usr/bin/perl -w
use strict;

my $USAGE = "skip #lines_to_skip\n";

die $USAGE if (scalar @ARGV == 0);
my $skip = shift @ARGV;

while ($skip > 0)
{
  my $junk = <>;
  $skip--;
}

while (<>)
{
  print;
}



