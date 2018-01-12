#!/bin/bash

scan_dir=$1
export outdir=$(echo $scan_dir | sed 's/data/ana/')
mkdir -p $outdir

rm intensity_list.txt
if [ -e $scan_dir/thr_list.txt ]; then
  echo "is a threshold scan"
  export scan_thr="true"
  cp $scan_dir/thr_list.txt thr_list.txt
  cp $scan_dir/thr_list.txt intensity_list.txt
elif [ -e $scan_dir/thr_list_pt.txt ]; then
  echo "is a threshold scan"
  export scan_thr="true"
  cp $scan_dir/thr_list_pt.txt thr_list.txt
  cp $scan_dir/thr_list_pt.txt intensity_list.txt
elif [ -e $scan_dir/vlist.txt ]; then
  echo "is a voltage scan"
  export scan_v="true"
  cp $scan_dir/vlist.txt vlist.txt
  cp $scan_dir/vlist.txt intensity_list.txt
else 
  cut -d " " -f 1 $scan_dir/point_list.txt > xlist.txt
  cut -d " " -f 2 $scan_dir/point_list.txt > ylist.txt
  cut -d " " -f 3 $scan_dir/point_list.txt > zlist.txt
fi

find $scan_dir -name "*.hld.root" | sort > root_files.txt

# for i in $(find $scan_dir -name "laser_intensity.txt" | sort); do
# echo $(cat $i) | sed 's/,/./' >> intensity_list.txt
# done
cp $scan_dir/triggers.txt ./intensity_list.txt

root -l 'compare.C()' $2 ## can add -q via #2