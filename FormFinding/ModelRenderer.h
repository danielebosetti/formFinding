#pragma once

//#include "stdafx.h"


class Model;
class WorkModel;
class ModelExporter;

class ModelRenderer {
public:
	Model *model;
	ModelExporter *modelExporter;

	ModelRenderer(void);
	~ModelRenderer(void);

	bool showPlates;

	void init3DView(Model *m, int argc, char* argv[]);
	void show3DView();


	void keyOperations (void);
	

	void setWorkModel(WorkModel *wm);
};

