#pragma once
class Model_Reduced
{
public:
	Model_Reduced();
	~Model_Reduced();

	Model_Reduced(const Model_Reduced& model);


	string modelNAME;

	map<long, Node*> nodes;
	map<long, Beam*> beams;
	map<long, Plate*> plates;

	vector<Restraint*> restraints;

	vector<Node*> nodes_restrained;
	vector<Node*> nodes_free;


	//COPIA PARAMETRI MODEL
	// mettiamo qui tutte le LINES da copiare "pari-pari"
	char **linesToCopy;
	// sectionFirstLine[i] è l'indice in linesToCopy delle prima linea da copiare uguale
	long *sectionFirstLine;
	// sectionLinesCount[i] è il numero di linee che compongono la sezione i-esima
	long *sectionLinesCount;
	long sectionsCount;

	void exportSectionToStream(int i, ostream& ofs);


	void clearModel();

	void printModelSummary();

	void writeCommentsToFiles(string basePathOut);


	void initRestrainedSets();
	void linkNodesToBeams();
	void updateNodePositions();
	void initBeamLengths();
	void initAdjacentSetsForNodes();


	Node* getCorrespondingNodeInThisModel(Node* n);
};

