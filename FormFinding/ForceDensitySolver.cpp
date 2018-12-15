#pragma warning (disable:4267)
#pragma warning (disable:4244)
#include "ForceDensitySolver.h"

#include "WorkModel.h"
#include "appSettings.h"
#include "Node.h"
#include "Beam.h"
#include "Plate.h"
#include "Model.h"
#include <iostream>
#include <fstream>

#include "Eigen/Sparse"
#include "Eigen/LU"

#include "MapTypesDef.h"
#include "BoolFunctions.h"

#include "SharedFunctions.h"
#include "SharedVariables.h"
#include "utilCode.h"
#include <cmath> 

using namespace std;

string default_subDir = "IO_txt\\data";

void exportVectorToFile(string prefixName, string subDir, vector<Node*> *m)
{
	string fullPath = appSettings::dataDir + subDir + "\\" + prefixName + ".txt";
	ofstream ofs;
	ofs.open(fullPath, ofstream::out);
	writeAllVectorToStream<Node>(*m, ofs);
	ofs.close();
}

void exportVectorToFile(string prefixName, string subDir, vector<Beam*> *m)
{
	string fullPath = appSettings::dataDir + subDir + "\\" + prefixName + ".txt";
	ofstream ofs;
	ofs.open(fullPath, ofstream::out);
	writeAllVectorToStream<Beam>(*m, ofs);
	ofs.close();
}

void exportVectorToFile(string prefixName, string subDir, vector<double> *m, long startingIndex)
{
	string fullPath = appSettings::dataDir + subDir + "\\" + prefixName + ".txt";
	ofstream ofs;
	ofs.open(fullPath, ofstream::out);
	ofs << "double-vector elements\n";
	ofs << "index\tvalue\n";
	for (long i = 0; i < m->size(); i++)
		ofs << (i + startingIndex) << "\t" << (*m)[i] << "\n";
	ofs.close();
}



void ForceDensitySolver::exportMatrixToFile(string prefixName, string subDir, SparseMatrix<double> *m)
{
	if (m->cols() > 100) {
		cout << "\nToo many elements, exporting of " << prefixName << " skipped.";
		return;
	}
	string fullPath = appSettings::dataDir + subDir + "\\" + prefixName + ".txt";
	ofstream ofs;
	ofs.open(fullPath, ofstream::out);
	ofs << *m;
}

void ForceDensitySolver::exportMatrixToFile(string prefixName, string subDir, VectorXd *v)
{
	if (v->size() > 100) {
		cout << "\nToo many elements, exporting of " << prefixName << " skipped.";
		return;
	}

	string fullPath = appSettings::dataDir + subDir + "\\" + prefixName + ".txt";
	ofstream ofs;
	ofs.open(fullPath, ofstream::out);
	ofs << *v;
}


void ForceDensitySolver::setWorkModel(WorkModel * wm)
{
	this->wm = wm;
}

void ForceDensitySolver::solveForceDensity()
{
	if (!wm) {
		//error!
		std::cout << "\nForceDensity : error! WorkModel is null.";
		return;
	}


	std::cout << "\n\n\nRecomputing for FFC..\n";
	wm->compute_forzePeso();

	wm->compute_normalPressures();


	std::cout << "\nConverting plates into beams..\n";

	// note!
	// 
	long startingBeamID = wm->model->beams.size() + 1;
	std::cout << "\nBeams starting from ID: " << startingBeamID;
	convertPlatesIntoBeamsForForceDensity(wm->model->plates, vectorBeamsConverted, forceDensityConverted, startingBeamID );


	std::cout << "\n\n\nBegin Force-Density solve..\n";
	initMatrixes();

	std::cout << "\nLogging converted beams to file";
	exportVectorToFile("beamsConverted", default_subDir, &vectorBeamsConverted);
	exportVectorToFile("force_densities_converted", default_subDir, &forceDensityConverted, num_beams);

	solveMatrixes();
	updatePositionToWorkModel();
	std::cout << "\nEnd Force-Density solve..";
}

