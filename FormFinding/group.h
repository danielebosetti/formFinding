#pragma once

#include <iostream>
#include <string>
using namespace std;
class fileLoader;


// "Group"

class group
{
public:

	long ID, color;
	string name;

	group(void);
	~group(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

