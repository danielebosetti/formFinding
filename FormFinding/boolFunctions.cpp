//#include "StdAfx.h"

#include "SharedVariables.h"
#include "BoolFunctions.h"

#include "Node.h"
#include "Beam.h"
#include "Plate.h"

/*
defines the node types, as node_tc (nodes subjected to the form-finding),
or node_steel (these nodes are fixed, and the form-finding does not move them).
*/


bool is_NODE_TC_L(Node *n){
	return (n->is_TC && !n->is_STEEL && !n->has_Restraint && !n->isRigidLink);
}

bool is_NODE_STEEL(Node *n){
	return (!n->is_TC && n->is_STEEL);
}

bool is_NODE_I(Node *n) {
	return (n->is_TC && n->is_STEEL);
}

bool is_NODE_RESTRAINED(Node *n) {
	return (n->has_Restraint);
}

bool is_NODE_TC(Node *n){
	return (n->is_TC && !n->is_STEEL && !n->isRigidLink);
}

bool is_NODE_RL(Node *n){
	return (n->isRigidLink);
}




bool is_ELEM_STEEL(Beam *b){
	return (b->material_type == STEEL);
}

bool is_ELEM_TC_SOAP(Beam *b){
	return (b->material_type == TC && !b->isRigidLink && !b->isElastic);
}
bool is_ELEM_TC_SOAP(Plate *p){
	return (p->material_type == TC);
}

bool is_ELEM_RL(Beam *b){
	return ( b->material_type == TC && b->isRigidLink);
}

bool is_ELEM_TC_CE(Beam *b){
	return (b->material_type == TC && !b->isRigidLink && b->isElastic);
}






bool is_BEAM_for_REAZ_V(Beam *b){
	return ( (is_NODE_I(b->pNode1) && is_NODE_TC_L(b->pNode2) ) || 
			 (is_NODE_TC_L(b->pNode1) && is_NODE_I(b->pNode2) )     ) ;
}

bool is_BEAM_TC(Beam *b){
	return ( b->material_type == TC ) ;
}

bool is_PLATE_TC(Plate *p){
	return ( p->material_type == TC ) ;
}


