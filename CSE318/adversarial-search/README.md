# Adversarial Search [CSE 318 July 2021]

## Problem
In this assignment, students are asked to implement game agents for `Mandala` game using `Minimax Algorithm with Alpha Beta Pruning`

Students were also asked to run the program (agent vs. agent) using different heuristic combination and share the findings.

## Report
A game agent has been implemented to solve the problem. The agent has several heuristic functions and modes as below:

### Heuristic Function
- `heuStorage`: Difference of number of stones/beads present in the storage of players.

- `heuStoragePits`: Difference of number of stones/beads present in the storage and pits of players.

- `heuStoragePitsMove`: Difference of number of stones/beads present in the storage and pits of players along with available extra move.

### Game Mode
The implemented program contains three basic game modes, such as-
- Minimax Algorithm
- Human Input
