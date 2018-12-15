#pragma once
//#include "stdafx.h"
//using namespace std;

#include <iostream>
#include <string>
#include <map>
#include <Eigen\Dense>
using namespace std;
class fileLoader;
class RigidGroup;
#include "ModelItemsForwardDeclare.h"
#include "MapTypesDef.h"

class Model
{
public:
	string modelNAME;

	map<long, FreedomCase*> freedomCases;
	// si usa UN SOLO freedom case!
	FreedomCase *selectedFreedomCase;

	map<long, Node*> nodes;
	map<long, Beam*> beams;
	map<long, Plate*> plates;

	map<long, group*> groups;
	map<long, LoadPattern*> loadPatterns;

	// le reazioni vincolari vanno associate a questo LoadPattern >> loadPattern_REAZ_V (LoadCase nel file di output)
	LoadPattern *loadPattern_REAZ_V;
	vector<NdForce* > REAZ_V;
	void generateNdForce_REAZ_V(map<Node*, Eigen::Vector3d>& mapREAZ_V);

	// force-density load pattern
	LoadPattern *loadPattern_FORCE_DENSITY;
	vector<BmPreLoad*> FORCE_DENSITY;
	void generateBmPreload_FORCE_DENSITY(map<Beam*, double>& mapFORCE_DENSITY);


	map<long, BeamProperty*> beamProperties;
	map<long, PlateProperty*> plateProperties;
	map<long, BmAngle*> bmAngles;
	map<long, CoordSys*> coordSys;
	map<long, Stage*> stages;
	map<long, PlAngle*> plAngles;
	
	map<longPair, PlPressure*> PlPressures;
	
	vector<Restraint*> restraints;
	vector<BmEndRelease*> bmEndReleasesT;
	vector<BmEndRelease*> bmEndReleasesR;
	vector<PlPatchType*> plPatchTypes;
	vector<NdMoment*> ndMoments;
	vector<NdForce*> ndForces;
	vector<PlShear*> plShears;

	vector<NdTemp*> ndTemps;
	vector<BmPreLoad*> bmPreLoads;
	vector<PlPreLoad*> plPreLoads;
	vector<PlGlobalLoad*> plGlobalLoads;

	//map<longPair, BmDistLoadL*> bmDistLoadLs;
	//map<bmDistLoadGkey, BmDistLoadG*> bmDistLoadGs;
	vector<BmDistLoadL*> bmDistLoadLs;
	vector<BmDistLoadG*> bmDistLoadGs;

	vector<LoadCaseCombination*> loadCaseCombinations;
	// pointer to the form-finding Combination
	LoadCaseCombination * ffc;


	vector<RigidGroup*> rigidGroups;

	//TODO aggiungere tutte le map per contenere i dati

	//COPIA PARAMETRI MODEL
	// mettiamo qui tutte le LINES da copiare "pari-pari"
	char **linesToCopy;
	// sectionFirstLine[i] è l'indice in linesToCopy delle prima linea da copiare uguale
	long *sectionFirstLine;
	// sectionLinesCount[i] è il numero di linee che compongono la sezione i-esima
	long *sectionLinesCount;
	long sectionsCount;


	void exportSectionToStream(int i, ostream& ofs);

	Model(void);
	~Model(void);

	void clearModel();


	void printModelSummary();
	//void printComments();
	void writeCommentsToFiles(string basePathOut);

};

