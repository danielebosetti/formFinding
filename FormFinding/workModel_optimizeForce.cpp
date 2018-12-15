
#include "StdAfx.h"
//#include "WorkModel.h"
//#include <algorithm>
//
//
//#include <nlopt.hpp>
////#include <omp.h>
//#pragma warning (disable:4018)
//
//
//Vector3d minimVersor;
//Vector3d iniPoint;
//Node* nOptimize;
//WorkModel *wmThis;
//
//double step_x;
//
//void setVersor(Vector3d& v) {minimVersor = v; }
//void setIniPoint(Vector3d& p) {iniPoint = p;}
//void setIniNode(Node* n) {nOptimize = n;}
//void setWorkModel(WorkModel* wm) {wmThis = wm;}
//
//Matrix3d jacobian;
//
//long countIter = 0;
//
//double WorkModel::fMinimize(double t){
//	Vector3d fOut;
//	// calcola la forze sul nodo DISPLACED
//	computeSingleForceNext(nOptimize, iniPoint + t * minimVersor, fOut);
//	//la funzione da minimizzare è la norma
//	return fOut.norm();
//}
//
//
////#define SHOWDEBUGFULL
//#define SHOWDEBUG
//
//// norm of force, searches in 3d
//double WorkModel::fMinimize3d(const double *x){
//	
//	Vector3d fOut, point1(x[0], x[1], x[2]);
//
//	// calcola la forze sul nodo DISPLACED
//	computeSingleForceNext(nOptimize, iniPoint + point1, fOut);
//	//la funzione da minimizzare è la norma
//	
//
//#ifdef SHOWDEBUGFULL 
//	cout << "\n"; printVectorDetailed(&point1);
//	cout << "  f= " << fOut.norm();
//#endif
//
//	return fOut.squaredNorm();
//
//}
//
//bool WorkModel::computeJacobian() {
//	Vector3d xDisp, fDisp, fDiff, f0;
//	Vector3d colJacob;
//
//	computeSingleForceNext(nOptimize, iniPoint, f0);
//
//
//	// df/dx
//	xDisp = iniPoint;
//	xDisp[0] = xDisp[0] + step_x;
//	computeSingleForceNext(nOptimize, xDisp, fDisp);
//	colJacob = (fDisp-f0)/step_x;
//
//	int col = 0;
//	jacobian(0,col) = colJacob[0];
//	jacobian(1,col) = colJacob[1];
//	jacobian(2,col) = colJacob[2];
//
//	// df/dy
//	xDisp = iniPoint;
//	xDisp[1] = xDisp[1] + step_x;
//	computeSingleForceNext(nOptimize, xDisp, fDisp);
//	colJacob = (fDisp-f0)/step_x;
//
//	col = 1;
//	jacobian(0,col) = colJacob[0];
//	jacobian(1,col) = colJacob[1];
//	jacobian(2,col) = colJacob[2];
//
//	// df/dz
//	xDisp = iniPoint;
//	xDisp[2] = xDisp[2] + step_x;
//	computeSingleForceNext(nOptimize, xDisp, fDisp);
//	colJacob = (fDisp-f0)/step_x;
//
//	col = 2;
//	jacobian(0,col) = colJacob[0];
//	jacobian(1,col) = colJacob[1];
//	jacobian(2,col) = colJacob[2];
//
//	return true;
//}
//
//double zero_tol = 10e-5;
//
//
//// USING JACOBIAN
////bool WorkModel::minimizeForce(Node *n){
////	//=========================================================
////	long nodeID = n->ID;
////	long nIter=0;
////	double f0norm, fIniNorm,  tol = 0.001;
////	//cout << "#";
////	Vector3d delta_x, f0, saveIniPos;
////
////	saveIniPos = *n->position;
////
////	setIniPoint(*n->position);
////	setIniNode(n);
////	step_x = deltaT;
////
////
////	computeSingleForceNext(n, *n->position, f0);
////	f0norm = f0.norm();
////
////	fIniNorm = f0norm;
////
////	do {
////		computeJacobian();
////		//cout << endl << "Jacobian: " << jacobian;
////		if (abs(jacobian.determinant()) > zero_tol) {
////
////			delta_x = jacobian.fullPivLu().solve(f0);
////
////			double delta_x_Norm = delta_x.norm();
////
////			if (delta_x_Norm > MAX_POS_STEP) {
////				//cout << endl << "Err qui? Node : " << n->ID ;
////
////
////				//cout << endl << "Jacobian : " << endl <<  jacobian;
////				//cout << endl << "f0 : " ; printVector(&f0);
////				//cout << endl << "delta_x : " ; printVector(&delta_x);
////				cout << "!";
////				computeSingleForceNext(n, *n->position, fI[nodeID] );
////				return false;
////
////			}
////
////			*n->position = *n->position - delta_x ;
////
////			computeSingleForceNext(n, *n->position, f0);
////
////			if (f0.norm() > fIniNorm) {
////				//cout << endl << "Jacobian: Increased Force on Node : " << n->ID ;
////				//cout << endl << "Try with scalar.. ";
////				cout << "&";
////				*n->position = saveIniPos;
////				computeSingleForceNext(n, *n->position, fI[nodeID] );
////				return false;
////			}
////
////			posIm1[nodeID] = posI[nodeID] ;
////			posIm2[nodeID] = posI[nodeID] ;
////			vI[nodeID] = 0.;
////			vIm1[nodeID] = 0.;
////			computeSingleForceNext(n, *n->position, fI[nodeID] );
////
////			return true;
////
////		}
////		else {
////			//cout << endl << "Node " << n->ID << " : Non-invertible jacobian?  step_x = " << step_x;
////			//cout << endl << "Position of Node: " ; printVector(n->position);
////			//cout << endl << "Jacobian: " << endl <<  jacobian;
////				cout << "J";
////				return false;
////		}
////
////		nIter++;
////	
////		f0norm = f0.norm();
////	}
////	while (f0norm > tol && nIter < 20);
////
////	cout << "e";
////	return false;
////
////}
//
//
//double myfunc(unsigned n, const double *x, double *grad, void *my_func_data)
//{
//    return wmThis->fMinimize(x[0]);
//}
//
//inline double myfunc3d(unsigned n, const double *x, double *grad, void *my_func_data)
//{
//    return wmThis->fMinimize3d(x);
//}
//
//// mf mf3d
//////// USING NLOPT
//bool WorkModel::minimizeForce(Node *n){
//	// ============================================================
//	// INIT optimization problem
//	long nodeID = n->ID;
//	double fIniNorm;
//	Vector3d f0, saveIniPos;
//
//	setIniPoint(*n->position);
//	setIniNode(n);
//	//step_x = deltaT;  <<< questa versione di <minimizeForce> non calcola la derivata!
//	wmThis = this;
//	// END INIT optimization problem
//	// ============================================================
//
//	computeSingleForceNext(n, *n->position, f0);
//	// backup ini position and iniNorm
//	saveIniPos = *n->position;
//	fIniNorm = f0.squaredNorm();
//
//	nlopt_opt opt;
//	//opt = nlopt_create(NLOPT_LD_MMA, 3); /* algorithm and dimensionality */
//	opt = nlopt_create(NLOPT_LN_COBYLA, 3); /* algorithm and dimensionality */
//
//	nlopt_set_min_objective(opt, myfunc3d, NULL);
//
//	//nlopt_set_xtol_rel(opt, 1e-7);
//	nlopt_set_xtol_abs1( opt, 1e-9);
//	//nlopt_set_stopval(opt, 1e-3);
//	//nlopt_set_ftol_rel( opt, 1e-2);
//	//nlopt_set_ftol_abs( opt, 1e-4);
//	//nlopt_set_ftol_rel(opt, 1e-3);
//	nlopt_set_maxeval(opt, 2000);
//	nlopt_set_maxtime(opt, 3.);
//
//	double x[3] = { 0., 0., 0. };  /* some initial guess */
//	double x0=0.;
//
//	double minf=100.; /* the minimum objective value, upon return */
//
//	
//#ifdef SHOWDEBUG 
//	cout << "3";
//#endif
//
//	if (nlopt_optimize(opt, x, &minf) >= 0) {
//		//printf("\nNode %d, found minimum at f(%0.5g, %0.5g, %0.5g) = %0.10g\n", nodeID,  x[0], x[1], x[2], minf);
//
//		Vector3d point1(x[0], x[1], x[2]);
//
//		*n->position = saveIniPos + point1;
//
//		posIm1[nodeID] = posI[nodeID] ;
//		posIm2[nodeID] = posI[nodeID] ;
//		vI[nodeID] = 0.;
//		vIm1[nodeID] = 0.;
//		computeSingleForceNext(n, *n->position, fI[nodeID] );
//
//		if (fI[nodeID].squaredNorm() > fIniNorm) {
//			//cout << endl << "COBYLA: Increased Force on Node : " << n->ID ;
//			//cout << endl << "Try with scalar.. ";
//		#ifdef SHOWDEBUG 
//			cout << "x";
//		#endif
//			*n->position = saveIniPos;
//			computeSingleForceNext(n, *n->position, fI[nodeID] );
//			return false;
//		}
//		else {
//		#ifdef SHOWDEBUG 
//			cout << "v";
//		#endif
//
//			return true;
//		}
//
//	}
//	else {
//		printf("\nNode %d, NLOPT failed?", nodeID);
//
//		return false;
//
//
//	}
//
//
//
//}
//
//int sep1;
//
//
//
//// USING NLOPT
//bool WorkModel::minimizeForceScalar(Node *n){
//	
//	// ============================================================
//	// INIT optimization problem
//	
//	long nodeID = n->ID;
//		
//#ifdef SHOWDEBUG 
//	cout << "@";
//#endif
//
//	Vector3d versor, saveIniPos;
//	double fIniNorm;
//
//	// compute versor on initial position for the node
//	computeSingleForceNext(n, *n->position , versor );
//	saveIniPos = *n->position;
//	fIniNorm = versor.norm();
//	versor.normalize();
//	
//	// sets minimization parameters
//	setVersor(versor);
//	setIniPoint(*n->position);
//	setIniNode(n);
//	wmThis = this;
//	// END INIT optimization problem
//	// ============================================================
//
//	nlopt_opt opt;
//
//	//opt = nlopt_create(NLOPT_LD_MMA, 1); /* algorithm and dimensionality */
//	opt = nlopt_create(NLOPT_LN_COBYLA, 1); /* algorithm and dimensionality */
//	
////	nlopt_set_lower_bounds(opt, lb);
//	nlopt_set_min_objective(opt, myfunc, NULL);
//
////	nlopt_set_xtol_rel(opt, 1e-12);
//	nlopt_set_xtol_abs1(opt, 1e-9);
//
//	double x[1] = { 0. };  /* some initial guess */
//	double x0=0.;
//
//	double minf = 100. ; /* the minimum objective value, upon return */
//
//	nlopt_result res;
//	res = nlopt_optimize(opt, x, &minf);
//	//cout << " res:" << res << " on Node:" << nodeID << " f=" << minf;
//	
//	if ( res >= 0) {
//		
//		if ( minf < 0.9 * fIniNorm) {
//			// x[0] realizes the "minimum",
//			*n->position = iniPoint + x[0] * minimVersor;
//
//			posIm1[nodeID] = posI[nodeID] ;
//			posIm2[nodeID] = posI[nodeID] ;
//			vI[nodeID] = 0.;
//			vIm1[nodeID] = 0.;
//			computeSingleForceNext(n, *n->position, fI[nodeID] );
//
//			nlopt_destroy(opt);
//			#ifdef SHOWDEBUG 
//				cout << "v";
//			#endif
//			//cout << "ok";
//			return true;
//		}
//	}
//	else {
//		printf("\nlopt failed!\n");
//		*n->position = saveIniPos;
//		computeSingleForceNext(n, *n->position, fI[nodeID] );
//	}
//
//
//	nlopt_destroy(opt);
//	#ifdef SHOWDEBUG 
//		cout << "x";
//	#endif
//
//	return false;
//}
//
////  NOT USING NLOPT
////bool WorkModel::minimizeForceScalar(Node *n){
////============================================================
////	long nodeID = n->ID;
////	//cout << "@";
////
////	Vector3d versor, saveIniPos;
////	double fIniNorm;
////
////	computeSingleForceNext(n, *n->position , versor );
////	// compute versor on initial position for the node
////
////	saveIniPos = *n->position;
////	fIniNorm = versor.norm();
////
////
////	versor.normalize();
////	
////	// sets minimization parameters
////	setVersor(versor);
////	setIniPoint(*n->position);
////	setIniNode(n);
////
////	double tol = 0.1;
////	double x0=0.;
////	double xTemp;
////	step_x = deltaT;
////	double x1;
////	double deriv_x0 , f0, f1;
////	double delta_x;
////	long nIter = 0;
////
////	f0 = fMinimize( x0 );
////
////	x1 = x0 + step_x;
////
////	do {
////
////		f1 = fMinimize( x1 );
////
////		deriv_x0 = (f1-f0)/(x1-x0);
////
////		if (abs(deriv_x0) < zero_tol){
////			step_x = (x1-x0) * 10.;
////			x1 = x0 + step_x;
////			nIter ++;
////		}
////		else {
////
////			delta_x = f0/deriv_x0;
////
////			xTemp = x1;
////			x1 = x0 - delta_x;
////			x0 = xTemp;
////
////			f0 = f1;
////
////			nIter++;
////
////			//if (nIter > 5)
////			//	cout << "\nMinim: " << nIter << " x1: " << x1 << " f1: " << f1;
////
////		}
////	}
////	while (abs(f1) > tol && nIter < 20);
////	
////
////	if (x0 > MAX_POS_STEP)
////		x0 = MAX_POS_STEP;
////	else if (x0 < -MAX_POS_STEP)
////		x0 = -MAX_POS_STEP;
////
////	// x1 realizes the "minimum",
////	*n->position = iniPoint + x0 * minimVersor;
////
////	posIm1[nodeID] = posI[nodeID] ;
////	posIm2[nodeID] = posI[nodeID] ;
////	vI[nodeID] = 0.;
////	vIm1[nodeID] = 0.;
////	computeSingleForceNext(n, *n->position, fI[nodeID] );
////
////
////
////	if ( fI[nodeID].norm() >= fIniNorm*0.9999 ) {
////		//cout << endl << "Scalar: Increased Force? Node : " << n->ID ;
////		//cout << endl << "Go normal.. ";
////		*n->position = saveIniPos;
////		computeSingleForceNext(n, *n->position, fI[nodeID] );
////
////		return false;
////
////	}
////
////	return true;
////}
////
////


void getRidOfLNK4221__optimizeForce() {}


