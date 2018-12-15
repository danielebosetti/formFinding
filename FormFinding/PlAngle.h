#pragma once

#include <iostream>
#include <string>

using namespace std;
class fileLoader;

class PlAngle
{
public:

	long ID;
	double angle;

	PlAngle(void);
	~PlAngle(void);


	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);
};

