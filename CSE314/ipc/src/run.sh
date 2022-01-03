#!/usr/bin/bash
rm a.out
g++  main.cpp -lpthread 
./a.out
cat output.txt
