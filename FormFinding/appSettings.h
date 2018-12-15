#pragma once

#include <string>
#include "SharedVariables.h"

using namespace std;

class appSettings
{
public:

	static bool LICENSE_CHECK_OK;


	static long MAX_LINES;
	static long MAX_WORDS_PER_LINE;
	static long MAX_NODE_ID;
	static long RENDERSTEPS;
	static long RENDER_ITERATION_STEPS;

	static double TIME_STEP;
//	static double MAX_POS_STEP;
	static double ALPHA_MASS;
	static double COEFF_VISC;


	static bool SMORZAMENTO_K_GLOBAL;
	static bool USE_C_CONSTANT;
	static double C_DEFAULT_VALUE;
	static double XI;
	static long N_RECALC_BEAMPLATES;
	static double FORCE_SKIP_MULTIPLIER;
	static long SKIP_N_STEP;

	static bool ENABLE_KINETIC_DAMPING;
	static double GREEN_LINES_MULTIPLIER;
	static long FORCE_DENSITY_TARGET;

	static double LAMBDA;
	static long N_ITER;

	static string fileName;
	static string workingFilePath;
	static string originalFilePath;
	static string outPath;
	static char ownPth[MAX_PATH];

	static string applicationDir;
	static string settingsDir;
	static string dataDir;
	static string fileInput;
	static string fileOutput;
	static string method;
	static string nIterations;



	appSettings(void);
	~appSettings(void);

	static void initAppSettings();
	static void initAppSettings_for_cmd();
	static void checkKey();
	static void readParamFromIni();
};

