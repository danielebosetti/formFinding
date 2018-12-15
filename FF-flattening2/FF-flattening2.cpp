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
	appSettings::fileInput = "sample001small.txt";
	appSettings::fileInput = "flat14.txt";

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


	// push node refs into nodes
	model.linkNodesToBeams();
	model.updateNodePositions();
	model.initBeamLengths();
	model.initAdjacentSetsForNodes();
	model.initRestrainedSets();

	// create a copy of the model
	Model_Reduced model_flat(model);
	model_flat.modelNAME = "Model_FLAT";

	Model_Reduced *wm = &model_flat;

	char *err_msg_iteration = "\n\n **ERROR** inverting matrix, matrix is singular?\n\n";

	// check
	cout << "\n model has:" << wm->nodes.size() << " nodes";
	cout << "\n model has:" << wm->nodes_free.size() << " free nodes";
	cout << "\n model has:" << wm->nodes_restrained.size() << " restrained nodes";
	cout << "\n number of NdRestraint elements found into file:" << wm->restraints.size();


	// init the solver object
	Flattening_Solver fs_iterative;
	bool solve_ok_iter = true;
	

	// flat the model_flat
	for (auto& x : model_flat.nodes) {
		x.second->z = 0.;
	}
	// update positions!!
	model_flat.updateNodePositions();


	// LOOP:
	// solve system again, using the new P
	for (long n_iter = 0; n_iter < appSettings::N_ITER ; n_iter++) {
		fs_iterative.checkCleanData();
		fs_iterative.setModel(&model_flat);

		fs_iterative.initMatrixes();
		fs_iterative.init_P(&model, &model_flat, appSettings::LAMBDA);
		// apply P to B
		fs_iterative.apply_P();

		// solve..
		// skip z axis!
		bool skip_z = true;
		solve_ok_iter = fs_iterative.solveSGM(skip_z);
		// export to flat model
		if (solve_ok_iter) fs_iterative.updatePositionToModel(skip_z);
		else exitWithMessage(err_msg_iteration);
	}

	// export the updated model
	string fileName = appSettings::dataDir + "\\" + appSettings::fileOutput;
	cout << "\nSaving output to " << fileName << " ..";

	ModelExporter_Reduced modelExporter;
	modelExporter.model = &model_flat;
	modelExporter.outBasePath = appSettings::dataDir;

	modelExporter.exportModel(fileName);
	cout << "\nEnd program";


	system("PAUSE");

	return 0;
}

