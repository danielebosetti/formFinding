#pragma once

class WorkModel;
class Node;
class Beam;
class Plate;
#include <Eigen/Sparse>
#include <Eigen/LU>
#include <string>
#include <map>
#include <vector>

class ForceDensitySolver
{
public:

	ForceDensitySolver();
	~ForceDensitySolver();

	void setWorkModel(WorkModel *wm);

	// this is the main entry point
	void solveForceDensity();

	void exportMatrixToFile(std::string prefixName, 
		std::string subDir, Eigen::SparseMatrix<double> *m);
	void ForceDensitySolver::exportMatrixToFile(std::string prefixName,
		std::string subDir, Eigen::VectorXd *Pfx);

private:

	WorkModel *wm = 0;


	//	Eigen::VectorXd *b, *x, *x0, *f0, *delta_x;
	Eigen::SparseMatrix<double> *Aff; // free-free
	Eigen::SparseMatrix<double> *Afr; // free-restrained
	Eigen::SparseMatrix<double> *Cf; // free
	Eigen::SparseMatrix<double> *Cr; // restrained
	Eigen::SparseMatrix<double> *Q; // Q matrix

	// external forces
	Eigen::VectorXd *Pfx;
	Eigen::VectorXd *Pfy;
	Eigen::VectorXd *Pfz;

	// FREE nodes positions
	Eigen::VectorXd *Pos_x;
	Eigen::VectorXd *Pos_y;
	Eigen::VectorXd *Pos_z;

	// RESTRAINED nodes positions
	Eigen::VectorXd *Res_x;
	Eigen::VectorXd *Res_y;
	Eigen::VectorXd *Res_z;

	// B arrays ( Pf -Afr*Xr)
	Eigen::VectorXd *B_x;
	Eigen::VectorXd *B_y;
	Eigen::VectorXd *B_z;

	//precision of the solution
	Eigen::VectorXd *delta_x;

	long free_nodes;
	long restrained_nodes;
	// num_beams in the original model
	long num_beams;
	// num_beams in the original model that enter the force_density
	long num_beams_orig_to_forcedensity;
	// num_beams converted from the plates
	long num_beams_converted;
	// toal beams number
	long num_beams_total;


	// mapping between model and sparse matrixes
	std::map<long, long> node_rest_mapinv;
	std::map<long, long> node_free_mapinv;
	std::map<Beam*, long> beams_mapinv;
	std::vector<Beam*> beams_vector; // for convenience. maps ids 0,1,etc. to the beams that enter the force density
	// the total number of beams that are used in the force density method (eg., skip steel beams)
	long num_beams_forcedensity = -1;

	void pushBeamIntoConnectivityMatrix(
		Beam *b,
		long index_beam,
		Eigen::SparseMatrix<double> *Cf, // free
		Eigen::SparseMatrix<double> *Cr // restrained
		);

	void checkCleanData();
	void initMatrixes();
	void solveMatrixes();
	void updatePositionToWorkModel();
	bool go_Solve(
		Eigen::VectorXd *Pos_x,
		Eigen::BiCGSTAB<Eigen::SparseMatrix<double> > &solver,
		Eigen::VectorXd *B_x,
		Eigen::VectorXd *delta_x,
		Eigen::SparseMatrix<double> *Aff
		);
	bool go_Solve(
		Eigen::VectorXd *Pos_x,
		Eigen::SparseLU<Eigen::SparseMatrix<double> > &solver,
		Eigen::VectorXd *B_x,
		Eigen::VectorXd *delta_x,
		Eigen::SparseMatrix<double> *Aff
		);

	void initializeQmatrix(Eigen::SparseMatrix<double> *Q);

	// for each tri-plate, inserts 3 beams into beamsContainer
	// the force density for each beam is put into the densityContainer
	// beams are given progressive IDs, starting from startingBeamID, inclusive
	void convertPlatesIntoBeamsForForceDensity(
		std::map<long, Plate*> &plates,
		std::vector<Beam*>& beamsContainer,
		std::vector<double>& forceDensityContainer,
		long startingBeamID
		);

	// generates the beam, composed by n1 and n2
	// oppositeNode is the third node in the plate, and stress is the FFC sumStresses
	void generateBeam(Beam* beam, Node *n1, Node *n2); 
	// computes normal forces due to Tri3 plate stress
	void computeNormalForceForTriPlate(Node *n, Node *n1, Node *n2, double stress, Eigen::Vector3d &forceOut);
	// containers for the conversion from Tri3 plates into beams
	std::vector<Beam*> vectorBeamsConverted;
	std::vector<double> forceDensityConverted;

};

