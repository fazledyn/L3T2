#ifndef PLAYER_H
#define PLAYER_H

#include "bits/stdc++.h"

#include "Board.hh"
#include "constant.hh"
#include "heuristic.hh"
#include "color.hh"

class Player {
    int id;
    int heuristic;
    int extraMoves;
    int mode;

    int calculateCost(Board board) {
        switch (heuristic) {
            case HEU_STORAGE:
                return heurStorage(id, board);
            case HEU_STORAGE_PITS:
                return heurStoragePits(id, board);
            case HEU_STORAGE_PITS_MOVES:
                return heurStoragePitsMove(id, extraMoves, board);
            default:
                return 0;
        }
    }

    // Minimize beta, Maximize Alpha
    int minimaxAlgorithm(Board board, bool isMax, int depthToGo, int alpha, int beta) {

        if (board.gameIsOver()) {
            if (board.getWinner() == id) { 
                return INT_MAX/2;
            } else if (board.getWinner() == 1 - id) {
                return INT_MIN/2;
            } else {
                return calculateCost(board);
            }
        }

        if (depthToGo == 0) {
            return calculateCost(board);
        }

        Board _board;
        _board.makeCopy(board);

        if (isMax) {
            int bestValue = INT_MIN;
            int succesor = -1;
            for (int i=0; i < N_PITS; i++) {
                if (board.getPit()[id][i] > 0) {

                    // printGreen(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " + to_string(i));
                    // board.printBoard();
                    int nextPlayer = board.makeMove(id, i);
                    // board.printBoard();
                    // printGreen(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " + to_string(i));
                    int currentVal;

                    if (nextPlayer == id) {
                        extraMoves++;
                        currentVal = minimaxAlgorithm(board, true, depthToGo - 1, alpha, beta);
                        extraMoves--;
                    } else {
                        currentVal = minimaxAlgorithm(board, false, depthToGo - 1, alpha, beta);
                    }

                    // cout << "Current value: " << currentVal << endl;
                    // cout << "Best value: " << bestValue << endl;
                       
                    if (currentVal > bestValue) { 
                        bestValue = currentVal;
                        succesor = i;
                    }

                    alpha = max(alpha, bestValue);
                    if (beta <= alpha) {
                        break;
                    }
                    // cout << "########### board ###########" << endl;
                    // board.printBoard();
                    // cout << "########## board = _board ###########" << endl;
                    board.makeCopy(_board);
                    // board.printBoard();
                }
            }
            if (depthToGo == MINIMAX_DEPTH) {
                return succesor;
            } else {
                return bestValue;
            }
        }
        else {
            int bestValue = INT_MAX;
            for (int i = 0; i < N_PITS; i++) {
                if (board.getPit()[1 - id][i] > 0) {
                    int nextPlayer = board.makeMove(1 - id, i);
                    int currentVal;

                    if (nextPlayer == 1 - id) {
                        extraMoves--;
                        currentVal = minimaxAlgorithm(board, false, depthToGo - 1, alpha, beta);
                        extraMoves++;
                    } else {
                        currentVal = minimaxAlgorithm(board, true, depthToGo - 1, alpha, beta);
                    }

                    if (currentVal < bestValue) {
                        bestValue = currentVal;
                    }

                    beta = min(beta, bestValue);
                    if (beta <= alpha) {
                        break;
                    }
                    board.makeCopy(_board);
                }
            }
            return bestValue;
        }
    }

    int humanMove() {
        int move;
        cout << "Enter move: ";
        cin >> move;
        return move;
    }

public:
    Player(int _id, int _heuristic, int _mode) {
        id = _id;
        mode = _mode;
        extraMoves = 0;
        heuristic = _heuristic;
    }

    int getId() { return id; }
    int getExtraMoves() { return extraMoves; }

    int makeMove(Board board) {
        switch(mode) {
            case MODE_MINIMAX: {
                int move = minimaxAlgorithm(board, true, MINIMAX_DEPTH, INT_MIN, INT_MAX);
                printYellow("Player " + to_string(id + 1) + "'s move: " + to_string(move));
                return move;
            }

            case MODE_HUMAN: {
                return humanMove();
            }
            
            default:
                return -1;
        }
    }
};

#endif