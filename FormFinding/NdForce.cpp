#include "fileLoader.h"

#include "NdForce.h"


NdForce::NdForce(void)
{
}


NdForce::~NdForce(void)
{
}

void NdForce::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   NdForce%21d%8d%25.14E%25.14E%25.14E\n", loadCaseID, nodeID, Fx, Fy, Fz);
	ofs << stringOut;
}


void NdForce::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID=atol(s);

		fL.getWord(&s);
		this->nodeID=atol(s);

		fL.getWord(&s);
		this->Fx=atof(s);

		fL.getWord(&s);
		this->Fy=atof(s);

		fL.getWord(&s);
		this->Fz=atof(s);

	}
	catch(exception& e)
	{ cout << " error group::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}

string NdForce::toString(){
	string s;
	char c[1000];
	sprintf_s(c,"NdForce ID: %d nodeID: %d  Fx %g Fy %g Fz %g \n", loadCaseID, nodeID, Fx, Fz, Fz  );
	s = c;
	return s;
}


