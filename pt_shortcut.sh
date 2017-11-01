#!/bin/bash

export TDC=0351 
export scan_thr=true 

chan=07 ./compare.sh $1; 
chan=17 ./compare.sh $1;

