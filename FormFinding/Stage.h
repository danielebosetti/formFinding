#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;
class fileLoader;

class Stage
{
public:

	long ID;
	string name;
	string setM, setRC;

	vector<char*> properties;

	Stage(void);
	~Stage(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);

};

