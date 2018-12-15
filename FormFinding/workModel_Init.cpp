#include "StdAfx.h"
#include "WorkModel.h"
#include "BoolFunctions.h"
#include <algorithm>
//#include <omp.h>
#pragma warning (disable:4018)



double WorkModel::getDeltaT(){
	return deltaT;
}

void WorkModel::clearWorkModel(void){

	delete[] posI;
	delete[] posIm1;
	delete[] posIm2;
	delete[] fI;
	delete[] vI;
	delete[] vIm1;

	ffds.clearFormFindingDataSet();
}

void WorkModel::updateMaxNodeId() {
	maxNodeId=-1;
	BOOST_FOREACH (mapElement_Node b1 , model->nodes ) {
		Node *n = b1.second;
		maxNodeId = max(maxNodeId, n->ID);
	}
}



// updates NODE coordinates from POSITION
void WorkModel::updateNodes(){
	//Node *n;

	BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
		long nodeId = n->ID;
		// gets the force 3d vector
		// sets resulting force to zero
		n->x = (*n->position).x();
		n->y = (*n->position).y();
		n->z = (*n->position).z();
	}
	BOOST_FOREACH (mapElement_Node n1, model->nodes ) { 
		Node *n = n1.second;
		if (is_NODE_RL(n)) {		
			long nodeId = n->ID;
			n->x = (*n->position).x();
			n->y = (*n->position).y();
			n->z = (*n->position).z();
		}
	}
}

void WorkModel::generateNODE_RESTRAINED() {
	Vector3d zeroForce(0., 0., 0.);
	ffds.NODE_RESTRAINED.clear();

	BOOST_FOREACH(mapElement_Node b1, model->nodes) {
		// gets node
		Node *n = b1.second;
		
		if (is_NODE_RESTRAINED(n)) {
			ffds.NODE_RESTRAINED.push_back(n);
		}
	}
}
void WorkModel::generateNODE_I() {
	Vector3d zeroForce(0.,0.,0.);
	ffds.NODE_I.clear();
	
	BOOST_FOREACH (mapElement_Node b1 , model->nodes ) {
		// gets node
		Node *n = b1.second;
		if (is_NODE_I(n)){
			ffds.NODE_I.push_back(n);
			ffds.REAZ_V[n] = zeroForce;
		}
	}
}

// puts all NODE_TC_L into the ffds.NODE_TC_L vector
void WorkModel::generateNODE_TC_L() {
	ffds.NODE_TC_L.clear();
	//	LOOP over the nodes
	BOOST_FOREACH (mapElement_Node b1 , model->nodes ) {
		// gets node
		Node *n = b1.second;
		//if is NODE_TC_L, push it
		if (is_NODE_TC_L(n))
			ffds.NODE_TC_L.push_back(n);
	}
}

// puts all NODE_RL into the ffds.NODE_RL vector
void WorkModel::generateNODE_RL() {
	ffds.NODE_RL.clear();
	//	LOOP over the nodes
	BOOST_FOREACH (mapElement_Node b1 , model->nodes ) {
		// gets node
		Node *n = b1.second;
		//if is NODE_RL, push it
		if (is_NODE_RL(n))
			ffds.NODE_RL.push_back(n);
	}
}


//  gets alpha_i for ffc, puts to LOAD CASE
void WorkModel::init_LoadCases_from_FFC_LoadCaseCombination(){
	cout << "\nUsing LoadCases from the FFCombination:";
	
	// LOOP over FFLCC rows, select those with the right FreedomCaseID only, 
	BOOST_FOREACH (loadCaseCombinationItem elem , model->ffc->components) {
		if (elem.freedomCaseID == selectedFreedomCaseID) {
			cout << "\n\tLoadCase: ID: " << elem.loadPatternID << " freedomCaseID: " << elem.freedomCaseID <<
				" alphaCoefficient: " << elem.loadMultiplierAlpha;

			// check if loadPattern exists, or give an error and exit!
			if (model->loadPatterns.find(elem.loadPatternID) == model->loadPatterns.end()) { 
				stringstream ss;
				ss << " check the FFC LoadCaseCombination! LoadCase ID: " << elem.loadPatternID << " found in FFC, but not in the Model ";
				exitWithMessage( ss.str() ) ;
			}

			// sets the ALPHA found in the model, to the right LOAD_CASE
			LoadPattern *lp = model->loadPatterns[elem.loadPatternID];

			lp->selectedForFF = true;
			lp->alphaCoefficient = elem.loadMultiplierAlpha ;
			lp->alpha_User = elem.loadMultiplierAlpha ;
		}
		else {
			// unexpected..
			// skip this FreedomCase ID, anyway
			cout << endl << "LoadCaseCombination FFC contains more than one FreedomCase??";
		}
	}



	cout <<" Done.";
}

