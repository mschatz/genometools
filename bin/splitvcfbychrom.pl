#!/usr/bin/perl -w
use strict;
use FileHandle;

my $USAGE = "splitvcfbychrom.pl vcf basename\n";

my $vcffile  = shift @ARGV or die $USAGE;
my $basename = shift @ARGV or die $USAGE;

open VCF, $vcffile or die "Cant open $vcffile ($!)\n";

my %chrout;
my @header;

while (<VCF>)
{
  if (/^#/) { push @header, $_; }
  else
  {
    my ($chr) = split /\s+/, $_;
    if (!exists $chrout{$chr})
    {
      my $fh = FileHandle->new("> $basename.$chr.vcf");
      $chrout{$chr} = $fh;
      print STDERR "Processing $chr...\n";

      foreach my $h (@header)
      {
        print $fh $h;
      }
    }

    my $fh = $chrout{$chr};
    print $fh $_;
  }
}
