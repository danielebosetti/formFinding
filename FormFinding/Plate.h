#pragma once
#include <iostream>
#include <string>

using namespace std;
class fileLoader;
class Node;
class PlateProperty;


// "Quad4", "Tri3"

class Plate
{
public:

	long ID;
	string type;
	long boh, Gr2, group, Property, node1, node2, node3, node4;

	int material_type;

	// sum of stresses, sum(MemThick * LoadCase.alphaMultiplier * px under FFLCC)
	double sumStresses;
	// sum of pressures, sum(PlPressure * LoadCase.alphaMultiplier under FFLCC)
	double sumPressures;
	// weight multiplier: density*thickness
	double weightMultiplier;
	


	// for optimization:: NODE pointers!
	Node *pNode1, *pNode2, *pNode3, *pNode4;
	// PLATE_PROPERTY pointer
	PlateProperty *pPlateProperty;

	Plate(void);
	~Plate(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();
	
	void exportToStream(ostream& ofs);

};

