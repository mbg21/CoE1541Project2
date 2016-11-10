#!/bin/bash

rm CPU+cache.o
rm cache_out.txt

gcc -o CPU+cache.o CPU+cache.c
./CPU+cache.o sample.tr 0 > cache_out.txt
open -a TextMate cache_out.txt