#include "bits/stdc++.h"

#include "lib/Player.hh"
#include "lib/Board.hh"
#include "lib/color.hh"
#include "lib/constant.hh"

using namespace std;

int main() {

	Board board;
	Player player1(0, HEU_STORAGE, MODE_MINIMAX, 8);
	Player player2(1, HEU_STORAGE, MODE_MINIMAX, 5);

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

		if (!board.isValid()) {
			printRed("\n*******************\n");
			printRed("\nBoard Rule Violated\n");
			printRed("\n*******************\n");
		}

	}
	
	int winner = board.getWinner();
	if (winner == -1) {
		printBlue("\nGame Over. Draw");
	}
	else {
		printBlue("\nGame over");
		printBlue("\nWinner: Player #" + to_string(winner + 1));
	}

	return 0;
}
