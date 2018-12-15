//#include "StdAfx.h"
#pragma warning (disable:4244)

#include "appSettings.h"
#include "INI_io.h"


#include <fstream>
#include <iostream>

using namespace std;

appSettings::appSettings(void)
{
}


appSettings::~appSettings(void)
{
}



char funDeCrit(double d, int i){
	return (d - i*2)/(i+2)-98;
}
const long N_CHARS = 1000;
double  nameDeCrit [N_CHARS];
char namePlainRead[N_CHARS];


void appSettings::checkKey(){
	int i;
	appSettings::LICENSE_CHECK_OK = false;
	
	FILE * pFileRead;
	fopen_s (&pFileRead, (appSettings::settingsDir + "prog.dat").c_str(),"r");
	if (pFileRead!=NULL) {
		fread(nameDeCrit,sizeof(int) , N_CHARS,pFileRead);
	}

	for (i=0; i<N_CHARS; i++) {
		namePlainRead[i] = funDeCrit(nameDeCrit[i], i);
	}

	ifstream ifs;
	ifs.open(namePlainRead, ifstream::in);
	if (ifs.is_open()) {
		char buf[1001];
		string keyRead;
		ifs.getline(buf,1000);
		keyRead=buf;
		if (keyRead == "abc123") {
			appSettings::LICENSE_CHECK_OK = true;
		}
	}

	if (appSettings::LICENSE_CHECK_OK )
		cout << "..;:FormFinding::..\n\n";
	else
		cout << "..::FormFinding::..\n\n";



}

void exitWithMessage(string msg);

void appSettings::initAppSettings() {

	ifstream ifs;
	ifs.open((appSettings::settingsDir + "settings.ini").c_str(), ifstream::in);
	if (!ifs.is_open())
		exitWithMessage(" FILE " + appSettings::settingsDir + "settings.ini" + " not found! exit now.");
	ifs.close();



}


void appSettings::initAppSettings_for_cmd() {

	//appSettings::exePath = appSettings::applicationDir;

	ifstream ifs;
	ifs.open((appSettings::settingsDir + "settings.ini").c_str(), ifstream::in);
	if (!ifs.is_open())
		exitWithMessage(" FILE [" + appSettings::settingsDir + "settings.ini] not found! exit now.");
	ifs.close();



}


/*
reads the parameters from the external settings.ini file
*/
void appSettings::readParamFromIni() {
	INI_io::getFromIni("MAX_LINES","2300000", appSettings::MAX_LINES);
	
	INI_io::getFromIni("MAX_WORDS_PER_LINE", "14", appSettings::MAX_WORDS_PER_LINE);
	INI_io::getFromIni("MAX_NODE_ID", "10000", appSettings::MAX_NODE_ID);
	INI_io::getFromIni("RENDERSTEPS", "1", appSettings::RENDERSTEPS) ;
	INI_io::getFromIni("RENDER_ITERATION_STEPS", "1", appSettings::RENDER_ITERATION_STEPS);
	INI_io::getFromIni("TIME_STEP", "0.0025", appSettings::TIME_STEP);
//	INI_io::getFromIni("MAX_POS_STEP", "10.", appSettings::MAX_POS_STEP);
	INI_io::getFromIni("ALPHA_MASS", "1.", appSettings::ALPHA_MASS);
	INI_io::getFromIni("COEFF_VISC", "0.1", appSettings::COEFF_VISC);


	INI_io::getFromIni("SMORZAMENTO_K_GLOBAL", "OFF", appSettings::SMORZAMENTO_K_GLOBAL);
	INI_io::getFromIni("USE_C_CONSTANT", "OFF", appSettings::USE_C_CONSTANT);

	INI_io::getFromIni("ENABLE_KINETIC_DAMPING", "ON", appSettings::ENABLE_KINETIC_DAMPING);


	INI_io::getFromIni("C_DEFAULT_VALUE", "1.", appSettings::C_DEFAULT_VALUE);
	INI_io::getFromIni("XI", "1.", appSettings::XI);
	INI_io::getFromIni("N_RECALC_BEAMPLATES", "10000", appSettings::N_RECALC_BEAMPLATES);
	INI_io::getFromIni("FORCE_SKIP_MULTIPLIER", "0.01", appSettings::FORCE_SKIP_MULTIPLIER);
	INI_io::getFromIni("SKIP_N_STEP", "20", appSettings::SKIP_N_STEP);

	INI_io::getFromIni("GREEN_LINES_MULTIPLIER", "1.0", appSettings::GREEN_LINES_MULTIPLIER);
	INI_io::getFromIni("FORCE_DENSITY_TARGET", "1", appSettings::FORCE_DENSITY_TARGET);

	INI_io::getFromIni("LAMBDA", "0.999", appSettings::LAMBDA);
	INI_io::getFromIni("N_ITER", "10", appSettings::N_ITER);


}














bool appSettings::LICENSE_CHECK_OK;

long appSettings::MAX_LINES;
long appSettings::MAX_WORDS_PER_LINE;
long appSettings::MAX_NODE_ID;
long appSettings::RENDERSTEPS;
long appSettings::RENDER_ITERATION_STEPS;

double appSettings::TIME_STEP;
//double appSettings::MAX_POS_STEP;
double appSettings::ALPHA_MASS;
double appSettings::COEFF_VISC;


bool appSettings::SMORZAMENTO_K_GLOBAL;
bool appSettings::USE_C_CONSTANT;
double appSettings::C_DEFAULT_VALUE;
double appSettings::XI;
long appSettings::N_RECALC_BEAMPLATES;
double appSettings::FORCE_SKIP_MULTIPLIER;
long appSettings::SKIP_N_STEP;

bool appSettings::ENABLE_KINETIC_DAMPING;
double appSettings::GREEN_LINES_MULTIPLIER;

long appSettings::FORCE_DENSITY_TARGET;

double appSettings::LAMBDA = 0.99;
long appSettings::N_ITER = 10;





string appSettings::fileName;
string appSettings::workingFilePath;
string appSettings::originalFilePath;
string appSettings::outPath;
char appSettings::ownPth[MAX_PATH];

string appSettings::applicationDir;
string appSettings::settingsDir;
string appSettings::dataDir;
string appSettings::fileInput;
string appSettings::fileOutput;
string appSettings::method;
string appSettings::nIterations;

