#!/bin/bash

#
# This script will perform the following:
# Sweep the amount of work for CPU and GPU from 0 to 15
# 
# Take md5(deadbeef)=4f41243847da6934f356c0486114bc6
# as our automatical testing

make

for i in {0..15}
do
  echo -n "CPU_WORK = $i, GPU_WORK = $((16 - $i))"
  ./a.out $i aae6635e044ac56046b2893a529b5114
done

