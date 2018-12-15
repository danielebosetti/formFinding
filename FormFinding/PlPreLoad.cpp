#include "StdAfx.h"
#include "PlPreLoad.h"


PlPreLoad::PlPreLoad(void)
{
	is_STRESS = false;
}


PlPreLoad::~PlPreLoad(void)
{
}


void PlPreLoad::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   PlPreLoad%18d%8d%8d%28.14E%28.14E%28.14E   %s\n",
			  loadCaseID, plateID, boh1, px, py, pz, stressStrain.c_str() );
	ofs << stringOut;
}


void PlPreLoad::readFromFileLoader(fileLoader& fL){
	string subs;
	char *s;

	try{
		fL.getWord(&s);
		this->loadCaseID=atol(s);

		fL.getWord(&s);
		this->plateID=atol(s);

		fL.getWord(&s);
		this->boh1=atol(s);

		fL.getWord(&s);
		this->px=atof(s);

		fL.getWord(&s);
		this->py=atof(s);

		if (px != py) {
			cout << "\nERROR, found PlPreLoad " << loadCaseID << "," << plateID << 
					" with px <> py ! " << px << "," << py << " continue...";
		}

		fL.getWord(&s);
		this->pz=atof(s);

		fL.getWord(&s);
		this->stressStrain=s;

		if (stressStrain == "Stress")
		{
			is_STRESS = true;
			//cout << " \n" <<loadCaseID << " " << plateID << " stress " ;
		}
	}
	catch(exception& e)
	{ cout << " error BmPreLoad::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

	
}