void ForceDensitySolver::checkCleanData() {
	if (Aff) delete Aff;
	if (Afr) delete Afr;
	if (Cf) delete Cf;
	if (Cr) delete Cr;
	if (Pfx) delete Pfx;
	if (Pfy) delete Pfy;
	if (Pfz) delete Pfz;
	if (Pos_x) delete Pos_x;
	if (Pos_y) delete Pos_y;
	if (Pos_z) delete Pos_z;
	if (Res_x) delete Res_x;
	if (Res_y) delete Res_y;
	if (Res_z) delete Res_z;
	if (Q) delete Q;

	Cf = NULL;
	Cr = NULL;
	Aff = NULL;
	Afr = NULL;
	Pfx = NULL;
	Pfy = NULL;
	Pfz = NULL;
	Pos_x = NULL;
	Pos_y = NULL;
	Pos_z = NULL;
	Res_x = NULL;
	Res_y = NULL;
	Res_z = NULL;
	Q = NULL;

}

ForceDensitySolver::ForceDensitySolver()
{
	Cf = NULL;
	Cr = NULL;
	Aff = NULL;
	Afr = NULL;
	Pfx = NULL;
	Pfy = NULL;
	Pfz = NULL;
	Pos_x = NULL;
	Pos_y = NULL;
	Pos_z = NULL;
	Res_x = NULL;
	Res_y = NULL;
	Res_z = NULL;
	Q = NULL;
}

//iqm
void ForceDensitySolver::initializeQmatrix(Eigen::SparseMatrix<double> *Q) {
	Q->setIdentity();

	// uniform multiplier:
	// compreso fra 0. e 1.

	// LOOP for all the forcedensity-beams in the original model
	for (long i = 0; i < num_beams_orig_to_forcedensity; i++) {
		// gets the beam
		Beam *b;
		b = beams_vector[i];
		double targetDensity;

		// apply force_density_XX override
		if (appSettings::FORCE_DENSITY_TARGET==1)
			// gets the density
			if (is_ELEM_TC_SOAP(b)) {
				targetDensity = b->initialAxialForce / b->getLength();
			}
			else {
				targetDensity = (b->getLength() - b->L0) * b->axialStiffness / b->getLength();
			}
		else if (appSettings::FORCE_DENSITY_TARGET == 0) {
			// gets the density
			targetDensity = b->initialAxialForce;
		}
		else {
			exitWithMessage("Invalid [FORCE_DENSITY_TARGET] parameter, please check your configuration. Exit now.");
		}

		double finalDensity = targetDensity;

		//check that force density is non-zero, else error
		if (std::abs(finalDensity) > 1.e-10) {
			// puts into the Q matrix
			Q->coeffRef(i, i) = finalDensity;
		}
		else {
			// log error
			std::cout << "\nFORCE DENSITY ERROR, force density on beam <" << b->ID << "> too small, val: " << finalDensity;
		}
	}


	// LOOP for all the forcedensity-beams converted from the plates
	for (long i = 0; i < this->vectorBeamsConverted.size(); i++) {
		// gets the beam
		Beam *b;
		b = vectorBeamsConverted[i];
		// gets the density
		double finalDensity = this->forceDensityConverted[i];
		// index into Q matrix!
		long finalIndex = num_beams_orig_to_forcedensity + i;

		//check that force density is non-zero, else error
		if (std::abs(finalDensity) > 1.e-10) {
			// puts into the Q matrix
			Q->coeffRef(i, i) = finalDensity;
		}
		else {
			// log error
			std::cout << "\nFORCE DENSITY ERROR, force density on beam <" << b->ID << "> too small, val: " << finalDensity;
		}
	}


	//*Q = *Q * appSettings::UNIFORM_FORCE_DENSITY_MULTIPLIER;
	exportMatrixToFile("Q", default_subDir, Q);
}


