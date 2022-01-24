package asif;//1705092 -> 092 % 3 = 2

import org.chocosolver.solver.Model;
import org.chocosolver.solver.Solver;
import org.chocosolver.solver.variables.IntVar;

public class KillerSudoku {
    public static void printBoard(IntVar[][] bd, int n){
        for ( int i = 0; i < n; i++) {
            for ( int j = 0; j < n; j++) {
                System.out.print(" ");
                System.out.print(bd [i][j].getValue());
            }
            System.out.println();
        }
    }

    public static void modelAndSolve(){
        Model model = new Model("Sudoku");
        int n=9;

        IntVar[][] bd = model.intVarMatrix("bd", n, n, 1, n);

        IntVar[] r0 = model.intVarArray("r0", n, 1, n);
        IntVar[] r1 = model.intVarArray("r1", n, 1, n);
        IntVar[] r2 = model.intVarArray("r2", n, 1, n);
        IntVar[] r3 = model.intVarArray("r3", n, 1, n);
        IntVar[] r4 = model.intVarArray("r4", n, 1, n);
        IntVar[] r5 = model.intVarArray("r5", n, 1, n);
        IntVar[] r6 = model.intVarArray("r6", n, 1, n);
        IntVar[] r7 = model.intVarArray("r7", n, 1, n);
        IntVar[] r8 = model.intVarArray("r8", n, 1, n);

        IntVar[] c0 = model.intVarArray("c0", n, 1, n);
        IntVar[] c1 = model.intVarArray("c1", n, 1, n);
        IntVar[] c2 = model.intVarArray("c2", n, 1, n);
        IntVar[] c3 = model.intVarArray("c3", n, 1, n);
        IntVar[] c4 = model.intVarArray("c4", n, 1, n);
        IntVar[] c5 = model.intVarArray("c5", n, 1, n);
        IntVar[] c6 = model.intVarArray("c6", n, 1, n);
        IntVar[] c7 = model.intVarArray("c7", n, 1, n);
        IntVar[] c8 = model.intVarArray("c8", n, 1, n);

        IntVar[] b0 = model.intVarArray("b0", n, 1, n);
        IntVar[] b1 = model.intVarArray("b1", n, 1, n);
        IntVar[] b2 = model.intVarArray("b2", n, 1, n);
        IntVar[] b3 = model.intVarArray("b3", n, 1, n);
        IntVar[] b4 = model.intVarArray("b4", n, 1, n);
        IntVar[] b5 = model.intVarArray("b5", n, 1, n);
        IntVar[] b6 = model.intVarArray("b6", n, 1, n);
        IntVar[] b7 = model.intVarArray("b7", n, 1, n);
        IntVar[] b8 = model.intVarArray("b8", n, 1, n);

        model.allDifferent(r0).post();
        model.allDifferent(r1).post();
        model.allDifferent(r2).post();
        model.allDifferent(r3).post();
        model.allDifferent(r4).post();
        model.allDifferent(r5).post();
        model.allDifferent(r6).post();
        model.allDifferent(r7).post();
        model.allDifferent(r8).post();

        model.allDifferent(c0).post();
        model.allDifferent(c1).post();
        model.allDifferent(c2).post();
        model.allDifferent(c3).post();
        model.allDifferent(c4).post();
        model.allDifferent(c5).post();
        model.allDifferent(c6).post();
        model.allDifferent(c7).post();
        model.allDifferent(c8).post();

        model.allDifferent(b0).post();
        model.allDifferent(b1).post();
        model.allDifferent(b2).post();
        model.allDifferent(b3).post();
        model.allDifferent(b4).post();
        model.allDifferent(b5).post();
        model.allDifferent(b6).post();
        model.allDifferent(b7).post();
        model.allDifferent(b8).post();

        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                model.arithm(bd[i][j], "=", b0[i*3+j]).post();
                model.arithm(bd[i][j+3], "=", b1[i*3+j]).post();
                model.arithm(bd[i][j+6], "=", b2[i*3+j]).post();
                model.arithm(bd[i+3][j], "=", b3[i*3+j]).post();
                model.arithm(bd[i+3][j+3], "=", b4[i*3+j]).post();
                model.arithm(bd[i+3][j+6], "=", b5[i*3+j]).post();
                model.arithm(bd[i+6][j], "=", b6[i*3+j]).post();
                model.arithm(bd[i+6][j+3], "=", b7[i*3+j]).post();
                model.arithm(bd[i+6][j+6], "=", b8[i*3+j]).post();
            }
        }

        for(int j = 0; j < n; j++){
            model.arithm(bd[0][j], "=", r0[j]).post();
            model.arithm(bd[1][j], "=", r1[j]).post();
            model.arithm(bd[2][j], "=", r2[j]).post();
            model.arithm(bd[3][j], "=", r3[j]).post();
            model.arithm(bd[4][j], "=", r4[j]).post();
            model.arithm(bd[5][j], "=", r5[j]).post();
            model.arithm(bd[6][j], "=", r6[j]).post();
            model.arithm(bd[7][j], "=", r7[j]).post();
            model.arithm(bd[8][j], "=", r8[j]).post();
        }

        for(int i = 0; i < n; i++){
            model.arithm(bd[i][0], "=", c0[i]).post();
            model.arithm(bd[i][1], "=", c1[i]).post();
            model.arithm(bd[i][2], "=", c2[i]).post();
            model.arithm(bd[i][3], "=", c3[i]).post();
            model.arithm(bd[i][4], "=", c4[i]).post();
            model.arithm(bd[i][5], "=", c5[i]).post();
            model.arithm(bd[i][6], "=", c6[i]).post();
            model.arithm(bd[i][7], "=", c7[i]).post();
            model.arithm(bd[i][8], "=", c8[i]).post();
        }

        model.sum(new IntVar[]{bd[0][0],bd[0][1],bd[0][2]}, "=", 11).post();
        model.sum(new IntVar[]{bd[0][3],bd[1][3],bd[2][3],bd[2][2]}, "=", 22).post();
        model.sum(new IntVar[]{bd[0][4],bd[1][4]}, "=", 15).post();
        model.sum(new IntVar[]{bd[0][5],bd[0][6]}, "=", 9).post();
        model.sum(new IntVar[]{bd[0][7],bd[0][8]}, "=", 13).post();
        model.sum(new IntVar[]{bd[1][0],bd[2][0],bd[3][0],bd[4][0]}, "=", 21).post();
        model.sum(new IntVar[]{bd[1][1],bd[1][2],bd[2][1],bd[3][1]}, "=", 18).post();
        model.sum(new IntVar[]{bd[1][5],bd[2][5],bd[2][4]}, "=", 10).post();
        model.sum(new IntVar[]{bd[1][6],bd[2][6],bd[3][6]}, "=", 18).post();
        model.sum(new IntVar[]{bd[1][7],bd[1][8]}, "=", 5).post();
        model.sum(new IntVar[]{bd[2][7],bd[2][8]}, "=", 10).post();
        model.sum(new IntVar[]{bd[3][2],bd[3][3]}, "=", 14).post();
        model.sum(new IntVar[]{bd[3][4],bd[3][5],bd[4][3],bd[4][4],bd[4][5],bd[5][3],bd[5][4]}, "=", 28).post();
        model.sum(new IntVar[]{bd[3][7],bd[3][8],bd[4][6],bd[4][7]}, "=", 17).post();
        model.sum(new IntVar[]{bd[4][1],bd[4][2],bd[5][0],bd[5][1]}, "=", 22).post();
        model.sum(new IntVar[]{bd[4][8],bd[5][8],bd[6][8],bd[7][8]}, "=", 23).post();
        model.sum(new IntVar[]{bd[5][2],bd[6][2],bd[7][2]}, "=", 11).post();
        model.sum(new IntVar[]{bd[5][5],bd[5][6]}, "=", 9).post();
        model.sum(new IntVar[]{bd[5][7],bd[6][7],bd[7][7],bd[7][6]}, "=", 28).post();
        model.sum(new IntVar[]{bd[6][0],bd[6][1]}, "=", 13).post();
        model.sum(new IntVar[]{bd[6][3],bd[6][4],bd[7][3]}, "=", 8).post();
        model.sum(new IntVar[]{bd[6][5],bd[6][6],bd[7][5],bd[8][5]}, "=", 22).post();
        model.sum(new IntVar[]{bd[7][0],bd[7][1]}, "=", 17).post();
        model.sum(new IntVar[]{bd[7][4],bd[8][4]}, "=", 10).post();
        model.sum(new IntVar[]{bd[8][0],bd[8][1]}, "=", 9).post();
        model.sum(new IntVar[]{bd[8][2],bd[8][3]}, "=", 9).post();
        model.sum(new IntVar[]{bd[8][6],bd[8][7],bd[8][8]}, "=", 13).post();

        Solver solver = model.getSolver();

        solver.showStatistics();
        solver.showSolutions();
        solver.findSolution();

        printBoard(bd, n);
    }

    public static void main(String[] args) {
        modelAndSolve();
    }
}