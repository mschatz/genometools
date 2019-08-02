#!/bin/bash

for i in `/bin/ls *.fasta` 
do 
  echo $i 
  for k in 40 45 50
  do 
    echo $i $k 
    ../../kmc -ci1 -k$k -m2 -fm $i $i.k$k.kmc . > $i.k$k.kmc.log
  done 
done
