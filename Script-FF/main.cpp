// Script-FF.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


void goIteration();
void goForceDensity();
//presi da MODEL RENDERER!!
// TODO: refactor

void get_iterations(long& num_iterations);




int main(int argc, char* argv[])
{

	parseParameters_for_cmd(argc, argv);
	appSettings::initAppSettings_for_cmd();

	std::cout << "\nUsing Application path: " << appSettings::applicationDir << "\n";

	appSettings::readParamFromIni();

	appSettings::workingFilePath = appSettings::dataDir + appSettings::fileInput;

	std::cout << "\nUsing File Input: " << appSettings::workingFilePath << "\n";

	appSettings::checkKey();
	if (!appSettings::LICENSE_CHECK_OK)
		return 0;

	//*************************************************************************************************************************
	//**    LOAD MODEL

	workingModelLoader.model = &workingModel;
	workingModel.modelNAME = "workingModel";
	workingModelLoader.fileName = appSettings::workingFilePath;
	workingModelLoader.loadModel();

	//*************************************************************************************************************************
	//******    WORK MODEL

	workModel.inputPath = appSettings::dataDir;
	workModel.outputPath = appSettings::dataDir;
	workModel.model = &workingModel;
	workModel.initWork();
	workModel.setDeltaT(appSettings::TIME_STEP);



	modelExporter.workingModel = &workingModel;
	modelExporter.originalModel= &workingModel;
	modelExporter.outBasePath = appSettings::dataDir;

	long num_iterations = 0;
	get_iterations(num_iterations);

	//===============================================================================
	// choose which method to use
	if (appSettings::method == "DR") {
		cout << "\nUsing method: Dynamic-Relaxation";

		//TODO: refactor
		// put the go-iteration into an Application method!
		cout << "\n Go Iteration()\r\n";
		clock_t begin = clock();


		for (long i = 0; i < num_iterations; i++) {
			workModel.doOneIteration();
		}
		workModel.updateNodes();

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		stringstream s2;
		s2 << "\nFinish iterations, time elapsed for " << num_iterations <<
			" iterations: " << elapsed_secs << " sec\r\n";
		cout << s2.str();
	}
	else if (appSettings::method == "FD") {
		cout << "\nUsing method: Force-Density";

		cout << "\nStart Force Density computation";
		clock_t begin = clock();

		Application::getInstance()->setWorkModel(&workModel);
		for (long i = 0; i < num_iterations; i++) {
			Application::getInstance()->goForceDensity();
		}
		workModel.updateNodes();

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		stringstream s2;
		s2 << "Time Elapsed for Force Density: " << elapsed_secs << " sec.\r\n";
	}
	else {
		cout << "\nError, unknown parameter METHOD=[" << appSettings::method << "] , exit now.";
		exit(0);
	}


	string fileName = appSettings::dataDir + "\\" + appSettings::fileOutput;
	cout << "\nSaving output to " << fileName << " ..";


	modelExporter.exportToSingleFile_ComposeFinal( fileName );
	cout << "\nEnd program";


    return 0;
}