void WorkModel::initPlateWeightMultiplier(){
	// sets weight multiplier
	BOOST_FOREACH (mapElement_Plate mepl, model->plates ) {
		Plate *pl = mepl.second;
		//only for TC plates
		if (pl->material_type != TC) continue;
		
		double memThick = pl->pPlateProperty->memThick;
		double density = pl->pPlateProperty->density;

		pl->weightMultiplier = memThick * density;
	}
}

void WorkModel::recomputeElasticBeams_FFC(){
	// LOOP over beams
	BOOST_FOREACH (mapElement_Beam b1, model->beams) {
		Beam *b = b1.second;
		// for elastic beams only
		if ( b->isElastic) {

			//if (b->pBeamProperty == NULL) {
			//	cout << " ERROR, Beam ID <"<< b->ID<<  "> has BeamProperty NULL , exit now"<<endl << endl;system("PAUSE");exit(0);
			//}

			// lunghezza a riposo: è la lunghezza iniziale
			b->L0 = b->LS - b->initialAxialForce / b->axialStiffness;
		}
	}
}

void WorkModel::initElasticBeams(){
	// LOOP over beams
	BOOST_FOREACH (mapElement_Beam b1, model->beams) {
		Beam *b = b1.second;
		b->LS = (*b->pNode2->position - *b->pNode1->position).norm() ;

		// for elastic beams only
		if ( b->isElastic) {

			if (b->pBeamProperty == NULL) {
				cout << " ERROR, Beam ID <"<< b->ID<<  "> has BeamProperty NULL , exit now"<<endl << endl;system("PAUSE");exit(0);
			}

			b->coeffEA = b->pBeamProperty->area * b->pBeamProperty->modulus * b->pBeamProperty->modulusMultiplier; 
			
			b->axialStiffness = min (b->pBeamProperty->stiffnessMax, b->coeffEA/b->LS );
			// lunghezza a riposo: è la lunghezza iniziale
			//23-05 b->L0 = b->LS * (1 - b->sumTensions/(b->coeffEA));
			b->L0 = b->LS - b->initialAxialForce / b->axialStiffness;
		}
	}
}



// sets the right pointers, from beam and plates, to their property
void WorkModel::initPlate_PropertyPointers() {
	//LOOP over plates
	BOOST_FOREACH (mapElement_Plate p1 , model->plates) {
		Plate *p = p1.second;
		// if plate properties exist, set it to the plate
		if (model->plateProperties.find(p->Property)!=model->plateProperties.end())
			p->pPlateProperty = model->plateProperties[p->Property];
		else {
			cout << "\n ERROR, non-existent Plate Property ID="<<p->Property<< " for Plate ID="<<p->ID<<"\n";
			system("PAUSE");
		}
	}
}

// sets the right pointers, from beam and plates, to their property
void WorkModel::initBeam_PropertyPointers(){
	//LOOP over plates
	BOOST_FOREACH (mapElement_Beam b1 , model->beams) {
		Beam *b = b1.second;
		// if beam properties exist, set it to the beam
		if (model->beamProperties.find(b->beamPropertyID)!=model->beamProperties.end())
			b->pBeamProperty = model->beamProperties[b->beamPropertyID];
		else {
			cout << "\n ERROR, non-existent Beam Property ID="<<b->beamPropertyID<< " for Beam ID="<<b->ID<<"\n";
			system("PAUSE");
		}
	}
}




