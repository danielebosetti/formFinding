#include "StdAfx.h"
#include "WorkModel.h"
#include <algorithm>
#include <cmath>
//#include <omp.h>
#pragma warning (disable:4018)

/*
this embeds part of the application object
*/

WorkModel::WorkModel(void){
	stopIterations = false;
	restraintPlatesTC_DXDY = false;
	useJacobian = false;
	deltaT = appSettings::TIME_STEP;
	//m = 1.;
	c = appSettings::COEFF_VISC;
	forceToSkip = 0.;
	maxForceNode = NULL;

	kinEnergy = 0.0;
	kinEnergyM1 = 0.0;
	kinEnergyM2 = 0.0;

	TC_propertyNames.clear();
	STEEL_propertyNames.clear();
	RIGIDLINKS_propertyNames.clear();
	ELASTICBEAM_propertyNames.clear();

}

WorkModel::~WorkModel(void)
{}

//sdt
void WorkModel::setDeltaT(double newDeltaT){
	deltaT = newDeltaT;
	// LOOP for each NODE
	// ASSIGN MASS
	BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
		//OLD value>>>    n->massNode = ALPHA_MASS * 0.025 * 0.025 * n->totalStiffness / 4 ;
		n->massNode = appSettings::ALPHA_MASS * deltaT * deltaT * n->totalStiffness / 4 ;

		double c;
		
		if (appSettings::USE_C_CONSTANT)
			c = appSettings::C_DEFAULT_VALUE;
		else
			c = appSettings::XI * 2. * sqrt(n->massNode * n->totalStiffness);

		n->coeff1 = 1. / ( n->massNode/(deltaT*deltaT) + c/(2.*deltaT) ) ;
		n->coeff2 = n->massNode / (deltaT*deltaT) - c/(2.*deltaT) ;
		n->coeff3 = 2.*n->massNode /(deltaT*deltaT) ;
	}

	stopAll();
	kinEnergyM2 = 0.0;
	kinEnergyM1 = 0.0;
	kinEnergy = 0.0;
}




//cmf
void WorkModel::computeMaxForce(){
	long nodeID;
	maxForceModule = 0.0;
	// LOOP over NODE_TC_L nodes, compute Max Force
	BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {

		// check, if has to be recomputed, then recompute FORCE
		//if (!n->forceIsComputed) {
			nodeID = n->ID;
			computeSingleForceNext(n, *n->position, fI[nodeID]);
			//n->forceIsComputed = true;
		//}

		//if (n->enabled) {
			nodeID = n->ID;
			if (fI[nodeID].norm() > maxForceModule) {
				maxForceModule = fI[nodeID].norm();
				nodeMaxForceModule = nodeID;
				maxForceNode = n;
			}
		//}
	}

} 

void WorkModel::stopNode(Node* n){
	long nodeID;
	nodeID = n->ID;
	n->enabled = true;
	posIm1[nodeID] = posI[nodeID] ;
	posIm2[nodeID] = posI[nodeID] ;
	vI[nodeID] = 0.;
	vIm1[nodeID] = 0.;
}

// stops all nodes, enables, and maxForce=0
void WorkModel::stopAll(){
	BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
		stopNode(n);
	}
	maxForceModule = 0.;
}


// cke
void WorkModel::checkKineticEnergy(){
	long nodeID;

	if (! (appSettings::ENABLE_KINETIC_DAMPING) ) {
		return;
	}

	// global kinetic energy
	if (appSettings::SMORZAMENTO_K_GLOBAL) {
		//cout << " k global ";
		kinEnergyM2 = kinEnergyM1;
		kinEnergyM1 = kinEnergy;

		////// NEW-1: sum of m*v^2 , 
		kinEnergy = 0.0;
		BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
			nodeID = n->ID;
			kinEnergy += vI[nodeID] * vI[nodeID] * n->massNode;
		}

		double eps = 1e-30;

		// if kinetic Energy decreases, then stop all nodes
		if ( ( kinEnergyM1 > kinEnergyM2 + eps ) && ( kinEnergy   < kinEnergyM1 + eps ) ) {
			// stop all nodes
			stopAll();
			kinEnergy = 0.0;
			kinEnergyM2 = 0.0;
			kinEnergyM1 = 0.0;
		}
	}

	// kinetic energy, single-points
	else {
		//cout << " k single-point ";
		BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
			if (n->enabled) {
				nodeID = n->ID;
				if (vI[nodeID] < vIm1[nodeID] ) {
					stopNode(n);
				}
			}
		}	
	}


}

extern long N_SKIP_COMPUTE;
long N_SKIP_COMPUTE = 5;

