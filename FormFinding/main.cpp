#include "stdafx.h"

using namespace std;


void GetRegistry(char* StringName);
DWORD WINAPI MyThreadProc(void* pContext);
void readParamFromIni();


int main(int argc, char* argv[]) {

			

	appSettings::initAppSettings();

	std::cout << "\nUsing Application path: " << appSettings::exePath << "\n";

	appSettings::checkKey();
	if (!appSettings::LICENSE_CHECK_OK)
		return 0;


	appSettings::originalFilePath = appSettings::exePath + "\\IO_txt\\sampleCushion_01.txt";
	appSettings::workingFilePath = appSettings::exePath + "\\IO_txt\\sampleCushion_01.txt";
	parseParameters(argc, argv);
	appSettings::readParamFromIni();


//*************************************************************************************************************************
//**    LOAD MODEL s
	originalModelLoader.model= &originalModel;
	originalModel.modelNAME ="originalModel";
	originalModelLoader.fileName = appSettings::originalFilePath;
	originalModelLoader.loadModel();

	workingModelLoader.model= &workingModel;
	workingModel.modelNAME ="workingModel";
	workingModelLoader.fileName = appSettings::workingFilePath;
	workingModelLoader.loadModel();

//*************************************************************************************************************************
//******    WORK MODEL
	//here1

	workModelOrig.inputPath = appSettings::exePath + "\\IO_txt\\";
	workModelOrig.outputPath = appSettings::exePath + "\\IO_txt\\";
	workModelOrig.model = &originalModel;
	workModelOrig.initWork();
	workModelOrig.setDeltaT(appSettings::TIME_STEP);



	workModel.inputPath = appSettings::exePath + "\\IO_txt\\";
	workModel.outputPath = appSettings::exePath + "\\IO_txt\\";
	workModel.model = &workingModel;
	workModel.initWork();
	workModel.setDeltaT(appSettings::TIME_STEP);


	modelRenderer.model=&workingModel;
	modelRenderer.setWorkModel(&workModel);

	modelExporter.originalModel = &originalModel;
	modelExporter.workingModel = &workingModel;
	modelExporter.outBasePath = appSettings::exePath + "\\IO_txt";

		
		
	// show win32 window
	HANDLE h = CreateThread( NULL, 0, MyThreadProc, NULL, 0L, NULL );


	modelRenderer.modelExporter = &modelExporter;
	// show graphic window
	modelRenderer.init3DView(&workingModel, argc, argv);
	modelRenderer.show3DView();


	cout <<"\nEnd program.\n\n";
	system("PAUSE");	
	return 0;

}











DWORD WINAPI MyThreadProc( void* pContext ) {
	winHelper *wh = new winHelper();

	// gives the handle to the window
	wh->setWorkModel(workModel);
	wh->showWin();
	return 0;
}
