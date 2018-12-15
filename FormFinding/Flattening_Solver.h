#pragma once


class Model_Reduced;
class Node;
class Beam;
class Plate;
#include <Eigen/Sparse>
#include <Eigen/LU>
#include <string>
#include <map>
#include <vector>


class Flattening_Solver
{
public:
	Flattening_Solver();
	~Flattening_Solver();


	void setModel(Model_Reduced *m);

	// steps to be called to solve, in this order
	void checkCleanData();
	void initMatrixes(bool show_elem_count = false);
	boolean solveSGM(bool skip_z_axis = false);
	void updatePositionToModel(bool skip_z_axis = false);

	void clear_P();
	void init_P(Model_Reduced * model_initial, Model_Reduced * model_flattened_solved, double lambda);
	void apply_P();



private:
	Model_Reduced *model = 0;

	//	Eigen::VectorXd *b, *x, *x0, *f0, *delta_x;
	Eigen::SparseMatrix<double> *Aff; // free-free
	Eigen::SparseMatrix<double> *Afr; // free-restrained

	long getIndex(Node* n);

	// FREE nodes positions
	Eigen::VectorXd *Pos_x;
	Eigen::VectorXd *Pos_y;
	Eigen::VectorXd *Pos_z;

	// FREE nodes positions
	Eigen::VectorXd *Delta_x;
	Eigen::VectorXd *Delta_y;
	Eigen::VectorXd *Delta_z;

	// B arrays ( Pf -Afr*Xr)
	Eigen::VectorXd *B_x;
	Eigen::VectorXd *B_y;
	Eigen::VectorXd *B_z;

	// P: pseudo-stress, stress applied to beam in order to have the right length after solving
	// P arrays ( P = -lambda * sum(r_m/l_m(x_msolv-x_f))
	Eigen::VectorXd *P_x;
	Eigen::VectorXd *P_y;


	//precision of the solution
	Eigen::VectorXd *delta_x;

	long free_nodes;
	long restrained_nodes;
	// num_beams in the original model
	long num_beams;
	// num_beams in the original model that enter the force_density
	long num_beams_orig_to_forcedensity;

	// toal beams number
	long num_beams_total;

	// mapping between model and sparse matrixes
	std::map<long, long> node_rest_mapinv;
	std::map<long, long> node_free_mapinv;
	//std::map<Beam*, long> beams_mapinv;
	//std::vector<Beam*> beams_vector; // for convenience. maps ids 0,1,etc. to the beams that enter the force density
	//								 // the total number of beams that are used in the force density method (eg., skip steel beams)
	long num_beams_forcedensity = -1;

};








