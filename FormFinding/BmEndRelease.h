#pragma once
//#include "stdafx.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;
class fileLoader;


// "BmEndReleaseT", "BmEndReleaseR"
class BmEndRelease
{
public:

	char charType; //R or T

	long ID;
	long end;

	vector<string> components;
	map<string, double> partialFixities;


	BmEndRelease(void);
	~BmEndRelease(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);
};

