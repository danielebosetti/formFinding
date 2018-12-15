//#include "StdAfx.h"

#include "WorkModel.h"
#include "Node.h"
#include "Beam.h"
#include "Plate.h"
#include "Model.h"
#include <algorithm>
#include <boost\foreach.hpp>

//#include <omp.h>
#pragma warning (disable:4018)



// generates the NODE POINTERS for all the BEAMS
void WorkModel::generateBeamNodePointers() {

	// delete all node pointers first
	BOOST_FOREACH(mapElement_Beam b1, model->beams) {
		Beam *b = b1.second;
		b->pNode1 = NULL;
		b->pNode2 = NULL;
	}

	Node *n1, *n2;
	Beam *b;
	BOOST_FOREACH (mapElement_Beam b1 , model->beams ) {
		// gets beam
		b = b1.second;
		// gets pointer to the node
		n1 = model->nodes[b->node1];
		n2 = model->nodes[b->node2];
		// add node pointer to the beam
		b->pNode1 = n1;
		b->pNode2 = n2;
	}
}


// generates the NODE POINTERS for all the PLATES
void WorkModel::generatePlateNodePointers() {

	//delete all pointers first
	BOOST_FOREACH(mapElement_Plate p1, model->plates) {
		Plate *p;
		p = p1.second;
		p->pNode1 = NULL;
		p->pNode2 = NULL;
		p->pNode3 = NULL;
		p->pNode4 = NULL;
	}

	Node *n1, *n2, *n3, *n4;
	Plate *p;
	BOOST_FOREACH (mapElement_Plate p1 , model->plates ) {
		// gets plate
		p = p1.second;
		// get pointer to the node
		n1 = model->nodes[p->node1];
		n2 = model->nodes[p->node2];
		n3 = model->nodes[p->node3];
		// add node pointer to the plate
		p->pNode1 = n1;
		p->pNode2 = n2;
		p->pNode3 = n3;

		if (p->node4 != -1){
			n4 = model->nodes[p->node4];
			p->pNode4 = n4;
		}

	}
}


// generates the INCIDENT BEAM SET for all the NODES
void WorkModel::generateIncidentBeams(){
	//delete all incident sets first
	BOOST_FOREACH(mapElement_Node n1, model->nodes) {
		Node *n;
		n = n1.second;
		n->incidentBeams.clear();
	}

	Beam *b;
	// LOOP over the beams
	BOOST_FOREACH (mapElement_Beam b1 , model->beams ) {
		// gets beam
		b = b1.second;

		// add to the two nodes' incidentBeams set, the current beam.
		model->nodes[b->node1]->incidentBeams.push_back(b);
		model->nodes[b->node2]->incidentBeams.push_back(b);
	}

}



// generates the INCIDENT PLATE SET for all the NODES
void WorkModel::generateIncidentPlates(){
	//delete all incident sets first
	BOOST_FOREACH(mapElement_Node n1, model->nodes) {
		Node *n;
		n = n1.second;
		n->incidentPlates.clear();
	}

	Plate *p;
	// LOOP over the plates
	BOOST_FOREACH (mapElement_Plate p1 , model->plates ) {
		// gets plate
		p = p1.second;

		// add to the nodes' incidentPlates set, the current plate.
		model->nodes[p->node1]->incidentPlates.push_back(p);
		model->nodes[p->node2]->incidentPlates.push_back(p);
		model->nodes[p->node3]->incidentPlates.push_back(p);
		if (p->node4 != -1)
			model->nodes[p->node4]->incidentPlates.push_back(p);
	}
}


// generates the ADJACENT NODE SET for all the NODES
void WorkModel::generateAdjacentNodes(){

	//delete all incident sets first
	BOOST_FOREACH(mapElement_Node n1, model->nodes) {
		Node *n;
		n = n1.second;
		n->adjacentNodes.clear();
	}

	Beam *b;
	Plate *p;
	Node *n1, *n2, *n3, *n4;

	// LOOP over the beams
	BOOST_FOREACH (mapElement_Beam b1 , model->beams ) {
		// gets beam
		b = b1.second;
		// gets nodes
		n1 = b->pNode1;
		n2 = b->pNode2;
		// two nodes are adjacent
		n1->adjacentNodes.insert(n2);
		n2->adjacentNodes.insert(n1);
	}


	// LOOP over plates
	BOOST_FOREACH (mapElement_Plate p1 , model->plates ) {
		// gets plate
		p = p1.second;
		// gets nodes
		n1 = p->pNode1;
		n2 = p->pNode2;
		n3 = p->pNode3;
		n4 = p->pNode4;
		// these nodes are adjacent 
		if (n4!=NULL) {
			n1->adjacentNodes.insert(n2);
			n1->adjacentNodes.insert(n3);
			n1->adjacentNodes.insert(n4);

			n2->adjacentNodes.insert(n1);
			n2->adjacentNodes.insert(n3);
			n2->adjacentNodes.insert(n4);

			n3->adjacentNodes.insert(n1);
			n3->adjacentNodes.insert(n2);
			n3->adjacentNodes.insert(n4);

			n4->adjacentNodes.insert(n1);
			n4->adjacentNodes.insert(n2);
			n4->adjacentNodes.insert(n3);
		}
		else {
			n1->adjacentNodes.insert(n2);
			n1->adjacentNodes.insert(n3);

			n2->adjacentNodes.insert(n1);
			n2->adjacentNodes.insert(n3);

			n3->adjacentNodes.insert(n1);
			n3->adjacentNodes.insert(n2);
		}
	}
}


// LINKS the Vector3d position for all the NODES
// copies the positions, from the Node-s, to the workmodel->position array
void WorkModel::linkNodePosition(){
	// LOOP over all nodes, generate the positions
	// NOTE: prima faceva NEW.. ora setta il riferimento all'array
	BOOST_FOREACH (mapElement_Node b1 , model->nodes ) {
		// gets node
		Node *n = b1.second;
		// SETS NODE POSITION
		n->position = &posI[n->ID];
		// SETS POSITION COORDINATES
		*n->position << n->x, n->y, n->z;
	}
}
