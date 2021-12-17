#include "bits/stdc++.h"

#include "lib/Player.hh"
#include "lib/Board.hh"
#include "lib/color.hh"
#include "lib/constant.hh"

using namespace std;
using namespace std::chrono;

#define N_EXPERIMENT 100

int main(int argc, char const *argv[]) {

    high_resolution_clock::time_point startTime, endTime;
    int winnerCount[2] = { 0, 0 };

    // Clock Start #########################
    startTime = high_resolution_clock::now();

    for (int i = 0; i < N_EXPERIMENT; i++) {

        Board board;
        Player player1(0, HEU_SPMC_CAPTURED, MODE_MINIMAX);
        Player player2(1, HEU_SPMC_CAPTURED, MODE_MINIMAX);

        int turn = 0, prevTurn = -1;
        int move;
        
        while (!board.gameIsOver()) {
            
            if (turn == INT_MIN) {
                turn = prevTurn;
            }
            prevTurn = turn;
            
            if (turn == 0) {
                move = player1.makeMove(board);
            }
            else if (turn == 1) {
                move = player2.makeMove(board);
            }
            else {
                // cout << "Error, no such player: " << turn << endl;
            }
            turn = board.makeMove(turn, move);
        }
        int winner = board.getWinner();
        //board.printBoard();

        if (winner == -1) {
            // printBlue("\nGame Over. Draw");
        }
        else {
            winnerCount[winner]++;
            // printBlue("\nGame over");
            // printBlue("\nWinner: Player #" + to_string(winner + 1));
        }
    }

    // Clock End ############################
    endTime = high_resolution_clock::now();
    auto timeTaken = duration_cast<milliseconds>(endTime - startTime).count();

    cout << "Heuristic: " << argv[1] << endl;
    cout << "--------------------------" << endl;

    cout << "N_EXPERIMENT: " << N_EXPERIMENT << endl;
    cout << "MINIMAX_DEPTH: " << MINIMAX_DEPTH << endl;
    cout << "Total Time: " << timeTaken << "ms" << endl;
    
    cout << endl;
    cout << "Winner Count: " << endl;
    cout << "[0]: " << winnerCount[0] << endl;
    cout << "[1]: " << winnerCount[1] << endl;

    cout << endl;
    cout << "Heuristic Weights: " << endl;
    cout << "W_STORAGE: " << W_STORAGE << endl;
    cout << "W_PITS: " << W_PITS << endl;
    cout << "W_MOVES: " << W_MOVES << endl;
    cout << "W_CAPTURED: " << W_CAPTURED << endl;
    cout << "W_CLOSE_TO_WIN: " << W_CLOSE_TO_WIN << endl;

	return 0;
}
