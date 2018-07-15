#!/usr/bin/perl -w
use strict;

#open COORDS, "show-coords -rclH $delta |" 
#  or die "Can't run show-coords -rclH $delta\n";

my $USAGE = "find-redundant.pl coords_file min_perc_id min_perc_cov > redundant.list\n";

my $coordsfile   = shift @ARGV or die $USAGE;
my $MIN_PERC_ID  = shift @ARGV or die $USAGE;
my $MIN_PERC_COV = shift @ARGV or die $USAGE;

open COORDS, "$coordsfile" or die "Can't open $coordsfile\n";
print STDERR "Processing coords file ($coordsfile)...\n";

my $alignments = 0;
my $validalignments = 0;
my %contigs;

while (<COORDS>)
{
  $alignments++;

  chomp;
  my @vals = split /\s+/, $_;

  my $rstart = $vals[1];
  my $rend   = $vals[2];

  my $alenr = $vals[7];
  my $alenq = $vals[8];

  my $pid  = $vals[10];
  my $lenr = $vals[12];
  my $lenq = $vals[13];

  my $covr = $vals[15];
  my $covq = $vals[16];

  my $rid  = $vals[18];
  my $qid  = $vals[19];

  $contigs{$rid}->{len} = $lenr;
  $contigs{$qid}->{len} = $lenq;

  next if ($pid < $MIN_PERC_ID);
  next if ($rid eq $qid);

  $validalignments++;

  my $ainfo;
  $ainfo->{rstart} = $rstart;
  $ainfo->{rend} = $rend;
  $ainfo->{qid}  = $qid;

  push @{$contigs{$rid}->{overlap}}, $ainfo;
}

print STDERR "Processed $alignments alignment records [$validalignments valid]\n";



## Look for jointly contained contigs
###############################################################################

my $jointcontained = 0;

## process from smallest to biggest, so that bigger contigs are preferred to be kept
foreach my $ctg (sort {$contigs{$a}->{len} <=> $contigs{$b}->{len}} keys %contigs)
{
  if (exists $contigs{$ctg}->{overlap})
  {
    my $clen = $contigs{$ctg}->{len};

    my %octgs;
    my $mappedbp = 0;
    my $lastend = -1;

    ## Plane sweep to find non-redundant mapped bases

    foreach my $ainfo (sort {$a->{rstart} <=> $b->{rstart}} @{$contigs{$ctg}->{overlap}})
    {
      ## skip alignments to stuff that is already contained
      next if (exists $contigs{$ainfo->{qid}}->{contained});

      my $mstart = $ainfo->{rstart};
      if ($lastend > $mstart) { $mstart = $lastend; }

      if ($ainfo->{rend} > $mstart)
      {
        my $newmap = $ainfo->{rend} - $mstart;
        $mappedbp += $newmap;
        $lastend = $ainfo->{rend};
        $octgs{$ainfo->{qid}} += $newmap;
      }
    }

    
    ## If a large fraction of this contig is mapped, mark it contained
    my $pcov = sprintf("%0.02f", 100.0 * $mappedbp / $clen);
    print "# $ctg [$clen] $pcov :";

    if ($pcov >= $MIN_PERC_COV)
    {
      $jointcontained++;

      foreach my $oid (sort keys %octgs)
      {
        my $olen = $contigs{$oid}->{len};
        my $omap = $octgs{$oid};
        print " $oid [$omap $olen]";

        push @{$contigs{$ctg}->{contained}}, $oid;
      }
    }

    print "\n";
  }
}

print STDERR "Found $jointcontained joint contained contigs\n";



## Print final results
###############################################################################

my $cnt = 0;
foreach my $ctg (sort keys %contigs)
{
  if (exists $contigs{$ctg}->{contained})
  {
    $cnt++;
    my $clen = $contigs{$ctg}->{len};
    print "$cnt $ctg [$clen] :";
    foreach my $parent (@{$contigs{$ctg}->{contained}})
    {
      my $plen = $contigs{$parent}->{len};
      print " $parent [$plen]";
    }

    print "\n";
  }
}

print STDERR "Printed $cnt total contained contigs\n";


