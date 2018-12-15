
#include "NdTemp.h"
#include "fileLoader.h"


NdTemp::NdTemp(void)
{
}


NdTemp::~NdTemp(void)
{
}


void NdTemp::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   NdTemp%21d%8d      %s%8d%30.14E\n", loadCaseID, nodeID, type.c_str(), boh1, temperature  );
	ofs << stringOut;
}


void NdTemp::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID=atol(s);

		fL.getWord(&s);
		this->nodeID=atol(s);

		fL.getWord(&s);
		this->type=s;

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		this->temperature=atof(s);
	
	}
	catch(exception& e)
	{ cout << " error group::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}

