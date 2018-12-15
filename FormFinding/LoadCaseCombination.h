#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "LoadCaseCombinationItem.h"

class fileLoader;

using namespace std;

class LoadCaseCombination
{
public:

	long ID;
	string name;
	vector<long> loadPatternID;
	vector<long> freedomCaseID;
	vector<double> loadMultiplierAlpha;

	vector<loadCaseCombinationItem> components;


	// lines to output...
	vector<char*> properties;

	LoadCaseCombination(void);
	~LoadCaseCombination(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

