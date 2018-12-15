#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;
class fileLoader;

// "PlPatchType"

class PlPatchType
{
public:

	long ID;
	long boh1;
	long type;
	vector<double> values;

	PlPatchType(void);
	~PlPatchType(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);


};

