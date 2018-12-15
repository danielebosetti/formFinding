#pragma once


#include <iostream>
#include <string>
using namespace std;
class fileLoader;

class NdTemp
{
public:

	long loadCaseID, nodeID, boh1;
	string type;
	double temperature;

	NdTemp(void);
	~NdTemp(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

