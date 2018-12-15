#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;
class fileLoader;

// "3dMembraneProp", "PatchPlateProp", "PlateShellProp"

class PlateProperty
{
public:

	string type, elastProperty;

	long ID, color;
	string name;
	string materialName;

	int material_type;

	double memThick;
	double density;

	double stiffnessMax;
	double modulusMultiplier;

	map <string, string> sMap;
	map <string, double> dMap;

	vector<char*> properties;

	PlateProperty(void);
	~PlateProperty(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

