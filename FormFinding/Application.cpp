#include "Application.h"
#include "WorkModel.h"

#include "ForceDensitySolver.h"


//NULL
Application* Application::instance = 0;


Application::Application()
{
	//Application::instance = new Application();

}


Application::~Application()
{
}

void Application::setWorkModel(WorkModel * wm)
{
	this->wm = wm;
}

void Application::goForceDensity()
{
	if (wm == 0)
	{
		//TODO: log ERROR
		return;
	}

	ForceDensitySolver *fds = new ForceDensitySolver();
	fds->setWorkModel(this->wm);
	fds->solveForceDensity();
	// solveForceDensity updates the FORCE_DENSITY map in the wm->ffds objects
}



Application * Application::getInstance()
{
	if (!instance)
		instance = new Application();
	return instance;
}

void glutPostRedisplay();
