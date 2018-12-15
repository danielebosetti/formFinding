#include "StdAfx.h"
#include "GraphSplit.h"


GraphSplit::GraphSplit(void)
{
	model = NULL;
	iStack = 0;
}


GraphSplit::~GraphSplit(void)
{
}

void GraphSplit::initWithModel(Model *m){
	model = m;
}

void GraphSplit::clearRigidGroups(){
	nodeList.clear();
	nodeColorMap.clear();


}


#define COLOR_WHITE 1
#define COLOR_GRAY 2
#define COLOR_BLACK 3


	//map<Node*, long> colorMap;
	//Beam* stack[10000];
	//long iStack;

void GraphSplit::pushNode(Node* n){
	stack[iStack] = n;
	nodeColorMap[n] = COLOR_GRAY;
	iStack++;
	if (iStack > 9000 ) {cout << " STACK ERROR, exit now\n\n"; system("PAUSE"); exit(0);}
}

Node* GraphSplit::popNode(){
	if (iStack == 0) 
		return NULL;
	iStack--;
	Node *res = stack[iStack];

	nodeColorMap[res] = COLOR_BLACK;
	return res;
}


void GraphSplit::initColors(){
	Beam *b;
	// LOOPS over beams
	BOOST_FOREACH (mapElement_Beam b1 , model->beams) {
		// get beam
		b = b1.second;

		if (b->isRigidLink) {
			// push graph elements
			nodeColorMap[b->pNode1] = COLOR_WHITE;
			nodeColorMap[b->pNode2] = COLOR_WHITE;

			nodeList.push_back(b->pNode1);
			nodeList.push_back(b->pNode2);
		}
	}
}

void GraphSplit::goBFS(Node *n){

	RigidGroup *rg = new RigidGroup();

	// first node
	pushNode(n);

	// while there are more nodes
	while (n = popNode()){
		// pops node, push into RigidLink_nodeSet
		rg->nodeList.push_back(n);

		BOOST_FOREACH (Beam *b , n->incidentBeams ) {
			// only for RIGID beams!
			if(!b->isRigidLink) continue;
			// gets node
			Node *node1 = b->pNode1;
			if (nodeColorMap[node1] == COLOR_WHITE)
				// push only white nodes!
				pushNode(node1);
			// gets node
			Node *node2 = b->pNode2;
			if (nodeColorMap[node2] == COLOR_WHITE)
				// push only white nodes!
				pushNode(node2);
		}
	}

	//if (rg->nodeList.size() > 1)
	//cout << "\n  Rigid link computed, it has " << rg->nodeList.size() << " nodes ";

	// push rigidGroup to model!
	model->rigidGroups.push_back(rg);

}


void GraphSplit::computeRigidGroups(){
	// LOOPS over nodes
	BOOST_FOREACH (Node* n , nodeList) {
		if (nodeColorMap[n] == COLOR_WHITE){
			//cout << "\n  BFS from Node " << n->ID;
			goBFS(n);
		}
	}

	cout <<"\nRigid Groups computed, model has " << model->rigidGroups.size() << " disconnected groups\n";
	
}







