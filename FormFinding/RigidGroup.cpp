#include "StdAfx.h"
#include "RigidGroup.h"


RigidGroup::RigidGroup(void)
{
	baricenterSpeed << 0., 0., 0.;
	angularMomentum << 0., 0., 0.;
	deltaT = 0.025;
}

void RigidGroup::setDeltaT(double newDeltaT){
	this->deltaT = newDeltaT;
}


RigidGroup::~RigidGroup(void)
{
}

void RigidGroup::computeBaricenter(){
	baricenter << 0., 0., 0.;
	BOOST_FOREACH(Node *n, nodeList){
		baricenter += *n->position;
	}
	baricenter = baricenter / nodeList.size();
}


void RigidGroup::setForceOnNode(Node *n, Vector3d& f){
	forceOnNode[n] = f;
}


void RigidGroup::computeSumTorque(){

	sumTorque << 0., 0., 0.;
	BOOST_FOREACH(Node *n, nodeList){
		// force momentum, 
		sumTorque += ((*n->position)-baricenter).cross(forceOnNode[n]);
	}

}


void RigidGroup::computeSumForces(){
	sumForces << 0., 0., 0.;
	BOOST_FOREACH(Node *n, nodeList){
		sumForces += forceOnNode[n];
	}
}

void RigidGroup::updateBaricenterMotion(){

	baricenterSpeed = sumForces * deltaT;

	//if (baricenterSpeed.norm() * deltaT > MAX_POS_STEP) {
	//	baricenterSpeed.normalize();
	//	baricenterSpeed	= baricenterSpeed * MAX_POS_STEP / deltaT;
	//}

	angularMomentum = sumTorque * deltaT;
}



extern double RIGID_X_TOL;
double RIGID_X_TOL = 10.e-10;

void RigidGroup::updatePoints(){

	//BOOST_FOREACH(Node *n, nodeList){
	//	(*n->position) +=	
	//		baricenterSpeed * deltaT -
	//		((*n->position)-baricenter).cross(angularMomentum) * deltaT;
	//}

	Matrix3d m;
	Vector3d rotVector = angularMomentum;
	double rotNorm = angularMomentum.norm();

	// rotation vector is ZERO! #IND rotation matrix
	if (rotNorm < RIGID_X_TOL) {
		//only shift position
		BOOST_FOREACH(Node *n, nodeList){
			(*n->position) += baricenterSpeed * deltaT ;
		}
	}
	else {

		// do nothing if speed is zero
		if (baricenterSpeed.norm() < RIGID_X_TOL)
			return;

		rotVector.normalize();
		// ROTATION matrix
		m = AngleAxisd( deltaT * rotNorm, rotVector);


		// SHIFT and ROTATE
		BOOST_FOREACH(Node *n, nodeList){
			Vector3d v1 = (*n->position)-baricenter;
			(*n->position) +=	
				baricenterSpeed * deltaT +
				m * ((*n->position)-baricenter) -v1;
		}
	}

}




