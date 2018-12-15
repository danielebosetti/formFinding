#pragma once
//#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

class fileLoader;

using namespace std;
// "CutoffProp", "BeamProp", "TrussProp"

class BeamProperty
{
public:

	string type;

	long ID, color;
	string name;
	string materialName;

	double density, area;
	double modulus;

	double stiffnessMax;
	double modulusMultiplier;

	int material_type;
	bool isRigidLink;
	bool isElastic;

	map<string, double> dMap;
	map<string, string> sMap;

	vector<char*> properties;

	BeamProperty(void);
	~BeamProperty(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

