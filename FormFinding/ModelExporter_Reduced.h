#pragma once

#include <string>

class Model_Reduced;

class ModelExporter_Reduced
{
public:
	ModelExporter_Reduced();
	~ModelExporter_Reduced();
	Model_Reduced *model;

	std::string outBasePath;


	void exportModel(std::string fileName);

};

