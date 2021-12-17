#ifndef COLOR_H
#define COLOR_H

#include <iostream>
using namespace std;

void printRed(string text) {
    cout << "\033[1;31m" << text << "\033[0m";
}

void printBlue(string text) {
    cout << "\033[1;34m" << text << "\033[0m";
}

void printYellow(string text) {
    cout << "\033[1;33m" << text << "\033[0m";
}

void printGreen(string text) {
    cout << "\033[1;32m" << text << "\033[0m";
}

#endif