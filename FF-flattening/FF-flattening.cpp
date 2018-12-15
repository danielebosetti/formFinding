// FF-flattening.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Flattening_Solver.h"

#pragma warning (disable : 4800)


int main(int argc, char* argv[])
{

#ifdef _DEBUG 
	string origdir = "C:\\Users\\danielebosetti.hp\\Desktop\\CO_FormFinding2\\trunk\\to_package\\package_dir\\";
	appSettings::applicationDir =
		"C:\\Users\\danielebosetti.hp\\Desktop\\CO_FormFinding2\\trunk\\to_package\\package_dir\\";
	appSettings::dataDir =
		"C:\\Users\\danielebosetti.hp\\Desktop\\CO_FormFinding2\\trunk\\to_package\\package_dir\\IO_txt\\";
	appSettings::settingsDir =
		"C:\\Users\\danielebosetti.hp\\Desktop\\CO_FormFinding2\\trunk\\to_package\\package_dir\\settings\\";
	appSettings::fileInput = "sample001small.txt";
	appSettings::fileInput = "sampleSIX-003-FM102-F_00.txt";
	appSettings::fileInput = "sample002.txt";
	appSettings::fileInput = "sample001.txt";
	//appSettings::fileInput = "out1-debug.txt";
	appSettings::fileOutput = "outdebug_1.txt";
	appSettings::fileOutput = "outdebug_2.txt";
	appSettings::fileOutput = "outdebug_1small.txt";
	//appSettings::fileOutput =	"out2-debug.txt";

#endif

#ifndef _DEBUG 
	// parse parameters
	parseParameters_for_cmd(argc, argv);

	//check key
	appSettings::checkKey();
	if (!appSettings::LICENSE_CHECK_OK)
		return 0;
#endif

	std::cout << "\nUsing Application path: " << appSettings::applicationDir << "\n";
	appSettings::readParamFromIni();

	// read the model
	appSettings::workingFilePath = appSettings::dataDir + appSettings::fileInput;
	std::cout << "\nUsing File Input: " << appSettings::workingFilePath << "\n";


	Model_Reduced model;
	ModelLoader_Reduced modelLoader;

	modelLoader.model = &model;
	//stretched grid method
	model.modelNAME = "SGM_Model";
	modelLoader.fileName = appSettings::workingFilePath;
	modelLoader.loadModel();

	ModelExporter_Reduced modelExporter;
	modelExporter.model = &model;
	modelExporter.outBasePath = appSettings::dataDir;


	// push node refs into nodes
	model.linkNodesToBeams();
	model.updateNodePositions();
	model.initBeamLengths();
	model.initAdjacentSetsForNodes();
	model.initRestrainedSets();


	// check
	cout << "\n model has:" << model.nodes.size() << " nodes";
	cout << "\n model has:" << model.nodes_free.size() << " free nodes";
	cout << "\n model has:" << model.nodes_restrained.size() << " restrained nodes";
	cout << "\n number of NdRestraint elements found into file:" << model.restraints.size() ;


	// init the solver object
	Flattening_Solver fs;
	fs.checkCleanData();
	fs.setModel(&model);
	fs.initMatrixes();
	// solve
	bool solve_ok = fs.solveSGM();
	// export to model
	if (solve_ok) fs.updatePositionToModel();
	else {
		exitWithMessage ("\n\n **ERROR** inverting matrix, matrix is singular?\n\n");
	}
	// solve by using the stretched grid method

	// export the updated model
	string fileName = appSettings::dataDir + "\\" + appSettings::fileOutput;
	cout << "\nSaving output to " << fileName << " ..";


	modelExporter.exportModel(fileName);
	cout << "\nEnd program";


	system("PAUSE");

    return 0;
}

