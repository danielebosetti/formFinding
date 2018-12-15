#include <stdafx.h>

using namespace std;


DWORD WINAPI MyThreadProc(void* pContext);
void readParamFromIni();


int main(int argc, char* argv[]) {

	parseParameters_for_cmd(argc, argv);
	// check settings exists
	appSettings::initAppSettings();


	appSettings::checkKey();
	if (!appSettings::LICENSE_CHECK_OK)
		return 0;

	appSettings::originalFilePath = appSettings::dataDir + "sampleCushion_01.txt";
	appSettings::workingFilePath = appSettings::dataDir + "sampleCushion_01.txt";

	//cout << "\nParsing parameters..";
	//parseParameters(argc, argv);

	appSettings::workingFilePath = appSettings::dataDir + appSettings::fileInput;
	appSettings::originalFilePath = appSettings::dataDir + appSettings::fileInput;


	cout << "\nParsing settings file..";
	appSettings::readParamFromIni();


	//*************************************************************************************************************************
	//**    LOAD MODEL s
	cout << "\nLoading model..";

	originalModelLoader.model = &originalModel;
	originalModel.modelNAME = "originalModel";
	originalModelLoader.fileName = appSettings::originalFilePath;
	originalModelLoader.loadModel();

	workingModelLoader.model = &workingModel;
	workingModel.modelNAME = "workingModel";
	workingModelLoader.fileName = appSettings::workingFilePath;
	workingModelLoader.loadModel();

	//*************************************************************************************************************************
	//******    WORK MODEL
	//here1
	cout << "\nInit work..";

	workModelOrig.inputPath = appSettings::dataDir + "\\";
	workModelOrig.outputPath = appSettings::dataDir + "\\";
	workModelOrig.model = &originalModel;
	workModelOrig.initWork();
	workModelOrig.setDeltaT(appSettings::TIME_STEP);



	workModel.inputPath = appSettings::dataDir;
	workModel.outputPath = appSettings::dataDir;
	workModel.model = &workingModel;
	workModel.initWork();
	workModel.setDeltaT(appSettings::TIME_STEP);


	modelRenderer.model = &workingModel;
	modelRenderer.setWorkModel(&workModel);

	modelExporter.originalModel = &originalModel;
	modelExporter.workingModel = &workingModel;
	modelExporter.outBasePath = appSettings::dataDir;



	// show win32 window
	HANDLE h = CreateThread(NULL, 0, MyThreadProc, NULL, 0L, NULL);


	modelRenderer.modelExporter = &modelExporter;
	// show graphic window
	modelRenderer.init3DView(&workingModel, argc, argv);
	modelRenderer.show3DView();


	cout << "\nEnd program.\n\n";
	system("PAUSE");
	return 0;

}











DWORD WINAPI MyThreadProc(void* pContext) {
	winHelper *wh = new winHelper();

	// gives the handle to the window
	wh->setWorkModel(workModel);
	wh->showWin();
	return 0;
}
