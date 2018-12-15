#pragma once

#include "Eigen/Sparse"
#include "Eigen/LU"

namespace LinearSolver {

	using namespace Eigen;

	bool go_Solve(
		VectorXd *Pos_x,
		SparseLU<SparseMatrix<double> > &solver,
		VectorXd *B_x,
		VectorXd *delta_x,
		Eigen::SparseMatrix<double> *Aff
		);
}