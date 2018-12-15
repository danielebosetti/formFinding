
#pragma warning (disable:4267)
#pragma warning (disable:4244)
#include "ForceDensitySolver.h"


#include "appSettings.h"
#include "Node.h"
#include "Beam.h"
#include "Plate.h"
#include "utilCode.h"


#include "Eigen/Sparse"
#include "Eigen/LU"

#include <cmath> 

using namespace std;



// for each tri-plate, inserts 3 beams into beamsContainer
// the force density for each beam is put into the densityContainer
// beams are given progressive IDs, starting from startingBeamID, inclusive
void ForceDensitySolver::convertPlatesIntoBeamsForForceDensity(
	std::map<long, Plate*>& plates, std::vector<Beam*>& beamsContainer,
	std::vector<double>& forceDensityContainer,
	long startingBeamID)
{
	// deletes the beamsContainer first
	deleteAllVector<Beam>(beamsContainer);
	forceDensityContainer.clear();
	long beamCounter = startingBeamID;
	for (map<long, Plate*>::iterator it = plates.begin(); it != plates.end(); ++it) {
		Plate *p = it->second;

		// gets TC_SOAP plates only
		if (!is_ELEM_TC_SOAP(p))
			continue;

		if (p->type == "Tri3") {
			Beam *b1, *b2, *b3;
			Node *n1, *n2, *n3;
			// get the nodes
			n1 = p->pNode1;
			n2 = p->pNode2;
			n3 = p->pNode3;
			// create the beams
			b1 = new Beam();
			b2 = new Beam();
			b3 = new Beam();

			// gets FFC stress for this plate
			double stressPlate = p->sumStresses;
			//generateBeam(Beam* beam, Node *n1, Node *n2);
			generateBeam(b1, n1, n2);
			generateBeam(b2, n2, n3);
			generateBeam(b3, n3, n1);

			//void computeNormalForceForTriPlate(Node *n, Node *n1, Node *n2, double stress, Eigen::Vector3d &forceOut) {
			Eigen::Vector3d force1, force2, force3;
			// force normal to beam 1
			computeNormalForceForTriPlate(n1, n2, n3, stressPlate, force1);
			// force normal to beam 2
			computeNormalForceForTriPlate(n2, n3, n1, stressPlate, force2);
			// force normal to beam 3
			computeNormalForceForTriPlate(n3, n1, n2, stressPlate, force3);

			Eigen::Vector3d f1sum, f2sum, f3sum;
			f1sum = force1 + force3;
			f2sum = force2 + force1;
			f3sum = force3 + force2;

			////////////////////////////////////////////////////////////////////////////////////////////
			// here: linear algebra, base matrixes, inversion, project orto forces on beams ////////////
			////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////

			Eigen::Vector3d v1, v2, v3, versor1, versor2, versor3;
			// tri plate: n1->n2->n3->n1
			// v1->v2->v3
			v1 = *n2->position - *n1->position;
			v2 = *n3->position - *n2->position;
			v3 = *n1->position - *n3->position;

			// beam lengths
			double l1 = v1.norm();
			double l2 = v2.norm();
			double l3 = v3.norm();

			versor1 = v1; versor1.normalize();
			versor2 = v2; versor2.normalize();
			versor3 = v3; versor3.normalize();

			Eigen::Vector3d orto1 = v1.cross(v3); orto1.normalize();

			Eigen::Matrix3d base1, base1inv;
			Eigen::Matrix3d base2, base2inv;
			Eigen::Matrix3d base3, base3inv;
			base1 << versor1, versor3, orto1;
			base2 << versor2, versor1, orto1;
			base3 << versor3, versor2, orto1;
			//std::cout << "\nComputing base matrix for plate conversion: base1: \n";
			//std::cout << base1;
			base1inv = base1.inverse();
			base2inv = base2.inverse();
			base3inv = base3.inverse();

			// get coefficients..
			//vc: vector of coefficients
			Eigen::Vector3d vc1 = base1inv * f1sum;
			Eigen::Vector3d vc2 = base2inv * f2sum;
			Eigen::Vector3d vc3 = base3inv * f3sum;

			// force densities, to associate to beam_1, beam_2, beam_3
			double q1 = vc1.x() / l1;
			double q2 = vc2.x() / l2;
			double q3 = vc3.x() / l3;

			// checks that force density is non-zero, otherwise skip the beam
			if (std::abs(q1) > 1.e-9) {
				b1->ID = beamCounter++;
				// put beams and densities into the containers
				beamsContainer.push_back(b1);
				forceDensityContainer.push_back(q1);
			}
			// checks that force density is non-zero, otherwise skip the beam
			if (std::abs(q2) > 1.e-9) {
				b2->ID = beamCounter++;
				// put beams and densities into the containers
				beamsContainer.push_back(b2);
				forceDensityContainer.push_back(q2);
			}
			// checks that force density is non-zero, otherwise skip the beam
			if (std::abs(q3) > 1.e-9) {
				b3->ID = beamCounter++;
				// put beams and densities into the containers
				beamsContainer.push_back(b3);
				forceDensityContainer.push_back(q3);
			}

		}
		else {
			std::cout << "\nFORCE DENSITY ERROR! Impossible to convert the Quad Plate (id:" << p->ID <<
				" into beams. Only Tri plates are supported, this plate is skipped.";
		}
	}
}
using namespace Eigen;

bool ForceDensitySolver::go_Solve(
	VectorXd *Pos_x,
	BiCGSTAB<SparseMatrix<double> > &solver,
	VectorXd *B_x,
	VectorXd *delta_x,
	Eigen::SparseMatrix<double> *Aff
	) {
	cout << endl << "Calling SOLVE ... ";
	*Pos_x = solver.solve(*B_x);
	*delta_x = *Aff**Pos_x - *B_x;
	if (solver.info() != Success) {
		cout << endl << "Solving FAILED ! ";
		cout << "\nPrecision of solution: |Ax-b|= " << delta_x->norm();
		return false;
	}
	else
		cout << endl << "Solving Success ";
	cout << endl << "#iterations:     " << solver.iterations();
	cout << endl << "estimated error: " << solver.error();
	cout << endl << "Precision of solution: |Ax-b|=" << delta_x->norm();
	return true;
}



bool ForceDensitySolver::go_Solve(
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

