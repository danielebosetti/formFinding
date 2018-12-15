#pragma once

#include <string>
#include <iostream>


//#include "stdafx.h"
using namespace std;


class Model;


class ModelExporter
{
public:
	Model *originalModel;
	Model *workingModel;

	string outBasePath;

	ModelExporter(void);
	~ModelExporter(void);

//	void exportModel();
//	void exportToSingleFile(string fileName);
	//void exportToSingleFile_Compose(string fileName);
	void exportToSingleFile_ComposeFinal(string fileName);

	//void exportToSingleFile2(string fileName);
};

