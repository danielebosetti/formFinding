#include "stdafx.h"
#include "Flattening_Solver.h"
#include "Linear_Solver.h"

#include "Model_Reduced.h"

Flattening_Solver::Flattening_Solver()
{
	Aff = NULL;
	Afr = NULL ;
	Pos_x = NULL;
	Pos_y = NULL;
	Pos_z = NULL;
	B_x = NULL;
	B_y = NULL;
	B_z = NULL;
	P_x = NULL;
	P_y = NULL;
	Delta_x = NULL;
	Delta_y = NULL;
	Delta_z = NULL;
}


Flattening_Solver::~Flattening_Solver()
{
}

void Flattening_Solver::setModel(Model_Reduced * m1)
{
	this->model = m1;
}

// bool skip_z_axis is false by default
boolean Flattening_Solver::solveSGM(bool skip_z_axis)
{
	SparseLU<SparseMatrix<double> > solver;
	std::cout << "\nDecomposing matrix..";
	delta_x = new VectorXd(free_nodes);


	Aff->makeCompressed();
	
	solver.analyzePattern(*Aff);
	solver.factorize(*Aff);

	if (solver.info() != Success) {
		// decomposition failed
		std::cout << " decomposition failed, matrix is singular? ";

		return false;
	}
	std::cout << endl << "Decomposition ok ";
	
	bool res = true;

	res = res && LinearSolver::go_Solve(Delta_x, solver, B_x, delta_x, Aff);
	res = res && LinearSolver::go_Solve(Delta_y, solver, B_y, delta_x, Aff);

	// check skip_z_axis ?
	if (!skip_z_axis)
		res = res && LinearSolver::go_Solve(Delta_z, solver, B_z, delta_x, Aff);

	return res;
}

void Flattening_Solver::checkCleanData()
{
	this->model = NULL;
	if (Aff) delete Aff;
	if (Afr) delete Afr;
	if (Pos_x) delete Pos_x;
	if (Pos_y) delete Pos_y;
	if (Pos_z) delete Pos_z;
	if (B_x) delete B_x;
	if (B_y) delete B_y;
	if (B_z) delete B_z;
	if (P_x) delete P_x;
	if (P_y) delete P_y;
	if (Delta_x) delete Delta_x;
	if (Delta_y) delete Delta_y;
	if (Delta_z) delete Delta_z;
	Aff = NULL;
	Afr = NULL;
	Pos_x = NULL;
	Pos_y = NULL;
	Pos_z = NULL;
	B_x = NULL;
	B_y = NULL;
	B_z = NULL;
	P_x = NULL;
	P_y = NULL;
	Delta_x = NULL;
	Delta_y = NULL;
	Delta_z = NULL;
}

// conversion of indexes, from model-global IDs, to mapping for free 
// and restrained matrixes (internal solver)
long Flattening_Solver::getIndex(Node* n) {
	if (n->has_Restraint)
		return this->node_rest_mapinv[n->ID];
	else 
		return this->node_free_mapinv[n->ID];
}

// uses the model data to initialize the internal matrixes
// (model.free_nodes and restrained nodes must be already set)
void Flattening_Solver::initMatrixes(bool show_elem_count)
{
	using namespace Eigen;

	//restrained_nodes = this->wm->ffds.NODE_RESTRAINED.size();
	long total_node_count = this->model->nodes.size();
	cout << "\nModel has " << total_node_count << " total nodes";

	// count the free nodes
	free_nodes = this->model->nodes_free.size();
	cout << "\nModel has " << free_nodes << " free nodes";

	// all non_TC_L nodes are restrained
	restrained_nodes = total_node_count - free_nodes;
	cout << "\nModel has " << restrained_nodes << " restrained nodes";
	// quick check
	if (restrained_nodes != this->model->nodes_restrained.size())
		exitWithMessage("\n\nerror on restrained nodes, exit now");

	num_beams = this->model->beams.size();
	cout << "\nModel has " << num_beams << " beams from original model";


	// build free_nodes array: it is the model.node_free array
	// build restrained_nodes array: it is the model.node_restrained array

	//===================================================================================
	//===================================================================================
	// sets the matrixes
	//===================================================================================
	node_rest_mapinv.clear();
	node_free_mapinv.clear();

	long i;
	// inverse mapping
	i = 0;
	for (auto& x : model->nodes_free) {
		node_free_mapinv[x->ID] = i++;
	}
	// inverse mapping
	i = 0;
	for (auto& x : model->nodes_restrained) {
		node_rest_mapinv[x->ID] = i++;
	}


	// init free elems
	// build the NODE_FREE arrays
	B_x = new VectorXd(free_nodes);
	B_y = new VectorXd(free_nodes);
	B_z = new VectorXd(free_nodes);

	// zero the b vectors
	for (long j = 0; j < free_nodes; j++) {
		(*B_x)[j] = 0.;
		(*B_y)[j] = 0.;
		(*B_z)[j] = 0.;
	}

	// build the NODE_FREE arrays
	// NOTE ? are these needed?
	// we solve on the delta_x arrays
	Pos_x = new VectorXd(free_nodes);
	Pos_y = new VectorXd(free_nodes);
	Pos_z = new VectorXd(free_nodes);
	for (auto& item : this->model->nodes_free) {
		long i1 = getIndex(item);
		(*Pos_x)[i1] = item->x;
		(*Pos_y)[i1] = item->y;
		(*Pos_z)[i1] = item->z;
	}

	// build the NODE_FREE delta-arrays
	Delta_x = new VectorXd(free_nodes);
	Delta_y = new VectorXd(free_nodes);
	Delta_z = new VectorXd(free_nodes);

	
	// init the A matrix
	Aff = new SparseMatrix<double>(free_nodes, free_nodes);
	i = 0;
	// inits matrix to zero
	for (auto& item : this->model->nodes_free) {
		Aff->coeffRef(i, i) = 0.;
		i++;
	}

	// LOOP over all FREE nodes
	for (auto& n1 : this->model->nodes_free) {

		//cout << "\n node " << n1->ID << " has " << n1->adjacentNodes.size() << " adjacent nodes ";
		long i1 = getIndex(n1);
		for (auto& n2 : n1->adjacentNodes) {
			long i2 = getIndex(n2);
				
			//if (n1->has_Restraint && n2->has_Restraint) // do nothing
			//{} 
			if (!n1->has_Restraint && n2->has_Restraint) // n1 FREE, n2 RESTRAINED
			{
				(*B_x)[i1] += n2->x;
				(*B_y)[i1] += n2->y;
				(*B_z)[i1] += n2->z;
				Aff->coeffRef(i1, i1) += 1.;
			}
			if (!n1->has_Restraint && !n2->has_Restraint) // n1 FREE, n2 FREE
			{
				(*B_x)[i1] += n2->x;
				(*B_y)[i1] += n2->y;
				(*B_z)[i1] += n2->z;
				Aff->coeffRef(i1, i1) += 1.;
				Aff->coeffRef(i1, i2) = -1.;
			}
		}
	}


	// complete the b vector, 
	for (auto& n : model->nodes_free) {
		long i1 = getIndex(n);
		double coeff = n->adjacentNodes.size();
		(*B_x)[i1] -= coeff * n->x;
		(*B_y)[i1] -= coeff * n->y;
		(*B_z)[i1] -= coeff * n->z;
	}


}


