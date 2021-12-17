#!/usr/bin/bash
g++ test.cpp -o testMancala
chmod +x testMancala
./testMancala $1 >> "output-$1"
ls -lA --color=auto
rm testMancala