// compute the force on Node n due to the beam b=(n,n1), and normal to this beam,
// assuming that n is on the Tri3 plate (n,n1,n2)
// plate has stress <stress>, and the force is written into forceOut
void ForceDensitySolver::computeNormalForceForTriPlate(Node *n, Node *n1, Node *n2, double stress, Eigen::Vector3d &forceOut) {
	Vector3d v1, v2, versor1, versor2, versor3;
	// NOTE: duplicate code here! see c_sfn
	// TODO: refactor the code, remove duplicates, DRY
	////////////////////////////////////////////////////////////////////
	// FORCE DUE TO BEAM (n,n1) on node n
	v1 = *n1->position - *n->position;
	v2 = *n2->position - *n1->position;
	versor1 = v1; versor1.normalize();
	versor2 = v2; versor2.normalize();

	versor3 = versor2 - versor1 * versor1.dot(versor2);
	versor3.normalize();
	// outForce: half of the total normal force
	forceOut = versor3 * v1.norm() * stress / 2;
	//std::cout << "\nstress: " << stress;
}

void ForceDensitySolver::generateBeam(Beam* b, Node *n1, Node *n2) {
	// sets nodes and nodeIDs
	b->pNode1 = n1;
	b->pNode2 = n2;
	b->node1 = n1->ID;
	b->node2 = n2->ID;
	// sets beam type to TC_SOAP
	b->material_type = TC;
	b->isRigidLink = false;
	b->isElastic = false;

	// set unknown params to zero
	b->IDGroup = 0;
	b->Gr2 = 0;
	b->beamPropertyID = 0;
}



void ForceDensitySolver::pushBeamIntoConnectivityMatrix(
	Beam *b,
	long index_beam,
	SparseMatrix<double> *Cf, // free
	SparseMatrix<double> *Cr // restrained
	) {
	beams_mapinv[b] = index_beam;
	beams_vector.push_back(b);
	Node *n1;
	Node *n2;
	n1 = b->pNode1;
	n2 = b->pNode2;
	// NODE_1 : give coefficient -1 into the adjency matrix
	//if (n1->has_Restraint) {
	if (is_NODE_TC_L(n1)) {
		// if free, then push to the free matrix 
		long indexFree = node_free_mapinv[n1->ID];
		Cf->insert(index_beam, indexFree) = -1.;
	}
	else {
		// if restrained, then push to the Afr matrix
		long indexRestrained = node_rest_mapinv[n1->ID];
		Cr->insert(index_beam, indexRestrained) = -1.;
	}
	// NODE_2 : give coefficient +1 into the adjency matrix
	//if (n2->has_Restraint) {
	if (is_NODE_TC_L(n2)) {
		// if free, then push to the free matrix 
		long indexFree = node_free_mapinv[n2->ID];
		Cf->insert(index_beam, indexFree) = +1.;
	}
	else {
		// if restrained, then push to the Afr matrix
		long indexRestrained = node_rest_mapinv[n2->ID];
		Cr->insert(index_beam, indexRestrained) = +1.;
	}
}

