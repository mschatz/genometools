#!/bin/bash

for i in `/bin/ls *.fasta`
do
  if [ ! -r $i.self.delta ]
  then 
    echo $i
    nucmer --maxmatch --no-simplify -l 15 -c 25 $i $i -p $i.self
  fi
done

for i in `/bin/ls *.fasta`
do
  echo -n "$i "
  awk '{print $8}' $i.stats | tr -d '\n'
  echo -n " "
  show-coords -H $i.self.delta | awk '{if (($1 < $4) && ($7 >= 100) && ($10 > 75)){print}}' | stats -f 8 
done

