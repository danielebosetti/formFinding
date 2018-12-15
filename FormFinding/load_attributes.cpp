#include "StdAfx.h"
#include "WorkModel.h"
#include <algorithm>
//WorkModel::


void readStringListFromFile(vector<string>& stringList, string fullFileName);
void readStringListFromFile(vector<string>& stringList, vector<double>& stringList2,
							vector<double>& stringList3, string fullFileName);

void WorkModel::loadPropertyFiles(){
	readStringListFromFile(TC_propertyNames, appSettings::settingsDir + "properties_TC.txt" );
	readStringListFromFile(STEEL_propertyNames, appSettings::settingsDir + "properties_Steel.txt" );
	readStringListFromFile(RIGIDLINKS_propertyNames, appSettings::settingsDir + "properties_rigidlLink.txt" );
	readStringListFromFile(ELASTICBEAM_propertyNames, ELASTICBEAM_modulusMultiplier,
						   ELASTICBEAM_stiffnessMax, appSettings::settingsDir + "properties_elasticBeam.txt" );

}


// sets the attributes to the BmProperties
void WorkModel::setAttributes_BmProperties(){

	// LOOPS over BeamProperties
	BOOST_FOREACH (mapElement_BeamProperty bp1, model->beamProperties) {
		BeamProperty *beamProperty = bp1.second;

		// set RIGID_LINK ( => and TC )
		BOOST_FOREACH (string RIGIDLINKS_propName, RIGIDLINKS_propertyNames)
			if (beamProperty->name == RIGIDLINKS_propName) {
				beamProperty->isRigidLink = true;
				beamProperty->material_type = TC;
			}

		
		//BOOST_FOREACH (string ELASTICBEAM_propName, ELASTICBEAM_propertyNames)
		// set ELASTIC ( => and TC )
		for(unsigned int i=0; i<ELASTICBEAM_propertyNames.size(); i++) {
			string ELASTICBEAM_propName = ELASTICBEAM_propertyNames[i];
			if (beamProperty->name == ELASTICBEAM_propName) {
				beamProperty->isElastic = true;
				beamProperty->material_type = TC;
				beamProperty->stiffnessMax = ELASTICBEAM_stiffnessMax[i];
				beamProperty->modulusMultiplier = ELASTICBEAM_modulusMultiplier[i];
				//cout << "\nElastic Beam Properties: " << ELASTICBEAM_propName << " stiff-Max: "  
				//	 << beamProperty->stiffnessMax << " mod-Multi:"   << beamProperty->modulusMultiplier << " "  ;
			}
		}


		// set TC
		BOOST_FOREACH (string TC_propName, TC_propertyNames)
			if (beamProperty->name == TC_propName)
				beamProperty->material_type = TC;
		// set STEEL
		BOOST_FOREACH (string STEEL_propName, STEEL_propertyNames)
			if (beamProperty->name == STEEL_propName) {
				if (beamProperty->material_type == TC) {
					cout << "\n\n\n ******************* ERROR , check!! ****************************"<<
					"\n BeamProperty " << beamProperty->name << " e'TC ed e' anche STEEL? check this property in input files! exit now.\n";
					system("PAUSE"); exit(0);
				}
				beamProperty->material_type = STEEL;
			}


		//CHECKS!
		if (beamProperty->isRigidLink && !beamProperty->material_type == TC){
			cout << "\n\n\n ******************* ERROR , check!! ****************************"<<
				"\n BeamProperty " << beamProperty->name << " e' RIGID_LINK ma non e' TC ! (e' anche STEEL?) check input files!";
			system("PAUSE");
		}
		if (beamProperty->isElastic && !beamProperty->material_type == TC){
			cout << "\n\n\n ******************* ERROR , check!! ****************************"<<
				"\n BeamProperty " << beamProperty->name << " e' ELASTIC ma non e' TC ! (e' anche STEEL?) check input files!";
			system("PAUSE");
		}


	}
}

// sets the attributes to the PlProperties
void WorkModel::setAttributes_PlProperties(){

	// LOOPS over PlProperties
	BOOST_FOREACH (mapElement_PlateProperty pp1, model->plateProperties) {
		PlateProperty *plateProperty = pp1.second;

		// set TC
		BOOST_FOREACH (string TC_propName, TC_propertyNames)
			if (plateProperty->name == TC_propName)
				plateProperty->material_type = TC;
		// set STEEL
		BOOST_FOREACH (string STEEL_propName, STEEL_propertyNames)
			if (plateProperty->name == STEEL_propName)
				plateProperty->material_type = STEEL;
	}
}

