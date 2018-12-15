
#include "stdafx.h"
#include <stdlib.h>

using namespace std;


void exitWithMessage(string msg) {
		cout << endl<<endl<< "ERROR " << msg.c_str() << " , exit now"<<endl;
		exit(0);
}

bool openDialog(string initialDir, string& fileNameOut){
	OPENFILENAME ofn;       // common dialog box structure
	//char szFile[260];       // buffer for file name
	wchar_t szFile[260];       // buffer for file name
//	HWND hwnd;              // owner window


	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
//	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = (LPSTR)szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = (LPCSTR)(L"All\0*.*\0*.TXT\0Text\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;

	////wcstombs_s
	//wchar_t dirName[260];
	//mbstowcs_s(NULL, dirName, 260, initialDir.c_str(), 260);

	//ofn.lpstrInitialDir = NULL;

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn)==TRUE) {
		char t[1000];
//		wcstombs_s( NULL, t, 999, ofn.lpstrFile, 999);
		//wcstombs(t, ofn.lpstrFile, 999);
		sprintf_s(t, "%s", ofn.lpstrFile );
		string s;
		s= string("Opening file.. <") + t +">";
		MessageBoxA(NULL, s.c_str() , "Opening: ",0);
		fileNameOut = t;
		return true;
	}
	return false;
}







//string exePath ;
//string fileName, workingFilePath, originalFilePath, outPath;
char ownPth[MAX_PATH];



//=====================================================
// APP WORKING SET
// TODO: embed into a CLASS!
Model originalModel, workingModel;
ModelLoader originalModelLoader, workingModelLoader;
WorkModel workModel, workModelOrig;
ModelRenderer modelRenderer;
ModelExporter modelExporter;



bool is_number(const std::string& s)
{
	if (s=="") return false;
    //std::string::const_iterator it = s.begin();
    //while (it != s.end() && isdigit(*it)) ++it;
	for (unsigned int i=0; i<s.size(); i++)
		if (!isdigit(s[i]) && s[i]!='.' && s[i]!='e' && s[i]!='-') return false;
	return true;
//    return !s.empty() && it == s.end();
}

string materialNameString(int materialName){
	string s;
	if (materialName == MATERIAL_UNKNOWN) s=" MATERIAL_UNKNOWN ";
	else if (materialName == TC) s=" TC ";
	else if (materialName == STEEL) s="STEEL";
	else s = " UNSUPPORTED_MATERIAL_TYPE ";

	return s;
}


void printVectorNorm(Vector3d* f){cout << " v3d_norm:" << f->norm() << " ";}
void printVector(Vector3d* f){cout << " v3d:" << f->x() << " " << f->y() << " " << f->z() << " ";}
void printVectorDetailed(Vector3d* f){	printf(" v3d: %12.6g %12.6g %12.6g", f->x(), f->y(), f->z());}

void coutVector(string label, Vector3d& v, ostream& os){
	os <<"\n" << label << "\t" << v.x()<<"\t" << v.y()<<"\t"<< v.z();
}

void appendSlashIfMissing(string& s) {
	int len = s.size();
	// append the trailing bar if missing!
	if (s.c_str()[len - 1] != '\\')
		s += "\\";
}

// parse parameters only.. with side effects ;)
// for the cmd version
void parseParameters_for_cmd(int argc, char* argv[] ) {
	string paramName, paramValue;

	for (int i = 1; i < argc; i++) {
		paramName = argv[i];

		if (paramName == "APPLICATION_DIR") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::applicationDir = paramValue;
			appendSlashIfMissing(appSettings::applicationDir);
			cout << "\n" << "APPLICATION_DIR=[" <<
				paramValue << "]";
		}
		else if (paramName == "SETTINGS_DIR") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::settingsDir = paramValue;
			appendSlashIfMissing(appSettings::settingsDir);
			cout << "\n" << "SETTINGS_DIR=[" <<
				paramValue << "]";
		}
		else if (paramName == "DATA_DIR") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::dataDir = paramValue;
			appendSlashIfMissing(appSettings::dataDir);
			cout << "\n" << "DATA_DIR=[" <<
				paramValue << "]";
		}
		else if (paramName == "FILE_INPUT") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::fileInput = paramValue;
			cout << "\n" << "FILE_INPUT=[" <<
				paramValue << "]";

		}
		else if (paramName == "FILE_OUTPUT") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::fileOutput = paramValue;
			cout << "\n" << "FILE_OUTPUT=[" <<
				paramValue << "]";
		}
		else if (paramName == "METHOD") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::method = paramValue;
			cout << "\n" << "METHOD=[" <<
				paramValue << "]";
		}
		else if (paramName == "N_ITERATIONS") {
			i++;
			if (i >= argc) { cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
			paramValue = argv[i];
			appSettings::nIterations = paramValue;
			cout << "\n" << "N_ITERATIONS=[" <<
				paramValue << "]";
		}
		else {
			cout << "\n\n ERROR IN COMMAND LINE PARAMETERS! \n";
			cout << "\n The parameters accepted are:\n";
			cout << "\n APPLICATION_DIR\t\t";
			cout << "\n SETTINGS_DIR\t\t";
			cout << "\n DATA_DIR\t\t";
			cout << "\n FILE_INPUT\t\t";
			cout << "\n FILE_OUTPUT\t\t";
			cout << "\n METHOD\t\t";
			cout << "\n N_ITERATIONS\t\t";
			cout << "\n\n Parameters are case sensitive.\n";

			exit(0);
		}

	}

}


