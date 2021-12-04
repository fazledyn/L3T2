#include <bits/stdc++.h>
using namespace std;

#ifndef NODE_H
#define NODE_H

class Node {

    int **board, **goalBoard;
    int gridSize;
    int move;
    int puzzleSize;

    Node *parent;
    int **boardPosition, **goalPosition;

    void updatePosition() {
        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                boardPosition[ board[i][j] ][0] = i;
                boardPosition[ board[i][j] ][1] = j;
            
                goalPosition[ goalBoard[i][j] ][0] = i;
                goalPosition[ goalBoard[i][j] ][1] = j;
            }
        }
    }

    void invokeExit() {
        cout << "\n\033[1;31m" << "[ INVOKE EXIT ]" << "\033[0m\n";
        exit(1);
    }

public:
    // friend bool operator <(Node&, Node&);
    // friend bool operator >(Node&, Node&);

    Node(int **_board, int **_goalBoard, int _gridSize, Node *_parent, int _move) {
        this->board = _board;
        this->goalBoard = _goalBoard;
        this->gridSize = _gridSize;
        this->puzzleSize = _gridSize*_gridSize;
        this->parent = _parent;
        this->move = _move;

        boardPosition = new int*[this->puzzleSize];
        goalPosition  = new int*[this->puzzleSize];

        for (int i=0; i < this->puzzleSize; i++) {
            boardPosition[i] = new int[2];
            goalPosition[i] = new int[2];
        }
        updatePosition();
    }

    int getMove() {
        return this->move;
    }

    int** getBoard() {
        return this->board;
    }

    Node* getParent() {
        return this->parent;
    }

    int** makeCopy() {
        int **newBoard;
        newBoard = new int*[gridSize];
        for (int i=0; i < gridSize; i++) {
            newBoard[i] = new int[gridSize];
        }

        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                newBoard[i][j] = board[i][j];
            }
        }
        return newBoard;
    }

    vector<Node*> makeChildren() {
        int row = boardPosition[0][0];
        int col = boardPosition[0][1];

        vector<Node*> childList;
        int **childBoard;
        
        if (row != 0) {
            childBoard = makeCopy();
            int temp = childBoard[row - 1][col];
            childBoard[row - 1][col] = 0;
            childBoard[row][col] = temp;

            if ((parent == nullptr) || !(parent->isSameBoard(childBoard))) {
                Node *child;
                child = new Node(childBoard, goalBoard, gridSize, this, move + 1);
                childList.push_back(child);
            }
        }
        
        if (row != gridSize - 1) {
            childBoard = makeCopy();
            int temp = childBoard[row + 1][col];
            childBoard[row + 1][col] = 0;
            childBoard[row][col] = temp;

            if ((parent == nullptr) || !(parent->isSameBoard(childBoard))) {
                Node *child;
                child = new Node(childBoard, goalBoard, gridSize, this, move + 1);
                childList.push_back(child);
            }
        }
        
        if (col != 0) {
            childBoard = makeCopy();
            int temp = childBoard[row][col - 1];
            childBoard[row][col - 1] = 0;
            childBoard[row][col] = temp;

            if ((parent == nullptr) || !(parent->isSameBoard(childBoard))) {
                Node *child;
                child = new Node(childBoard, goalBoard, gridSize, this, move + 1);
                childList.push_back(child);
            }
        }

        if (col != gridSize - 1) {
            childBoard = makeCopy();
            int temp = childBoard[row][col + 1];
            childBoard[row][col + 1] = 0;
            childBoard[row][col] = temp;
    
            if ((parent == nullptr) || !(parent->isSameBoard(childBoard))) {
                Node *child;
                child = new Node(childBoard, goalBoard, gridSize, this, move + 1);
                childList.push_back(child);
            }
        }
        return childList;
    }

    int getHammingCost() {
        int count = 0;
        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                if ((board[i][j] != goalBoard[i][j]) && (board[i][j] != 0)) {
                    count++;
                }
            }
        }
        return count;
    }

    int getManhattanCost() {
        // updatePosition();
        int count = 0;
        
        for (int i=1; i < puzzleSize; i++) {
            count += abs(boardPosition[i][0] - goalPosition[i][0]) + abs(boardPosition[i][1] - goalPosition[i][1]);
        }
        return count;
    }

    int getLinearConflict() {
        // updatePosition();
        int count = 0;

        for (int i=0; i < gridSize; i++) {              //  For Each Row
            for (int j=0; j < gridSize; j++) {          //  For Each Column Pair
                for (int k=j+1; k < gridSize; k++) {    //  For Each Column Pair
                    if (boardPosition[ board[i][j] ][0] == goalPosition[ board[i][j] ][0]) {    // If left  where should be
                        if (boardPosition[ board[i][k] ][0] == goalPosition[ board[i][k] ][0]) {// If right where should be
                            if (board[i][j] > board[i][k]) {                            // If left > right
                                if (board[i][k] != 0){                                  // If right != 0
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        }
        return count;
    }

    bool isSameBoard(int **another) {
        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                if (board[i][j] != another[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool isSolveable() {

        int nInversion = 0;
        int index = 0, linearBoard[puzzleSize];
        
        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                linearBoard[index++] = board[i][j];
            }
        }

        for (int i=0; i < puzzleSize; i++) {
            for (int j=i+1; j < puzzleSize; j++) {
                if ((linearBoard[i] > linearBoard[j]) && (linearBoard[j] != 0)) {
                    nInversion++;
                }
            }
        }

        if (gridSize % 2 == 0) { // Grid Size is Even
            int rowBlank = -1;

            for (int i=0; i < gridSize; i++) {
                for (int j=0; j < gridSize; j++) {
                    if (board[i][j] == 0) {
                        rowBlank = i+1;
                        break;
                    }
                }
            }

            if (((rowBlank%2 == 0) && (nInversion%2 == 0)) || ((rowBlank%2 == 1) && (nInversion%2 == 1))) {
                return true;
            }

            cout << "Blank in row # " << rowBlank << endl;
            cout << "Total # of inversions: " << nInversion << endl;
            return false;
        }
        else { // Grid Size is Odd
            if (nInversion % 2 == 0) {
                return true;
            } 
            cout << "Total # of inversions: " << nInversion << endl;    
            return false;
        }
    }

    void print() {
        cout << "\n\t    |\n";
        cout << "\t    V\n\n";
        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                if (board[i][j] == 0)   cout << "*\t";
                else                    cout << board[i][j] << "\t";
            }
            cout << "\n";
        }
    }

    void printGoalBoard() {
        cout << "\nGoal Board\n";
        for (int i=0; i < gridSize; i++) {
            for (int j=0; j < gridSize; j++) {
                if (goalBoard[i][j] == 0)   cout << "*\t";
                else                        cout << goalBoard[i][j] << "\t";
            }
            cout << "\n";
        }
    }

    void printPosition() {
        cout << "\nBoard Position\n";
        for (int i=0; i < puzzleSize; i++) {
            cout << i << " : " << boardPosition[i][0] << ", " << boardPosition[i][1] << endl;
        }
    }

    void printHeuristics() {
        cout << "\n\nTotal Heuristic Cost: " << getManhattanCost() + 2*getLinearConflict();
    }

};

/*
    ####################################################################
    ########### Operator Overloading f/ Node Class  ####################
    ####################################################################

    This is to note that,

    Priority queues are a type of container adaptors, specifically
    designed such that its first element is always the greatest of the
    elements it contains, according to some strict weak ordering criterion.

    Which is why, we are overloading the operator function is such a way that
    the priority queue will always return the node with the lowest heuristic.

*/

struct Comparator {
    bool operator()(Node *a, Node *b) const {
        return a->getMove() + a->getHammingCost() > b->getMove() + b->getHammingCost();
    }
};

#endif// NODE_H