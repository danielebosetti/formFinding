#pragma once
//#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

class fileLoader;

using namespace std;
// "BmAngle"
class BmAngle
{
public:

	long ID;
	double angle;

	BmAngle(void);
	~BmAngle(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

