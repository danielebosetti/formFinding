#pragma once
//#include "stdafx.h"

#include <iostream>
#include <string>

using namespace std;
class fileLoader;


// "FreedomCase"

class FreedomCase
{
public:

	long ID, boh1, boh2;
	string name;

	FreedomCase(void);
	~FreedomCase(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

