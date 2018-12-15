//#include "StdAfx.h"
#include "ModelExporter.h"
#include "SharedFunctions.h"
#include "ModelItemsDeclare.h"
#include <boost/foreach.hpp>


ModelExporter::ModelExporter(void)
{
	workingModel = NULL;
	originalModel = NULL;
}

ModelExporter::~ModelExporter(void)
{
}


// copy & paste from  exportToSingleFile except
// > new forces are added
// > tela is the result of form-finding
void ModelExporter::exportToSingleFile_ComposeFinal(string fileName){
	ofstream ofs;
	ofs.open(fileName, ofstream::out);
	if (!ofs.is_open()) {
		cout << "\n\nError, unable to open file [" << fileName << "] , exit now..";
		exit(0);
	}
	
	originalModel->exportSectionToStream(1, ofs);

	writeAllMapToStream<group>(originalModel->groups,  ofs );
	ofs << endl;

	writeAllMapToStream<FreedomCase>(originalModel->freedomCases,  ofs );
	ofs << endl;

	writeAllMapToStream<LoadPattern>(originalModel->loadPatterns,  ofs );
	ofs << endl;

//====================================================================================================================
//====================       DISABLED              ===================================================================
//====================================================================================================================
	// export FORM FINDING LOAD CASE COMBINATION
	ofs << "/ FORM FINDING LOAD CASE COMBINATION" << endl;
	// gets the FF LCC
	LoadCaseCombination *ffc = originalModel->ffc;
	char stringOut[10000];
	// writes first row
	sprintf_s(stringOut, "   LoadCaseCombination%8d   %s\n", ffc->ID, ffc->name.c_str() );
	ofs << stringOut;
	// writes LoadCases _ User Alpha _ rows
	BOOST_FOREACH(mapElement_LoadPattern lp1, workingModel->loadPatterns) {
		LoadPattern *lp = lp1.second;
		//if (lp->alpha_User > 0.000000001) 
		{
			sprintf_s(stringOut, "      %3d  %3d  %15.13g\n", lp->ID, ffc->freedomCaseID[0], lp->alpha_User);
			ofs << stringOut;			
		}
	}
	// export LOAD CASE COMBINATIONS except FFC
	ofs << "\n\n/ LOAD CASE COMBINATIONS except FFC" << endl;
	BOOST_FOREACH(LoadCaseCombination *lcc, originalModel->loadCaseCombinations) {
		if (lcc != originalModel->ffc)
			lcc->exportToStream(ofs);
	}
//====================================================================================================================
//====================      END  DISABLED          ===================================================================
//====================================================================================================================
	//// export FORM FINDING LOAD CASE COMBINATION
	//writeAllVectorToStream<LoadCaseCombination>(originalModel->loadCaseCombinations,  ofs );


	ofs << endl;

	writeAllMapToStream<CoordSys>(originalModel->coordSys,  ofs );
	ofs << endl;

	writeAllMapToStream<Stage>(originalModel->stages,  ofs );
	ofs << endl;

	// export NON-NODE_TC_L from ORIGINAL MODEL
	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/ ALL NON-TC_L NODES" << endl;
	for (map<long, Node*>::iterator it=originalModel->nodes.begin(); it!=originalModel->nodes.end(); ++it)
		if(it->second)
			if (!is_NODE_TC_L(it->second) && !is_NODE_RL(it->second) )
				it->second->exportToStream(ofs);

	ofs << endl;

	// export NODE_TC_L from WORKING MODEL
	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/ NODE_TC_L modified" << endl;
//	for (map<long, Node*>::iterator it=model->nodes.begin(); it!=model->nodes.end(); ++it)
	BOOST_FOREACH (mapElement_Node n1, workingModel->nodes) {
		Node *n = n1.second;
		if(n)
			if (is_NODE_TC_L(n) || is_NODE_RL(n))
				n->exportToStream(ofs);
	}


	//writeAllMapToStream<Node>(originalModel->nodes,  ofs );
	//ofs << endl;
	
	ofs << endl;

	writeAllMapToStream<Beam>(originalModel->beams, ofs );
	ofs << endl;

	writeAllMapToStream<BmAngle>(originalModel->bmAngles, ofs);
	ofs << endl;

	writeAllVectorToStream<BmEndRelease>(originalModel->bmEndReleasesT, ofs);
	ofs << endl;
	writeAllVectorToStream<BmEndRelease>(originalModel->bmEndReleasesR, ofs);
	ofs << endl;
	writeAllMapToStream<Plate>(originalModel->plates,  ofs );
	ofs << endl;

	writeAllMapToStream<PlAngle>(originalModel->plAngles,  ofs );
	ofs << endl;
	writeAllVectorToStream<PlPatchType>(originalModel->plPatchTypes,  ofs );
	ofs << endl;

	writeAllVectorToStream<Restraint>(originalModel->restraints,  ofs );
	ofs << endl;

	writeAllVectorToStream<BmDistLoadL>(originalModel->bmDistLoadLs, ofs);
	ofs << endl;

	//heref
	//writeAllVectorToStream<NdForce>(originalModel->ndForces,  ofs );
//	void writeAllVectorToStream(vector<T*>& v, ostream& os){		
	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  all FORCES EXCEPT REAZ_V" << endl;
	{
		// export all FORCES EXCEPT the REAZ_V forces!
		long reaz_v_ID = originalModel->loadPattern_REAZ_V->ID;
		for (unsigned long i=0; i<originalModel->ndForces.size(); i++)
			if (originalModel->ndForces[i])
				if (originalModel->ndForces[i]->loadCaseID != reaz_v_ID )
					originalModel->ndForces[i]->exportToStream(ofs);
		//for (unsigned long i=0; i<v.size(); i++)
		//	if (v[i]) v[i]->exportToStream(os);
	}

	ofs << endl;

	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  NdMoment" << endl;
	writeAllVectorToStream<NdMoment>(originalModel->ndMoments,  ofs );
	ofs << endl;

	writeAllVectorToStream<BmDistLoadG>(originalModel->bmDistLoadGs, ofs);
	ofs << endl;
	writeAllVectorToStream<PlShear>(originalModel->plShears,  ofs );
	ofs << endl;


	//writeAllVectorToStream<BmPreLoad>(originalModel->bmPreLoads, ofs);

	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  BmPreLoads for NON-ELASTIC beams" << endl;

	BOOST_FOREACH (BmPreLoad *bpl, workingModel->bmPreLoads) {
		Beam *b = workingModel->beams[bpl->beamID];
		if (!is_ELEM_TC_CE(b))
			bpl->exportToStream(ofs);
	}

	ofs << endl;


	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  BmPreLoads for ELASTIC beams EXCLUDED from FFC:" << endl;
	BOOST_FOREACH (BmPreLoad *bpl, workingModel->bmPreLoads) {
		Beam *b = workingModel->beams[bpl->beamID];
		LoadPattern *lp = workingModel->loadPatterns[bpl->loadCaseID];
		//if (is_ELEM_TC_CE(b) && !lp->selectedForFF)
		if (is_ELEM_TC_CE(b) && (abs(lp->alpha_User) <1.e-12) )
			bpl->exportToStream(ofs);
	}

	ofs << endl;


	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  BmPreLoads for ELASTIC beams INCLUDED in FFC" << endl;


	BOOST_FOREACH (BmPreLoad *bpl, workingModel->bmPreLoads) {
		Beam *b = workingModel->beams[bpl->beamID];
		LoadPattern *lp = workingModel->loadPatterns[bpl->loadCaseID];
		//if (is_ELEM_TC_CE(b) && lp->selectedForFF) {
		if (is_ELEM_TC_CE(b) && (abs(lp->alpha_User)>=1.e-12) ) {

			Vector3d versor;
			Node *node1, *node2;
			//cout << "\n elastic beam: " ;
			node1 = b->pNode1;
			node2 = b->pNode2;

			versor =  *(node2->position) - *(node1->position);
			double L_dist = versor.norm(); 
			//double forceModulus = (L_dist - b->L0) * b->coeffEA / b->LS ; // azione assiale finale
			double forceModulus = (L_dist - b->L0) * b->axialStiffness ; // azione assiale finale
			double gamma = forceModulus / b->initialAxialForce; // scaling factor

			double oldP0 = bpl->P0;
			
			bpl->P0 = bpl->P0 * gamma;
			bpl->exportToStream(ofs);
			bpl->P0 = oldP0;

		}
	}

	ofs << endl;




	ofs << endl;
	writeAllVectorToStream<PlPreLoad>(originalModel->plPreLoads,  ofs );
	ofs << endl;
	writeAllMapToStream2<longPair, PlPressure>(originalModel->PlPressures,  ofs );
	ofs << endl;
	writeAllVectorToStream<PlGlobalLoad>(originalModel->plGlobalLoads,  ofs );
	ofs << endl;

	writeAllVectorToStream<NdTemp>(originalModel->ndTemps,  ofs );
	ofs << endl;

	writeAllMapToStream<BeamProperty >(originalModel->beamProperties, ofs);
	ofs << endl;

	writeAllMapToStream<PlateProperty>(originalModel->plateProperties,  ofs );
	ofs << endl;


	// HERE:: export REAZ_V from the WORKING model
//	writeAllVectorToStream<NdForce>(model->REAZ_V,  ofs );
	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  NdFORCES: REAZ_V" << endl;
	writeAllVectorToStream<NdForce>(workingModel->REAZ_V,  ofs );
	ofs << endl;


	// exports the BmPreLoad-s created by the FORCE_DENSITY
	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/  BmPreLoad: FORCE_DENSITY" << endl;
	long loadCaseID = this->workingModel->loadPattern_FORCE_DENSITY->ID;
	writeAllVectorToStream<BmPreLoad>(workingModel->FORCE_DENSITY, ofs);

	ofs << endl;


	// export all the "unknown" sections
	for(int i=2; i<originalModel->sectionsCount; i++)
		originalModel->exportSectionToStream(i, ofs);


	originalModel->exportSectionToStream(originalModel->sectionsCount, ofs);



	ofs.close();

}