void ForceDensitySolver::initMatrixes()
{
	using namespace Eigen;

	//restrained_nodes = this->wm->ffds.NODE_RESTRAINED.size();
	long total_node_count = this->wm->model->nodes.size();
	cout << "\nModel has " << total_node_count << " total nodes";

	// count the free nodes
	free_nodes = this->wm->ffds.NODE_TC_L.size();
	cout << "\nModel has " << free_nodes << " free nodes";

	// all non_TC_L nodes are restrained
	restrained_nodes = total_node_count - free_nodes;
	cout << "\nModel has " << restrained_nodes << " restrained nodes";

	num_beams = this->wm->model->beams.size();
	cout << "\nModel has " << num_beams << " beams from original model";
	num_beams_converted = this->vectorBeamsConverted.size();
	cout << "\nModel has " << num_beams_converted << " beams converted from the tri-plates";
	num_beams_total = num_beams + num_beams_converted;
	cout << "\nTotal beams number for force-density: " << num_beams_total;

	// build free_nodes array: it is the ffds.NODE_TC_L array
	// build restrained_nodes array: it is the ffds.NODE_RESTRAINED array

	//===================================================================================
	//===================================================================================
	// SOLVE
	//===================================================================================

	checkCleanData();


	node_rest_mapinv.clear();
	node_free_mapinv.clear();
	beams_mapinv.clear();
	beams_vector.clear(); 

	// build the NODE_FREE arrays
	Pfx = new VectorXd(free_nodes);
	Pfy = new VectorXd(free_nodes);
	Pfz = new VectorXd(free_nodes);
	B_x = new VectorXd(free_nodes);
	B_y = new VectorXd(free_nodes);
	B_z = new VectorXd(free_nodes);



	// build the NODE_RESTRAINED arrays
	Res_x = new VectorXd(restrained_nodes);
	Res_y = new VectorXd(restrained_nodes);
	Res_z = new VectorXd(restrained_nodes);
	// build the NODE_FREE arrays
	Pos_x = new VectorXd(free_nodes);
	Pos_y = new VectorXd(free_nodes);
	Pos_z = new VectorXd(free_nodes);
	// build the NODE_RESTRAINED array

	// fill the xr array
	//for (long i = 0; i < this->wm->ffds.NODE_RESTRAINED.size(); i++) {
	long i = -1;
	for ( map<long, Node*>::iterator it = this->wm->model->nodes.begin(); it != this->wm->model->nodes.end(); ++it ) {
		// gets the node
		Node *n = it->second;
		//skip free nodes
		if (is_NODE_TC_L(n))
			continue;
		// gets the x coord
		node_rest_mapinv[n->ID] = ++i;

		(*Res_x)[i] = n->position->x();
		(*Res_y)[i] = n->position->y();
		(*Res_z)[i] = n->position->z();
	}

	exportMatrixToFile("Res_x", default_subDir, Res_x);
	exportMatrixToFile("Res_y", default_subDir, Res_y);
	exportMatrixToFile("Res_z", default_subDir, Res_z);

	exportVectorToFile("NODE_TC_L", default_subDir, &(this->wm->ffds.NODE_TC_L));

	//h12
	// fill the NODE_FREE arrays
	for (long i = 0; i < this->wm->ffds.NODE_TC_L.size(); i++) {
		// gets the node
		Node *n;
		n = this->wm->ffds.NODE_TC_L[i];
		// gets the x coord
		node_free_mapinv[n->ID] = i;

		Vector3d *forzaPeso = &(this->wm->ffds.forzaPeso[n->ID]);
		// insert forze peso
		(*Pfx)[i] = forzaPeso->x();
		(*Pfy)[i] = forzaPeso->y();
		(*Pfz)[i] = forzaPeso->z();
		// insert forces from FFC_ndForces
		Vector3d *forceFFC = &(this->wm->ffds.ndForces_FFC[n->ID]);
		(*Pfx)[i] += forceFFC->x();
		(*Pfy)[i] += forceFFC->y();
		(*Pfz)[i] += forceFFC->z();
		// insert forces from PLATEPressure_normalForces
		Vector3d *forceNormalPlate = &(this->wm->ffds.forcePressure[n->ID]);
		(*Pfx)[i] += forceNormalPlate->x();
		(*Pfy)[i] += forceNormalPlate->y();
		(*Pfz)[i] += forceNormalPlate->z();
	}

	exportMatrixToFile("Pf_x", default_subDir, Pfx);
	exportMatrixToFile("Pf_y", default_subDir, Pfy);
	exportMatrixToFile("Pf_z", default_subDir, Pfz);


	//=============================================================
	//===  External FORCES: gravity and plate-normal-pressure only
	//=============================================================

	// count the number of beams to go into the force density!
	// soap beams in the original model, + nodes converted
	long count_beam = 0;
	// original-beams for model: va da 1 a num_beams
	for (long i = 1; i <= num_beams; i++) {
		Beam *b = this->wm->model->beams[i];
		// skip non-tc beams!
		if ((!is_ELEM_TC_SOAP(b)) && (!is_ELEM_TC_CE(b)))
			continue;
		// here, can increment the count_beam
		count_beam = count_beam + 1;
	}
	
	// FINALLY
	long force_density_beams_count = count_beam + this->vectorBeamsConverted.size();

	Cf = new SparseMatrix<double>(force_density_beams_count, free_nodes);
	Cr = new SparseMatrix<double>(force_density_beams_count, restrained_nodes);

	long index_beam = -1;
	// original-beams for model: va da 1 a num_beams
	for (long i = 1; i <= num_beams; i++ ) {
		Beam *b = this->wm->model->beams[ i ];
		// skip non-tc beams!
		if ((!is_ELEM_TC_SOAP(b)) && (!is_ELEM_TC_CE(b)))
			continue;
		// here, can increment the index_beam
		index_beam = index_beam + 1;
		pushBeamIntoConnectivityMatrix(b, index_beam, Cf, Cr);
	}
	// IMPORTANT!!
	num_beams_orig_to_forcedensity = index_beam + 1;

	// then, push all the converted beams into the force-density solver
	for (long i = 0; i < this->vectorBeamsConverted.size(); i++) {
		Beam *b = vectorBeamsConverted[i];
		// push all the beams (all plates are soap (?)
		// here, can increment the index_beam
		index_beam = index_beam + 1;
		pushBeamIntoConnectivityMatrix(b, index_beam, Cf, Cr);
	}

	// update the total beam count
	// note, it is equal to... force_density_beams_count! not num_beams+beams_converted = total_beams
	num_beams_forcedensity = index_beam + 1;
	
	exportMatrixToFile("Cf", default_subDir, Cf);
	exportMatrixToFile("Cr", default_subDir, Cr);


	Eigen::SparseMatrix<double> *Cft = new SparseMatrix<double>(free_nodes , num_beams_forcedensity);
	*Cft = Cf->transpose();
	exportMatrixToFile("Cft", default_subDir, Cft);

	//Q = new SparseMatrix<double>(num_beams, num_beams);
	Q = new SparseMatrix<double>(num_beams_forcedensity, num_beams_forcedensity);
	initializeQmatrix(Q);


	Aff = new SparseMatrix<double>(free_nodes, free_nodes);
	*Aff = *Cft * *Q * *Cf;
	exportMatrixToFile("Aff", default_subDir, Aff);


	//std::cout << "\n\nMatrixes computed, \n Aff = \n";
	//std::cout << *Aff;

	Afr = new SparseMatrix<double>(free_nodes, restrained_nodes);
	*Afr = *Cft * *Q * *Cr;
	exportMatrixToFile("Afr", default_subDir, Afr);


	//===================================================================================
}



