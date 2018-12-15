#pragma once

#include "fileLoader.h"
#include <string>

using namespace std;

class fileLoader;
class Model;

class ModelLoader
{
public:

	string fileName;
	Model *model;

	fileLoader fL;

	ModelLoader(void);
	~ModelLoader(void);

	void loadModel();

};

