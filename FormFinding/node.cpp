
#include "stdafx.h"

#include "node.h"




Node::Node(void)
{
//	cout << " create node ";
	is_TC = false;
	is_STEEL = false;
	has_Restraint = false;
	isRigidLink = false;
	totalStiffness = 1.;

	skipCompute = 0;
	enabled = true;
	forceIsComputed = false;

	position = NULL;
}


Node::~Node(void)
{
//	cout << " destroy node ";
}

Node::Node(const Node& n)
{
	this->boh1 = n.boh1;
	// generate stuff for REDUCED model only
	this->coeff1 = this->coeff2 = this->coeff3 = 0.0;
	this->enabled = true;
	this->has_Restraint = n.has_Restraint;
	this->ID = n.ID;
	this->x = n.x;
	this->y = n.y;
	this->z = n.z;
	this->position = NULL;

	this->totalStiffness = n.totalStiffness;
}



// mark the adjacent nodes as to-be-recompute the force.
//void Node::toRecomputeForceOnAdjacentNodes() {
//	BOOST_FOREACH (Node *n, this->adjacentNodes) {
//		n->forceIsComputed = false;
//	}
//}

void Node::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   Node%22d%8d%24.14E%24.14E%24.14E\n", ID, boh1, x, y, z );
	ofs << stringOut;
}

void Node::init_position()
{
	if (this->position != NULL) {
		// vector already exists
	}
	else {
		// else, create vector3d elem and init it with this node values
		this->position = new Vector3d();
	}
	// update position values!
	*(this->position) << this->x, this->y, this->z;

}


// legge i dati usando il fileLoader che gli viene passato
void Node::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->ID=atol(s);

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		subs=s;
		this->x= stod(subs);
//		this->x= atof(s);

		fL.getWord(&s);
		this->y = atof(s);

		fL.getWord(&s);
		subs=s;
		this->z = stod(subs);
//		this->z = atof(s);

		appSettings::MAX_NODE_ID = max(appSettings::MAX_NODE_ID , ID) ;
	}
	catch(exception& e)
	{ cout << " error Node::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';
	if (subs.find("\t")!=std::string::npos) { cout << " ERROR, TABS found in input files, not allowed! exit now."; system("PAUSE"); exit (0);}
	
	}

}






// restituisce 
string Node::toString()
{
	string s;
	char c[1000];
	sprintf_s(c,"Node ID: %d x: %15.14E y: %15.14E z: %15.14E \n", ID, x, y, z);
	s = c;
	return s;
}




