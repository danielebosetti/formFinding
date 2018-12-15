//#include "StdAfx.h"

#include "fileLoader.h"
#include "PlPatchType.h"

using namespace std;

PlPatchType::PlPatchType(void)
{
}


PlPatchType::~PlPatchType(void)
{
}



void PlPatchType::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   PlPatchType%18d%8d%8d", ID, boh1, type);
	ofs << stringOut;

	for (unsigned int i=0; i< values.size(); i++){
		sprintf_s(stringOut, "%24.14E", values[i]);
		ofs << stringOut;
	}

}


// legge i dati usando il fileLoader che gli viene passato
void PlPatchType::readFromFileLoader(fileLoader& fL){
	string subs;
	double num;
	char *s;

	try{
		fL.getWord(&s);
		this->ID=atol(s);

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		this->type= atol(s);

		// read numbers until end of line
		while (!fL.nextWordIsEmpty()){
			fL.getWord(&s);
			num = atof(s);
			values.push_back(num);
		}

	}
	catch(exception& e)
	{ cout << " error PlPatchType::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

}






// restituisce 
string PlPatchType::toString()
{
	string s;
	char c[1000];
	sprintf_s(c,"PlPatchType ID: %d boh1: %d type: %d \n", ID, boh1, type);
	s = c;

	return s;
}
