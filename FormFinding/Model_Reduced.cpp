#include "stdafx.h"
#include "Model_Reduced.h"


Model_Reduced::Model_Reduced()
{
	this->sectionFirstLine = NULL;
	this->sectionLinesCount = NULL;
}


Model_Reduced::~Model_Reduced()
{
}

Model_Reduced::Model_Reduced(const Model_Reduced & model)
{
	// copy all nodes
	for (auto x : model.nodes) {
		Node *n = x.second;
		long node_id = n->ID;
		Node *node_new = new Node(*n);
		this->nodes[node_id] = node_new;
		node_new->init_position();
	}

	// copy all beams
	for (auto x : model.beams) {
		Beam *b = x.second;
		long beam_id = b->ID;
		this->beams[beam_id] = new Beam(*b);
	}

	// copy all restraints
	for (auto x : model.restraints) {
		Restraint *r = x;
		this->restraints.push_back(new Restraint(*r));
	}

	// copy all plates
	for (auto x : model.plates) {
		Plate *p = x.second;
		long plate_id = p->ID;
		this->plates[plate_id] = new Plate(*p);
	}


	//..do inits here!
	this->initRestrainedSets();
	this->linkNodesToBeams();
	this->updateNodePositions();
	this->initBeamLengths();
	this->initAdjacentSetsForNodes();

	// VERY UGLY, TODO FIX THIS
	this->sectionsCount = model.sectionsCount;
	this->sectionFirstLine = model.sectionFirstLine;
	this->sectionLinesCount = model.sectionLinesCount;
	this->linesToCopy = model.linesToCopy;
}

void Model_Reduced::exportSectionToStream(int i, ostream & ofs)
{
	long j;
	if (sectionFirstLine == NULL || sectionLinesCount == NULL)
		exitWithMessage("\nERROR\nexportSectionToStream : section not initialized\n" );

	for (j = sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++) {
		ofs << linesToCopy[j] << "\n";
	}
}

void Model_Reduced::clearModel()
{
	deleteAllMap<Node>(nodes);
	deleteAllMap<Beam>(beams);
	deleteAllMap<Plate>(plates);
	deleteAllVector<Restraint>(restraints);

}

void Model_Reduced::printModelSummary()
{
}

void Model_Reduced::writeCommentsToFiles(string basePathOut)
{
	ofstream ofs;
	string baseFileName = basePathOut + "\\x_section";
	//	string lineOut;
	char fileName[1000];


	long i, j;

	cout << "\n exporting " << sectionsCount << " sections \n";

	for (i = 1; i <= sectionsCount; i++) {

		sprintf_s(fileName, "%s%02d.txt", baseFileName.c_str(), i);
		cout << fileName << endl;
		ofs.open(fileName, ofstream::out);


		for (j = sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++) {
			ofs << linesToCopy[j] << "\n";
			//printf("\n %s", linesToCopy[j]);
		}

		ofs.close();
	}
}

void Model_Reduced::linkNodesToBeams()
{
	// push node refs into nodes
	for (auto item : beams) {
		Beam *b = item.second;
		Node* n1 = nodes[b->node1];
		Node* n2 = nodes[b->node2];
		b->pNode1 = n1;
		b->pNode2 = n2;
	}

}

void Model_Reduced::initRestrainedSets()
{
	// clear restraint flags  on nodes
	for (auto pair : nodes) {
		Node* n = pair.second;
		n->has_Restraint = false;
	}

	// split element, find the constrained points and the
	// floating points
	for (auto& r : restraints) {
		nodes[r->nodeID]->has_Restraint = true;
	}

	// clear nodes set
	nodes_free.clear();
	nodes_restrained.clear();

	// push restrained nodes to their set
	for (auto pair : nodes) {
		Node* n = pair.second;
		if (n->has_Restraint) {
			nodes_restrained.push_back(n);
		}
	}
	// push free nodes to their set
	for (auto pair : nodes) {
		Node* n = pair.second;
		if (!n->has_Restraint) {
			nodes_free.push_back(n);
		}
	}
}


void Model_Reduced::updateNodePositions()
{
	// init node positions
	for (auto item : nodes) {
		Node *n = item.second;
		n->init_position();
	}
}


// TODO: rename into updateBeamLengths
void Model_Reduced::initBeamLengths()
{
	// push beams initial length into beams
	for (auto item : beams) {
		Beam *b = item.second;
		b->initialLength = b->getLength();
	}
}

// returns the corresponding node
Node * Model_Reduced::getCorrespondingNodeInThisModel(Node * n)
{
	Node *res = this->nodes[n->ID];
	return res;
}

void Model_Reduced::initAdjacentSetsForNodes()
{

	// clear adjacent sets for nodes
	for (auto item : nodes)
		item.second->adjacentNodes.clear();
	// LOOP over all beams
	for (auto& item : this->beams) {
		Beam* b = item.second;
		Node *n1 = nodes[b->node1];
		Node *n2 = nodes[b->node2];
		// generate adjacent nodes
		n1->adjacentNodes.insert(n2);
		n2->adjacentNodes.insert(n1);
	}
}


