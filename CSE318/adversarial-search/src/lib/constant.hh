#ifndef CONSTANT_H
#define CONSTANT_H

//  Mancala Game Constant
#define N_BEADS         4
#define N_PITS          6

// Algorithm Constant
#define MINIMAX_DEPTH   5

//  Heuristic Constant
//  Options must be added in `calculateCost()` of `Player.hh` file
#define HEU_STORAGE             1
#define HEU_STORAGE_PITS        2
#define HEU_STORAGE_PITS_MOVES  3

// Game Mode Constant
#define MODE_HUMAN      1
#define MODE_MINIMAX    2
#define MODE_ITERATIVE  3

#endif