//iwlm
void WorkModel::initWork_load_model() {
	//***************************************************************************************************************************
	//*******		INIT
	updateMaxNodeId();
	cout  << " \n MAX NODE ID: " << maxNodeId;

	initPlate_PropertyPointers();
	initBeam_PropertyPointers();

	//********************************
	//****	LOAD DATA FILES AND SET PROPERTIES
	// Loads material types and sets beam, nodes, material types
	loadPropertyFiles();

	setAttributes_BmProperties();
	setAttributes_PlProperties();
	setAttributes_Plates();
	setAttributes_Beams();
	setAttributes_Nodes();
	setRestraint_Nodes();


	selectedFreedomCaseID = model->selectedFreedomCase->ID;
	
	//********************************
	//********************************
	// GENERATE POINTERS
	generateBeamNodePointers();
	generatePlateNodePointers();


	//********************************
	// GENERATE SETS
	generateNODE_TC_L();
	//generateNODE_RL();
	generateNODE_I();
	generateNODE_RESTRAINED();


	//****************************************************************
	// GENERATE INCIDENT BEAMS, PLATES, NODES
	generateIncidentBeams();
	generateIncidentPlates();
	generateAdjacentNodes();


	// GET the loadCases for this LoadCaseCombination
	// inits LoadPattern objects, whit alpha and alpha_user
	init_LoadCases_from_FFC_LoadCaseCombination();

	initPlateWeightMultiplier();

	//****************************************************************
	// generate the Position and Force and Velocity vectors;
	posI = new Vector3d[appSettings::MAX_NODE_ID+1];
	posIm1 = new Vector3d[appSettings::MAX_NODE_ID+1];
	posIm2 = new Vector3d[appSettings::MAX_NODE_ID+1];
	fI= new Vector3d[appSettings::MAX_NODE_ID+1];
	vI= new double[appSettings::MAX_NODE_ID+1];
	vIm1= new double[appSettings::MAX_NODE_ID+1];

	//********************************
	// LINK NODE positions to POS_I array, for Vector3d
	linkNodePosition();

	// INIT ELASTIC BEAMS
	// note: compute LS and L0 wrt initial conditions
	initElasticBeams();

	//****************************************************************
	// INIT RIGIDLINKS
	GraphSplit gs;
	gs.initWithModel(this->model);
	gs.clearRigidGroups();
	gs.initColors();
	gs.computeRigidGroups();
	//****************************************************************




}


//****************************************************************
// recompute BEAM PreLoad_FFC
// recompute PLATE PreLoad_FFC
//****************************************************************
// this function updates all the data dependent from ALPHA-s
//rffc rf
void WorkModel::recompute_for_FFC() {
	cout << "\n\n Recomputing the LoadCase Combination.... (forces, loads, etc)";
	// GET the BmPreLoads for this LoadCaseCombination
	cout << "\nSetting Beam-PreLoads for the FFCombination..";
	recompute_BmPreLoad_FFC();

	// GET the PlPreLoads for this LoadCaseCombination
	cout << "\nSetting Plate-PreLoads for the FFCombination..";
	recompute_PlPreLoad_FFC();

	// GET the PlPressure for this LoadCaseCombination
	cout << "\nSetting Plate-Pressures for the FFCombination..";
	recompute_PlPressure_FFC();


	// recompute L0 for elstic beams, that has changed if alpha*BmPreLoad has changed
	recomputeElasticBeams_FFC();

	// recompute gravity and forze peso
	recomputeGravity();
	compute_forzePeso();
	
	// compute ND FORCES
	compute_ndForces_FFC();

	// compute normal PlatePressures
	compute_normalPressures();


	// RECOMPUTE Stiffnesses:
	clearNodeStiffness();
	// SUM STIFFNESS FROM PLATES
	sumStiffnessFromPlates();
	// SUM STIFFNESS FROM BEAMS
	sumStiffnessFromBeams();

	// to init NodeMasses and and values
	setDeltaT(deltaT);
}



clock_t time_begin;

//iw 
void WorkModel::initWork(){
	cout << "\nINIT WORK..";

	// initialization, ONE TIME on load
	initWork_load_model();

	// THE FormFindingCombination is model->ffc

	cout << "\nMax Beam-Node ID : " << appSettings::MAX_NODE_ID << "\n";
	cout <<"\nBegin Form Finding\n";
	cout <<"\nUsing FreedomCase ID: " << selectedFreedomCaseID ;


	//****************************************************************
	// recompute BEAM PreLoad_FFC
	// recompute PLATE PreLoad_FFC
	//****************************************************************
	// this function updates all the data dependent from ALPHA-s
	recompute_for_FFC();


	writeUserAlpha_toFile();

	iterCounter = 0;

	// init all to zero
	stopAll();


	time_begin = clock();
}




