#include "StdAfx.h"
#include "MainWindow.h"
#include "INI_io.h"


//#define IDC_MAIN_BUTTON	101
#define IDC_MAIN_EDIT	102
#define IDC_STOP_BUTTON	103
#define IDC_GO_BUTTON	104
#define IDC_SET_PARAMS_BUTTON	105
#define IDC_DELTAT_EDIT	106
//#define IDC_MAX_POS_STEP_EDIT	116
#define IDC_RENDERSTEPS_EDIT	107
#define IDC_RENDER_ITERATION_STEPS_EDIT	108
#define IDC_GOITERATION_BUTTON	109
#define IDC_OPENMODEL_BUTTON	110
//#define IDC_EXPORTMODEL_BUTTON	111
//#define IDC_GOITERATION_PARALLEL_BUTTON	112
#define IDC_EXPORTMODELFINAL_BUTTON	113
#define IDC_TOGGLE_PLATERESTRAINTS_BUTTON	114
#define IDC_TOGGLE_SHOWPLATES_BUTTON	117
//#define IDC_TOGGLE_JACOBIAN	118
#define IDC_CHANGE_USER_ALPHA_LOADPATTERNS	119
#define WINBUTTON_GO_FORCEDENSITY 120


HWND hEdit;
HWND hEdit_deltaT;
HWND hEdit_RENDERSTEPS;
HWND hEdit_mpx;
HWND hEdit_RENDER_ITERATION_STEPS;


winHelper *wh;
WorkModel *wm2;
stringstream ss;
void goIteration();
void goForceDensity();
void goDisplay();
void forceRedraw();
///void glutPostRedisplay();
//presi da MODEL RENDERER!!
extern Model *mod;
extern WorkModel *wm;
extern ModelRenderer *mr;


void logToWindow(string s){
	// SET TEXT
	ss << s;
	SendMessageA(hEdit, WM_SETTEXT, NULL, (LPARAM)ss.str().c_str());
	SendMessage( hEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

winHelper::winHelper(void)
{
}


winHelper::~winHelper(void)
{
}

void winHelper::setWorkModel(WorkModel& workmodel){
	wm2 = &workmodel;
}

LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

int winHelper::showWin(){
	wh = this;

	int nShowCmd = 10;

	HINSTANCE hInst = NULL;

	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));
	wClass.cbClsExtra=NULL;
	wClass.cbSize=sizeof(WNDCLASSEX);
	wClass.cbWndExtra=NULL;
	wClass.hbrBackground=(HBRUSH)COLOR_WINDOW;
	wClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon=NULL;
	wClass.hIconSm=NULL;
	wClass.hInstance=hInst;
	wClass.lpfnWndProc=(WNDPROC)WinProc;
	wClass.lpszClassName="Window Class";
	wClass.lpszMenuName=NULL;
	wClass.style=CS_HREDRAW|CS_VREDRAW;

	// registerClass
	if(!RegisterClassEx(&wClass)) {
		int nResult=GetLastError();
		MessageBoxA(NULL, "Window class creation failed\r\n", "Window Class Failed", MB_ICONERROR);
	}

	// create main window
	HWND hWnd=CreateWindowExA(NULL, "Window Class", ".: Form Finding :.", WS_OVERLAPPEDWINDOW,
			5, 5, 310, 600,
			NULL, NULL, hInst, NULL);
	//error checking
	if(!hWnd)	{
		int nResult=GetLastError();
		MessageBoxA(NULL, "Window creation failed\r\n", "Window Creation Failed", MB_ICONERROR);
	}

    ShowWindow(hWnd,nShowCmd);

	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));

	//MSG cycle
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

// gets a number from the text-box and writes to the model
void sendDoubleToWorkModel(HWND& handle, double& dest, string paramName){
	char buffer[2501];
	SendMessageA(handle, WM_GETTEXT, 2500, (LPARAM)buffer);
	stringstream ss;
	//here3
	if (is_number(buffer)) {
		double newParam = atof(buffer);
		ss << paramName << " = " << newParam << "\r\n";
		logToWindow(ss.str());
		dest = newParam;
	}
	else{
		ss.clear();
		ss << "Error: " << paramName << " is not a number \r\n";
		logToWindow(ss.str());
	}
}

