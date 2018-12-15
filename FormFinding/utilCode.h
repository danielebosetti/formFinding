#pragma once

//#include "stdafx.h"

#include <string>
#include <map>
#include <vector>

class Vector3D;


bool is_number(const std::string& s);
std::string materialNameString(int materialName);
bool openDialog(std::string initialDir, std::string& fileNameOut);
void parseParameters(int argc, char* argv[]);
void parseParameters_for_cmd(int argc, char* argv[]);
void exitWithMessage(std::string msg);
//void writeToIni(char* paramName, double value);
//void writeToIni(char* paramName, long value);


template <class T> void deleteAllMap(std::map<long,T*>& m);
template <class T> void deleteAllVector(std::vector<T*>& v);

template <class T>
void deleteAllMap(map<long,T*>& m){
	for (map<long,T*>::iterator it=m.begin(); it!=m.end(); ++it)
		if(it->second) delete it->second;
	m.clear();
}


template <class T>
void deleteAllVector(vector<T*>& v){
	for (unsigned long i=0; i<v.size(); i++)
		if (v[i]) delete v[i];
	v.clear();
}



class Node;
class Beam;
class Plate;
class Model;
class ModelLoader;
class WorkModel;
class ModelRenderer;
class ModelExporter;

//class Vector3d;

bool is_NODE_TC_L(Node *n);

bool is_NODE_STEEL(Node *n);
bool is_NODE_I(Node *n);
bool is_NODE_TC(Node *n);
bool is_NODE_RL(Node *n);


bool is_BEAM_for_REAZ_V(Beam *b);
bool is_BEAM_TC(Beam *b);
bool is_PLATE_TC(Plate *b);
bool is_ELEM_TC_SOAP(Plate *b);

bool is_ELEM_STEEL(Beam *b);
bool is_ELEM_TC_SOAP(Beam *b);
bool is_ELEM_RL(Beam *b);
bool is_ELEM_TC_CE(Beam *b);




// forward declaration of App Data
extern Model originalModel;
extern Model workingModel;

extern ModelLoader originalModelLoader;
extern ModelLoader workingModelLoader;

extern WorkModel workModel;
extern WorkModel workModelOrig;

extern ModelRenderer modelRenderer;
extern ModelExporter modelExporter;