#include "StdAfx.h"
#include "FormFindingDataSet.h"


FormFindingDataSet::FormFindingDataSet(void)
{
	forzaPeso.clear();
	ndForces_FFC.clear();

}


FormFindingDataSet::~FormFindingDataSet(void)
{
}

void FormFindingDataSet::clearFormFindingDataSet(void){

	NODE_TC_L.clear();
	NODE_I.clear();
	NODE_RL.clear();
	NODE_RESTRAINED.clear();

	REAZ_V.clear();
	forzaPeso.clear();
	ndForces_FFC.clear();
	FORCE_DENSITY.clear();
	forcePressure.clear();
}


