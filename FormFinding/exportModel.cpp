// codice fuff.. file di appoggio x routines in via di sviluppo
//#include "stdafx.h"

#include "SharedFunctions.h"
#include "ModelItemsDeclare.h"


#pragma warning (disable:4018)

template <class T>
void printAllMap(map<long,T*> m){
	for (map<long,T*>::iterator it=m.begin(); it!=m.end(); ++it)
		if(it->second) std::cout << it->first << " => " << it->second->toString() ;
}

template <class T>
void printAllMapPairKey(map<pair<long,long>,T*> m){
	for (map<pair<long,long>,T*>::iterator it=m.begin(); it!=m.end(); ++it)
		if(it->second) std::cout << "<" << it->first.first << "-" <<  it->first.second << "> => " << it->second->toString() ;
}


//
//// copy & paste from  exportToSingleFile except...
//void ModelExporter::exportToSingleFile_Compose(string fileName) {
//	ofstream ofs;
//	ofs.open(fileName, ofstream::out);
//
//	// USE the ORIGINAL model.. TODO check if this works..
//	//	Model *model = this->originalModel;
//
//	originalModel->exportSectionToStream(1, ofs);
//
//	writeAllMapToStream<group>(originalModel->groups, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<FreedomCase>(originalModel->freedomCases, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<LoadPattern>(originalModel->loadPatterns, ofs);
//	ofs << endl;
//
//	writeAllVectorToStream<LoadCaseCombination>(originalModel->loadCaseCombinations, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<CoordSys>(originalModel->coordSys, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<Stage>(originalModel->stages, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<Node>(originalModel->nodes, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<Beam>(originalModel->beams, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<BmAngle>(originalModel->bmAngles, ofs);
//	ofs << endl;
//
//	writeAllVectorToStream<BmEndRelease>(originalModel->bmEndReleasesT, ofs);
//	ofs << endl;
//	writeAllVectorToStream<BmEndRelease>(originalModel->bmEndReleasesR, ofs);
//	ofs << endl;
//	writeAllMapToStream<Plate>(originalModel->plates, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<PlAngle>(originalModel->plAngles, ofs);
//	ofs << endl;
//	writeAllVectorToStream<PlPatchType>(originalModel->plPatchTypes, ofs);
//	ofs << endl;
//
//	writeAllVectorToStream<Restraint>(originalModel->restraints, ofs);
//	ofs << endl;
//
//	writeAllVectorToStream<BmDistLoadL>(originalModel->bmDistLoadLs, ofs);
//	ofs << endl;
//	writeAllVectorToStream<NdForce>(originalModel->ndForces, ofs);
//	ofs << endl;
//	writeAllVectorToStream<NdMoment>(originalModel->ndMoments, ofs);
//	ofs << endl;
//
//	writeAllVectorToStream<BmDistLoadG>(originalModel->bmDistLoadGs, ofs);
//	ofs << endl;
//	writeAllVectorToStream<PlShear>(originalModel->plShears, ofs);
//	ofs << endl;
//	writeAllVectorToStream<BmPreLoad>(originalModel->bmPreLoads, ofs);
//	ofs << endl;
//	writeAllVectorToStream<PlPreLoad>(originalModel->plPreLoads, ofs);
//	ofs << endl;
//	writeAllMapToStream2<longPair, PlPressure>(originalModel->PlPressures, ofs);
//	ofs << endl;
//	writeAllVectorToStream<PlGlobalLoad>(originalModel->plGlobalLoads, ofs);
//	ofs << endl;
//
//	writeAllVectorToStream<NdTemp>(originalModel->ndTemps, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<BeamProperty >(originalModel->beamProperties, ofs);
//	ofs << endl;
//
//	writeAllMapToStream<PlateProperty>(originalModel->plateProperties, ofs);
//	ofs << endl;
//
//	// HERE:: export REAZ_V from the WORKING model
//	//	writeAllVectorToStream<NdForce>(model->REAZ_V,  ofs );
//	writeAllVectorToStream<NdForce>(workingModel->REAZ_V, ofs);
//	ofs << endl;
//
//
//	for (int i = 2; i<originalModel->sectionsCount; i++)
//		originalModel->exportSectionToStream(i, ofs);
//
//	originalModel->exportSectionToStream(originalModel->sectionsCount, ofs);
//
//	ofs.close();
//}
//

/*
comments only below
*/