// gets a number from the text-box and writes to the model
void sendLongToWorkModel(HWND& handle, long& dest, string paramName){
	char buffer[2501];
	SendMessageA(handle, WM_GETTEXT, 2500, (LPARAM)buffer);
	stringstream ss;
	if (is_number(buffer)) {
		long newParam = atol(buffer);
		ss << paramName << " = " << newParam << "\r\n";
		logToWindow(ss.str());
		dest = newParam;
	}
	else{
		ss.clear();
		ss << "Error: " << paramName << " is not a number \r\n";
		logToWindow(ss.str());
	}
}

// main window loop
LRESULT CALLBACK WinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		// executes on window creation
		case WM_CREATE:
		{
			int xPos = 10, yPos = 10;
			int buttW = 40, buttH = 20;

			// gets the font to apply to all the objects
			HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);
			// Create an edit box
			hEdit=CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD|WS_VISIBLE| WS_VSCROLL | WS_HSCROLL |ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				5, 5, 300 -15, 150,
				hWnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			// SET FONT
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			// SET TEXT
			SendMessageA(hEdit, WM_SETTEXT, NULL, (LPARAM)"Ready..");


			yPos = 180;
			// Create a STOP button
			HWND hWndButton2=CreateWindowExA(NULL, "BUTTON", "Block", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos, yPos, 70, buttH,
				hWnd, (HMENU)IDC_STOP_BUTTON, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			// Create a GO button
			HWND hWndButton3=CreateWindowExA(NULL, "BUTTON", "Unblock", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos+80, yPos, 70, buttH,
				hWnd, (HMENU)IDC_GO_BUTTON, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton3, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			yPos += 25;

			// Create a deltaT TextBox
			hEdit_deltaT=CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD|WS_VISIBLE,
				xPos, yPos, 70, buttH,
				hWnd, (HMENU)IDC_DELTAT_EDIT, GetModuleHandle(NULL), NULL);
			SendMessage(hEdit_deltaT, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			char buffer[100];
			sprintf_s(buffer, "%.12g", wm2->getDeltaT());
			SendMessageA(hEdit_deltaT, WM_SETTEXT, NULL, (LPARAM)buffer);

			// Create a deltaT Label
			HWND hWndLabel1=CreateWindowExA(WS_EX_CLIENTEDGE, "STATIC", "", WS_CHILD|WS_VISIBLE,
				xPos+80, yPos, 160, buttH,
				hWnd, (HMENU)IDC_DELTAT_EDIT+1000, GetModuleHandle(NULL), NULL);
			SendMessage(hWndLabel1, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			SendMessageA(hWndLabel1, WM_SETTEXT, NULL, (LPARAM)"Delta_T");

			yPos += 25;
			
			// Create a ITERATION TextBox
			hEdit_RENDERSTEPS=CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD|WS_VISIBLE,
				xPos, yPos, 70, buttH,
				hWnd, (HMENU)IDC_RENDERSTEPS_EDIT, GetModuleHandle(NULL), NULL);
			// SET FONT
			SendMessage(hEdit_RENDERSTEPS, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			// SET TEXT
			sprintf_s(buffer, "%d", appSettings::RENDERSTEPS);
			SendMessageA(hEdit_RENDERSTEPS, WM_SETTEXT, NULL, (LPARAM)buffer);

			// Create a ITERATION Label
			HWND hWndLabel2=CreateWindowExA(WS_EX_CLIENTEDGE, "STATIC", "", WS_CHILD|WS_VISIBLE,
				xPos+80, yPos, 160, buttH,
				hWnd, (HMENU)IDC_RENDERSTEPS_EDIT+1000, GetModuleHandle(NULL), NULL);
			SendMessage(hWndLabel2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			SendMessageA(hWndLabel2, WM_SETTEXT, NULL, (LPARAM)"Iteration Steps");

			yPos += 25;

			// Create a RENDER_ITERATION_STEPS TextBox
			hEdit_RENDER_ITERATION_STEPS=CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD|WS_VISIBLE,
				xPos, yPos, 70, buttH,
				hWnd, (HMENU)IDC_RENDER_ITERATION_STEPS_EDIT, GetModuleHandle(NULL), NULL);
			SendMessage(hEdit_RENDER_ITERATION_STEPS, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			sprintf_s(buffer, "%d", appSettings::RENDER_ITERATION_STEPS);
			SendMessageA(hEdit_RENDER_ITERATION_STEPS, WM_SETTEXT, NULL, (LPARAM)buffer);

			// Create a RENDER_ITERATION_STEPS Label
			HWND hWndLabel3=CreateWindowExA(WS_EX_CLIENTEDGE, "STATIC", "", WS_CHILD|WS_VISIBLE,
				xPos+80, yPos, 160, buttH,
				hWnd, (HMENU)IDC_RENDER_ITERATION_STEPS_EDIT+1000, GetModuleHandle(NULL), NULL);
			SendMessage(hWndLabel3, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));
			SendMessageA(hWndLabel3, WM_SETTEXT, NULL, (LPARAM)"Render Steps");

			yPos += 25;
			// Create a push button
			HWND hWndButton=CreateWindowExA(NULL, "BUTTON", "Set Params", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos, yPos, 100, buttH,
				hWnd, (HMENU)IDC_SET_PARAMS_BUTTON, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			// Create a GO_ITERATION button
			HWND hWndButton4=CreateWindowExA(NULL, "BUTTON", "Go Iteration", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos+100, yPos, 100, buttH,
				hWnd, (HMENU)IDC_GOITERATION_BUTTON, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton4, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			yPos += 25;

			// Create a GO_FORCE_DENSITY button
			HWND hWndButton_fd = CreateWindowExA(NULL, "BUTTON", "Go Force Density", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				xPos, yPos, 100, buttH,
				hWnd, (HMENU)WINBUTTON_GO_FORCEDENSITY, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton_fd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

			yPos += 25;


			// Create a RESTRAINT_PLATE_TC DX DY checkbox
			HWND hWndButtonCB1=CreateWindowExA(NULL, "BUTTON", "Restraint PLATE TC DX, DY", 
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                     xPos, yPos, 185, 20,        
                     hWnd, (HMENU) IDC_TOGGLE_PLATERESTRAINTS_BUTTON, GetModuleHandle(NULL), NULL);
			//CheckDlgButton(hWnd, IDC_TOGGLE_PLATERESTRAINTS_BUTTON, BST_CHECKED);
			SendMessage(hWndButtonCB1, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			yPos += 25;
			// Create a SHOWPLATES checkbox
			HWND hWndButtonCB2=CreateWindowExA(NULL, "BUTTON", "Toggle Show Plates", 
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                     xPos, yPos, 185, 20,        
                     hWnd, (HMENU) IDC_TOGGLE_SHOWPLATES_BUTTON, GetModuleHandle(NULL), NULL);
			CheckDlgButton(hWnd, IDC_TOGGLE_SHOWPLATES_BUTTON, BST_CHECKED);
			SendMessage(hWndButtonCB2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			yPos += 35;
			// Create a CHANGE_USER_ALPHA_LOADPATTERNS button
			HWND hWndButton_CUA=CreateWindowExA(NULL, "BUTTON", "Change FFC..", 
				WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos, yPos, 160, buttH,
				hWnd, (HMENU)IDC_CHANGE_USER_ALPHA_LOADPATTERNS, GetModuleHandle(NULL), NULL);
			// set FONT
			SendMessage(hWndButton_CUA, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));


			yPos += 40;
			// Create a OPEN_FILE button
			HWND hWndButton6=CreateWindowExA(NULL, "BUTTON", "Open Model MOD_FULL_DEF..", 
				WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos, yPos, 250, buttH,
				hWnd, (HMENU)IDC_OPENMODEL_BUTTON, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton6, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));


			yPos += 25;
			// Create a EXPORT_MODEL_FINAL button
			HWND hWndButton8=CreateWindowExA(NULL, "BUTTON", "Export Model MOD_FULL_F", 
				WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
				xPos, yPos, 250, buttH,
				hWnd, (HMENU)IDC_EXPORTMODELFINAL_BUTTON, GetModuleHandle(NULL), NULL);
			SendMessage(hWndButton8, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

		}
		break;

		// executes on events
		case WM_COMMAND:
			switch(LOWORD(wParam))
            {
				case IDC_STOP_BUTTON: {
					wm2->stopIterations = true;
					logToWindow(string("Msg: Dynamic Relaxation is stopped\r\n"));
				} break;
				case IDC_GO_BUTTON: {
					wm2->stopIterations = false;
					logToWindow(string("Msg: Resume Dynamic Relaxation\r\n"));
				} break;
				case IDC_GOITERATION_BUTTON: {
					logToWindow(string("Msg: Go Iteration()\r\n"));
					clock_t begin = clock();

					for (long i = 0; i<appSettings::RENDER_ITERATION_STEPS; i++) {
						goIteration();

					}
					clock_t end = clock();
					double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
					stringstream s2;
					s2 << "Time Elapsed for iterations: " << elapsed_secs << "\r\n";
					logToWindow(s2.str());


				} break;

				case WINBUTTON_GO_FORCEDENSITY: {
					logToWindow(string("Msg: Start Force Density computation\r\n"));
					clock_t begin = clock();

					Application::getInstance()->setWorkModel(wm2);
					Application::getInstance()->goForceDensity();

					clock_t end = clock();
					double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
					stringstream s2;
					s2 << "Time Elapsed for Force Density: " << elapsed_secs << " sec.\r\n";
					logToWindow(s2.str());
					goDisplay();
					forceRedraw();
				} break;


				case IDC_CHANGE_USER_ALPHA_LOADPATTERNS: {
					wm->loadUserAlpha_fromFile();
					wm->recompute_for_FFC();
				} break;


				case IDC_TOGGLE_PLATERESTRAINTS_BUTTON: {
					BOOL checked = IsDlgButtonChecked(hWnd, IDC_TOGGLE_PLATERESTRAINTS_BUTTON);
					if (checked) {
						CheckDlgButton(hWnd, IDC_TOGGLE_PLATERESTRAINTS_BUTTON, BST_UNCHECKED);
						wm->restraintPlatesTC_DXDY = false;
					} else {
						CheckDlgButton(hWnd, IDC_TOGGLE_PLATERESTRAINTS_BUTTON, BST_CHECKED);
						wm->restraintPlatesTC_DXDY = true;
					}
				} break;

				case IDC_TOGGLE_SHOWPLATES_BUTTON: {
					BOOL checked = IsDlgButtonChecked(hWnd, IDC_TOGGLE_SHOWPLATES_BUTTON);
					if (checked) {
						CheckDlgButton(hWnd, IDC_TOGGLE_SHOWPLATES_BUTTON, BST_UNCHECKED);
						mr->showPlates = false;
					} else {
						CheckDlgButton(hWnd, IDC_TOGGLE_SHOWPLATES_BUTTON, BST_CHECKED);
						mr->showPlates = true;
					}
					
					forceRedraw();
				} break;

				case IDC_SET_PARAMS_BUTTON: {
					// GET TEXT
					double newDeltaT;
					sendDoubleToWorkModel(hEdit_deltaT, newDeltaT, "Delta_T");
					INI_io::writeToIni("TIME_STEP", newDeltaT);
					wm2->setDeltaT(newDeltaT);

					sendLongToWorkModel(hEdit_RENDERSTEPS, appSettings::RENDERSTEPS, "Iteration Steps");
					INI_io::writeToIni("RENDERSTEPS", appSettings::RENDERSTEPS);

					sendLongToWorkModel(hEdit_RENDER_ITERATION_STEPS, appSettings::RENDER_ITERATION_STEPS, "Render Steps");
					INI_io::writeToIni("RENDER_ITERATION_STEPS", appSettings::RENDER_ITERATION_STEPS);

				} break;

				case IDC_OPENMODEL_BUTTON:{
					string newFileName("");
					if (openDialog(wm2->inputPath, newFileName)){
						// CLOSE current working model, and repoen
						// new working file name
						appSettings::workingFilePath = newFileName;
						// delete all beams, nodes, etc.
						workingModel.clearModel();
						// sets new file name
						workingModelLoader.fileName = appSettings::workingFilePath;
						// loads model.
						// note: it cleans old data
						workingModelLoader.loadModel();
						// workModel: clear data and redo 
						workModel.clearWorkModel();
						workModel.initWork();
						//re draw
						goDisplay();
						forceRedraw();
					}
				} break;
				case IDC_EXPORTMODELFINAL_BUTTON:{
					time_t rawtime;
					struct tm timeinfo;
					time(&rawtime);
					localtime_s(&timeinfo, &rawtime);
			
					char fileName[1000];
					sprintf_s(fileName, "%s\\MOD_FULL_F_FINAL %04d-%02d-%02d %02d-%02d-%02d.txt", 
						mr->modelExporter->outBasePath.c_str(), 
						timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, 
						timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );
					mod->generateNdForce_REAZ_V(wm->ffds.REAZ_V);
					mod->generateBmPreload_FORCE_DENSITY(wm->ffds.FORCE_DENSITY);
					mr->modelExporter->exportToSingleFile_ComposeFinal(string(fileName));
					char msg[1000];
					sprintf_s(msg, "Exported as <%s>", fileName);
					MessageBoxA(NULL, msg , "File Saved",0);
				} break;

			}
			break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			exit(0);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}


