#pragma once
#include <iostream>
#include <string>

using namespace std;
class fileLoader;


// "PlPressure"

class PlPressure
{
public:

	long loadPatternID, plateID;
	double plPressureValue;

	PlPressure(void);
	~PlPressure(void);

	void readFromFileLoader(fileLoader& fL);
	string toString();

	void exportToStream(ostream& ofs);
};

