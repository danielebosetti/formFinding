#pragma once
class INI_io
{
public:
	INI_io(void);
	~INI_io(void);

	static void writeToIni(char* paramName, double value);
	static void writeToIni(char* paramName, long value);
	static void getFromIni(char* paramName, char* defaultValue, long& outValue);
	static void getFromIni(char* paramName, char* defaultValue, double& outValue);
	static void getFromIni(char* paramName, char* defaultValue, char* outValue);
	static void getFromIni(char* paramName, char* defaultValue, string& outValue);
	static void getFromIni(char* paramName, char* defaultValue, bool& outValue);
};

