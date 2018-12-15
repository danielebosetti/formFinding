#pragma once


#include <iostream>
#include <string>

using namespace std;
class fileLoader;


class BmPreLoad
{
public:

	long loadCaseID, beamID, boh1;
	double P0;
	string tensionStrain;

	BmPreLoad(void);
	~BmPreLoad(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);
};

