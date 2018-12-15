#include "StdAfx.h"
#include "BmDistLoadG.h"


BmDistLoadG::BmDistLoadG(void)
{
	this->proj="";
}


BmDistLoadG::~BmDistLoadG(void)
{
}

void BmDistLoadG::readFromFileReader(fileLoader& fL)
{
	char *s;
	string subs;
	try
	{
		fL.getWord(&s);
		subs=s;
		this->loadCase=stol(subs);

		fL.getWord(&s);
		subs=s;
		this->beam=stol(subs);
		
		fL.getWord(&s);
		subs=s;
		this->dir=subs;

		fL.getWord(&s);
		subs=s;
		this->boh=stol(subs);

		fL.getWord(&s);
		subs=s;
		this->fx=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->fy=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->fz=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->par1=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->par2=stod(subs);

		fL.getWord(&s);
		subs=s;
		this->par3=stod(subs);

		if(fL.getWord(&s))
		{
			subs=s;
			this->proj=subs;
		}
	}
	catch(exception& e)
	{ cout << " error BmDistLoadG::readFromFileLoader  <" << subs  << "> " << e.what() << '\n';}
}

string BmDistLoadG::toString()
{
	char c[1000];
	char d[1000];
	string s;
	sprintf_s(c,"BmDistLoadG loadCase: %d beam: %d dir: ", loadCase, beam);
	sprintf_s(d," boh: %d Fx: %g Fy: %g Fz: %g par1: %g par2: %g par3: %g ", boh, fx, fy, fz, par1, par2, par3);
	s=c+dir+d+proj+"\n";
	return s;
}

void BmDistLoadG::exportToStream(ostream& ofs){
	char stringOut[10000];
	sprintf_s(stringOut, "   BmDistLoadG%15d%8d      %s%8d%24.14E%24.14E%24.14E%24.14E%24.14E%24.14E \n", loadCase, beam, dir.c_str(), boh, fx, fy, fz, par1, par2, par3   );
	ofs << stringOut;
}

