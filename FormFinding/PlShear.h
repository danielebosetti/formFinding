#pragma once
#include <iostream>
#include <string>

using namespace std;
class fileLoader;


// "PlShear"

class PlShear
{
public:

	long loadCaseID, plateID;
	double Tx, Ty;


	PlShear(void);
	~PlShear(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);


};

