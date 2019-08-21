#!/bin/sh

for f in $*
do
  if [ ! -r $f.stats ]
  then
    echo "computing stats of $f"
    stats -big 0,5000,10000,15000,20000,25000,30000,35000,40000,45000,50000,55000,60000,65000,70000,75000 $f > $f.stats &
  fi

done

wait

for f in $*
do
  if [ ! -r $f.stats.cov ]
  then
    echo "reformating cov for $f"
    tr ' ' '\n' < $f.stats | grep '^s>' | tr '=' ' ' | tr '>' ' ' | awk 'BEGIN{total=0}{if ($3 > total){total = $3} print $2, $3, $3/total}' > $f.stats.cov
  fi

done


