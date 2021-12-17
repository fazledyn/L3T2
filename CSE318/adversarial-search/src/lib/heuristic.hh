#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Board.hh"
#include "constant.hh"

#define W_STORAGE   1
#define W_PITS      2
#define W_MOVES     3

// 1
// stones_in_my_storage - stones_in_opponent_storage    
int heurStorage(int myId, Board board) {
    return board.getStorage()[myId] - board.getStorage()[1 - myId];
}

// 2
// W1 * (stones_in_my_storage – stones_in_opponents_storage) +
// W2 * (stones_on_my_side – stones_on_opponents_side)
int heurStoragePits(int myId, Board board) {
    int myPit = 0, opponentPit = 0;

    for (int i=0; i < N_PITS; i++) {
        myPit += board.getPit()[myId][i];
        opponentPit += board.getPit()[1-myId][i];
    }
    return W_STORAGE * heurStorage(myId, board) + W_PITS * (myPit - opponentPit);
}

// 3
// W1 * (stones_in_my_storage – stones_in_opponents_storage) +
// W2 * (stones_on_my_side – stones_on_opponents_side) +
// W3 * (additional_move_earned)
int heurStoragePitsMove(int myId, int extraMoves, Board board) {
    return heurStoragePits(myId, board) + W_MOVES * extraMoves;
}


#endif