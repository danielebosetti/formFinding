#include "StdAfx.h"
#include "WorkModel.h"
#include <algorithm>
//#include <omp.h>
#pragma warning (disable:4018)



//cndf cnf
void WorkModel::compute_ndForces_FFC() {

	ffds.ndForces_FFC.clear();
	ffds.ndForces_FFC.resize(maxNodeId + 1);

	// azzera tutte le forze ND_FORCE
	//BOOST_FOREACH (Vector3d v , ffds.ndForces_FFC ) {
	for(int i=0;i<ffds.ndForces_FFC.size(); i++) {
		ffds.ndForces_FFC[i] << 0., 0., 0.;
	}
	// LOOP over NdForces
	BOOST_FOREACH (NdForce *nd , model->ndForces) {
		// get NODE
		Node *n = model->nodes[nd->nodeID];
		// get LoadPattern
		LoadPattern *lp = model->loadPatterns[nd->loadCaseID];
		//if (lp->selectedForFF) {
		//if (lp->alpha_User > 0.) {
		{
			// get the force to ADD
			Vector3d forceToAdd( nd->Fx, nd->Fy, nd->Fz );
			forceToAdd = forceToAdd * lp->alpha_User;
			// sun the force to the TOTAL FORCE on NODE
			Vector3d *force = &(ffds.ndForces_FFC[n->ID]);
			*force += forceToAdd;
		}
	}
}


//cnp
// copmute the normal pressures, by doing..
// plate area * normal vector * platePressure_FFC
// only on Tri3 plates!
void WorkModel::compute_normalPressures() {
	ffds.forcePressure.clear();
	ffds.forcePressure.resize(maxNodeId + 1);
	// azzera tutte le forze PRESSURE
	for (int i = 0; i<ffds.forcePressure.size(); i++)
		ffds.forcePressure[i] << 0., 0., 0.;

	// LOOP over PLATES - COMPUTE AREA and NORMAL BEAM
	BOOST_FOREACH(mapElement_Plate p1, model->plates) {
		// gets the plate
		Plate *p = p1.second;
		// Only for Tri3
		if (p->node4 != -1) continue;
		// small optimization, skip if no pressures
		if ( abs(p->sumPressures) < 1.e-8 ) continue;

		// compute normal vector , times area, 
		Vector3d l1, l2;
		Vector3d cross;
		l1 = *p->pNode2->position - *p->pNode1->position;
		l2 = *p->pNode3->position - *p->pNode2->position;
		// normal pressure = area * normal vector * platePressure_FFC
		// DIVIDED BY 3, for each node
		cross = l1.cross(l2) / 2. * p->sumPressures / 3.;
		// apply normal force to the 3 points
		ffds.forcePressure[p->node1] += cross;
		ffds.forcePressure[p->node2] += cross;
		ffds.forcePressure[p->node3] += cross;
	}
}

