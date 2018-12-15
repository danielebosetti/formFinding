
#include "Beam.h"
#include "Node.h"
#include "PlateProperty.h"
#include "SharedVariables.h"
#include "fileLoader.h"
#include "appSettings.h"

#include <Eigen\Dense>
/*
BEAM ROW:
Beam
ID
Gr2
IdGroup
BeamPropertyID
Node1
Node2
*/


Beam::Beam(void)
{
	material_type = MATERIAL_UNKNOWN;
	// init tension to 0
	initialAxialForce = 0.0;
	initialLength = 0.0;

	pNode1 = NULL;
	pNode2 = NULL;
	isRigidLink = false;
	isElastic = false;
	pBeamProperty = NULL;
}

Beam::Beam(const Beam &b)
{
	this->axialStiffness = b.axialStiffness;
	this->beamPropertyID = b.beamPropertyID;
	this->coeffEA = b.coeffEA;
	this->Gr2 = b.Gr2;
	this->ID = b.ID;
	this->IDGroup = b.IDGroup;
	this->initialAxialForce = b.initialAxialForce;
	this->initialLength = b.initialLength;
	this->isElastic = b.isElastic;
	this->isRigidLink = b.isRigidLink;
	this->L0 = b.L0;
	this->LS = b.LS;
	this->material_type = b.material_type;
	this->node1 = b.node1;
	this->node2 = b.node2;

	this->pBeamProperty = NULL;
	this->pNode1 = NULL;
	this->pNode2 = NULL;

}


Beam::~Beam(void)
{
}


void Beam::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   Beam%22d%8d%8d%8d%8d%8d\n", ID, Gr2, IDGroup, beamPropertyID, node1, node2);
	ofs << stringOut;
}

double Beam::getLength()
{
	using namespace Eigen;
	if (pNode1 == NULL || pNode2 == NULL) {
		std::cout << "\nUnable to compute length, NULL pointers!"; return 0.; }
	if (pNode1->position == NULL || pNode2->position == NULL) { 
		std::cout << "\nUnable to compute length, NULL position pointers!"; return 0.; }
	Vector3d l = *this->pNode1->position - *this->pNode2->position;

	return l.norm();
}


void Beam::readFromFileLoader(fileLoader& fL)
{
	string subs;
	char *s;

	try{

		fL.getWord(&s);
		subs=s;
		this->ID=stol(subs);

		fL.getWord(&s);
		subs=s;
		this->Gr2= stol(subs);


		fL.getWord(&s);
		subs=s;
		this->IDGroup= stol(subs);


		fL.getWord(&s);
		subs=s;
		this->beamPropertyID = stol(subs) ;


		fL.getWord(&s);
		subs=s;
		this->node1 = stol(subs);

		
		fL.getWord(&s);
		subs=s;
		this->node2 = stol(subs);

		appSettings::MAX_NODE_ID = max(appSettings::MAX_NODE_ID , ID) ;
	}
	catch(exception& e)
	{ cout << " error Beam::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

}


string Beam::toString(void)
{
	string s;
	char c[1000];
	sprintf_s(c,"Beam ID: %d IDGroup: %d ID_beamProperty: %d node1: %d node2: %d \n", ID, IDGroup, beamPropertyID, node1, node2);
	s = c;
	return s;
}



