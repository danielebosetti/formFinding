#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
class fileLoader;


// "NdFreedom"

class Restraint
{
public:


	long IDFreedomCase;
	long nodeID;
	long coordSysID;

	vector<string> components;
	map<string, double> imposedDisplacements;

	Restraint(void);
	~Restraint(void);

	Restraint(const Restraint&);


	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