//cfp
void WorkModel::compute_forzePeso() {

	ffds.forzaPeso.clear();
	ffds.forzaPeso.resize(maxNodeId + 1);

	// azzera tutte le forze PESO
	for (int i=0; i<ffds.forzaPeso.size(); i++)
		ffds.forzaPeso[i] << 0., 0., 0.;

	// LOOP over BEAMS - COMPUTE PESO 
	BOOST_FOREACH (mapElement_Beam b1 , model->beams ) {
		//Per ogni Beam di ELEM_TC, si considera sul nodo iniziale e finale una NdForce in direzione Z pari a
		//FZ = - LUNGHEZZA_BEAM / 2 * AREA_BEAM * DENSITY
		//
		//dove AREA_BEAM e DENSITY sono definite nelle BeamProp.
		//Questa forza nodale va sommata, su ogni NODO_TC_L, alle forze dovute alle Beam tese.

		BeamProperty *bp; Beam *b; 
		double beamLength;
		// gets beam
		b = b1.second;
		if (!b) continue;

		if (is_BEAM_TC(b)){
			// gets property of beam (TODO : check if beamPropertyID is right)
			//bp = model->beamProp_erties[b->beamPropertyID];
			bp = b->pBeamProperty;

			beamLength = (*b->pNode1->position - *b->pNode2->position).norm();

			double forzaPesoBeam;
			// note: in kilo-Newton! kN
			forzaPesoBeam = bp->area * bp-> density * beamLength * FFC_gravity / 1000;

			Vector3d forzaPesoNode(0., 0., forzaPesoBeam/2.);

			//ffds.forzaPeso[b->pNode1] = ffds.forzaPeso[b->pNode1] + forzaPesoNode;
			//ffds.forzaPeso[b->pNode2] = ffds.forzaPeso[b->pNode2] + forzaPesoNode;
			ffds.forzaPeso[b->pNode1->ID] += forzaPesoNode;
			ffds.forzaPeso[b->pNode2->ID] += forzaPesoNode;

			//if (is_ELEM_TC_CE(b)) {
			//	cout << "\n Cable-Elastic Forza peso: " << forzaPesoBeam ;
			//}
			//else {
			//	cout << "\n Soap-Cable Forza peso: " << forzaPesoBeam ;
			//}

		}
	}

	// LOOP OVER PLATES - COMPUTE PESO 
	BOOST_FOREACH (mapElement_Plate p1 , model->plates ) {
		// gets plate
		Plate *p = p1.second;
		// Only for Tri3
		if (p->node4 != -1) continue;

		// apply weight for TC plates only
		if (is_PLATE_TC(p)){

			Vector3d l1, l2;
			Vector3d v1, h ;

			l1 = *p->pNode2->position - *p->pNode1->position;
			v1 = l1; v1.normalize();

			l2 = *p->pNode3->position - *p->pNode1->position;
			h = l2 - v1 * v1.dot(l2);

			double plateArea = l1.norm() * h.norm() /2; 
			double plateWeight = plateArea * p->weightMultiplier;
			// note: in kilo-Newton! kN
			double forzaPesoPlate = plateWeight * FFC_gravity / 1000;

			Vector3d forzaPesoNode(0., 0., forzaPesoPlate/3.);

			//ffds.forzaPeso[p->pNode1] = ffds.forzaPeso[p->pNode1] + forzaPesoNode;
			//ffds.forzaPeso[p->pNode2] = ffds.forzaPeso[p->pNode2] + forzaPesoNode;
			//ffds.forzaPeso[p->pNode3] = ffds.forzaPeso[p->pNode3] + forzaPesoNode;
			ffds.forzaPeso[p->pNode1->ID] += forzaPesoNode;
			ffds.forzaPeso[p->pNode2->ID] += forzaPesoNode;
			ffds.forzaPeso[p->pNode3->ID] += forzaPesoNode;
		}
	}
}

void WorkModel::recompute_BmPreLoad_FFC(){
	//CLEAR ALL first
	BOOST_FOREACH (mapElement_Beam b1 , model->beams) {
		Beam *b;
		b = b1.second;
		b->initialAxialForce = 0.0;
	}


	// LOOP over BmPreLoads rows, 
	BOOST_FOREACH (BmPreLoad *bpl , model->bmPreLoads ) {

		// if LoadCase for this PreLoad is selected for LCCFF
		//if ( model->loadPatterns[bpl->loadCaseID]->selectedForFF) {
//		if ( model->loadPatterns[bpl->loadCaseID]->alpha_User > 0.) {
		{
			//double alpha = model->loadPatterns[bpl->loadCaseID]->alphaCoefficient;
			double alpha = model->loadPatterns[bpl->loadCaseID]->alpha_User;
			double p0 = bpl->P0 ;

			if (model->beams.find(bpl->beamID) == model->beams.end()) { 
				cout << endl << "Warning: BmPreLoad found with ID " << bpl->beamID << " but the BEAM does not exist";
			}

			else
			// INCREMENTS the sum of axial tension for the BEAM
			model->beams[bpl->beamID]->initialAxialForce += alpha * p0;
		}
	}
}

// rplp
// compute the total plPressure over every plate, 
// by taking the sum-product of plPressures and the alpha-coefficients
// for the FFC (form finding combination)
void WorkModel::recompute_PlPressure_FFC() {
	//CLEAR ALL first
	BOOST_FOREACH(mapElement_Plate p1, model->plates) {
		Plate *p;
		p = p1.second;
		p->sumPressures = 0.;
	}

	// LOOP over Plate_Pressures
	BOOST_FOREACH(mapElement_PlatePressure item, model->PlPressures) {
		// gets the plPressure, must sum-product for FFC and put into plate
		PlPressure *plpr = item.second;
		//long loadPatternId = plpr->loadPatternID;
		double pressure = plpr->plPressureValue;
		double plateId = plpr->plateID;
		double alpha = model->loadPatterns[plpr->loadPatternID]->alpha_User;
		// gets the plate
		Plate *p = model->plates[plpr->plateID];
		// sum-product the plate-pressure
		p->sumPressures += alpha * pressure;
	}
}

