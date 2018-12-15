#include "StdAfx.h"
#include "Stage.h"


Stage::Stage(void)
{
}


Stage::~Stage(void)
{
}


void Stage::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   Stage%19d   %s   %s   %s\n", ID, name.c_str(), setM.c_str(), setRC.c_str());
	ofs << stringOut;

	unsigned long i;
	for(i=0; i< properties.size(); i++){
		sprintf_s(stringOut, "%s\n", properties[i] );
		ofs << stringOut;
	}

}


void Stage::readFromFileLoader(fileLoader& fL){
	char* s;
	string subs;
		
	try{
		fL.getWord(&s);
		subs=s;
		this->ID= stol(subs);

		fL.getWord(&s);
		this->name= s;

		fL.getWord(&s);
		this->setM= s;

		fL.getWord(&s);
		this->setRC = s;

		while (is_number(fL.firstWordOfNextLine())) {
			fL.getLine(&s);
			// salva la line
			properties.push_back(s);
		}
	}
	catch(exception& e)
	{ cout << " error Stage::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

}

string Stage::toString()
{
	string s;
	char c[1000];
	sprintf_s(c, "Stage ID: %d  name: %s setM: %s setRC: %s\n", ID, name.c_str(), setM.c_str(), setRC.c_str());
	s = c;
	return s;
}