// parse parameters.. plus side effects
// DEPRECATED
//void parseParameters(int argc, char* argv[] ){
//	string paramName, paramValue;
//
//	for(int i=1; i<argc; i++){
//		paramName = argv[i];
//
//		if (paramName == "FILE_INPUT"){
//			i++;
//			if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
//			paramValue = argv[i];
//			appSettings::workingFilePath = appSettings::exePath + "\\IO_txt\\" + paramValue;
//			appSettings::originalFilePath = appSettings::exePath + "\\IO_txt\\" + paramValue;
//		}
		//else if (paramName == "MAX_LINES"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	MAX_LINES = stol(paramValue);
		//}
		//else if (paramName == "MAX_ID"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	MAX_NODE_ID = stol(paramValue);
		//}
		//else if (paramName == "RENDERSTEPS"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	RENDERSTEPS = stol(paramValue);
		//}
		//else if (paramName == "RENDER_ITERATION_STEPS"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	RENDER_ITERATION_STEPS = stol(paramValue);
		//}
		//else if (paramName == "MAX_WORDS_PER_LINE"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	MAX_WORDS_PER_LINE = stol(paramValue);
		//}
		//else if (paramName == "MAX_POS_STEP"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	MAX_POS_STEP = stod(paramValue);
		//}

		//else if (paramName == "TIME_STEP"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	TIME_STEP = stod(paramValue);
		//}
		//else if (paramName == "ALPHA_MASS"){
		//	i++;
		//	if(i>=argc){ cout << " ERROR IN COMMAND LINE PARAMETERS! "; exit(0); }
		//	paramValue = argv[i];
		//	ALPHA_MASS = stod(paramValue);
		//}
		//else { cout << "\n\n ERROR IN COMMAND LINE PARAMETERS! \n";
		//cout <<"\n The parameters accepted are:\n";
		//cout <<"\n FILE_INPUT\t\tfile input name";
		////cout <<"\n MAX_LINES\t\tset the maximum input file lines number";
		//cout <<"\n MAX_WORDS_PER_LINE\t\tset the maximum words per line number";
		//cout <<"\n RENDERSTEPS\t\tNumber of dynamic relaxation steps between two 3D rendering";
		//cout <<"\n RENDER_ITERATION_STEPS\t\tDoes Dynamic Relaxation+ Render, this many times between two 'g' key presses";
		//cout <<"\n TIME_STEP\t\tSets the Time Step Delta_T";
		//cout <<"\n MAX_POS_STEP\t\tSets the MAX_POS_STEP";
		//cout <<"\n ALPHA_MASS\t\tSets the ALPHA_MASS";
		//cout <<"\n MAX_ID\t\tMAX ID for Nodes and Beams";


//	cout<<"\n\n";system("PAUSE");
//		exit(0); }
//	}
//}






void readStringListFromFile(vector<string>& stringList, string fullFileName){

	stringList.clear();
	string line;
	ifstream myfile;
	myfile.open( (fullFileName).c_str());
	if (myfile.is_open()) {
		while (!myfile.eof()){
			getline(myfile,line);
			stringList.push_back(line);

			if ( (line.find("\n")!=std::string::npos) ||
				(line.find("\r")!=std::string::npos) ) {
					cout << " ERROR , input file " << fullFileName << " contains wrong line delimiter ? exit now. \n\n";
					system("PAUSE"); exit(0);
			}
		}
		myfile.close();
	}
	else {
		cout << "Unable to open file <" << fullFileName << "> !! Exit now..\n";
		system("PAUSE"); exit(1);
	}
}

// 3 columns!
void readStringListFromFile(vector<string>& stringList,
							vector<double>& doubleList1,
							vector<double>& doubleList2,
							string fullFileName){
	stringList.clear();
	doubleList1.clear();
	doubleList2.clear();
	string line;
	ifstream myfile;
	myfile.open( (fullFileName).c_str());
	if (myfile.is_open()) {
		while (!myfile.eof()){
			getline(myfile,line);

			//cout << "\n Line length: " << line.length() << " ";
			if (line.length() == 0)
				continue;

			int i1 = line.find('"');
			int i2 = line.find('"', i1+1);

			if (i1==-1 || i2==-1) {
				exitWithMessage(" check <properties_elasticBeam.txt> , property_names must be enclosed by double-apices, \"  ");
			}

			//cout << "\n i1 : " << i1 << " i2:" << i2;
			string word;
			word = line.substr(i1,i2-i1+1);
			//cout << "\n PROPERTY NAME: <" << word << "> ";
			stringList.push_back(word);

			string line2 = line.substr(i2+1, line.length()-i2-1);
			//cout << " line2: <" << line2 << "> ";
			istringstream iss(line2);
		    
			
			
			try {
				
				iss >> word;doubleList1.push_back(stod(word));
				//cout << "<" << stod(word)<< "> ";
					iss >> word;doubleList2.push_back(stod(word));
				//cout << "<" << stod(word)<< "> ";

			} catch(...) {cout << "\n Error in <properties_elasticBeam.txt> , check values! ";}

			if ( (line.find("\n")!=std::string::npos) ||
				(line.find("\r")!=std::string::npos) ) {
					cout << " ERROR , input file " << fullFileName << " contains wrong line delimiter ? exit now. \n\n";
					system("PAUSE"); exit(0);
			}
		}
		myfile.close();
	}
	else {
		cout << "Unable to open file " << fullFileName << " !! Exit now..\n";
		system("PAUSE"); exit(1);
	}
}

void get_iterations(long& num_iterations) {
	try {
		num_iterations = stol(appSettings::nIterations);
	}
	catch (exception& e)
	{
		cout << " Error reading the number N_ITERATIONS: starting value is <" <<
			appSettings::nIterations << "> , error is: " << e.what() << '\n';
		exit(0);
	}
}










