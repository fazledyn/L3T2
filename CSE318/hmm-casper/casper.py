#!/usr/bin/env python3

#   Author: github.com/fazledyn
#   Date:   Feb 8, 2022

from utils import *

#   Casper's location is determined according
#   to the highest probability value in the grid.
#   However, corner cases such as same value over
#   multiple cells are not considered.

def find_casper(grid):
    N_ROW, N_COL = len(grid), len(grid[0])
    mr, mc = -1, -1
    max_prob = 0

    for row in range(N_ROW):
        for col in range(N_COL):
            if grid[row][col] > max_prob:
                mr, mc = row, col
                max_prob = grid[row][col]

    print("\nProbable location of Casper:", (mr, mc))
    print_grid(grid)

#   A `new_grid` is created with initial value of 0.
#   For each cell, the probability of ghost being in
#   neighboring cell is calculated and += appended.
#
#   As a result, the new probability of ghost being in
#   a cell is determined.

def spend_time(grid):
    N_ROW, N_COL = len(grid), len(grid[0])
    new_grid = [ [0] * N_COL for _ in range(N_ROW) ]

    for row in range(N_ROW):
        for col in range(N_COL):
            if grid[row][col] == -1:
                new_grid[row][col] = -1
            else:
                edge_cells = get_edge_cells(grid, row, col)
                diag_cells = get_diag_cells(grid, row, col)
                for each in edge_cells:
                    new_grid[each[0]][each[1]] += (PROB_EDGE/len(edge_cells)) * grid[row][col]
                for each in diag_cells:
                    new_grid[each[0]][each[1]] += (PROB_DIAG/len(diag_cells)) * grid[row][col]

    print("\nGrid after spending a time unit:")
    print_grid(new_grid)
    return new_grid    


#   `spend_time(grid)` is called and the new_grid is returned.
#   After that, based on the blink value, the new_grid values
#   are updated according to sensor correctness and thereby
#   normalized to align with the total probablitiy 1.

def read_sensor(grid, row, col, blink):
    N_ROW, N_COL = len(grid), len(grid[0])
    new_grid = spend_time(grid)
    
    if new_grid[row][col] == -1:
        print("ERROR: The grid is full of garbage")
        return

    adj_cells = get_edge_cells(new_grid, row, col) + get_diag_cells(new_grid, row, col)
    if blink == 1:
        # The probability of ghost being around this cell is high
        for row in range(N_ROW):
            for col in range(N_COL):
                if new_grid[row][col] != -1:
                    if (row, col) in adj_cells:
                        new_grid[row][col] = new_grid[row][col] * PROB_CORRECT
                    else:
                        new_grid[row][col] = new_grid[row][col] * (1 - PROB_CORRECT)

    elif blink == 0:
        # The probability of ghost being around this cell is low
        for row in range(N_ROW):
            for col in range(N_COL):
                if new_grid[row][col] != -1:
                    if (row, col) in adj_cells:
                        new_grid[row][col] = new_grid[row][col] * (1 - PROB_CORRECT)
                    else:
                        new_grid[row][col] = new_grid[row][col] * PROB_CORRECT

    else:
        print("Value of `b` is not permissible")
        return

    # Count the sum of total value for normalization
    total_prob = 0
    for row in range(N_ROW):
        for col in range(N_COL):
            if new_grid[row][col] != -1:
                total_prob += new_grid[row][col]
    
    # Normalize the grid
    for row  in range(N_ROW):
        for col in range(N_COL):
            if new_grid[row][col] != -1:
                new_grid[row][col] = new_grid[row][col] / total_prob

    print("Grid after reading sensor")
    print_grid(new_grid)
    return new_grid



def main():

    print("Enter N_ROW, N_COL, N_OBS (separated by space):")
    N_ROW, N_COL, N_OBS = [ int(x) for x in sanitize_input() ]
    grid = [ [0] * N_COL for i in range(N_ROW) ]

    print("Enter position of obstacles (r c):")
    for _ in range(N_OBS):
        r, c = [ int(x) for x in sanitize_input() ]
        grid[r][c] = -1

    #   Initialize the starting state of the grid
    #   Set probability
    total_empty = N_ROW * N_COL - N_OBS
    prob = 1 / total_empty

    for row in range(N_ROW):
        for col in range(N_COL):
            if grid[row][col] != -1:
                grid[row][col] = prob

    print("\nInitial state of the grid:")
    print_grid(grid)

    while True:
        print_menu()
        command = sanitize_input()

        #   Read value from sensor (Eg. R [u] [v] [b])
        if command[0] == "R":
            row, col, blink = [ int(x) for x in command[1:] ]
            grid = read_sensor(grid, row, col, blink)

        #   Spend a time unit
        elif command[0] == "T":
            grid = spend_time(grid)

        #   Find probable value of Casper
        elif command[0] == "C":
            find_casper(grid)

        #   Quit the game
        elif command[0] == "Q":
            print("Bye bye, Casper!")
            break


if __name__ == "__main__":
    main()