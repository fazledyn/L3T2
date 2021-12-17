#ifndef CONSTANT_H
#define CONSTANT_H

//  Mancala Game Constant
#define N_BEADS         4
#define N_PITS          6

// Algorithm Constant
#define MINIMAX_DEPTH   5

//  Heuristic Constants
//  Weights
#define W_STORAGE       1
#define W_PITS          5
#define W_MOVES         10
#define W_CLOSE_TO_WIN  15
#define W_CAPTURED      20

//  Options
//  Must be added in `calculateCost()` of `Player.hh` file
#define HEU_STORAGE         1
#define HEU_S_PITS          2
#define HEU_SP_MOVES        3
#define HEU_SPM_CLOSEWIN    4
#define HEU_SPMC_CAPTURED   5


// Game Mode Constant
#define MODE_HUMAN      1
#define MODE_MINIMAX    2
#define MODE_ITERATIVE  3

#endif
