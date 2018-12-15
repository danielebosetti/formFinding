#include "StdAfx.h"
#include "PlShear.h"


PlShear::PlShear(void)
{
}


PlShear::~PlShear(void)
{
}



void PlShear::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   PlShear%20d%8d%25.13E%25.13E\n", loadCaseID, plateID, Tx, Ty );
	ofs << stringOut;
}


void PlShear::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID = atol(s);

		fL.getWord(&s);
		this->plateID = atol(s);

		fL.getWord(&s);
		this->Tx = atof(s);

		fL.getWord(&s);
		this->Ty = atof(s);
	}
	catch(exception& e)
	{ cout << " error group::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}

string PlShear::toString(){
	string s;
	char c[1000];
	sprintf_s(c,"PlShear loadCaseID:  %d   plateId: %d  Tx: %g  Ty: %g \n", loadCaseID, plateID, Tx, Ty  );
	s = c;
	return s;
}
