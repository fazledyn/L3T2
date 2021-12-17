#include "bits/stdc++.h"

#include "lib/Player.hh"
#include "lib/Board.hh"
#include "lib/color.hh"
#include "lib/constant.hh"

using namespace std;

int main() {

	Board board;
	Player player1(0, HEU_STORAGE_PITS_MOVES, MODE_MINIMAX);
	Player player2(1, HEU_STORAGE_PITS_MOVES, MODE_MINIMAX);

	int turn = 0, prevTurn = -1;
	int move;
	
	board.printBoard();
	while (!board.gameIsOver()) {
		
		if (turn == INT_MIN) {
			printRed("Illegal move. Try again");
			turn = prevTurn;
		}
		prevTurn = turn;
		printGreen("\nPlayer " + to_string(turn + 1) + "'s turn\n");

		if (turn == 0) {
			move = player1.makeMove(board);
		}
		else if (turn == 1) {
			move = player2.makeMove(board);
		}
		else {
			cout << "Error, no such player: " << turn << endl;
		}
		turn = board.makeMove(turn, move);
		board.printBoard();
	}

	printBlue("\nGame over");
	printBlue("\nWinner: Player #" + to_string(board.getWinner() + 1));
	
	return 0;
}
