#pragma once


#include <map>
#include <vector>
#include <iostream>
#include <fstream>


//#include "stdafx.h"
using namespace std;



template <class T>
void writeAllMapToStream(map<long, T*>& m, ostream& ofs) {
	for (map<long, T*>::iterator it = m.begin(); it != m.end(); ++it)
		//		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
		if (it->second) it->second->exportToStream(ofs);
}

template <class S, class T>
void writeAllMapToStream2(map<S, T*>& m, ostream& ofs) {
	for (map<S, T*>::iterator it = m.begin(); it != m.end(); ++it)
		//		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
		if (it->second) it->second->exportToStream(ofs);
}



template <class T>
void writeAllVectorToStream(vector<T*>& v, ostream& os) {
	for (unsigned long i = 0; i<v.size(); i++)
		//		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
		if (v[i]) v[i]->exportToStream(os);
}



template <class T>
void writeAllMapToFile(map<long, T*>& m, const char* fileName) {
	if (DISABLE_LOG_1) cout << "\n export " << fileName << " ";
	ofstream ofs;
	ofs.open(fileName, ofstream::out);

	//	for (map<long,T*>::iterator it=m.begin(); it!=m.end(); ++it)
	////		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
	//		if(it->second) it->second->exportToStream(ofs);
	writeAllMapToStream<T>(m, ofs);

	ofs.close();

}

template <class S, class T>
void writeAllMapToFile2(map<S, T*>& m, const char* fileName) {
	if (DISABLE_LOG_1) cout << "\n export " << fileName << " ";
	ofstream ofs;
	ofs.open(fileName, ofstream::out);

	//	for (map<long,T*>::iterator it=m.begin(); it!=m.end(); ++it)
	////		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
	//		if(it->second) it->second->exportToStream(ofs);
	writeAllMapToStream2<S, T>(m, ofs);

	ofs.close();

}



template <class T>
void writeAllVectorToFile(vector<T*>& v, const char* fileName) {
	if (DISABLE_LOG_1) cout << "\n export " << fileName << " ";
	ofstream ofs;
	ofs.open(fileName, ofstream::out);

	//	for (unsigned long i=0; i<v.size(); i++)
	////		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
	//		if (v[i]) v[i]->exportToStream(ofs);
	writeAllVectorToStream<T>(v, ofs);
	ofs.close();

}

