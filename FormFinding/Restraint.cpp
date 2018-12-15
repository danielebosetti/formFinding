#include "StdAfx.h"
#include "Restraint.h"


Restraint::Restraint(void)
{
}


Restraint::~Restraint(void)
{
}

Restraint::Restraint(const Restraint &r)
{
	// what to do with sets?
	//this->components = r.components;
	this->coordSysID = r.coordSysID;
	this->IDFreedomCase = r.IDFreedomCase;
	//this->imposedDisplacements = r.imposedDisplacements
	this->nodeID = r.nodeID;
}



void Restraint::readFromFileLoader(fileLoader& fL){
	string subs, nextString;
	double num;
	char *s;

	try{
		// read first ID
		fL.getWord(&s);
		this->IDFreedomCase=atol(s);

		// read node ID
		fL.getWord(&s);
		this->nodeID=atol(s);

		// read coordSys ID
		fL.getWord(&s);
		this->coordSysID=atol(s);

		//while more words in line
		while (!fL.nextWordIsEmpty()){
			fL.getWord(&s);
			subs = s;
			// if length=2, then RX, RY, etc
			if (subs.size() == 2) {
				components.push_back(subs);
				// next word is number?
				nextString= fL.nextWord();
				if (nextString.size() > 2){
					fL.getWord(&s);
					num = atof (s);
					imposedDisplacements[subs] = num;
				}
			}
			else 
				//not empty, not legth==2 .. ???
				cout << "  warning, Restraint::readFromFileLoader , word <" << subs << "> ??";
		}

		// more on next line?
		nextString = fL.firstWordOfNextLine();
		if ( nextString=="RX" || nextString=="RY" || nextString=="RZ" ||
			 nextString=="DX" || nextString=="DY" || nextString=="DZ" ){

			// more data, go to next line
			fL.getLine(&s);

			// LOOP over the row
			while (!fL.nextWordIsEmpty()){
				fL.getWord(&s);
				subs = s;
				// if length=2, then RX, RY, etc
				if (subs.size() == 2) {
					components.push_back(subs);
					// next word is number?
					nextString= fL.nextWord();
					if (nextString.size() > 2){
						fL.getWord(&s);
						num = atof (s);
						imposedDisplacements[subs] = num;
					}
				}
				else 
					//not empty, not legth==2 .. ???
					cout << "  warning, Restraint::readFromFileLoader , word <" << subs << "> ??";
			}
		}

	}
	catch(exception& e)
	{ cout << " error group::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}



void Restraint::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   NdFreedom%17d%8d%8d", IDFreedomCase, nodeID, coordSysID );
	ofs << stringOut;

	unsigned int i;
	for(i=0;i<components.size(); i++){
		sprintf_s(stringOut, "   %s", components[i].c_str());
		ofs << stringOut;
		if (imposedDisplacements.find(components[i]) != imposedDisplacements.end()){
			sprintf_s(stringOut, "%24.14E", imposedDisplacements[components[i]] );
			ofs << stringOut;
		}
	}

	ofs << endl;

}