//calc here
//void WorkModel::recomputeXi_parallel(){
//
//	#pragma omp parallel for
//	for(long i=0; i<ffds.NODE_TC_L.size(); i++) {
//		Node *n;
//		long nodeID;
//
//		n=ffds.NODE_TC_L[i];
//		nodeID = n->ID;
//
//		//OVERRIDE_NODEFORCE_RECOMPUTE_VERSOR = true;
//		//if (OVERRIDE_NODEFORCE_RECOMPUTE_VERSOR)
//			computeSingleForceNext(n, posIm1[nodeID], fI[nodeID] );
//
//		posI[nodeID] = coeff1 * ( (m*2.*posIm1[nodeID] - posIm2[nodeID])/(deltaT*deltaT) + c* posIm2[nodeID]/(2.*deltaT) + fI[nodeID]  );
//		//*n->position = posI[nodeID];
//
//	}
//
//}
//





//
//
//
//
//#define MAX_THREADS 4
//typedef struct MyData {
//	WorkModel *wm;
//    long firstN;
//    long nObjects;
//} MYDATA, *PMYDATA;
//
//	PMYDATA pDataArray[MAX_THREADS];
//	DWORD   dwThreadIdArray[MAX_THREADS];
//	HANDLE  hThreadArray[MAX_THREADS]; 
//
//DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
//{ 
//    PMYDATA pDataArray;
//    // Cast the parameter to the correct data type.
//    // The pointer is known to be valid because 
//    // it was checked for NULL before the thread was created.
// 
//    pDataArray = (PMYDATA)lpParam;
//	WorkModel *wm = pDataArray->wm;
//
//	double deltaT = wm->getDeltaT();
////	for(long i=0; i<ffds.NODE_TC_L.size(); i++) {
//	for(long i= pDataArray->firstN; i<pDataArray->firstN + pDataArray->nObjects; i++) {
//		Node *n;
//		long nodeID;
//
//		n=wm->ffds.NODE_TC_L[i];
//		nodeID = n->ID;
//
//		//OVERRIDE_NODEFORCE_RECOMPUTE_VERSOR = true;
//		//if (OVERRIDE_NODEFORCE_RECOMPUTE_VERSOR)
//			wm->computeSingleForceNext(n, wm->posIm1[nodeID], wm->fI[nodeID] );
//		wm->posI[nodeID] = wm->coeff1 * ( (wm->m*2.*wm->posIm1[nodeID] - wm->posIm2[nodeID])/(deltaT*deltaT) + 
//			wm->c* wm->posIm2[nodeID]/(2.*deltaT) + wm->fI[nodeID]  );
//		*n->position = wm->posI[nodeID];
//
//	}
//
//
//    return 0; 
//} 
//
//
//void WorkModel::recomputeXi_parallel_msThreads(){
//	cout << " GO PARALLEL ";
//	long N = ffds.NODE_TC_L.size();
//
//    // Create MAX_THREADS worker threads.
//    for( int i=0; i<MAX_THREADS; i++ )
//    {
//        // Allocate memory for thread data.
//        pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(MYDATA));
//        if( pDataArray[i] == NULL ) {cout <<" error allocating "; system("PAUSE"); ExitProcess(2); }
//
//        // Generate unique data for each thread to work with.
//        pDataArray[i]->firstN = i*N/MAX_THREADS;
//        pDataArray[i]->nObjects = N/MAX_THREADS;
//		pDataArray[i]->wm = this;
//
//        // Create the thread to begin execution on its own.
//        hThreadArray[i] = CreateThread( NULL, 0,
//            MyThreadFunction,       // thread function name
//            pDataArray[i],          // argument to thread function 
//            0,                      // use default creation flags 
//            &dwThreadIdArray[i]);   // returns the thread identifier 
//
//        // Check the return value for success.
//        // If CreateThread fails, terminate execution. 
//        // This will automatically clean up threads and memory. 
//        if (hThreadArray[i] == NULL) { cout<< " error threads!"; system("PAUSE"); ExitProcess(3); }
//    } // End of main thread creation loop.
//
//    // Wait until all threads have terminated.
//    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
//
//    // Close all thread handles and free memory allocations.
//    for(int i=0; i<MAX_THREADS; i++) {
//        CloseHandle(hThreadArray[i]);
//        if(pDataArray[i] != NULL) {
//            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
//            pDataArray[i] = NULL;    // Ensure address is not reused.
//        }
//    }
//}




//void goTestSolver() {
//	NewtonSolver *ns = new NewtonSolver();
//
//	ns->initData(1000);
//	ns->testSolve(1000);
//	ns->initData(3000);
//	ns->testSolve(3000);
//	ns->initData(10000);
//	ns->testSolve(10000);
//	ns->initData(50000);
//	ns->testSolve(50000);
//	ns->initData(1000000);
//	ns->testSolve(1000000);
//}


// to hide 
// exportModel.obj : warning LNK4221: This object file does not define any 
// previously undefined public symbols, so it will not be used by any link operation that consumes this library

void getRidOfLNK4221__exportModel() {}


