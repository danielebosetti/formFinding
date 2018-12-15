//#include "StdAfx.h"
#include "ModelLoader.h"
#include "fileLoader.h"

#include "ModelItemsDeclare.h"



#include "Straus_Constants.h"





ModelLoader::ModelLoader(void){}
ModelLoader::~ModelLoader(void){}

void ModelLoader::loadModel(void)
{
	ifstream ifs;
    ifs.open (fileName, ifstream::in);

	//check file open, altrimenti dà errore e esce
	if (!ifs){
		cout << " ERROR : file not found: " << fileName << "\n";
		system("PAUSE");
		exit(0);
	}
	ifs.close();

	char  *s2;

	cout << "\nFile open " << fileName;
	fL.fileName=fileName;
	fL.initFileLoader();

	cout<<"\n\nLoading File..\n";
	fL.loadFile();
	fL.splitAllFile();
	cout<<"End loading file.\n";

	int i=0;
	

	string line, subs;

	cout<<"\nCreating objects..";
	cout<<"\n100k > ";
	long count100k=0;
	char *line1;
	
	////DISABLE plPressure
	//sprintf_s(str_PlPressure,"xyz");
	//DISABLE PlGlobalLoad
	sprintf_s(str_PlGlobalLoad,"xyz");


	while (fL.getLine(&line1))
	{
		count100k=count100k+1;
		if((count100k%100000)==0) cout<<".";

		fL.getWord(&s2);
		subs=s2;

		// legge un NODE
		if (strcmp(s2, str_Node) == 0 ){
			Node *n;
			n = new Node();
			n->readFromFileLoader(fL);
			model->nodes[n->ID] = n;
			i++;
		}

		// TODO: tutti gli altri tipi
		else if (strcmp(s2, str_FreedomCase) == 0){
			FreedomCase *fc;
			fc = new FreedomCase();
			fc->readFromFileLoader(fL);
			model->freedomCases[fc->ID] = fc;
			i++;

			if (model->selectedFreedomCase ==NULL){
				model->selectedFreedomCase = fc;
				cout << "\nFreedom Case selected for Form Finding: ID:" << fc->ID << " name:" << fc->name;
			}
		}
		else if (strcmp(s2, str_Group) == 0){
			group *gr;
			gr = new group();
			gr->readFromFileLoader(fL);
			model->groups[gr->ID] = gr;
			i++;
		}
		else if (strcmp(s2, str_LoadCase) == 0){
			LoadPattern *lP;
			lP = new LoadPattern();
			lP->readFromFileLoader(fL);
			model->loadPatterns[lP->ID] = lP;
			if (lP->name == "\"FF_REAZ_V\"")
				model->loadPattern_REAZ_V = lP;
			if (lP->name == "\"FF_FORCE_DENSITY\"")
				model->loadPattern_FORCE_DENSITY = lP;
			i++;
		}
		else if (strcmp(s2, str_NdFreedom) == 0){
			Restraint *rest;
			rest = new Restraint();
			rest->readFromFileLoader(fL);
			model->restraints.push_back(rest);
		}
		else if (strcmp(s2, str_PlGlobalLoad) == 0){
			PlGlobalLoad *pgl;
			pgl = new PlGlobalLoad();
			pgl->readFromFileLoader(fL);
			model->plGlobalLoads.push_back(pgl);

		}
		else if (strcmp(s2, str_NdForce) == 0){
			NdForce *nd;
			nd = new NdForce();
			nd->readFromFileLoader(fL);
			model->ndForces.push_back(nd);
		}
//		else if (subs=="BeamProp" || subs=="CutoffProp" || subs=="TrussProp" || subs=="CableProp" ){
		else if ( (strcmp(s2, str_BeamProp) == 0) || (strcmp(s2, str_CutoffProp) == 0) || (strcmp(s2, str_TrussProp) == 0) || (strcmp(s2, str_CableProp) == 0) ){
			BeamProperty *bP;
			bP = new BeamProperty();
			bP->type = subs;
			bP->readFromFileLoader(fL);
			model->beamProperties[bP->ID] = bP;
			i++;

		}
		else if (strcmp(s2, str_Beam) == 0){
			Beam *b;
			b=new Beam();
			b->readFromFileLoader(fL);
			model->beams[b->ID]=b;
			i++;
			//cout << "\nbeam " << b->ID;
		}
		else if (strcmp(s2, str_PlPressure) == 0){
			PlPressure *plp;
			plp=new PlPressure();
			plp->readFromFileLoader(fL);
			model->PlPressures[ make_pair (plp->loadPatternID, plp->plateID) ] = plp;
			i++;
		}


		else if (strcmp(s2, str_Tri3) == 0){
			//		/ PLATE ELEMENTS
			Plate *plate;
			plate=new Plate();
			plate->type="Tri3";
			plate->readFromFileLoader(fL);
			model->plates[plate->ID]=plate;
			i++;
		}
		else if (strcmp(s2, str_Quad4) == 0){
			//		/ PLATE ELEMENTS
			Plate *plate;
			plate=new Plate();
			plate->type="Quad4";
			plate->readFromFileLoader(fL);
			model->plates[plate->ID]=plate;
			i++;
		}
		else if (strcmp(s2, str_BAR) == 0){
			// str_BAR =  str_/
			//		COMMENteD LINE? insert COMMENT >>
			fL.pushComment();
		}
//		else if ( (subs=="3DMembraneProp") || (subs=="PatchPlateProp") || (subs=="PlateShellProp") ){
		else if ( (strcmp(s2, str_3DMembraneProp) == 0) || (strcmp(s2, str_PatchPlateProp) == 0) || (strcmp(s2, str_PlateShellProp) == 0) ){
			PlateProperty *pP;
			pP = new PlateProperty();
			pP->type = subs;
			pP->readFromFileLoader(fL);
			model->plateProperties[pP->ID]=pP;
			i++;
		}
		else if (strcmp(s2, str_BmAngle) == 0){
			BmAngle *bmA;
			bmA = new BmAngle();
			bmA->readFromFileLoader(fL);
			model->bmAngles[bmA->ID]=bmA;
			i++;
		}
		else if (strcmp(s2, str_BmDistLoadL) == 0){
			BmDistLoadL *bmDistL;
			bmDistL=new BmDistLoadL();
			bmDistL->readFromFileLoader(fL);
			model->bmDistLoadLs.push_back(bmDistL);
//			model->bmDistLoadLs[ make_pair(bmDistL->IDFreedomCase, bmDistL->node) ] = bmDistL;
			i++;
		}
		else if (strcmp(s2, str_BmDistLoadG) == 0){
			BmDistLoadG *bmDistG;
			bmDistG=new BmDistLoadG();
			bmDistG->readFromFileReader(fL);
			model->bmDistLoadGs.push_back(bmDistG);
//			model->bmDistLoadGs[ make_pair( make_pair( bmDistG->loadCase, bmDistG->beam) , bmDistG->dir) ] = bmDistG;
			i++;
		}
		else if (strcmp(s2, str_CoordSys) == 0){
			CoordSys *cS;
			cS = new CoordSys();
			cS->readFromFileLoader(fL);
			model->coordSys[ cS->ID ] = cS;
			i++;
		}
		else if (strcmp(s2, str_BmEndReleaseT) == 0){
			BmEndRelease *bmEnd;
			bmEnd = new BmEndRelease();
			bmEnd->charType='T';
			bmEnd->readFromFileLoader(fL);
			model->bmEndReleasesT.push_back(bmEnd);
			i++;
		}
		else if (strcmp(s2, str_BmEndReleaseR) == 0){
			BmEndRelease *bmEnd;
			bmEnd = new BmEndRelease();
			bmEnd->charType='R';
			bmEnd->readFromFileLoader(fL);
			model->bmEndReleasesR.push_back(bmEnd);
			i++;
		}
		else if (strcmp(s2, str_Stage) == 0){
			Stage *stage;
			stage = new Stage();
			stage->readFromFileLoader(fL);
			model->stages[ stage->ID ] = stage;
			i++;
		}
		else if (strcmp(s2, str_PlAngle) == 0){
			PlAngle *plAngle;
			plAngle = new PlAngle();
			plAngle->readFromFileLoader(fL);
			model->plAngles[ plAngle->ID ] = plAngle;
			i++;
		}
		else if (strcmp(s2, str_NdMoment) == 0){
			NdMoment *nm;
			nm = new NdMoment();
			nm->readFromFileLoader(fL);
			model->ndMoments.push_back(nm);
			i++;
		}
		else if (strcmp(s2, str_PlPatchType) == 0){
			PlPatchType *ppt;
			ppt = new PlPatchType();
			ppt->readFromFileLoader(fL);
			model->plPatchTypes.push_back( ppt );
			i++;
		}
		else if (strcmp(s2, str_BmPreLoad) == 0){
			BmPreLoad *bpl;
			bpl = new BmPreLoad();
			bpl->readFromFileLoader(fL);
			model->bmPreLoads.push_back( bpl );
			i++;
		}
		else if (strcmp(s2, str_PlPreLoad) == 0){
			PlPreLoad *ppl;
			ppl = new PlPreLoad();
			ppl->readFromFileLoader(fL);
			model->plPreLoads.push_back( ppl );
			i++;
		}
		else if (strcmp(s2, str_NdTemp) == 0){
			NdTemp *nt;
			nt = new NdTemp();
			nt->readFromFileLoader(fL);
			model->ndTemps.push_back( nt );
			i++;
		}
		else if (strcmp(s2, str_PlShear) == 0){
			PlShear *ps;
			ps = new PlShear();
			ps->readFromFileLoader(fL);
			model->plShears.push_back( ps );
			i++;
		}
		else if (strcmp(s2, str_LoadCaseCombination) == 0){
			LoadCaseCombination *lcc;
			lcc = new LoadCaseCombination();
			lcc->readFromFileLoader(fL);
			model->loadCaseCombinations.push_back( lcc );
			if (lcc->name == "\"FFC\"")
				model->ffc = lcc;
			i++;
		}
		else if (strcmp(s2, str_PressureUnit) == 0){
			fL.getWord(&s2);
			subs=s2;
			if (subs!="kPa")
				exitWithMessage(" PressureUnit deve essere <kPa> , e' diverso! controllare il file di input ");
			fL.pushComment();
		}
		else if (strcmp(s2, str_ForceUnit) == 0){
			fL.getWord(&s2);
			subs=s2;
			if (subs!="kN")
				exitWithMessage(" PressureUnit deve essere <kN> , e' diverso! controllare il file di input ");
			fL.pushComment();
		}

		else {
			//		COMMENteD LINE? insert COMMENT >>
			fL.pushComment();
			//printf( " unknown line?  <%s>   line size: %d", &(fL.buffer[fL.iniLine]), fL.lineLength() );
		}

	}
	
	cout<<"\nObjects created.\n";
// COPY COMMENTED SECTIONS TO MODEL
	model->linesToCopy=fL.linesToCopy;
	model->sectionFirstLine = fL.sectionFirstLine;
	model->sectionLinesCount = fL.sectionLinesCount;
	model->sectionsCount = fL.sectionsCount;

cout << "Found: " << i << " elements\n";
cout << "Number of lines in input file: "<< fL.fileLinesCount << "\n" ;
cout << "Number of Comments/Unknown sections found: " << model->sectionsCount << "\n";



//************************************************************************************************
//************************************************************************************************
//******   checks
	if (model->selectedFreedomCase == NULL){
		cout << "\n\nERROR, no FreedomCases Found! Exit now..";
		system("PAUSE");
		exit(0);
	}
	if (model->ffc == NULL){
		cout << "\n\nERROR, FFC LoadCaseDefinition NOT Found! Exit now..";
		system("PAUSE");
		exit(0);
	}
	if (model->loadPattern_REAZ_V == NULL) {
		cout << "\n\n ERROR! FF_REAZ_V Load Case not found in input file ?? exit now. \n\n"; system("PAUSE"); exit(0);
	}
	if (model->loadPattern_FORCE_DENSITY == NULL) {
		cout << "\n\n ERROR! FF_FORCE_DENSITY Load Case not found in input file ?? exit now. \n\n"; system("PAUSE"); exit(0);
	}

}




