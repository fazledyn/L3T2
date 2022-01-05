#!/usr/bin/bash
rm output.txt
rm a.out
g++  main.cpp -lpthread 
./a.out
cat output.txt
