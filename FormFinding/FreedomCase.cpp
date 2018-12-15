#include "StdAfx.h"
#include "FreedomCase.h"


FreedomCase::FreedomCase(void)
{
}


FreedomCase::~FreedomCase(void)
{
}

void FreedomCase::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   FreedomCase%15d%8d%8d   %s\n", this->ID, this->boh1, this->boh2, this->name.c_str() );
	ofs << stringOut;
}

void FreedomCase::readFromFileLoader(fileLoader& fL)
{
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->ID=atol(s);

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		this->boh2=atol(s);

		fL.getWord(&s);
		this->name=s;
	}
	catch(exception& e)
	{ cout << " error FreedomCase::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

}

// restituisce 
string FreedomCase::toString()
{
	string s;
	char c[1000];
	sprintf_s(c,"FreedomCase ID: %d Name: <%s> \n", ID, name.c_str() );
	s = c;
	return s;
}



