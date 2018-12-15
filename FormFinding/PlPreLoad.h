#pragma once

#include <iostream>
#include <string>

using namespace std;
class fileLoader;

class PlPreLoad
{
public:

	long loadCaseID, plateID, boh1;

	double px, py, pz;
	string stressStrain;
	bool is_STRESS;

	PlPreLoad(void);
	~PlPreLoad(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

