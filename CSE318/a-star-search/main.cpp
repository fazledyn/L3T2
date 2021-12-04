#include <bits/stdc++.h>
#include "Node.h"
using namespace std;

int** createBoard(int N) {
    int **board;
    board = new int*[N];
    
    for (int i=0; i < N; i++) {
        board[i] = new int[N];
    }

    for (int i=0; i < N; i++) {
        for (int j=0; j < N; j++) {
            cin >> board[i][j];
        }
    }
    return board;
}

int** getGoalBoard(int N) {
    int **board, count = 0;
    
    board = new int*[N];
    for (int i=0; i < N; i++) {
        board[i] = new int[N];
    }

    for (int i=0; i < N; i++) {
        for (int j=0; j < N; j++) {
            board[i][j] = i*N + j + 1;
        }
    }
    board[N-1][N-1] = 0;
    return board;
}

void printSolution(Node* self) {
    if (self == nullptr) {
        cout << "nullptr\n";
        return;
    }
    printSolution(self->getParent());
    self->print();
}

bool inClosedList(vector<Node*> list, Node* item) {
    for (int i=0; i < list.size(); i++) {
        if (list[i]->isSameBoard(item->getBoard())) {
            return true;
        }
    }
    return false;
}

int main () {

    freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
    
    int gridSize;
    cin >> gridSize;

    int **board = createBoard(gridSize);
    int **goalBoard = getGoalBoard(gridSize);
    int move = 0;
    
    priority_queue<Node*, vector<Node*>, Comparator> openList;
    vector<Node*> closedList;

    Node root(board, goalBoard, gridSize, nullptr, 0);

    Node *finalBoard;
    openList.push(&root);

    if (!root.isSolveable()) {
        cout << "Puzzle not solveable" << endl;
        return 0;
    }

    while (!openList.empty()) {

        Node* current = openList.top();
        openList.pop();

        if (!inClosedList(closedList, current)) {
        
            if (current->isSameBoard(goalBoard)) {
                finalBoard = current;
                cout << "\nSolution has been found!\n";
                break;
            }
            
            closedList.push_back(current);
            vector<Node*> children = current->makeChildren();
            
            for (int i=0; i < children.size(); i++) {
                bool nodeNotIn = true;
                for (int j=0; j < closedList.size(); j++) {
                    if (closedList[j]->isSameBoard(children[i]->getBoard())) {
                        nodeNotIn = false;
                        break;
                    }
                }
                if (nodeNotIn) {
                    openList.push(children[i]);
                }
            }

        }   
    }

    printSolution(finalBoard);

    cout << "Optimal Cost: " << finalBoard->getMove() << endl;
    cout << "Expanded #: " << closedList.size() << endl;
    cout << "Explored #: " << openList.size() + closedList.size() << endl;

    return 0;
}
