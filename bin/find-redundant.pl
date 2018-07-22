#!/usr/bin/perl -w
use strict;

#open COORDS, "show-coords -rclH $delta |" 
#  or die "Can't run show-coords -rclH $delta\n";

my $USAGE = "find-redundant.pl coords_file min_perc_id min_perc_cov > redundant.list\n";

my $coordsfile   = shift @ARGV or die $USAGE;
my $MIN_PERC_ID  = shift @ARGV or die $USAGE;
my $MIN_PERC_COV = shift @ARGV or die $USAGE;

my $MAX_CHAIN_DIST = 25000;

my $VERBOSE = 1;
my $PATHVERBOSE = 0;


## Parse the coords file for valid alignments
###############################################################################

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

  my $qstart = $vals[4];
  my $qend   = $vals[5];

  my $qoo    = "F";
  if ($qstart > $qend) { $qoo = "R" };

  my $alenr = $vals[7];
  my $alenq = $vals[8];

  my $pid  = $vals[10];

  my $lenr = $vals[12];
  my $lenq = $vals[13];

  my $rid  = $vals[18];
  my $qid  = $vals[19];

  $contigs{$rid}->{len} = $lenr;
  $contigs{$qid}->{len} = $lenq;

  next if ($pid < $MIN_PERC_ID);
  next if ($rid eq $qid);

  $validalignments++;

  my $ainfo;
  $ainfo->{rstart} = $rstart;
  $ainfo->{rend}   = $rend;

  $ainfo->{alenr}  = $alenr;

  $ainfo->{qid}    = $qid;
  $ainfo->{qstart} = $qstart;
  $ainfo->{qend}   = $qend;

  push @{$contigs{$rid}->{align}->{$qid}->{$qoo}}, $ainfo;
}

print STDERR "Processed $alignments alignment records [$validalignments valid]\n";



## Find the longest alignment chain per sequence
###############################################################################

my $numcontigs = scalar keys %contigs;
my $totaledges = 0;

print STDERR "Finding chains for $numcontigs contigs...\n";

