#pragma once


// "NdForce"

#include <iostream>
#include <string>
using namespace std;
class fileLoader;

class NdForce
{
public:

	long loadCaseID, nodeID;
	double Fx, Fy, Fz;

	NdForce(void);
	~NdForce(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);


};

