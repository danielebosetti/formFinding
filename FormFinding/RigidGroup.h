#pragma once
#include "stdafx.h"


using namespace std;

class RigidGroup
{
public:
	Vector3d baricenter, baricenterSpeed, angularMomentum ;
	Vector3d sumTorque, sumForces;



	vector<Beam*> beamList;
	vector<Node*> nodeList;
	map <Node*, Vector3d> forceOnNode;

	void setDeltaT(double newDeltaT);
	double deltaT;

	void computeBaricenter();
	void computeSumTorque();
	void computeSumForces();

	void setForceOnNode(Node *n, Vector3d& f);

	void updateBaricenterMotion();
	void updatePoints();

	RigidGroup(void);
	~RigidGroup(void);
};

