#pragma warning (disable:4244)

#include "Linear_Solver.h"

#include <cmath> 
#include <iostream> 

using namespace std;

namespace LinearSolver {

	using namespace Eigen;

	bool go_Solve(
		VectorXd *Pos_x,
		SparseLU<SparseMatrix<double> > &solver,
		VectorXd *B_x,
		VectorXd *delta_x,
		Eigen::SparseMatrix<double> *Aff
		) {
		Eigen::SimplicialLDLT<SparseMatrix<double>> sparseSolver(*Aff);
		cout << endl << "Calling SOLVE ... ";
		*Pos_x = solver.solve(*B_x);
		*delta_x = *Aff**Pos_x - *B_x;
		if (solver.info() != Success) {
			// solving failed
			cout << endl << "Solving FAILED ! ";
			cout << "\nPrecision of solution: |Ax-b|= " << delta_x->norm();
			return false;
		}
		else
			cout << endl << "Solving Success ";
		cout << endl << "Precision of solution: |Ax-b|=" << delta_x->norm();
		return true;
	}




}