void ForceDensitySolver::solveMatrixes()
{
	std::cout << "\nComputing B_x, B_y, B_z";
	*B_x = *Pfx - *Afr * *Res_x;
	*B_y = *Pfy - *Afr * *Res_y;
	*B_z = *Pfz - *Afr * *Res_z;

	SparseLU<SparseMatrix<double> > solver;
	std::cout << "\nDecomposing matrix..";
	delta_x = new VectorXd(free_nodes);

	solver.analyzePattern(*Aff);
	solver.factorize(*Aff);

	if (solver.info() != Success) {
		// decomposition failed
		std::cout << " decomposition failed ";
		return;
	}
	std::cout << endl << "Decomposition ok ";

	go_Solve(Pos_x, solver, B_x, delta_x, Aff);
	go_Solve(Pos_y, solver, B_y, delta_x, Aff);
	go_Solve(Pos_z, solver, B_z, delta_x, Aff);

	exportMatrixToFile("Pos_x", default_subDir, Pos_x);
	exportMatrixToFile("Pos_y", default_subDir, Pos_y);
	exportMatrixToFile("Pos_z", default_subDir, Pos_z);
}

void ForceDensitySolver::updatePositionToWorkModel()
{

	std::cout << "\n\nSetting result value into model..";
	

	// updates the positions into the WorkModel
	for (long i = 0; i < this->wm->ffds.NODE_TC_L.size(); i++) {
		// gets the node
		Node *n;
		n = this->wm->ffds.NODE_TC_L[i];
		// sets the computed position into the node
		n->position->x() = (*Pos_x)[i];
		n->position->y() = (*Pos_y)[i];
		n->position->z() = (*Pos_z)[i];
	}
	this->wm->updateNodes();


	// creates the FORCE_DENSITY map into the ffds object
	// loops over the beam_vector
	for (long i = 0; i < this->beams_vector.size(); i++) {
		Beam *b = beams_vector[i];
		// put to the beam map
		this->wm->ffds.FORCE_DENSITY[b] = Q->coeff(i,i);
	}
}




ForceDensitySolver::~ForceDensitySolver()
{
}
 