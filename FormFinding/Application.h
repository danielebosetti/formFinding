#pragma once

class WorkModel;

class Application
{
private:
	static Application *instance;
	Application();
public:
	
	~Application();

	WorkModel *wm = 0;
	void setWorkModel(WorkModel *wm);
	void goForceDensity();

	

	static Application *getInstance();

};

