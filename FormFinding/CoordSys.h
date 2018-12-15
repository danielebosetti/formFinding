#pragma once

#include <iostream>
#include <string>

#include <vector>
using namespace std;
class fileLoader;


using namespace std;

class CoordSys
{
public:

	long ID;
	string name, type;

	vector<char*> otherProperties;


	CoordSys(void);
	~CoordSys(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