// takes data from internal matrixes and put them into the model
void Flattening_Solver::updatePositionToModel(bool skip_z_axis)
{
	for (auto node : model->nodes_free) {
		long index = this->node_free_mapinv[node->ID];
		node->x += (*Delta_x)[index];
		node->y += (*Delta_y)[index];
		if (!skip_z_axis)
			node->z += (*Delta_z)[index];
		*node->position << node->x, node->y, node->z;
	}
	return;
}



double getNodeDistance(Node *n1, Node *n2) {
	return (*n1->position - *n2->position).norm();
}

void Flattening_Solver::clear_P() {
	if (P_x) delete P_x; P_x = NULL;
	if (P_y) delete P_y; P_y = NULL;
	P_x = new VectorXd(free_nodes);
	P_y = new VectorXd(free_nodes);
	// zero the P vector
	for (long i = 0; i < free_nodes; i++) {
		(*P_x)[i] = 0.0;
		(*P_y)[i] = 0.0;
	}
}


// initial model, flattened solved model, lambda
void Flattening_Solver::init_P(Model_Reduced * mod_ini, Model_Reduced * mod_flat_solved, double lambda)
{
	clear_P();

	// update lengths in flattened model
	mod_flat_solved->initBeamLengths();

	// accumulation of p_k_lt
	double P_acc_x = 0.0;
	double P_acc_y = 0.0;

	// n_t : node indexed by t at formula (13)
	for (auto n_t : mod_flat_solved->nodes_free ) {
		P_acc_x = 0.0;
		P_acc_y = 0.0;

		Node *n_t_ini = mod_ini->getCorrespondingNodeInThisModel(n_t);

		// n_m : node indexed by m at formula (13)
		for (auto n_m: n_t->adjacentNodes) {
			// gets initial beam length
			Node *n_m_ini = mod_ini->getCorrespondingNodeInThisModel(n_m);

			double R_m = getNodeDistance(n_m_ini, n_t_ini);
			double L_m = getNodeDistance(n_m, n_t);

			// check for debug
			//double v1 = n_m->position->x();
			//double v2 = n_m->position->y();
			//double v3 = n_t->position->x();
			//double v4 = n_t->position->y();
			//double w1 = n_m->x;
			//double w2 = n_m->y;
			//double w3 = n_t->x;
			//double w4 = n_t->y;

			//if (v1 != w1 || v2 != w2 || v3 != w3 || v4 != w4)
			//	cout << "\n error: different values here ?!?" <<
			//	"\n" << v1 << "  " << w1 <<
			//	"\n" << v2 << "  " << w2 <<
			//	"\n" << v3 << "  " << w3 <<
			//	"\n" << v4 << "  " << w4;

			P_acc_x += -lambda * R_m / L_m * (n_m->position->x() - n_t->position->x());
			P_acc_y += -lambda * R_m / L_m * (n_m->position->y() - n_t->position->y());
		}


		// put acc vaues at right point
		long index_free = node_free_mapinv[n_t->ID];
		(*P_x)[index_free] = P_acc_x;
		(*P_y)[index_free] = P_acc_y;	
	}

}

void Flattening_Solver::apply_P()
{
	*B_x += *P_x;
	*B_y += *P_y;
}









