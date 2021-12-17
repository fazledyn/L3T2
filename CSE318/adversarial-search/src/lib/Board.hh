#ifndef BOARD_H
#define BOARD_H

#include "bits/stdc++.h"
#include "constant.hh"
#include "color.hh"

using namespace std;


class Board {

    int* pit[2];
    int storage[2];

public:
    Board() {
        for (int i=0; i < 2; i++) {
            pit[i] = new int[N_PITS];
            for (int j=0; j < N_PITS; j++) {
                pit[i][j] = N_BEADS;
            }
            storage[i] = 0;
        }
    }

    ~Board() {
    }

    void makeCopy(Board board) {
        for (int i=0; i < 2; i++) {
            for (int j=0; j < N_PITS; j++) {
                pit[i][j] = board.pit[i][j];
            }
            storage[i] = board.storage[i];
        }
    }

    Board(const Board& board) {
        for (int i=0; i < 2; i++) {
            pit[i] = new int[N_PITS];
            for (int j=0; j < N_PITS; j++) {
                pit[i][j] = board.pit[i][j];
            }
            storage[i] = board.storage[i];
        }
    }

    int** getPit() {
        return pit;
    }

    int* getStorage() {
        return storage;
    }

    /*
        Board makes a move as a player and
        return the next player's turn 
    */
    int makeMove(int playerId, int pitPos) {

        int availableBeads = pit[playerId][pitPos];
        pit[playerId][pitPos] = 0;

        if ((pitPos > N_PITS) || (pitPos < 0) || (availableBeads == 0)) {
            return INT_MIN;
        }

        int opponentId = (1 - playerId);
        int nextPlayer = opponentId;

        int totalPit = N_PITS*2 + 1;
        int beadsPerPit = floor(availableBeads/totalPit);
        int beadsLeft = availableBeads % totalPit;

        for (int i=0; i < N_PITS; i++) {
            pit[playerId][i] += beadsPerPit;
            pit[opponentId][i] += beadsPerPit;
        }
        storage[playerId] += beadsPerPit;

        /*
            If number of beads are '13', then the last bead exactly
            falls into the same pit (empty) of the player. And, we get 
            the opposite pit beads along with our last bead.
        */
        if (availableBeads == 13) {
            storage[playerId] += pit[opponentId][N_PITS - pitPos + 1];
            storage[playerId] += 1;
            pit[opponentId][N_PITS - pitPos + 1] = 0;
            pit[playerId][pitPos] = 0;
        }

        /*
            Filling up own bins in usual manner
            (This doesnt get affected by the previous stuffs)
        */
        for (int i = pitPos + 1; i < N_PITS; i++) {
            if (beadsLeft > 0) {
                pit[playerId][i] += 1;
                beadsLeft--;

                if ((beadsLeft == 0) && (pit[playerId][i] == 1)) {
                    if (pit[opponentId][N_PITS - i + 1] > 0) {
                        storage[playerId] += pit[opponentId][N_PITS - i - 1];
                        storage[playerId] += 1;
                        pit[opponentId][N_PITS - i - 1] = 0;
                        pit[playerId][i] = 0;
                    }
                }
            }
        }

        if (beadsLeft > 0) {
            storage[playerId] += 1;
            beadsLeft--;
            if (beadsLeft == 0) {
                nextPlayer = playerId;
            }
        }

        for (int i=0; i < N_PITS; i++) {
            if (beadsLeft > 0) {
                pit[opponentId][i] += 1;
                beadsLeft--;
            }
        }

        for (int i=0; i < pitPos + 1; i++) {
            if (beadsLeft > 0) {
                pit[playerId][i] += 1;
                beadsLeft--;
            }
        }
        return nextPlayer;
    }

    bool gameIsOver() {
        
        // Case 1: If no player has any beads left
        if (storage[0] + storage[1] == N_PITS * N_BEADS * 2) {
            return true;
        }

        // Case 2: If player #0 has no beads left
        int beadsInPit = 0;      
        for (int i=0; i < N_PITS; i++) {
            beadsInPit += pit[0][i];
        }
        if (beadsInPit == 0) {
            return true;
        }

        // Case 3: If player #1 has no beads left
        beadsInPit = 0;
        for (int i=0; i < N_PITS; i++) {
            beadsInPit += pit[1][i];
        }
        if (beadsInPit == 0) {
            return true;
        }

        return false;
    }

    int getWinner() {
        if (!gameIsOver()) {
            printRed("\nGame is not over yet!\n");
            return INT_MIN;
        }

        int player0Beads = storage[0];
        int player1Beads = storage[1];

        for (int i=0; i < N_PITS; i++) {
            player0Beads += pit[0][i];
            player1Beads += pit[1][i];
        }

        if (player0Beads > player1Beads) {
            return 0;
        } else if (player1Beads > player0Beads) {
            return 1;
        } else {
            return -1;
        }
    }

    void printBoard() {
        cout << endl;
        cout << "    [Player #2]      " << endl << endl;
        
        cout << "   ";
        for (int i=0; i < N_PITS; i++) {
            cout << "<" << N_PITS - i - 1 << ">";
        }
        cout << endl;

        cout << "   ";
        for (int i=0; i < N_PITS; i++) {
            cout << " " << pit[1][N_PITS - i - 1] << " ";
        }
        cout << "\n";
        cout << storage[1];
        cout << "   ";
        for (int i=0; i < N_PITS; i++) {
            cout << "  ";
        }
        cout << "       ";
        cout << storage[0];
        cout << "\n";
        cout << "   ";
        for (int i=0; i < N_PITS; i++) {
            cout << " " << pit[0][i] << " ";
        }

        cout << endl << "   ";
        for (int i=0; i < N_PITS; i++) {
            cout << "<" << i << ">";
        }
        cout << endl << endl;
        cout << "    [Player #1]     " << endl;
    }

};

#endif