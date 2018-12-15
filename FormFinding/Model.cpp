#include "StdAfx.h"
#include "Model.h"


Model::Model(void)
{
	selectedFreedomCase = NULL;
	ffc = NULL;
	loadPattern_REAZ_V = NULL;
	loadPattern_FORCE_DENSITY = NULL;
}


Model::~Model(void)
{
}



// clear all the containers
void Model::clearModel(){
	
	deleteAllMap<FreedomCase>(freedomCases);
	deleteAllMap<Node>(nodes);
	deleteAllMap<Beam>(beams);
	deleteAllMap<group>(groups);
	deleteAllMap<LoadPattern>(loadPatterns);
	deleteAllMap<BeamProperty>(beamProperties);
	deleteAllMap<PlateProperty>(plateProperties );
	deleteAllMap< BmAngle>(bmAngles);
	deleteAllMap<Plate>(plates);
	deleteAllMap<CoordSys>(coordSys);
	deleteAllMap<Stage>(stages);
	deleteAllMap<PlAngle>(plAngles);
	//deleteAllMap<>();
	deleteAllVector<NdTemp>(ndTemps);
	deleteAllVector<BmPreLoad>(bmPreLoads);
	deleteAllVector<PlPreLoad>(plPreLoads);
	deleteAllVector<PlGlobalLoad>(plGlobalLoads);
	deleteAllVector<Restraint>(restraints);
	deleteAllVector<BmEndRelease>(bmEndReleasesT);
	deleteAllVector<BmEndRelease>(bmEndReleasesR);
	deleteAllVector<PlPatchType>(plPatchTypes);
	deleteAllVector<NdMoment>(ndMoments);
	deleteAllVector<NdForce>(ndForces);
	deleteAllVector<PlShear>(plShears);
	deleteAllVector<BmDistLoadL>(bmDistLoadLs);
	deleteAllVector<BmDistLoadG>(bmDistLoadGs);
	deleteAllVector<NdForce>(REAZ_V);

	//LoadPattern *loadPattern_REAZ_V;

	//map<longPair, PlPressure*> PlPressures;
	selectedFreedomCase = NULL;
	ffc = NULL;
	loadPattern_REAZ_V = NULL;
	loadPattern_FORCE_DENSITY = NULL;

}


void Model::generateNdForce_REAZ_V(map<Node*, Vector3d>& mapREAZ_V) {
	//vector<NdForce* > REAZ_V;
	deleteAllVector<NdForce>(REAZ_V);
	REAZ_V.clear();
	NdForce *forceNew;
	long loadPattern_REAZ_V_ID = -999;


	// gets the LoadPattern/LoadCase for REAZ_V
	if (loadPattern_REAZ_V != NULL)
		loadPattern_REAZ_V_ID = loadPattern_REAZ_V->ID;

	// LOOPS over the REAZ_V forces
	for (map<Node*, Vector3d>::iterator it = mapREAZ_V.begin(); it != mapREAZ_V.end(); it++) {
		//cout << "\n generating NdForce : nodeID: "<< it->first->ID << " force: x: " << it->second.x() << " y: " << it->second.y() << " z: " << it->second.z() ;

		forceNew = new NdForce();
		forceNew->loadCaseID = loadPattern_REAZ_V_ID;
		forceNew->nodeID = it->first->ID;
		forceNew->Fx = it->second.x();
		forceNew->Fy = it->second.y();
		forceNew->Fz = it->second.z();

		REAZ_V.push_back(forceNew);

	}
}

/*
takes in input the association Beam => Q_i of the Force-Density method
creates the new BmPreLoad items to export the pre-loads
*/
void Model::generateBmPreload_FORCE_DENSITY(map<Beam*, double>& mapFORCE_DENSITY) {
	//vector<NdForce* > REAZ_V;
	deleteAllVector<BmPreLoad>(FORCE_DENSITY);
	FORCE_DENSITY.clear();
	BmPreLoad *bplNew;
	long loadPattern_FORCE_DENSITY_ID = -998;


	// gets the LoadPattern/LoadCase for FORCE_DENSITY
	if (loadPattern_FORCE_DENSITY != NULL)
		loadPattern_FORCE_DENSITY_ID = loadPattern_FORCE_DENSITY->ID;
	//else we are "crashed" here... to see

	// LOOPS over the FORCE_DENSITY density
	for (map<Beam*, double>::iterator it = mapFORCE_DENSITY.begin(); it != mapFORCE_DENSITY.end(); it++) {
		Beam *b;
		b = it->first;
		//cout << "\n generating BmPreLoad : beamID: " << b->ID << " density: " << it->second ;

		bplNew = new BmPreLoad();
		bplNew->loadCaseID = loadPattern_FORCE_DENSITY_ID;
		bplNew->beamID = b->ID;
		bplNew->boh1 = 1;
		// compute Tension as Q_i * L_i
		bplNew->P0 = b->getLength() * mapFORCE_DENSITY[b];
		bplNew->tensionStrain = "Tension";
		FORCE_DENSITY.push_back(bplNew);
	}
}




// TODO: stampare quanti elementi ci sono in ogni container (num. nodi, num. Beam, etc)
void Model::printModelSummary(){
	
	cout << "\n MODEL SUMMARY       ******************";
	cout << "\nnodes count: " << nodes.size();
	cout << "\nbeams count: " << beams.size();
	cout << "\nfreedomCases count: " << freedomCases.size();
	cout << "\ngroups count: " << groups.size();
	cout << "\n END MODEL SUMMARY       ******************\n";



}




//void Model::printComments(){
//	cout << endl <<endl << "Commented + unknown Lines:" << endl ;
//
//	long i, j;
//
//	for (i=1; i<= sectionsCount; i++) {
//		cout << endl << "Section " << i << endl;
//		for (j=sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++)
//			printf("\n %s", linesToCopy[j]);
//	}
//}



void Model::exportSectionToStream(int i, ostream& ofs){
	long j;
	for (j=sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++){
		ofs << linesToCopy[j] << "\n";
	}
}


void Model::writeCommentsToFiles(string basePathOut){
	ofstream ofs;
	string baseFileName = basePathOut + "\\x_section";
//	string lineOut;
	char fileName[1000];

	
	long i, j;

	cout << "\n exporting " << sectionsCount << " sections \n";

	for (i=1; i<= sectionsCount; i++) {
		
		sprintf_s(fileName, "%s%02d.txt", baseFileName.c_str(), i); 
		cout << fileName << endl;
		ofs.open(fileName, ofstream::out);


		for (j=sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++){
			ofs << linesToCopy[j] << "\n";
			//printf("\n %s", linesToCopy[j]);
		}

		ofs.close();

	}	
}


