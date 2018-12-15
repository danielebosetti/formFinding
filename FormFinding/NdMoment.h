#pragma once


#include <iostream>
#include <string>
using namespace std;
class fileLoader;

class NdMoment
{
public:

	long loadCaseID;
	long nodeID;
	double	Mx, My, Mz;

	NdMoment(void);
	~NdMoment(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

