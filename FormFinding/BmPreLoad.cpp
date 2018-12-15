#include "StdAfx.h"
#include "BmPreLoad.h"


BmPreLoad::BmPreLoad(void)
{
}


BmPreLoad::~BmPreLoad(void)
{
}


void BmPreLoad::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   BmPreLoad%21d%8d%8d%30.14E   %s\n", loadCaseID, beamID, boh1, P0, tensionStrain.c_str());
	ofs << stringOut;
}


void BmPreLoad::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID=atol(s);

		fL.getWord(&s);
		this->beamID=atol(s);

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		this->P0=atof(s);

		fL.getWord(&s);
		this->tensionStrain=s;
	}
	catch(exception& e)
	{ cout << " error BmPreLoad::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}


