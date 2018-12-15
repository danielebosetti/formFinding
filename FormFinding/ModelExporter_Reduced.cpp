#include "stdafx.h"
#include "ModelExporter_Reduced.h"

#include "SharedFunctions.h"
#include "Model_Reduced.h"

//#include "ModelItemsDeclare.h"
//#include <boost/foreach.hpp>


ModelExporter_Reduced::ModelExporter_Reduced()
{
}


ModelExporter_Reduced::~ModelExporter_Reduced()
{
}

void ModelExporter_Reduced::exportModel(string fileName)
{
	ofstream ofs;
	ofs.open(fileName, ofstream::out);
	if (!ofs.is_open()) {
		cout << "\n\nError, unable to open file [" << fileName << "] , exit now..";
		exit(0);
	}

	// export the first comments/misc section
	model->exportSectionToStream(1, ofs);


	ofs << endl;

	// export ALL NODES from MODEL
	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/ ALL MODEL NODES" << endl;
//	for (map<long, Node*>::iterator it = model->nodes.begin(); it != model->nodes.end(); ++it)
	for (auto& item : model->nodes)
		if (item.second)
			item.second->exportToStream(ofs);

	ofs << endl;

	ofs << endl;

	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/ ALL MODEL BEAMS" << endl;

	writeAllMapToStream<Beam>(model->beams, ofs);
	ofs << endl;

	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/ ALL MODEL PLATES" << endl;
	writeAllMapToStream<Plate>(model->plates, ofs);
	ofs << endl;

	ofs << "/ ______________________________________________________________________________" << endl;
	ofs << "/ ALL MODEL RESTRAINTS" << endl;

	writeAllVectorToStream<Restraint>(model->restraints, ofs);
	ofs << endl;



	// export all the remaining sections
	for (int i = 2; i<=model->sectionsCount; i++)
		model->exportSectionToStream(i, ofs);

	ofs.close();

}
