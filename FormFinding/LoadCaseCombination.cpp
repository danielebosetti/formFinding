#include "StdAfx.h"
#include "LoadCaseCombination.h"


LoadCaseCombination::LoadCaseCombination(void)
{
}


LoadCaseCombination::~LoadCaseCombination(void)
{
}

void LoadCaseCombination::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;
	loadCaseCombinationItem item;

	try{
		
		fL.getWord(&s);
		ID=atol(s);

		fL.getWord(&s);
		name=s;
		
		if (name=="") {
			cout << "\n ERROR LoadCaseCombination::readFromFileLoader : empty name? \n";
			system("PAUSE");
		}

		do {
			fL.getLine(&s);
			if (*s==0)
				break;

			// salva la line
			if((*s)!=0)
				properties.push_back(s);

			fL.getWord(&s);
			loadPatternID.push_back(atol(s));
			item.loadPatternID = atol(s);

			fL.getWord(&s);
			freedomCaseID.push_back(atol(s));
			item.freedomCaseID = atol(s);

			fL.getWord(&s);
			loadMultiplierAlpha.push_back(atof(s));
			item.loadMultiplierAlpha = atof(s);

			components.push_back(item);

		} while(!fL.emptyLine());

	}
	catch(exception& e)
	{ cout << " error LoadCaseCombination::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}
	

}

string LoadCaseCombination::toString(){
	string s = " NOT IMPLEMENTED YET ";
	return s;
}

void LoadCaseCombination::exportToStream(ostream& ofs) {
	char stringOut[10000];
	sprintf_s(stringOut, "   LoadCaseCombination%8d   %s\n", ID, name.c_str() );
	ofs << stringOut;

	unsigned long i;
	for(i=0; i< properties.size(); i++){
		sprintf_s(stringOut, "%s\n", properties[i] );
		ofs << stringOut;
	}

	// NOTE: export using the map is disabled, VECTOR is used instead.
	//for (map<string,string>::iterator it=sMap.begin(); it!=sMap.end(); ++it)
	//	if (it->second != ""){
	//		sprintf_s(stringOut, "     %-22s%s\n", it->first.c_str(), it->second.c_str() );
	//		ofs << stringOut;
	//	}

	ofs << endl;
}
