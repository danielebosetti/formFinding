#include "fileLoader.h"

#include "PlGlobalLoad.h"


PlGlobalLoad::PlGlobalLoad(void)
{
	projectedComp="";
}


PlGlobalLoad::~PlGlobalLoad(void)
{
}



void PlGlobalLoad::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   PlGlobalLoad%18d%8d%28.14E%28.14E%28.14E   %s\n", loadCaseID, plateID, qx, qy, qz, projectedComp.c_str() );
	ofs << stringOut;
}


void PlGlobalLoad::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID=atol(s);

		fL.getWord(&s);
		this->plateID=atol(s);

		fL.getWord(&s);
		this->qx=atof(s);

		fL.getWord(&s);
		this->qy=atof(s);

		fL.getWord(&s);
		this->qz=atof(s);

		fL.getWord(&s);
		this->projectedComp=s;
	}
	catch(exception& e)
	{ cout << " error PlGlobalLoad::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}
