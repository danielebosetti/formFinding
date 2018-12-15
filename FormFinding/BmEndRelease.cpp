#include "StdAfx.h"
#include "BmEndRelease.h"


BmEndRelease::BmEndRelease(void)
{
}


BmEndRelease::~BmEndRelease(void)
{
}




void BmEndRelease::readFromFileLoader(fileLoader& fL){
	string subs, nextString;
	double num;
	char *s;

	try{
		// read first ID
		fL.getWord(&s);
		this->ID=atol(s);

		// read end
		fL.getWord(&s);
		this->end=atol(s);

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
					partialFixities[subs] = num;
				}
			}
			else 
				//not empty, not legth==2 .. ???
				cout << "  warning, BmEndRelease::readFromFileLoader , word <" << subs << "> ??";
		}

		// more on next line?
		nextString = fL.firstWordOfNextLine();
		if ( nextString=="RX" || nextString=="RY" || nextString=="RZ" ||
			 nextString=="DX" || nextString=="DY" || nextString=="DZ" ||
			 nextString=="TX" || nextString=="TY" || nextString=="TZ" ){

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
						partialFixities[subs] = num;
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



void BmEndRelease::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   BmEndRelease%c%17d%8d", charType, ID, end );
	ofs << stringOut;

	unsigned int i;
	for(i=0;i<components.size(); i++){
		sprintf_s(stringOut, "   %s", components[i].c_str());
		ofs << stringOut;
		if (partialFixities.find(components[i]) != partialFixities.end()){
			sprintf_s(stringOut, "%24.14E", partialFixities[components[i]] );
			ofs << stringOut;
		}
	}

	ofs << endl;

}
