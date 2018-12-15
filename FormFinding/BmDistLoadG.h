#pragma once
//#include "stdafx.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

class fileLoader;

using namespace std;
// "BmDistLoadG"
class BmDistLoadG
{
public:

	long loadCase, beam;
	string dir;
	long boh;
	double fx, fy, fz, par1, par2, par3;
	string proj;

	BmDistLoadG(void);
	~BmDistLoadG(void);

	void readFromFileReader(fileLoader& fL);	
	string toString();

	void exportToStream(ostream& ofs);
};

