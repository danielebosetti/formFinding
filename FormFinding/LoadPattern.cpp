#include "StdAfx.h"
#include "LoadPattern.h"


LoadPattern::LoadPattern(void)
{
	gravity=0.;
	selectedForFF = false;
	alphaCoefficient = 0.0;
	alpha_User = 0.0;
	gravity = 0.0;
}


LoadPattern::~LoadPattern(void)
{
}


void LoadPattern::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   LoadCase%18d%8d   %s\n", this->ID, this->boh1, this->name.c_str() );
	ofs << stringOut;

	if (gravity != 0.){
		sprintf_s(stringOut, "     Gravity%17d    %15.14E\n", this->numGravity, this->gravity );
		ofs << stringOut;
	}

	for (unsigned int i=0; i<this->otherProperties.size(); i++)
		ofs << otherProperties[i] << endl;

}


void LoadPattern::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->ID=atol(s);

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		this->name=s;
		
		do {
			fL.getLine(&s);

			fL.getWord(&s);

			subs=s;
			if (subs=="Gravity") {

				fL.getWord(&s);
				this->numGravity = atol(s);

				fL.getWord(&s);
				subs=s;
				this->gravity = stod(s);

			}
			else
				otherProperties.push_back(fL.lines[fL.currentLineIndex]);

//		} while(fL.lineLength()>0);
//		} while(fL.lineLength()>0);
		} while(!fL.emptyLine());

	}
	catch(exception& e)
	{ cout << " error LoadPattern::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}


}

string LoadPattern::toString(){
	string s;
	char s1[1000];
	sprintf_s(s1,1000," ID: %d name: %s gravity: %g\n", this->ID , this->name.c_str(), this->gravity);
	s = s1;
	return s;
}



