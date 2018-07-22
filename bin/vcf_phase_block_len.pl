#!/usr/bin/perl -w
use strict;

my %blocks;

while (<>)
{
  next if /^#/;

  chomp;
  my @fields = split /\s+/, $_;

  my $chr = $fields[0];
  my $pos = $fields[1];
  my $codestr = $fields[8];
  my $datastr = $fields[9];

  my @codes = split /:/, $codestr;
  my @datas = split /:/, $datastr;

  for (my $i = 0; $i < scalar (@codes); $i++)
  {
    if ($codes[$i] eq "PS")
    {
      my $ps = $datas[$i];

      my $key = "$chr:$ps";

      if (!exists $blocks{$key})
      {
        $blocks{$key}->{chr}   = $chr;
        $blocks{$key}->{start} = $pos;
        $blocks{$key}->{end}   = $pos;
        $blocks{$key}->{snp}   = 1;
      }
      else
      {
        my $ochr = $blocks{$key}->{chr};
        if ($chr ne $ochr)
        {
          print STDERR "Chromosome mismatch $chr != $ochr: $_\n";
        }

        $blocks{$key}->{end} = $pos;
        $blocks{$key}->{snp} += 1;
      }
    }
  }
}

foreach my $b (keys %blocks)
{
  my $start = $blocks{$b}->{start};
  my $end   = $blocks{$b}->{end};
  my $snp   = $blocks{$b}->{snp};
  my $span  = $end - $start + 1;
  print "$b\t$span\t$snp\n";
}
