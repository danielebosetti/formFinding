#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;


class fileLoader;
class Node;
class Beam;


//#include "glm/glm.hpp"
//#include "glm/gtc/type_ptr.hpp"

class FormFindingDataSet
{
public:



	vector<Node*> NODE_TC_L;
	vector<Node*> NODE_RL;
	vector<Node*> NODE_I;

	vector<Node*> NODE_RESTRAINED;


	map<Node*, Vector3d> REAZ_V;
	map<Beam*, double> FORCE_DENSITY;

	//	map<Node*, Vector3d> forzaPeso;
	vector<Vector3d> forzaPeso;
	vector<Vector3d> ndForces_FFC;

	// resulting force on a node, due to PlatePressure
	vector<Vector3d> forcePressure;


	FormFindingDataSet(void);
	~FormFindingDataSet(void);
	void clearFormFindingDataSet(void);
};