//calc here xrec recx rcx
void WorkModel::recomputeXi(){
	long nodeID;

	double coeff1, coeff2, coeff3 ;

	BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
		nodeID = n->ID;

		// if node is disabled, decrement the skip-count
		if (!n->enabled) {
			n->skipCompute--;
			if (n->skipCompute <= 0)
				n->enabled = true;
		}

		else 
{// IF NODE ENABLED
		// computes the new force on the node, and stores the vector to fI
		computeSingleForceNext(n, posIm1[nodeID], fI[nodeID] );
		
		// ================================================================================================		
		// ================================================================================================		
		// 2016-01-01 disabled
		// bool computed_ok = false;
		//if (useJacobian) {
		//	// //if force is large, apply optimization
		//	if (fI[nodeID].norm() > 0.9 * maxForceModule && maxForceModule > 0.0001) {
		//		//if (!computed_ok) {
		//			// try minimizing...
		//		// try 3D
		//		computed_ok = minimizeForce(n);
		//		if (computed_ok ) {
		//			stopNode(n);
		//			//n->toRecomputeForceOnAdjacentNodes();
		//			goto endNodeEnabled;
		//		}
		//		// try scalar
		//		computed_ok = minimizeForceScalar(n);
		//		if (computed_ok ) {
		//			stopNode(n);
		//			//n->toRecomputeForceOnAdjacentNodes();
		//			goto endNodeEnabled;
		//		}
		//	}
		//}
		// ================================================================================================		
		// ================================================================================================		

		// gets node coefficients
		coeff1 = n->coeff1 ;
		coeff2 = n->coeff2 ;
		coeff3 = n->coeff3 ;

		// compute the new node position, based on the force fI
		posI[nodeID] = coeff1 * ( fI[nodeID] - ( coeff2 )* posIm2[nodeID]  + 
			(  coeff3 ) * posIm1[nodeID] ) ;
		// compute the node speed
		vI[nodeID] = (posI[nodeID] - posIm1[nodeID]).norm();


		 //////if force is small, disable the node
		if (fI[nodeID].norm() < forceToSkip) {
			n->enabled = false;
			//n->skipCompute = N_SKIP_COMPUTE;
			n->skipCompute = appSettings::SKIP_N_STEP;
			// saltato nel prox STEP_XI
			posIm2[n->ID]=posIm1[n->ID];
			posIm1[n->ID]=posI[n->ID];
			vIm1[n->ID] = vI[n->ID];
		}

		//n->toRecomputeForceOnAdjacentNodes();
		//endNodeEnabled: {}

}// END IF NODE ENABLED
		
	}

	
	// for each RIGID GROUP
	BOOST_FOREACH (RigidGroup *rg, model->rigidGroups){
		rg->setDeltaT(this->deltaT);
		// for each NODE of the rigid group
		BOOST_FOREACH (Node *n, rg->nodeList){
			Vector3d resForce;
			//compute force
			computeSingleForceNext(n, *n->position, resForce);
			// set force to RigidGroup
			rg->setForceOnNode(n, resForce);
		}

		rg->computeBaricenter();
		rg->computeSumTorque();
		rg->computeSumForces();

		rg->updateBaricenterMotion();
		rg->updatePoints();
	}


}


// move Xi to Xi-1, and Xi-2 to Xi-2, and Force_i to Force_i-1, and vI to vI-1
void WorkModel::stepXi(){
		BOOST_FOREACH (Node *n , ffds.NODE_TC_L ) {
			if (n->enabled) {
				posIm2[n->ID]=posIm1[n->ID];
				posIm1[n->ID]=posI[n->ID];

				vIm1[n->ID] = vI[n->ID];
			}
		}
}


extern clock_t time_begin;

// dooi
void WorkModel::doOneIteration(){ // dynamic relaxation

	if (stopIterations) return;
	iterCounter ++;

	stepXi();
	recomputeXi();

	// CHECK energia cinetica, stoppa il punto se K_i< K_i-1
	checkKineticEnergy();
	computeMaxForce();
	//Force to skip
	forceToSkip = maxForceModule * appSettings::FORCE_SKIP_MULTIPLIER ;


	//========================================================
	// recompute
	if(!(iterCounter % appSettings::N_RECALC_BEAMPLATES)) {
		
		//****************************************************************
		// compute forze peso
		compute_forzePeso();
		// RECOMPUTE Stiffnesses:
		clearNodeStiffness();
		// SUM STIFFNESS FROM PLATES
		sumStiffnessFromPlates();
		// SUM STIFFNESS FROM BEAMS
		sumStiffnessFromBeams();
		// compute normal PlatePressures
		compute_normalPressures();
	}



	//========================================================
	// view 
	if(!(iterCounter % 200)) {
		clock_t time_end = clock();
		double elapsed_secs = double(time_end - time_begin) / CLOCKS_PER_SEC;
		
		if (elapsed_secs > 2.) {
			if (iterCounter > 1000)
				cout << " STEP " << iterCounter/1000 << "k";
			else 
				cout << " STEP " << iterCounter;
			time_begin = time_end;
		}
	}

	if((iterCounter % appSettings::RENDERSTEPS) == 0 ) {
		// print Max Force Module
		printf("\nmaxForceModule: %8.6E on NODE %6d", maxForceModule , nodeMaxForceModule ) ;
		//** RE-COMPUTE REAZ_V
		compute_REAZ_V();
	}

}

void WorkModel::goForceDensity()
{
}
