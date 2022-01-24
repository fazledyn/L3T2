package asif;//1705010 -> 010 % 3 = 1

import org.chocosolver.solver.Model;
import org.chocosolver.solver.Solver;
import org.chocosolver.solver.variables.IntVar;


public class Futoshiki {
    
    public static void main(String[] args) {

        int i, j, k;

        // 1. Create a Model
        Model model = new Model("futoshiki");

        // 2. Create variables
        IntVar[][] bd = model.intVarMatrix("bd", 9, 9, 1, 9);


        /* the nine rows */
        /* each row is an array of 9 integer variables taking their value in [1, 9] */
        IntVar[][] r = model.intVarMatrix("r",9, 9, 1, 9);

        /* the nine columns */
        /* each column is an array of 9 integer variables taking their value in [1, 9] */
        IntVar[][] c = model.intVarMatrix("c",9, 9, 1, 9);


        // 3. Post constraints
        /* post constraints for the given hints or clues */

        model.arithm (bd[0][5], "=", 9).post();
        model.arithm (bd[0][8], "=", 2).post();

        model.arithm (bd[1][1], "=", 8).post();
        model.arithm (bd[1][7], "=", 4).post();

        model.arithm (bd[2][6], "=", 2).post();
        model.arithm (bd[2][7], "=", 3).post();

        model.arithm (bd[3][5], "=", 2).post();

        model.arithm (bd[4][0], "=", 3).post();
        model.arithm (bd[4][2], "=", 5).post();
        model.arithm (bd[4][5], "=", 6).post();

        model.arithm (bd[5][1], "=", 7).post();
        model.arithm (bd[5][6], "=", 6).post();

        model.arithm (bd[7][3], "=", 7).post();

        /* post constraints for the given hints or clues */

        model.arithm (bd[0][4], ">", bd[0][3]).post();
        model.arithm (bd[0][7], ">", bd[0][6]).post();

        model.arithm (bd[0][2], ">", bd[1][2]).post();
        model.arithm (bd[0][6], ">", bd[1][6]).post();
        model.arithm (bd[0][8], ">", bd[1][8]).post();

        model.arithm (bd[1][4], ">", bd[1][3]).post();
        model.arithm (bd[1][6], ">", bd[1][5]).post();

        model.arithm (bd[2][2], ">", bd[2][3]).post();

        model.arithm (bd[2][0], ">", bd[3][0]).post();
        model.arithm (bd[3][1], ">", bd[2][1]).post();

        model.arithm (bd[3][5], ">", bd[3][4]).post();

        model.arithm (bd[4][4], ">", bd[4][5]).post();

        model.arithm (bd[5][3], ">", bd[4][3]).post();
        model.arithm (bd[4][6], ">", bd[5][6]).post();
        model.arithm (bd[4][8], ">", bd[5][8]).post();

        model.arithm (bd[5][8], ">", bd[5][7]).post();

        model.arithm (bd[6][0], ">", bd[5][0]).post();
        model.arithm (bd[6][3], ">", bd[5][3]).post();
        model.arithm (bd[6][4], ">", bd[5][4]).post();
        model.arithm (bd[5][8], ">", bd[6][8]).post();

        model.arithm (bd[6][0], ">", bd[6][1]).post();

        model.arithm (bd[7][6], ">", bd[7][5]).post();

        model.arithm (bd[7][1], ">", bd[8][1]).post();
        model.arithm (bd[7][7], ">", bd[8][7]).post();
        model.arithm (bd[7][8], ">", bd[8][8]).post();

        model.arithm (bd[8][2], ">", bd[8][3]).post();
        model.arithm (bd[8][6], ">", bd[8][5]).post();

        /* for the nine row variables */
        /* each row variable is associated with appropriate cell positions in board */

        for ( int r_id = 0; r_id < 9; r_id++)
        {
            for ( j = 0; j < 9; j++)
                model.arithm (bd[r_id][j], "=", r[r_id][j]).post();
        }


        /* for the nine column variables */
        /* each column variable is associated with appropriate cell positions in board */
        for ( int c_id = 0; c_id < 9; c_id++)
        {
            for ( i = 0; i < 9; i++)
                model.arithm (bd[i][c_id], "=", c[c_id][i]).post();
        }

        /* post global constraint alldiff for the nine rows */

        for ( int r_id = 0; r_id < 9; r_id++)
        {
            model.allDifferent(r[r_id]).post();
        }

        /* post global constraint alldiff for the nine columns */

        for ( int c_id = 0; c_id < 9; c_id++)
        {
            model.allDifferent(c[c_id]).post();
        }

        // 4. Solve the problem
        Solver solver = model.getSolver();

        solver.showStatistics();
        solver.showSolutions();
        solver.findSolution();

        // 5. Print the solution
        for ( i = 0; i < 9; i++)
        {
            for ( j = 0; j < 9; j++)
            {

                System.out.print(" ");
                /* get the value for the board position [i][j] for the solved board */
                k = bd [i][j].getValue();
                System.out.print(k );
            }
            System.out.println();
        }


    }

}
