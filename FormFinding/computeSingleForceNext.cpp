#include "StdAfx.h"
#include "WorkModel.h"
#include <algorithm>



//here csfn
// compute the force on node N when the node is moved to position "nextPosition" (if all the model is unchanged except for node n)
void WorkModel::computeSingleForceNext(Node *n, Vector3d nextPosition, Vector3d& nextForce){ 
	Vector3d versor;
	Node *node1, *node2, *otherNode;

	// sets resulting force to zero
	nextForce << 0.,0.,0.;	

	// ************************************************************************************************************
	// ************************************************************************************************************
	// COMPUTE FORCE FROM BEAMS
	// LOOP over incident beams for NODE N
	BOOST_FOREACH (Beam *b , n->incidentBeams ) {

		if ( is_ELEM_TC_SOAP(b) ) {

			node1 = b->pNode1;
			node2 = b->pNode2;

			if (n == node1)
				otherNode = node2;
			else
				otherNode = node1;

			versor =  *(otherNode->position) - nextPosition;
			versor.normalize();

			nextForce = nextForce + (versor * b->initialAxialForce);
		}
		else if ( is_ELEM_TC_CE(b) ) {
			//cout << "\n elastic beam: " ;
			node1 = b->pNode1;
			node2 = b->pNode2;

			if (n == node1)
				otherNode = node2;
			else
				otherNode = node1;

			versor =  *(otherNode->position) - nextPosition;
			double L_dist = versor.norm(); 
			versor.normalize();

			//nextForce = nextForce + (versor * (L_dist - b->L0) * b->coeffEA / b->LS  );
			nextForce = nextForce + (versor * (L_dist - b->L0) * b->axialStiffness );
			//nextForce = nextForce + (versor * b->sumTensions);


		}
	}
	

	// ************************************************************************************************************
	// ************************************************************************************************************
	// COMPUTE FORCES FROM PLATES
	// LOOP over incident PLATES for NODE N
	BOOST_FOREACH (Plate *p , n->incidentPlates ) {

		//for TRI3 plates only
		if (p->type=="Quad4") {
		
		// ************************************************************************************************************
		// COMPUTE FORCES FOR QUAD4 PLATES

		// get NODES for this plate
		if (n == p->pNode1){
			node1 = p->pNode2;
			node2 = p->pNode4;
		}
		else if (n == p->pNode2){
			node1 = p->pNode3;
			node2 = p->pNode1;
		}
		else if (n == p->pNode3){
			node1 = p->pNode4;
			node2 = p->pNode2;
		}
		else if (n == p->pNode4){
			node1 = p->pNode1;
			node2 = p->pNode3;
		}
		else { cout << " computeSingleForceNext: ERROR BEAMS?? "; system("PAUSE"); exit(0); }

		//do force on node N
		Vector3d v1, v2, versor1, versor2, versor3, forcePlate;

		////////////////////////////////////////////////////////////////////
		// FORCE DUE TO BEAM_1

		v1 = *node1->position - *n->position; 
		v2 = *node2->position - *node1->position;

		versor1 = v1; versor1.normalize();
		versor2 = v2; versor2.normalize();

		versor3 = versor2 - versor1 * versor1.dot(versor2);
		versor3.normalize();

		forcePlate = versor3 * v1.norm() * p->sumStresses / 2;
		//cout << endl << "Node " << n->ID << "  plateForce " << forcePlate.norm();

		//OVERRIDE: RESTRAINT DX E DY
		if (restraintPlatesTC_DXDY){
			// TODO: fix this..
			// non azzeriamo le forze della tela sui nodi interfaccia, perchè ci servono le reazioni vincolari
			if (!is_NODE_I(n)) {
				forcePlate[0] = 0.;
				forcePlate[1] = 0.;
			}
		}

		nextForce = nextForce + forcePlate;


		////////////////////////////////////////////////////////////////////
		// FORCE DUE TO BEAM_2

		v1 = *n->position - *node2->position; 
		v2 = *node1->position - *n->position;

		versor1 = v1; versor1.normalize();
		versor2 = v2; versor2.normalize();

		versor3 = versor2 - versor1 * versor1.dot(versor2);
		versor3.normalize();

		forcePlate = versor3 * v1.norm() * p->sumStresses / 2;
		//cout << endl << "Node " << n->ID << "  plateForce " << forcePlate.norm();

		//OVERRIDE: RESTRAINT DX E DY
		if (restraintPlatesTC_DXDY){
			// TODO: fix this..
			// non azzeriamo le forze della tela sui nodi interfaccia, perchè ci servono le reazioni vincolari
			if (!is_NODE_I(n)) {
				forcePlate[0] = 0.;
				forcePlate[1] = 0.;
			}
		}

		nextForce = nextForce + forcePlate;

		}


		if (p->type=="Tri3") {

		// ************************************************************************************************************
		// COMPUTE FORCES FOR TRI_3 PLATES

		// get NODES for this plate
		if (n == p->pNode1){
			node1 = p->pNode2;
			node2 = p->pNode3;
		}
		else if (n == p->pNode2){
			node1 = p->pNode3;
			node2 = p->pNode1;
		}
		else if (n == p->pNode3){
			node1 = p->pNode1;
			node2 = p->pNode2;
		}
		else { cout << " computeSingleForceNext: ERROR BEAMS?? "; system("PAUSE"); exit(0); }


		//do force on node N
		Vector3d v1, v2, versor1, versor2, versor3, forcePlate;

		////////////////////////////////////////////////////////////////////
		// FORCE DUE TO BEAM_1

		v1 = *node1->position - *n->position; 
		v2 = *node2->position - *node1->position;

		versor1 = v1; versor1.normalize();
		versor2 = v2; versor2.normalize();

		versor3 = versor2 - versor1 * versor1.dot(versor2);
		versor3.normalize();

		forcePlate = versor3 * v1.norm() * p->sumStresses / 2;
		//cout << endl << "Node " << n->ID << "  plateForce " << forcePlate.norm();


		//OVERRIDE: RESTRAINT DX E DY
		if (restraintPlatesTC_DXDY){
			// TODO: fix this..
			// non azzeriamo le forze della tela sui nodi interfaccia, perchè ci servono le reazioni vincolari
			if (!is_NODE_I(n)) {
				forcePlate[0] = 0.;
				forcePlate[1] = 0.;
			}
		}

		nextForce = nextForce + forcePlate;



		////////////////////////////////////////////////////////////////////
		// FORCE DUE TO BEAM_2

		v1 = *n->position - *node2->position; 
		v2 = *node1->position - *n->position;

		versor1 = v1; versor1.normalize();
		versor2 = v2; versor2.normalize();

		versor3 = versor2 - versor1 * versor1.dot(versor2);
		versor3.normalize();

		forcePlate = versor3 * v1.norm() * p->sumStresses / 2;
		//cout << endl << "Node " << n->ID << "  plateForce " << forcePlate.norm();

		//OVERRIDE: RESTRAINT DX E DY
		if (restraintPlatesTC_DXDY){
			// TODO: fix this..
			// non azzeriamo le forze della tela sui nodi interfaccia, perchè ci servono le reazioni vincolari
			if (!is_NODE_I(n)) {
				forcePlate[0] = 0.;
				forcePlate[1] = 0.;
			}
		}

		nextForce = nextForce + forcePlate;
		}

	}

	// somma la forza Peso
	nextForce = nextForce + ffds.forzaPeso[n->ID];

	// somma la normal Pressure
	nextForce = nextForce + ffds.forcePressure[n->ID];

	// somma la NdForce_FFC
	nextForce = nextForce + ffds.ndForces_FFC[n->ID];



}

