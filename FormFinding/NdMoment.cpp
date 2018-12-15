
#include "fileLoader.h"

#include "NdMoment.h"


NdMoment::NdMoment(void)
{
}


NdMoment::~NdMoment(void)
{
}

void NdMoment::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   NdMoment%18d%8d%25.14E%25.14E%25.14E\n", loadCaseID, nodeID, Mx, My, Mz );
	ofs << stringOut;
}


void NdMoment::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID=atol(s);

		fL.getWord(&s);
		this->nodeID=atol(s);

		fL.getWord(&s);
		this->Mx=atof(s);

		fL.getWord(&s);
		this->My=atof(s);

		fL.getWord(&s);
		this->Mz=atof(s);

	}
	catch(exception& e)
	{ cout << " error group::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}

string NdMoment::toString(){
	string s;
	char c[1000];
	sprintf_s(c,"NdMoment ID: %d loadCase: %d Mx: %g My: %g Mz: %g\n", loadCaseID, nodeID, Mx, My, Mz );
	s = c;
	return s;
}





