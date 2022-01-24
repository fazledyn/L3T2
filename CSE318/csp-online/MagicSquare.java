import org.chocosolver.solver.*;
import org.chocosolver.solver.constraints.IIntConstraintFactory;
import org.chocosolver.solver.variables.*;
import org.chocosolver.solver.variables.IVariableFactory;

import java.util.Arrays;


public class MagicSquare {
    
    private void modelAndSolve(int n) {
    
        Model model = new Model("Magic-Solver");
        int total = n * (n * n + 1)/2;
        
        IntVar[][]  matrix = new IntVar[n][n];
        IntVar[][]  invMatrix = new IntVar[n][n];
        IntVar[]    vars    = new IntVar[n*n];
        
        int k = 0;
        for (int i=0; i < n; i++) {
            for (int j=0; j < n; j++) {
                matrix[i][j] = model.intVar("s(" + i + "," + j + ")", 1, n*n);
                vars[k] = matrix[i][j];
                invMatrix[j][i] = matrix[i][j];
                k++;
            }
        }
        
        IntVar[]    dg1 = new IntVar[n];
        IntVar[]    dg2 = new IntVar[n];
        
        for (int i=0; i < n; i++) {
            dg1[i] = matrix[i][i];
            dg2[i] = matrix[ n - 1 - i ][i];
        }
        
        model.allDifferent(vars).post();
        
        int[] coeff = new int[n];
        Arrays.fill(coeff, 1);
        IntVar totalVar = model.intVar(total);
        
        model.arithm(totalVar, "=", total);
        for (int i=0; i < n; i++) {
            model.scalar(matrix[i], coeff, "=", total).post();
            model.scalar(invMatrix[i], coeff, "=", total).post();
        }
        
        model.scalar(dg1, coeff, "=", total).post();
        model.scalar(dg2, coeff, "=", total).post();
        
        
        // Something about symetry, IDK
        model.arithm(matrix[0][n-1], "<", matrix[n-1][0]).post();
        model.arithm(matrix[0][0], "<", matrix[n-1][n-1]).post();
        model.arithm(matrix[0][0], "<", matrix[n-1][0]).post();
    
        Solver solver = model.getSolver();
        solver.showStatistics();
        solver.showSolutions();
        
        System.out.println(solver.findSolution().toString());
        System.out.println(matrix[n-1][0].getValue());
    }
    
    public static void main(String[] args) {
        new MagicSquare().modelAndSolve(3);
    }
    
}
