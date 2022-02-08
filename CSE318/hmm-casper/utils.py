PROB_EDGE = 0.9
PROB_DIAG = 1 - PROB_EDGE

PROB_CORRECT = 0.85


def sanitize_input():
    return input().lstrip().rstrip().split(" ")


def print_menu():
    msg =   "R r c b :   Read value from sensor \n" +\
            "T       :   Spend a time unit      \n" +\
            "C       :   Find Casper's location \n" +\
            "Q       :   Quit                   \n" +\
            "Choice > "
    print(msg, end="")


def print_grid(grid):
    N_ROW, N_COL = len(grid), len(grid[0])
    for row in range(N_ROW):
        for col in range(N_COL):
            if grid[row][col] == -1:
                print("X", end="\t")
            else:
                print(round(grid[row][col], 4), end="\t")
        print()
    print()

def get_edge_cells(grid, row, col):
    pair = []
    N_ROW, N_COL = len(grid), len(grid[0])

    if row - 1 > -1 and grid[row-1][col] != -1:
        pair.append((row - 1, col))

    if row + 1 < N_ROW and grid[row+1][col] != -1:
        pair.append((row + 1, col))

    if col - 1 > -1 and grid[row][col-1] != -1:
        pair.append((row, col - 1))

    if col + 1 < N_COL and grid[row][col+1] != -1:
        pair.append((row, col + 1))
    
    return pair


def get_diag_cells(grid, row, col):
    pair = []
    N_ROW, N_COL = len(grid), len(grid[0])

    if row - 1 > -1:
        if col - 1 > -1 and grid[row-1][col-1] != -1:
            pair.append((row - 1, col - 1))

        if col + 1 < N_COL and grid[row-1][col+1] != -1:
            pair.append((row - 1, col + 1))
    
    if row + 1 < N_ROW:
        if col - 1 > -1 and grid[row+1][col-1] != -1:
            pair.append((row + 1, col - 1))

        if col + 1 < N_COL and grid[row+1][col+1] != -1:
            pair.append((row + 1, col + 1))

    pair.append((row, col))
    return pair


#   Validation function for grid
#   To check if the sum of probabiliy over the grid
#   is equal to 1 or not.   (It must be 1)

def validate_grid(grid):
    total = 0
    N_ROW, N_COL = len(grid), len(grid[0])

    for row in range(N_ROW):
        for col in range(N_COL):
            if grid[row][col] != -1:
                total += grid[row][col]

    print("SUM(Probability):", total)