## process from smallest to biggest, so that bigger contigs are preferred to be kept
foreach my $ctg (sort {$contigs{$a}->{len} <=> $contigs{$b}->{len}} keys %contigs)
{
  if (exists $contigs{$ctg}->{align})
  {
    my $clen = $contigs{$ctg}->{len};

    foreach my $qid (sort keys %{$contigs{$ctg}->{align}})
    {
      my $bestspanall = -1;
      my $bestpathall = undef;

      my %salign;
      $salign{F} = undef;
      $salign{R} = undef;

      foreach my $dir ('F', 'R')
      {
        if (exists $contigs{$ctg}->{align}->{$qid}->{$dir})
        {
          my @align = sort {$a->{rstart} <=> $b->{rstart}} @{$contigs{$ctg}->{align}->{$qid}->{$dir}};
          $salign{$dir} = \@align;

          if ($PATHVERBOSE)
          {
            my $qlen = $contigs{$qid}->{len};
            print "$ctg [$clen] $qid [$qlen] $dir\n";
            for (my $i = 0; $i < scalar @align; $i++)
            {
              my $rstart = $align[$i]->{rstart};
              my $rend   = $align[$i]->{rend};
              my $qstart = $align[$i]->{qstart};
              my $qend   = $align[$i]->{qend};
              print "\t<$i$dir>\t$rstart\t$rend\t|\t$qstart\t$qend\n";
            }
          }

          for (my $i = 0; $i < scalar @align; $i++)
          {
            for (my $j = 0; $j < $i; $j++)
            {
               ## sorted scan: 0 ... j ... i ... n
               ## check if alignment j is compatible with alignment i
               my $rdist = $align[$i]->{rstart} - $align[$j]->{rend};
               my $qdist = $align[$i]->{qstart} - $align[$j]->{qend};
               if ($dir eq "R") { $qdist = $align[$j]->{qend} - $align[$i]->{qstart} }

               my $valid = 0;

               ## First check the distance between the alignments and ref position
               if ((abs($rdist) < $MAX_CHAIN_DIST) && 
                   (abs($qdist) < $MAX_CHAIN_DIST) &&
                   ($align[$i]->{rstart} > $align[$j]->{rstart}) &&
                   ($align[$i]->{rend}   > $align[$j]->{rend}))
               {
                 $valid = 1;
               }

               ## Now check the query positions
               if ($valid)
               {
                 $valid = 0;

                 if ($dir eq "F")
                 {
                   ##    ----------------------------------------------------
                   ##          s----j------->e
                   ##                             s------i------->e
                   
                   if (($align[$i]->{qstart} > $align[$j]->{qstart}) &&
                       ($align[$i]->{qend}   > $align[$j]->{qend}))
                   {
                     $valid = 1;
                   }
                 }
                 else
                 {
                   ##    ----------------------------------------------------
                   ##          <s----j-------e
                   ##                             <s-----i--------e

                   if (($align[$j]->{qstart} > $align[$i]->{qstart}) &&
                       ($align[$j]->{qend}   > $align[$i]->{qend}))
                   {
                     $valid = 1;
                   }
                 }
               }

               if ($valid)
               {
                 $totaledges++;
                 push @{$align[$j]->{edge}}, $i;
               }
            }
          }

          if ($PATHVERBOSE)
          {
            for (my $i = 0; $i < scalar @align; $i++)
            {
              if (exists $align[$i]->{edge})
              {
                print "edges from <$i$dir>:";
                foreach my $j (@{$align[$i]->{edge}})
                {
                  print "\t<$j$dir>";
                }
                print "\n";
              }
            }
          }

          ## find the longest chain starting at each node (if not already visited)
          for (my $i = 0; $i < scalar @align; $i++)
          {
            next if exists $align[$i]->{visit};

            ## start a DFS at node i to explore chains passing through it
            my $path;
            $path->{chainstart} = $align[$i]->{rstart};
            $path->{chainend}   = $align[$i]->{rend};
            $path->{dir}        = $dir;
            push @{$path->{nodes}}, $i;

            my $bestspani = -1;
            my $bestpathi = undef;

            my @stack;
            push @stack, $path;

            while (scalar @stack > 0)
            {
              my $path = pop @stack;
              my $pathlen = scalar @{$path->{nodes}};
              
              my $lastnode  = $path->{nodes}->[$pathlen-1];
              $align[$lastnode]->{visit}++;

              if (exists $align[$lastnode]->{edge})
              {
                ## If I can keep extending, extend with all children
                foreach my $e (@{$align[$lastnode]->{edge}})
                {
                  my @nodes = @{$path->{nodes}};
                  push @nodes, $e;
                  my $newpath;
                  $newpath->{nodes} = \@nodes;
                  $newpath->{chainstart} = $path->{chainstart};
                  $newpath->{chainend}   = $path->{chainend};
                  $newpath->{dir} = $dir;
                  if ($align[$e]->{rend} > $newpath->{chainend}) { $newpath->{chainend} = $align[$e]->{rend}; }
                  push @stack, $newpath;
                }
              }
              else
              {
                ## no place else to go, score the path
                my $chainstart = $path->{chainstart};
                my $chainend   = $path->{chainend};
                my $chainspan  = $chainend - $chainstart + 1;

                if ($chainspan > $bestspani)
                {
                  $bestspani = $chainspan;
                  $bestpathi = $path;
                }
              }
            }

            ## best path from node i
            if ($PATHVERBOSE)
            {
              print "bestspani <$i$dir>\t$bestspani";

              if (defined $bestpathi)
              {
                print "\t|\t$bestpathi->{chainstart}\t$bestpathi->{chainend}\t|\t";
                foreach my $n (@{$bestpathi->{nodes}})
                {
                  print "\t<$n$dir>";
                }
                print "\n";
              }
            }

            ## check if this is the best overall
            if ($bestspani > $bestspanall)
            {
              $bestspanall = $bestspani;
              $bestpathall = $bestpathi;
            }
          }
        }
      }
            
      ## overall best chain between this pair of contigs
      if ($VERBOSE)
      {
        my $clen = $contigs{$ctg}->{len};
        my $qlen = $contigs{$qid}->{len};

        print "bestspanall $ctg [$clen] $qid [$qlen] : $bestspanall\n";

        if (defined $bestpathall)
        {
          my $dir = $bestpathall->{dir};
          print "\t$dir\t|\t$bestpathall->{chainstart}\t$bestpathall->{chainend}\t|\t";
          foreach my $n (@{$bestpathall->{nodes}})
          {
            print "\t<$n$dir>";
          }
          print "\n";

          foreach my $n (@{$bestpathall->{nodes}})
          {
            my $rstart = $salign{$dir}->[$n]->{rstart};
            my $rend   = $salign{$dir}->[$n]->{rend};
            my $qstart = $salign{$dir}->[$n]->{qstart};
            my $qend   = $salign{$dir}->[$n]->{qend};
            print "\t<$n$dir>\t$rstart\t$rend\t|\t$qstart\t$qend\n";
          }

          print "\n\n";
        }
      }
    }
  }
}

print STDERR "Found $totaledges total edges\n";



## Look for jointly contained contigs
###############################################################################

my $jointcontained = 0;

## process from smallest to biggest, so that bigger contigs are preferred to be kept
foreach my $ctg (sort {$contigs{$a}->{len} <=> $contigs{$b}->{len}} keys %contigs)
{
  if (exists $contigs{$ctg}->{chain})
  {
    my $clen = $contigs{$ctg}->{len};

    my %octgs;
    my $mappedbp = 0;
    my $lastend = -1;

    ## Plane sweep to find non-redundant mapped bases

    foreach my $ainfo (sort {$a->{rstart} <=> $b->{rstart}} @{$contigs{$ctg}->{chain}})
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

      foreach my $oid (sort {$octgs{$b} <=> $octgs{$a}} keys %octgs)
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