// sets the material_type attribute to the Plates
void WorkModel::setAttributes_Plates(){
	// LOOPS over Plates
	BOOST_FOREACH (mapElement_Plate p1, model->plates) {
		Plate *p = p1.second;
		// SET material type to PLATE
		p->material_type = p->pPlateProperty->material_type;
	}
}

// sets the attributes to the Beams
void WorkModel::setAttributes_Beams(){
	// LOOPS over Beams
	BOOST_FOREACH (mapElement_Beam b1, model->beams) {
		Beam *b = b1.second;
		// SET attributes to BEAM
		b->material_type = b->pBeamProperty->material_type;
		b->isRigidLink = b->pBeamProperty->isRigidLink;
		b->isElastic = b->pBeamProperty->isElastic;
	}
}


// sets the attributes to the Nodes
void WorkModel::setAttributes_Nodes(){
		
	// LOOPS over Beams
	BOOST_FOREACH (mapElement_Beam b1, model->beams) {
			Beam *b = b1.second;
			long node1 = b->node1;
			long node2 = b->node2;

			if (model->nodes.find(node1)==model->nodes.end()) {
				stringstream ss; ss << " Error: Beam ID :" << b->ID << " has Node:" << b->node1 << " , the node DOES NOT exist in the model!"; 
				exitWithMessage(ss.str());}
			if (model->nodes.find(node2)==model->nodes.end()) {
				stringstream ss; ss << " Error: Beam ID :" << b->ID << " has Node:" << b->node2 << " , the node DOES NOT exist in the model!"; 
				exitWithMessage(ss.str());}

			// SET attributes to NODE
			if (b->material_type == TC) {
				model->nodes[node1]->is_TC = true;
				model->nodes[node2]->is_TC = true;
			}
			else if (b->material_type == STEEL) {
				model->nodes[node1]->is_STEEL = true;
				model->nodes[node2]->is_STEEL = true;
			}
			if (b->isRigidLink == true) {
				model->nodes[node1]->isRigidLink = true;
				model->nodes[node2]->isRigidLink = true;
			}

		}

	// LOOPS over Plates
	BOOST_FOREACH (mapElement_Plate p1, model->plates) {
			Plate *p = p1.second;
			long node1 = p->node1;
			long node2 = p->node2;
			long node3 = p->node3;
			long node4 = p->node4;

			if (model->nodes.find(node1)==model->nodes.end()) {
				stringstream ss; ss << " Error: Plate ID :" << p->ID << " has Node:" << node1 << " , the node DOES NOT exist in the model!"; 
				exitWithMessage(ss.str());}
			if (model->nodes.find(node2)==model->nodes.end()) {
				stringstream ss; ss << " Error: Plate ID :" << p->ID << " has Node:" << node2 << " , the node DOES NOT exist in the model!"; 
				exitWithMessage(ss.str());}
			if (model->nodes.find(node3)==model->nodes.end()) {
				stringstream ss; ss << " Error: Plate ID :" << p->ID << " has Node:" << node3 << " , the node DOES NOT exist in the model!"; 
				exitWithMessage(ss.str());}
			if (node4!=-1) if (model->nodes.find(node4)==model->nodes.end()) {
				stringstream ss; ss << " Error: Plate ID :" << p->ID << " has Node:" << node4 << " , the node DOES NOT exist in the model!"; 
				exitWithMessage(ss.str());}

			// SET attributes to NODE
			if (p->material_type == TC) {
				model->nodes[node1]->is_TC = true;
				model->nodes[node2]->is_TC = true;
				model->nodes[node3]->is_TC = true;
				if (node4 != -1)
					model->nodes[node4]->is_TC = true;
			}
			else if (p->material_type == STEEL) {
				model->nodes[node1]->is_STEEL = true;
				model->nodes[node2]->is_STEEL = true;
				model->nodes[node3]->is_STEEL = true;
				if (node4 != -1)
					model->nodes[node4]->is_STEEL = true;
			}
		}
}


void WorkModel::setRestraint_Nodes(){
	selectedFreedomCaseID = model->selectedFreedomCase->ID;

	// LOOPS over Restraints
	BOOST_FOREACH (Restraint *r, model->restraints)
		if (r->IDFreedomCase == selectedFreedomCaseID)
			model->nodes[r->nodeID]->has_Restraint = true;

}














