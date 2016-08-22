#!/bin/bash

#
# This script will perform the following:
# Sweep the amount of work for CPU and GPU from 0 to 15
# 

make

for i in {0..15}
do
  echo -n "CPU_WORK = $i, GPU_WORK = $((256 - $i))"
  ./a.out $i $1
done

