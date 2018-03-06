#!/usr/bin/perl 
use strict;

my $USAGE = "binpairs.pl binsize < pairs > bins.txt\n";

## plot with this in R
## t<-read.table("scaffolds85.b100k.txt")
## m<-as.matrix(t)
## image(-log(1+m,10), main="scaffoldXXX 100kb bins")

my $binsize = shift @ARGV or die $USAGE;

my %counts;

my $maxbin = 0;

while (<>)
{
  chomp;
  my ($s1, $s2) = split /\s+/, $_;

  my $b1 = int ($s1 / $binsize);
  my $b2 = int ($s2 / $binsize);

  #print "$s1 $s2 | $b1 $b2\n";

  $counts{$b1}->{$b2}++;
  $counts{$b2}->{$b1}++;

  if ($b1 > $maxbin) { $maxbin = $b1; }
  if ($b2 > $maxbin) { $maxbin = $b2; }
}

for (my $b1 = 0; $b1 <= $maxbin; $b1++)
{
  for (my $b2 = 0; $b2 <= $maxbin; $b2++)
  {
    if ($b2 > 0) { print "\t"; }

    my $c = 0;

    if (exists $counts{$b1}->{$b2})
    {
      $c = $counts{$b1}->{$b2};
    }

    print $c;
  }

  print "\n";
}
