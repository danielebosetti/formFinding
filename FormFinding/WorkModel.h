#pragma once
//#include "stdafx.h"

#include <Eigen\Dense>
#include <string>
#include <vector>
#include <string>
#include "FormFindingDataSet.h"

class Model;
class Node;

using namespace std;

class WorkModel
{
private:
	double deltaT ;
	
public:
	string inputPath, outputPath;


	// working arrays
	Eigen::Vector3d *posI, *posIm1, *posIm2;
	double *vI, *vIm1;
	Eigen::Vector3d *fI;




	double c ; 
	//double m, coeff1 ; 

	double kinEnergy, kinEnergyM1, kinEnergyM2;

	double maxForceModule;
	long nodeMaxForceModule;

	void setDeltaT(double newDeltaT);
	double getDeltaT();

	void setAlphaForLoadCase(double newAlpha, long loadCaseID);

	long iterCounter;
	long selectedFreedomCaseID;

	long maxNodeId;
	void updateMaxNodeId();
	
	Model *model;
	FormFindingDataSet ffds;

	// contiene i PropertyName per i TC, STEEL, RIGIDLINKS, ELASTIC elements
	vector<string> TC_propertyNames;
	vector<string> STEEL_propertyNames;
	vector<string> RIGIDLINKS_propertyNames;
	vector<string> ELASTICBEAM_propertyNames;
	vector<double> ELASTICBEAM_modulusMultiplier;
	vector<double> ELASTICBEAM_stiffnessMax;


	bool stopIterations;
	bool restraintPlatesTC_DXDY;
	bool useJacobian;

	// working methods
	void doOneIteration(); // dynamic relaxation
	void computeSingleForceNext(Node *n, Vector3d newPosition, Vector3d& f);
	void computeMaxForce();
	void checkKineticEnergy();
	void recomputeXi();
	void stepXi();

	double forceToSkip;
	double FFC_gravity;
	void recomputeGravity();


	// FORCE DENSITY
	void goForceDensity();


	// optimization methods/stuff
	//bool minimizeForce(Node *n);
	//bool minimizeForceScalar(Node *n);
	//double fMinimize(double t);
	//double fMinimize3d(const double *x);
	bool computeJacobian();

	Node* maxForceNode;


	// init model elements

	// initWork_load_model: initialization ONE-TIME (beam pointers, properties pointers, etc etc
	void initWork_load_model();
	// init_LoadCases_forFFC: initialization ONE-TIME (...)
	void init_LoadCases_from_FFC_LoadCaseCombination();

	// recompute for FFC: recompute loads, forces, gravity etc, when FFC combination is modified
	void recompute_for_FFC();

	void initWork();

	// sets the right pointers, from beam and plates, to their property
	void initPlate_PropertyPointers();
	void initBeam_PropertyPointers();

	void initElasticBeams();
	void initPlateWeightMultiplier();

	void clearNodeStiffness();
	void sumStiffnessFromPlates();
	void sumStiffnessFromBeams();





	WorkModel(void);
	~WorkModel(void);



	void clearWorkModel(void);


	void generateNODE_TC_L();
	void generateNODE_RL();

	void generateNODE_I();
	void generateNODE_RESTRAINED();
	void generateIncidentBeams();
	void generateIncidentPlates();
	void generateAdjacentNodes();

	void generateBeamNodePointers();
	void generatePlateNodePointers();
	void linkNodePosition();

	//void generateVersorsBeams();

	void compute_REAZ_V();
	void compute_forzePeso();
	void compute_normalPressures();

	void compute_ndForces_FFC();
	void recompute_BmPreLoad_FFC();
	void recompute_PlPreLoad_FFC();
	void recompute_PlPressure_FFC();

	void recomputeElasticBeams_FFC();

	void updateNodes();

	void stopNode(Node* n);
	void stopAll();


	void writeUserAlpha_toFile();
	void loadUserAlpha_fromFile();

	//********************************************************************************
	//********************************************************************************
	//*		SET Form Finding properties
	//********************************************************************************
	
	// LEGGE i PropertyName per i TC, STEEL, RIGIDLINKS, ELASTIC elements
	void loadPropertyFiles();
	// MATERIAL TYPES, RIGID LINKS, ELASIC BEAMS >> move properties to the model
	void setAttributes_BmProperties();
	void setAttributes_PlProperties();
	void setAttributes_Plates();
	void setAttributes_Beams();
	void setAttributes_Nodes();
	void setRestraint_Nodes();


	// old stuff, parallel methods
	//void recomputeXi_parallel();
	//void recomputeXi_parallel_msThreads();
	//void recomputeXi_parallel_OMP();

	//ofstream oLogger;
};

