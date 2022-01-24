import org.chocosolver.solver.*;
import org.chocosolver.solver.variables.IntVar;

public class NQueen {

    private void modelAndSolve() {
    
        int N = 8;
        Model model = new Model(N + "-Queen Problem");
    
        IntVar[] vars = new IntVar[N];
        for (int i=0; i < N; i++) {
            vars[i] = model.intVar("Q_" + i, 1, N);
        }
        
        for (int i=0; i < N-1; i++) {
            for (int j=i+1; j < N; j++) {
                model.arithm(vars[i], "!=", vars[j]).post();
                System.out.println(vars[i] + " != " + vars[j]);
                
                model.arithm(vars[i], "!=", vars[j], "-", (j - i)).post();
                System.out.println(vars[i] + " != " + vars[j] + " - " + (j-i));
                
                model.arithm(vars[i], "!=", vars[j], "+", (j - i)).post();
                System.out.println(vars[i] + " != " + vars[j] + " + " + (j-i));
            }
        }
        
        Solver solver = model.getSolver();
        solver.showStatistics();
        System.out.println(solver.findSolution());
    }
    
    public static void main(String[] args) {
        new NQueen().modelAndSolve();
    }
    
}
