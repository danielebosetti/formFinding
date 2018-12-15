#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include <Eigen/Dense>

using namespace std;
class fileLoader;

//using namespace Eigen;

	

class Beam;
class Plate;

// "Node"
class Node
{
public:
	Node(void);
	~Node(void);

	Node(const Node&);


	long ID, boh1;
	double x,y,z;
	Eigen::Vector3d *position;

	bool is_TC, is_STEEL, has_Restraint, isRigidLink;

	double totalStiffness, massNode, coeff1, coeff2, coeff3;
	
	long skipCompute;
	bool enabled;
	bool forceIsComputed;

	// incident Beams
	vector<Beam*> incidentBeams;
	// incident Plates
	vector<Plate*> incidentPlates;

	// incident Nodes
	set<Node*> adjacentNodes;

	// mark adjacentNodes as >> to recompute force 
	//void toRecomputeForceOnAdjacentNodes();

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

	void init_position();

};

