#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Board.hh"
#include "constant.hh"


// 1 - Specs
// stones_in_my_storage - stones_in_opponent_storage    
int heurStorage(int myId, Board board) {
    return board.getStorage()[myId] - board.getStorage()[1 - myId];
}

// 2 - Specs
// W1 * (stones_in_my_storage – stones_in_opponents_storage) +
// W2 * (stones_on_my_side – stones_on_opponents_side)
int heurStoragePits(int myId, Board board) {
    int myPit = 0, opponentPit = 0;

    for (int i=0; i < N_PITS; i++) {
        myPit += board.getPit(myId, i);
        opponentPit += board.getPit(1-myId, i);
    }
    return W_STORAGE * heurStorage(myId, board) + W_PITS * (myPit - opponentPit);
}

// 3 - Specs
// (stones_in_my_storage – stones_in_opponents_storage) +
// (stones_on_my_side – stones_on_opponents_side) +
// (additional_move_earned)
int heurStoragePitsMove(int myId, int extraMoves, Board board) {
    return heurStoragePits(myId, board) + W_MOVES * extraMoves;
}

// 4
// how close I am to winning
int heurCloseToWinning(int myId, Board board) {
    int HALF_BEADS = N_PITS * N_BEADS;
    return floor(100 * board.getStorage()[myId] / HALF_BEADS);
}

// 5
// How many I have captured in the last move
int heurCaptured(int myId, Board board, Board oldBoard) {
    int captured = 0;
    for (int i=0; i < N_PITS; i++) {
        if (oldBoard.getPit(myId, i) == 0) {
            if (board.getPit(myId, i) == 0) {
                if (oldBoard.getPit(1-myId, N_PITS-i-1) != 0) {
                    if (board.getPit(1-myId, N_PITS-i-1) == 0) {
                        captured += oldBoard.getPit(1-myId, N_PITS-i-1);
                    }
                }
            }
        }
    }
    return W_CAPTURED * captured;
}

// 6
// How many moves that can lead to a bonus move
int heurPossibleExtraMoves(int myId, Board board) {
    int moves = 0;
    for (int i=0; i < N_PITS; i++) {
        if (board.getPit(myId, i) + i == N_PITS) {
            moves++;
        }
    }
    return W_POSSIBLE_EXTRA_MOVES * moves;
}

// ############# MEGA HEURISTIC ################
// #############################################

// 7
// SPM + CAPTURED + ADDITIONAL_MOVES + CLOSEWIN
int heurMega(int myId, int extraMoves, Board board, Board oldBoard) {
    return heurStoragePitsMove(myId, extraMoves, board) + heurCaptured(myId, board, oldBoard) \
    + heurPossibleExtraMoves(myId, board) + heurCloseToWinning(myId, board);
}

#endif