void WorkModel::recompute_PlPreLoad_FFC(){
	//CLEAR ALL first
	BOOST_FOREACH (mapElement_Plate p1 , model->plates) {
		Plate *p;
		p = p1.second;
		p->sumStresses = 0.;
	}

	// LOOP over Plate_PreLoads
	BOOST_FOREACH (PlPreLoad *ppl , model->plPreLoads ) {
		
		//if ( model->loadPatterns[ppl->loadCaseID]->alpha_User > 0. )
		{
			//double alpha = model->loadPatterns[ppl->loadCaseID]->alphaCoefficient;
			double alpha = model->loadPatterns[ppl->loadCaseID]->alpha_User;
			double STRESS = ppl->px;
			long plateID = ppl->plateID;

			//if plate not found, then skip
			if (model->plates.find(plateID)==model->plates.end()) { 
				cout << endl << "Warning: PlPreLoad found with ID " << ppl->plateID << " but the PLATE does not exist";
			}
			else {
				Plate *p = model->plates[plateID];
				//double MEMTHICK = model->plateProp_erties[p->Property]->memThick;
				double MEMTHICK = p->pPlateProperty->memThick;

				p->sumStresses += alpha * STRESS * MEMTHICK;
			}
		}
	}
}

void WorkModel::clearNodeStiffness() {
	// LOOP over all NODEs
	BOOST_FOREACH (mapElement_Node b1 , model->nodes ) {
		Node *n = b1.second;
		n->totalStiffness = 1.;
	}
}

void WorkModel::sumStiffnessFromBeams() {
	// LOOP over beams - SUM to NODE > TOTAL_STIFFNESS 
	BOOST_FOREACH (mapElement_Beam b1, model->beams) {
		Beam *b = b1.second;
		double LS = (*b->pNode2->position - *b->pNode1->position).norm() ;

		// for elastic beams only
		if ( b->isElastic) {
			// compute stiffness to add
			//double axialStiffness = min( b->coeffEA / LS * b->pBeamProperty->modulusMultiplier ,
			//							 b->pBeamProperty->stiffnessMax );
			double axialStiffness = b->axialStiffness;

			b->pNode1->totalStiffness += axialStiffness;
			b->pNode2->totalStiffness += axialStiffness;
			
			//SUM NODE STIFFNESS
			//Node *n;
			//n = b->pNode1;
			//n->totalStiffness = n->totalStiffness + (b->coeffEA) / LS;
			//n = b->pNode2;
			//n->totalStiffness = n->totalStiffness + (b->coeffEA) / LS;
			//n->totalStiffness = n->totalStiffness + (b->coeffEA) / LS;
		}
		else if (b->material_type==TC) {
			//SUM NODE STIFFNESS
			Node *n;
			n = b->pNode1;
			n->totalStiffness = n->totalStiffness + b->initialAxialForce / LS;
			n = b->pNode2;
			n->totalStiffness = n->totalStiffness + b->initialAxialForce / LS;
		}
	}
}

void WorkModel::sumStiffnessFromPlates() {
	Node *n, *n1, *n2;
	double l1, l2;
	// LOOP over plates - SUM to NODE > TOTAL_STIFFNESS 
	BOOST_FOREACH (mapElement_Plate p1, model->plates) {
		Plate *p = p1.second;
		// IF PLATE is SOAP
		if (is_ELEM_TC_SOAP(p)) {
			
			// if PLATE is TRI3
			if (p->node4==-1) {
				// increment TOTAL_STIFFNESS for NODE_1
				n = p->pNode1;
				n1 = p->pNode3;
				n2 = p->pNode2;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);
				// increment TOTAL_STIFFNESS for NODE_2
				n = p->pNode2;
				n1 = p->pNode1;
				n2 = p->pNode3;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);
				// increment TOTAL_STIFFNESS for NODE_3
				n = p->pNode3;
				n1 = p->pNode2;
				n2 = p->pNode1;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);
			}
			//else, if plate is QUAD4
			else{
				// increment TOTAL_STIFFNESS for NODE_1
				n = p->pNode1;
				n1 = p->pNode4;
				n2 = p->pNode2;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);
				// increment TOTAL_STIFFNESS for NODE_2
				n = p->pNode2;
				n1 = p->pNode1;
				n2 = p->pNode3;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);
				// increment TOTAL_STIFFNESS for NODE_3
				n = p->pNode3;
				n1 = p->pNode2;
				n2 = p->pNode4;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);
				// increment TOTAL_STIFFNESS for NODE_4
				n = p->pNode4;
				n1 = p->pNode3;
				n2 = p->pNode1;
				l1 = (*n1->position-*n->position).norm();
				l2 = (*n2->position-*n->position).norm();
				if (l1+l2>1e-20)
					n->totalStiffness = n->totalStiffness + p->sumStresses/((l1+l2)/2.);

			}
		}
	}
}

