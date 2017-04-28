#!/bin/bash

scan_dir=$1
export outdir=$(echo $scan_dir | sed 's/data/ana/')
mkdir -p $outdir

cut -d " " -f 1 $scan_dir/point_list.txt > xlist.txt
cut -d " " -f 2 $scan_dir/point_list.txt > ylist.txt
cut -d " " -f 3 $scan_dir/point_list.txt > zlist.txt
find $scan_dir -name "*.root" | sort > root_files.txt

root -l 'compare.C()' 