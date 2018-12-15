#pragma once
//#include "stdafx.h"

//using namespace Eigen;
class BeamProperty;
class Node;
#include <iostream>
#include <string>
using namespace std;
class fileLoader;

// "Beam"
class Beam
{
public:

	long ID, Gr2, IDGroup, beamPropertyID, node1, node2;

	int material_type;

	bool isRigidLink;
	bool isElastic;

	double L0;
	double LS;
	double coeffEA; // product of area and (elastic-)modulus
	double axialStiffness;

	// sum of axial tensions, sum(LP.P0 * LoadCase.alphaMultiplier under FFLCC)
	double initialAxialForce;

	// initial beam length (introduced after flattening)
	double initialLength;


	// versore da node1 a node2
	//Vector3d versor;

	// for optimization:: NODE pointers!
	Node *pNode1, *pNode2;
	// BEAM_PROPERTY pointer
	BeamProperty *pBeamProperty;

	Beam(void);
	Beam(const Beam&);
	~Beam(void);

	void readFromFileLoader(fileLoader& fL);

	string toString();
	void exportToStream(ostream& ofs);

	double getLength();
};

