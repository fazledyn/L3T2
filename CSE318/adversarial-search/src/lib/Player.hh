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
    int DEPTH;

    int calculateCost(Board board, Board oldBoard) {
        switch (heuristic) {

            case HEU_STORAGE:
                return heurStorage(id, board);
            case HEU_S_PITS:
                return heurStoragePits(id, board);
            case HEU_SP_MOVES:
                return heurStoragePitsMove(id, extraMoves, board);
            case HEU_CLOSEWIN:
                return heurCloseToWinning(id, board);
            case HEU_CAPTURED:
                return heurCaptured(id, board, oldBoard);
            case HEU_POSSIBLE_EXTRA_MOVES:
                return heurPossibleExtraMoves(id, board);
            case HEU_MEGA:
                return heurMega(id, extraMoves, board, oldBoard);
                
            default:
                return 0;
        }
    }

    // Minimize beta, Maximize Alpha
    int minimaxAlgorithm(Board board, Board oldBoard, bool isMax, int depthToGo, int alpha, int beta) {

        if (board.gameIsOver()) {
            if (board.getWinner() == id) { 
                return INT_MAX/2;
            } else if (board.getWinner() == 1 - id) {
                return INT_MIN/2;
            } else {
                return calculateCost(board, oldBoard);
            }
        }

        if (depthToGo == 0) {
            return calculateCost(board, oldBoard);
        }

        Board _board;
        _board.makeCopy(board);

        if (isMax) {
            int bestValue = INT_MIN;
            int succesor = -1;
            // ******************** Move Ordering Stuff ********************
            // *************************************************************
            vector<int> moveTracker, moveTrackerRest;
            for (int i=0; i < N_PITS; i++) {
                if (board.getPit(id, i) + i == N_PITS) {
                    moveTracker.push_back(i);
                }
                else {
                    moveTrackerRest.push_back(i);
                }
            }

            for (int i=0; i < moveTrackerRest.size(); i++) {
                moveTracker.push_back(moveTrackerRest.at(i));
            }
            // *************************************************************
            // *************************************************************

            for (int i=0; i < N_PITS; i++) {
                int currentMove = moveTracker.at(i);

                if (board.getPit(id, currentMove) > 0) {
                    int nextPlayer = board.makeMove(id, currentMove);
                    int currentVal;

                    if (nextPlayer == id) {
                        extraMoves++;
                        currentVal = minimaxAlgorithm(board, _board, true, depthToGo - 1, alpha, beta);
                        extraMoves--;
                    } else {
                        currentVal = minimaxAlgorithm(board, _board, false, depthToGo - 1, alpha, beta);
                    }

                    // cout << "Current value: " << currentVal << endl;
                    // cout << "Best value: " << bestValue << endl;
                       
                    if (currentVal > bestValue) { 
                        bestValue = currentVal;
                        succesor = currentMove;
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
            if (depthToGo == DEPTH) {
                return succesor;
            } else {
                return bestValue;
            }
        }
        else {
            // ******************** Move Ordering Stuff ********************
            // *************************************************************
            vector<int> moveTracker, moveTrackerRest;
            for (int i=0; i < N_PITS; i++) {
                if (board.getPit(id, i) + i == N_PITS) {
                    moveTracker.push_back(i);
                }
                else {
                    moveTrackerRest.push_back(i);
                }
            }

            for (int i=0; i < moveTrackerRest.size(); i++) {
                moveTracker.push_back(moveTrackerRest.at(i));
            }
            // *************************************************************
            // *************************************************************

            int bestValue = INT_MAX;
            for (int i = 0; i < N_PITS; i++) {
                int currentMove = moveTracker.at(i);

                if (board.getPit(1-id, currentMove) > 0) {
                    int nextPlayer = board.makeMove(1 - id, currentMove);
                    int currentVal;

                    if (nextPlayer == 1 - id) {
                        extraMoves--;
                        currentVal = minimaxAlgorithm(board, _board, false, depthToGo - 1, alpha, beta);
                        extraMoves++;
                    } else {
                        currentVal = minimaxAlgorithm(board, _board, true, depthToGo - 1, alpha, beta);
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
    Player(int _id, int _heuristic, int _mode, int _depth) {
        id = _id;
        mode = _mode;
        extraMoves = 0;
        heuristic = _heuristic;
        DEPTH = _depth;
    }

    int getId() { return id; }
    int getExtraMoves() { return extraMoves; }

    int makeMove(Board board) {
        switch(mode) {
            case MODE_MINIMAX: {
                extraMoves = 0;
                int move = minimaxAlgorithm(board, board, true, DEPTH, INT_MIN, INT_MAX);
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