// calcola le reazioni vincolari sui nodi di interfaccia
void WorkModel::compute_REAZ_V(){
	// LOOPS sui nodi interfaccia NODE_I
	BOOST_FOREACH (Node *n , ffds.NODE_I ) {
		Vector3d reactForce(0.,0.,0.);
		computeSingleForceNext(n, *n->position,  reactForce);
		// mette la reaz_v_calcolata in REAZ_V
		ffds.REAZ_V[n] = reactForce;
	}
}


void WorkModel::recomputeGravity(){
	
	FFC_gravity = 0.0;

	BOOST_FOREACH ( mapElement_LoadPattern lp1, model->loadPatterns) {
		LoadPattern * lp = lp1.second;
		FFC_gravity += lp->alpha_User * lp->gravity;
	}

	cout << "\n FFC Gravity: " << FFC_gravity;
}

void WorkModel::loadUserAlpha_fromFile() {
	string filename_alpha = "user_alpha.txt" ;
	string fullname_alpha = appSettings::settingsDir + filename_alpha ;

	cout << "\n\n LOADING ALPHA from file: <" << fullname_alpha << ">";

	ifstream ifs;
	ifs.open(fullname_alpha);
	if (!ifs.is_open()) {
		cout << "\nError: file not found: " << fullname_alpha ;
		return;
	}
	// else CONTINUE
	

	double newAlpha;
	int loadCaseID;
	string line;
	
	while (!ifs.eof()){
		getline(ifs,line);
		
		stringstream ss(line);
		string word;
		ss >> word;
		if(word=="/" || word=="LOADCASEID" || word=="") continue;
		//ss >> loadCaseID;
		loadCaseID = stol(word);
		ss >> newAlpha;
		//cout << endl << "READ LoadCase ID <" << loadCaseID<< ">  alpha: <" << newAlpha << ">";
		setAlphaForLoadCase(newAlpha, loadCaseID);
	}
	
	ifs.close();



}


void WorkModel::writeUserAlpha_toFile() {
	string filename_alpha = "user_alpha.txt" ;
	string fullname_alpha = appSettings::settingsDir + filename_alpha ;

	cout << "\n\n WRITING ALPHA to file: <" << fullname_alpha << ">";

	ofstream ofs;
	ofs.open(fullname_alpha);
	if (!ofs.is_open()) {
		cout << "\nErrore, impossibile scrivere sul file: <" << fullname_alpha << ">";
		return;
	}


	ofs << "LOADCASEID    ALPHA_COEFFICIENT    LOADCASE_NAME" << endl; 
	char sbuf[1000];
	BOOST_FOREACH(mapElement_LoadPattern lp1 , model->loadPatterns) {
		LoadPattern *lp = lp1.second;
		sprintf_s(sbuf,999, "%10d%19g    %s", lp->ID, lp->alpha_User, lp->name.c_str());
		ofs << sbuf << endl;
	}
	ofs.close();
}


void WorkModel::setAlphaForLoadCase(double newAlpha, long loadCaseID) {
	if (model->loadPatterns.find(loadCaseID)==model->loadPatterns.end()) {
		cout << endl << "ERROR: LOAD CASE ID <" << loadCaseID << "> non esiste! "; 
		return;
	}
	LoadPattern *lp = model->loadPatterns[loadCaseID];
	lp->alpha_User = newAlpha;
	char sbuf[1000];
	sprintf_s(sbuf, 999, "Set LoadCase Multiplier <%6.2g> for LoadCase<%-20s>-<%3d>", newAlpha, lp->name.c_str(), lp->ID);
	//cout << endl << "Setting Alpha_LoadCase <" << newAlpha <<  "> for LoadCase<" << lp->name<<   ">  ID:<" << lp->ID << "> "; 
	cout << endl << sbuf;
}






