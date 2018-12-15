#include "StdAfx.h"
#include "BmDistLoadL.h"


BmDistLoadL::BmDistLoadL(void)
{
}


BmDistLoadL::~BmDistLoadL(void)
{
}

void BmDistLoadL::readFromFileLoader(fileLoader& fL){
	char *s;
	string subs;
	try{
		fL.getWord(&s);
		subs=s;
		this->IDFreedomCase=stol(subs);

		fL.getWord(&s);
		subs=s;
		this->node=stol(subs);
		
		fL.getWord(&s);
		subs=s;
		this->coordSys=stol(subs);
		
		fL.getWord(&s);
		subs=s;
		this->boh=stol(subs);
		
		fL.getWord(&s);
		subs=s;
		this->comp1=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->comp2=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->disp1=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->disp2=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->disp3=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->disp4=stod(subs);
	}
	catch(exception& e)
	{ cout << " error Beam::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}

}

string BmDistLoadL::toString(){
	char c[1000];
	string s;
	sprintf_s(c,"BmDistLoadL IDFreedomCase: %d node: %d coordSys: %d boh: %d comp1: %g comp2: %g disp1: %g disp2: %g disp3: %g disp4: %g \n", IDFreedomCase, node, coordSys, boh, comp1, comp2, disp1, disp2, disp3, disp4);
	s=c;
	return s;
}

void BmDistLoadL::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   BmDistLoadL%15d%8d%8d%8d%24.14E%24.14E%24.14E%24.14E%24.14E%24.14E\n", IDFreedomCase, node, coordSys, boh, comp1, comp2, disp1, disp2, disp3, disp4 );
	ofs << stringOut;
}
