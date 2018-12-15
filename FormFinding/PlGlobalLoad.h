#pragma once
#include <iostream>
#include <string>

using namespace std;
class fileLoader;

// "PlGlobalLoad"

class PlGlobalLoad
{
public:

	long loadCaseID, plateID;
	double qx, qy, qz;
	string projectedComp;

	PlGlobalLoad(void);
	~PlGlobalLoad(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);


};

