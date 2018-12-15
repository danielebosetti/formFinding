#include "fileLoader.h"

#include "PlAngle.h"


PlAngle::PlAngle(void)
{
}


PlAngle::~PlAngle(void)
{
}



void PlAngle::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   PlAngle%19d%25.14E\n", ID, angle);
	ofs << stringOut;
}


void PlAngle::readFromFileLoader(fileLoader& fL){
	char* s;
	string subs;
		
	try{
		fL.getWord(&s);
		subs=s;
		this->ID= stol(subs);

		fL.getWord(&s);
		this->angle= atof(s);
	}
	catch(exception& e)
	{ cout << " error Stage::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

}

string PlAngle::toString()
{
	string s;
	char c[1000];
	sprintf_s(c, "PlAngle ID: %d  angle %g \n", ID, angle);
	s = c;
	return s;
}
