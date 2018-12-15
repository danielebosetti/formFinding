//#include "stdafx.h"
#pragma warning (disable:4244)
#include <boost\foreach.hpp>

#include "ModelRenderer.h"
#include "appSettings.h"
#include "BoolFunctions.h"
#include "Application.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <ctime>



#include <stdio.h>
#include <Eigen\Dense>
#include <algorithm>
#include "Node.h"
#include "Beam.h"
#include "Plate.h"
#include "Model.h"
#include "WorkModel.h"
#include "FormFindingDataSet.h"

using namespace std;
using namespace Eigen;

Vector3d centerOfRotation(0., 0., 0);

float xLocation = 0.0f; // Keep track of our position on the y axis.
float yLocation = 00.0f; // Keep track of our position on the y axis.

						 //float yRotationAngle = 0.0f; // The angle of rotation for our object
						 //float xRotationAngle = 0.0f; // The angle of rotation for our object
						 //float zRotationAngle = 0.0f; // The angle of rotation for our object

double winWidth, winHeight;
int iniWidth = 800, iniHeight = 800;
double maxDim;

float viewWidth = 20000.f, viewHeight = 20000.f;
float viewMinX = -10000.f, viewMinY = -10000.f;
float viewMinZ = -100000.f, viewMaxZ = 100000.f;

double zoomFactor = 1.;

void goIteration();
void keyPressed(unsigned char key, int x, int y);
void mouseMoveFunc(int x, int y);
void mouseFunc(int wheel, int direction, int x, int y);
int mouseX, mouseY;
void getNearestNode(int mouseX, int mouseY);

glm::mat4 mRotation;

void rotateX(double angle) {
	glm::vec3 xAxis(1., 0., 0.);
	glm::mat4 mRotX = glm::mat4(1.0);
	mRotX = glm::rotate<float, glm::highp>(mRotX, angle, xAxis);
	mRotation = mRotX * mRotation;
}
void rotateY(double angle) {
	glm::vec3 xAxis(0., 1., 0.);
	glm::mat4 mRotX = glm::mat4(1.0);
	mRotX = glm::rotate<float, glm::highp>(mRotX, angle, xAxis);
	mRotation = mRotX * mRotation;
}

Model *mod;
WorkModel *wm;
ModelRenderer *mr;

// note.... questo è molto brutto ;( !
// wm è variabile globale, sta fuori dalla classe. (vedi 3 righe sopra).
// questo xke glut dava problemi facendo glutSetDisplay(ModelRenderer::method_X)
void ModelRenderer::setWorkModel(WorkModel *wm_in) { wm = wm_in; }

int mouseSel;
int ROTATE = 0;
int PAN = 1;
int ZOOM = 2;
const int EXPORT = 3;
const int OPEN_FILE = 4;
const int SELECT_POINT = 5;


const int ZOOM_EXTENSION = 980;
#define EXIT_BUTTON 990

ModelRenderer::ModelRenderer(void)
{
	wm = NULL;
	modelExporter = NULL;
	showPlates = true;
}


ModelRenderer::~ModelRenderer(void)
{
}



static bool x_compare(pair<long, Node*> node1, pair<long, Node*> node2) {
	return (node1.second->x < node2.second->x);
}
static bool y_compare(pair<long, Node*> node1, pair<long, Node*> node2) {
	return (node1.second->y < node2.second->y);
}
static bool z_compare(pair<long, Node*> node1, pair<long, Node*> node2) {
	return (node1.second->z < node2.second->z);
}

double xMin, xMax, yMin, yMax, zMin, zMax;

void getBoxView(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax) {

	xMin = std::min_element(mod->nodes.begin(), mod->nodes.end(), x_compare)->second->x;
	xMax = std::max_element(mod->nodes.begin(), mod->nodes.end(), x_compare)->second->x;

	yMin = std::min_element(mod->nodes.begin(), mod->nodes.end(), y_compare)->second->y;
	yMax = std::max_element(mod->nodes.begin(), mod->nodes.end(), y_compare)->second->y;

	zMin = std::min_element(mod->nodes.begin(), mod->nodes.end(), z_compare)->second->z;
	zMax = std::max_element(mod->nodes.begin(), mod->nodes.end(), z_compare)->second->z;

}


typedef struct {
	float x1, y1, z1;
	float x2, y2, z2;
	float r, g, b;
} beamDraw;

typedef struct {
	float x1, y1, z1;
	float r, g, b;
} nodeDraw;

typedef struct {
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float r, g, b;
} tri3Draw;

typedef struct {
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float x4, y4, z4;
	float r, g, b;
} quad4Draw;


beamDraw *bufferBeams;
long bufferBeamSize;

nodeDraw *bufferNodes;
long bufferNodeSize;

quad4Draw *bufferQuad4;
long bufferQuad4Size;

tri3Draw *bufferTri3;
long bufferTri3Size;

void deleteBufferModelBeams() {
	delete[] bufferBeams;
	bufferBeamSize = 0;
}

void deleteBufferModelNodes() {
	delete[] bufferNodes;
	bufferNodeSize = 0;
}

void deleteBufferModelPlates() {
	delete[] bufferQuad4;
	delete[] bufferTri3;
	bufferTri3Size = 0; bufferQuad4Size = 0;
}

void initBufferModelBeams() {

	bufferBeams = new beamDraw[appSettings::MAX_NODE_ID + 1];
	beamDraw * buffDraw;
	Node* nodeCopy;
	Beam* beamCopy;
	long i = 0;
	// LOOP over beams and get the points
	for (map<long, Beam*>::iterator it = mod->beams.begin(); it != mod->beams.end(); ++it) {
		if (it->second != NULL) {
			// get Beam to copy
			beamCopy = it->second;
			// get Destination in buffer
			buffDraw = &bufferBeams[i];

			// copy first node
			nodeCopy = mod->nodes[it->second->node1];
			buffDraw->x1 = nodeCopy->position->x();
			buffDraw->y1 = nodeCopy->position->y();
			buffDraw->z1 = nodeCopy->position->z();

			// copy second node
			nodeCopy = mod->nodes[it->second->node2];
			buffDraw->x2 = nodeCopy->position->x();
			buffDraw->y2 = nodeCopy->position->y();
			buffDraw->z2 = nodeCopy->position->z();

			// set BEAM COLOR!!
			if (is_ELEM_STEEL(beamCopy)) {
				// STEEL_COLOR: bluette
				bufferBeams[i].r = 0.2f;
				bufferBeams[i].g = 0.573f;
				bufferBeams[i].b = 1.0f;
			}
			else if (is_ELEM_TC_CE(beamCopy)) {
				// TC_COLOR: red-orange
				bufferBeams[i].r = 0.995f;
				bufferBeams[i].g = 0.247f;
				bufferBeams[i].b = 0.127f;
			}
			else if (is_ELEM_RL(beamCopy)) {
				// RL_COLOR: viola
				bufferBeams[i].r = 0.624f;
				bufferBeams[i].g = 0.208f;
				bufferBeams[i].b = 0.969f;
			}
			else if (is_ELEM_TC_SOAP(beamCopy)) {
				// CE_COLOR: yellow
				bufferBeams[i].r = .94f;
				bufferBeams[i].g = 0.984f;
				bufferBeams[i].b = 0.1235f;
			}
			else {
				// UNKNOWN TYPE: gray?
				buffDraw->r = .9f;
				buffDraw->g = .9f;
				buffDraw->b = .9f;
			}

			i++;
		}
	}
	bufferBeamSize = i;
}

void initBufferModelNodes() {
	bufferNodes = new nodeDraw[appSettings::MAX_NODE_ID + 1];
	nodeDraw * buffDraw;
	Node* nodeCopy;
	//	Beam* beamCopy;
	long i = 0;

	// LOOP over NODES and get the points
	for (map<long, Node*>::iterator it = mod->nodes.begin(); it != mod->nodes.end(); ++it) {
		if (it->second != NULL) {
			// get Beam to copy
			nodeCopy = it->second;

			// get Destination in buffer
			buffDraw = &bufferNodes[i];

			// copy first node
			buffDraw->x1 = nodeCopy->position->x();
			buffDraw->y1 = nodeCopy->position->y();
			buffDraw->z1 = nodeCopy->position->z();
			//buffDraw->x1 = nodeCopy->x;
			//buffDraw->y1 = nodeCopy->y;
			//buffDraw->z1 = nodeCopy->z;

			// set COLOR!!
			if (is_NODE_I(nodeCopy) && !nodeCopy->has_Restraint) {
				// NODE_I_L, GREEN
				buffDraw->r = 0.0f;
				buffDraw->g = 1.0f;
				buffDraw->b = 0.0f;
			}
			else if (is_NODE_I(nodeCopy)) {
				// NODI_I_R : DARK GREEN
				buffDraw->r = 0.0f;
				buffDraw->g = 0.5f;
				buffDraw->b = 0.0f;
			}
			else if (nodeCopy->has_Restraint) {
				// RESTRAINED NODE, MAGENTA
				buffDraw->r = 1.0f;
				buffDraw->g = 0.1f;
				buffDraw->b = 1.0f;
			}
			else if (is_NODE_TC_L(nodeCopy)) {
				// NODI_TC_L : ORANGE
				buffDraw->r = 1.0f;
				buffDraw->g = 0.4f;
				buffDraw->b = 0.4f;
			}
			else if (is_NODE_STEEL(nodeCopy)) {
				// NODI_STEEL_L: BLU
				buffDraw->r = 0.2f;
				buffDraw->g = 0.2f;
				buffDraw->b = 0.827f;
			}
			else if (is_NODE_RL(nodeCopy)) {
				// NODI_RL: viola
				buffDraw->r = 0.524f;
				buffDraw->g = 0.308f;
				buffDraw->b = 0.869f;
			}
			else {
				// UNKNOWN TYPE: gray?
				buffDraw->r = .3f;
				buffDraw->g = .3f;
				buffDraw->b = .3f;
			}

			i++;
		}
	}
	bufferNodeSize = i;
}

void initBufferModelPlates() {


	bufferQuad4 = new quad4Draw[appSettings::MAX_NODE_ID + 1];
	bufferTri3 = new tri3Draw[appSettings::MAX_NODE_ID + 1];
	quad4Draw * buffDrawQuad4;
	tri3Draw * buffDrawTri3;
	Node* nodeCopy;
	Plate* plateCopy;
	long i = 0;
	long j = 0;
	for (map<long, Plate*>::iterator it = mod->plates.begin(); it != mod->plates.end(); ++it) {
		if (it->second != NULL) {
			// get Plate to copy
			plateCopy = it->second;

			// get Destination in buffer
			if (plateCopy->type == "Quad4") {
				buffDrawQuad4 = &bufferQuad4[i];


				// copy first node
				nodeCopy = mod->nodes[it->second->node1];
				buffDrawQuad4->x1 = nodeCopy->position->x();
				buffDrawQuad4->y1 = nodeCopy->position->y();
				buffDrawQuad4->z1 = nodeCopy->position->z();

				// copy second node
				nodeCopy = mod->nodes[it->second->node2];
				buffDrawQuad4->x2 = nodeCopy->position->x();
				buffDrawQuad4->y2 = nodeCopy->position->y();
				buffDrawQuad4->z2 = nodeCopy->position->z();

				// copy third node
				nodeCopy = mod->nodes[it->second->node3];
				buffDrawQuad4->x3 = nodeCopy->position->x();
				buffDrawQuad4->y3 = nodeCopy->position->y();
				buffDrawQuad4->z3 = nodeCopy->position->z();

				nodeCopy = mod->nodes[it->second->node4];
				buffDrawQuad4->x4 = nodeCopy->position->x();
				buffDrawQuad4->y4 = nodeCopy->position->y();
				buffDrawQuad4->z4 = nodeCopy->position->z();


				// set PLATE COLOR!!
				if (plateCopy->material_type == STEEL) {
					// STEEL_COLOR: blue
					bufferQuad4[i].r = 0.2f;
					bufferQuad4[i].g = 0.1f;
					bufferQuad4[i].b = 1.0f;
				}
				else if (plateCopy->material_type == TC) {
					// TC_COLOR: orange
					bufferQuad4[i].r = 1.f;
					bufferQuad4[i].g = 0.5f;
					bufferQuad4[i].b = 0.15f;
				}
				else {
					// UNKNOWN TYPE: gray
					bufferQuad4[i].r = .6f;
					bufferQuad4[i].g = .6f;
					bufferQuad4[i].b = .6f;
				}

				i++;
			}
			if (plateCopy->type == "Tri3") {
				buffDrawTri3 = &bufferTri3[j];


				// copy first node
				nodeCopy = mod->nodes[it->second->node1];
				buffDrawTri3->x1 = nodeCopy->position->x();
				buffDrawTri3->y1 = nodeCopy->position->y();
				buffDrawTri3->z1 = nodeCopy->position->z();

				// copy second node
				nodeCopy = mod->nodes[it->second->node2];
				buffDrawTri3->x2 = nodeCopy->position->x();
				buffDrawTri3->y2 = nodeCopy->position->y();
				buffDrawTri3->z2 = nodeCopy->position->z();

				// copy third node
				nodeCopy = mod->nodes[it->second->node3];
				buffDrawTri3->x3 = nodeCopy->position->x();
				buffDrawTri3->y3 = nodeCopy->position->y();
				buffDrawTri3->z3 = nodeCopy->position->z();


				// set PLATE COLOR!!
				if (plateCopy->material_type == STEEL) {
					// STEEL_COLOR: gray
					bufferTri3[j].r = 0.2f;
					bufferTri3[j].g = 0.1f;
					bufferTri3[j].b = 1.0f;
				}
				else if (plateCopy->material_type == TC) {
					// TC_COLOR: orange
					bufferTri3[j].r = 1.f;
					bufferTri3[j].g = 0.5f;
					bufferTri3[j].b = 0.15f;
				}
				else {
					// UNKNOWN TYPE: gray
					bufferQuad4[i].r = .6f;
					bufferQuad4[i].g = .6f;
					bufferQuad4[i].b = .6f;
				}

				j++;
			}

		}
	}
	bufferQuad4Size = i;
	bufferTri3Size = j;
}

void drawFromBeamBuffer() {
	//	glEnable(GL_LINE_SMOOTH);
	//glLineWidth(1.0);

	glBegin(GL_LINES);
	glColor3f(1, 1, 0);
	long i;
	beamDraw *b;
	// LOOPS over the BUFFER
	for (i = 0; i< bufferBeamSize; i++) {
		b = &(bufferBeams[i]);
		//glColor3f(1,1,0);
		glColor3f(b->r, b->g, b->b);
		glVertex3d(b->x1, b->y1, b->z1);
		glVertex3d(b->x2, b->y2, b->z2);
	}
	glEnd();
}

void drawFromNodeBuffer() {


	if (appSettings::MAX_NODE_ID > 20000)
		glPointSize(4.0);
	else
		glPointSize(7.0);
	glBegin(GL_POINTS);
	//999
	long i;
	nodeDraw *b;
	// LOOPS over the BUFFER
	for (i = 0; i< bufferNodeSize; i++) {
		b = &(bufferNodes[i]);
		//glColor3f(1,1,0);
		glColor3f(b->r, b->g, b->b);
		glVertex3d(b->x1, b->y1, b->z1);

		//glutBitmapString(NULL, "asd");
	}
	glEnd();
}

void drawFromQuad4Buffer() {
	//glBegin(GL_QUADS);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	glColor3f(1, 1, 1);
	long i;
	quad4Draw *b;
	// LOOPS over the BUFFER
	for (i = 0; i< bufferQuad4Size; i++) {
		b = &(bufferQuad4[i]);
		//glColor3f(1,1,0);
		//glColor3f( b->r , b->g , b->b );
		//glVertex3d( b->x1 , b->y1 , b->z1 );
		//glVertex3d( b->x2 , b->y2 , b->z2 );
		//glVertex3d( b->x3 , b->y3 , b->z3 );
		//glVertex3d( b->x4 , b->y4 , b->z4 );

		glBegin(GL_LINE_STRIP);
		glColor3f(b->r, b->g, b->b);
		glVertex3d(b->x1, b->y1, b->z1);
		glVertex3d(b->x2, b->y2, b->z2);
		glVertex3d(b->x3, b->y3, b->z3);
		glVertex3d(b->x4, b->y4, b->z4);
		glVertex3d(b->x1, b->y1, b->z1);

		glEnd();

	}
	//glEnd();
}

void drawFromTri3Buffer() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

	//	glBegin(GL_TRIANGLES);
	//	glColor3f(1,1,0);
	long i;
	tri3Draw *b;
	// LOOPS over the BUFFER
	for (i = 0; i< bufferTri3Size; i++) {
		b = &(bufferTri3[i]);
		//glColor3f(1,1,0);

		//glColor3f( b->r , b->g , b->b );
		//glVertex3d( b->x1 , b->y1 , b->z1 );
		//glVertex3d( b->x2 , b->y2 , b->z2 );
		//glVertex3d( b->x3 , b->y3 , b->z3 );

		glBegin(GL_LINE_STRIP);
		glColor3f(b->r, b->g, b->b);
		glVertex3d(b->x1, b->y1, b->z1);
		glVertex3d(b->x2, b->y2, b->z2);
		glVertex3d(b->x3, b->y3, b->z3);
		glVertex3d(b->x1, b->y1, b->z1);
		glEnd();
	}
	//glEnd();
}

void drawFromPlateBuffer() {
	drawFromTri3Buffer();
	drawFromQuad4Buffer();
}

void drawReactForces() {
	glLineWidth(4.0);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	Node *n; Vector3d *f;
	Vector3d iniV, endV;

	for (unsigned long i = 0; i< wm->ffds.NODE_I.size(); i++) {
		n = wm->ffds.NODE_I[i];
		f = &(wm->ffds.REAZ_V[n]);

		iniV = *(n->position);

		// >> note, disegna al contrario, così nn si sovrappone con la beam
		endV = iniV - (*f) / 10.;
		glVertex3d(iniV.x(), iniV.y(), iniV.z());
		glVertex3d(endV.x(), endV.y(), endV.z());
	}

	glEnd();
	glLineWidth(1.0);

}

void drawForces() {

	glLineWidth(2.0);
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	Vector3d iniV, endV;
	// LOOP over NODES and get the points
	Node *n; Vector3d *f;
	for (unsigned long i = 0; i< wm->ffds.NODE_TC_L.size(); i++) {
		n = wm->ffds.NODE_TC_L[i];
		f = &wm->fI[n->ID];
		iniV = wm->posI[n->ID];
		endV = iniV + (*f) / 10;
		glVertex3d(iniV.x(), iniV.y(), iniV.z());
		glVertex3d(endV.x(), endV.y(), endV.z());
	}
	glEnd();
	glLineWidth(1.0);
}

void display(void) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	//glTranslatef(-centerOfRotation.x(), -centerOfRotation.y(), -centerOfRotation.z() );
	//glMultMatrixf(glm::value_ptr(mRotation));
	//glTranslatef(centerOfRotation.x(), centerOfRotation.y(), centerOfRotation.z() );

	//glm::vec4 Position1 = glm::vec4(centerOfRotation.x(), centerOfRotation.y(), centerOfRotation.z(), 1.0);
	//Position1 = mRotation * Position1;

	// moves model matrix to the center of screen
	glTranslatef(xLocation + winWidth / 2, yLocation + winHeight / 2, 0.0f);

	// rotates-scales model matrix
	glMultMatrixf(glm::value_ptr(mRotation));
	glScalef(zoomFactor, zoomFactor, zoomFactor);

	// center of drawing is centerOfRotation (translate wrt the model matrix)
	glTranslatef(-centerOfRotation.x(), -centerOfRotation.y(), -centerOfRotation.z());



	if (mr->showPlates)
		drawFromPlateBuffer();
	drawFromBeamBuffer();
	drawFromNodeBuffer();

	drawForces();
	drawReactForces();

	//////draw red point: centerOfRotation
	glPointSize(7.0);
	glBegin(GL_POINTS);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3d(centerOfRotation.x(), centerOfRotation.y(), centerOfRotation.z());
	glVertex3d(centerOfRotation.x() + 0.1, centerOfRotation.y(), centerOfRotation.z());
	glVertex3d(centerOfRotation.x(), centerOfRotation.y() + 0.1, centerOfRotation.z());
	glVertex3d(centerOfRotation.x(), centerOfRotation.y(), centerOfRotation.z() + 0.1);
	glVertex3d(centerOfRotation.x() - 0.1, centerOfRotation.y(), centerOfRotation.z());
	glVertex3d(centerOfRotation.x(), centerOfRotation.y() - 0.1, centerOfRotation.z());
	glVertex3d(centerOfRotation.x(), centerOfRotation.y(), centerOfRotation.z() - 0.1);

	glEnd();


	if (wm->maxForceNode) {
		glPointSize(7.0);
		glBegin(GL_POINTS);
		glColor3f(1.f, 1.f, 0.f);
		glVertex3d((*wm->maxForceNode->position).x(), (*wm->maxForceNode->position).y(), (*wm->maxForceNode->position).z());
		glVertex3d((*wm->maxForceNode->position).x() + 0.1, (*wm->maxForceNode->position).y(), (*wm->maxForceNode->position).z());
		glVertex3d((*wm->maxForceNode->position).x(), (*wm->maxForceNode->position).y() + 0.1, (*wm->maxForceNode->position).z());
		glVertex3d((*wm->maxForceNode->position).x(), (*wm->maxForceNode->position).y(), (*wm->maxForceNode->position).z() + 0.1);
		glVertex3d((*wm->maxForceNode->position).x() - 0.1, (*wm->maxForceNode->position).y(), (*wm->maxForceNode->position).z());
		glVertex3d((*wm->maxForceNode->position).x(), (*wm->maxForceNode->position).y() - 0.1, (*wm->maxForceNode->position).z());
		glVertex3d((*wm->maxForceNode->position).x(), (*wm->maxForceNode->position).y(), (*wm->maxForceNode->position).z() - 0.1);

		glEnd();
	}
	//cout << " center Rotation: " << " " << centerOfRotation.x()<< " " << 
	//	centerOfRotation.y()<< " " <<  centerOfRotation.z() << endl;


	glutSwapBuffers();

	//glutPostRedisplay();
}

void goDisplay() {
	deleteBufferModelNodes();
	deleteBufferModelBeams();
	deleteBufferModelPlates();
	initBufferModelNodes();
	initBufferModelBeams();
	initBufferModelPlates();
	display();
}

void reshape(int width, int height) {
	winWidth = width;
	winHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)

					  //glOrtho(viewMinX, viewMinX + (1.*width/iniWidth*viewWidth ) * zoomFactor, viewMinY, viewMinY + (1.*height/iniHeight*viewHeight ) * zoomFactor, viewMinZ * zoomFactor, viewMaxZ * zoomFactor);
	glOrtho(0., 1.*width, 0, 1.*height, viewMinZ * zoomFactor, viewMaxZ * zoomFactor);

	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}


void initBox() {
	getBoxView(xMin, xMax, yMin, yMax, zMin, zMax);
	//	cout <<" BOX: " << " " << xMin<< " " <<  xMax<< " " <<  yMin<< " " <<  yMax<< " " <<  zMin<< " " <<  zMax << "\n";

	maxDim = max(max(xMax - xMin, yMax - yMin), zMax - zMin);
	viewHeight = (float)maxDim * 1.2;
	viewWidth = (float)maxDim * 1.2;

	viewMinX = (float)xMin - viewHeight * 0.2;
	viewMinY = (float)yMin - viewWidth * 0.2;

	viewMinZ = (float)zMin - 100000.f, viewMaxZ = (float)zMax + 100000.f;

	zoomFactor = 1.*iniWidth / viewWidth;

	//	centerOfRotation << 0., 0., 0.;
}





void processMenuEvents(int option) {
	string newFileName("");

	switch (option) {
	case EXIT_BUTTON:
		exit(0);
		break;
	case ZOOM_EXTENSION:
		initBox();
		zoomFactor = 1.*iniWidth / viewWidth;
		xLocation = 0.;
		yLocation = 0.;
		mRotation = glm::mat4(1.0);
		display();
		break;

	case SELECT_POINT:
		cout << "\n go Select Point " << mouseX << " " << mouseY;
		getNearestNode(mouseX, mouseY);

		break;
	default:
		mouseSel = option;
	}

}



void createGLUTMenus() {

	int menu;

	menu = glutCreateMenu(processMenuEvents);


	//add entries to our menu
	//glutAddMenuEntry("Zoom",ZOOM);
	glutAddMenuEntry("Zoom Extension", ZOOM_EXTENSION);

	glutAddMenuEntry("Rotate", ROTATE);
	glutAddMenuEntry("Pan", PAN);
	glutAddMenuEntry("Select", SELECT_POINT);
	glutAddMenuEntry("Exit", EXIT_BUTTON);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void OnIdle(void)
{
	//printf("\n idle.. sleep 300");
	Sleep(200);

}

void ModelRenderer::init3DView(Model *model, int argc, char* argv[]) {
	glutInit(&argc, argv);
	mod = model;
	mr = this;

	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(iniWidth, iniHeight); // Set the width and height of the window
	glutInitWindowPosition(500, 100); // Set the position of the window
	glutCreateWindow(".:OpenGL:."); // Set the title for the window

	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering
	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping
	glutMouseFunc(mouseFunc);
	glutIdleFunc(OnIdle);

	glutMotionFunc(mouseMoveFunc);
	glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
	glEnable(GL_DEPTH_TEST);

	initBox();
	initBufferModelBeams();
	initBufferModelNodes();
	initBufferModelPlates();
	createGLUTMenus();
	mouseSel = ROTATE;

	mRotation = glm::mat4(1.0f);

	getNearestNode(0, 0);

	xLocation = 0.;
	yLocation = 0.;
}


void ModelRenderer::show3DView() {


	glutMainLoop();
}

void forceRedraw()
{
	glutPostRedisplay();
	const char *windowTitle = ".:OpenGL:.";
	//HWND windowHandle = FindWindow(NULL, windowTitle);
	//HDC deviceContext = GetDC(windowHandle);
	//HGLRC renderingContext = wglGetCurrentContext();
	//SetActiveWindow(windowHandle);
	//SetFocus(windowHandle);

	// Create a generic keyboard event structure
	//INPUT ip;
	//ip.type = INPUT_KEYBOARD;
	//ip.ki.wScan = 0;
	//ip.ki.time = 0;
	//ip.ki.dwExtraInfo = 0;
	//// Press the "1" key
	//ip.ki.wVk = '1';
	//ip.ki.dwFlags = 0; // 0 for key press
	//SendInput(1, &ip, sizeof(INPUT));
	//// Release the "1" key
	//ip.ki.wVk = '1';
	//ip.ki.dwFlags = KEYEVENTF_KEYUP;
	//SendInput(1, &ip, sizeof(INPUT));

	//glutPostRedisplay();
	//const char *windowTitle2 = ".:Form Finding:.";
	//HWND windowHandle2 = FindWindow(NULL, windowTitle);

}






























void goIteration() {

	for (int i = 0; i<appSettings::RENDERSTEPS; i++) {
		wm->doOneIteration();
	}
	wm->updateNodes();

	goDisplay();

	//	glutPostRedisplay();

}



void keyPressed(unsigned char key, int x, int y) {

	if (key == 'g' || key == 'G')
		for (long i = 0; i<appSettings::RENDER_ITERATION_STEPS; i++) {
			goIteration();
			display();
			//glutPostRedisplay();
		}

	if (mouseSel == ROTATE) {
		float rotAngle = 0.3f;
		if (key == 'w') rotateX(rotAngle);
		if (key == 's') rotateX(-rotAngle);
		if (key == 'a') rotateY(rotAngle);
		if (key == 'd') rotateY(-rotAngle);
	}

	if (mouseSel == PAN) {
		if (key == 'w') yLocation = yLocation + 0.1f * viewHeight;
		if (key == 's') yLocation = yLocation - 0.1f * viewHeight;
		if (key == 'a') xLocation = xLocation - 0.1f * viewWidth;
		if (key == 'd') xLocation = xLocation + 0.1f * viewWidth;
	}

	if (key == 'o' || key == 'O')
		zoomFactor = zoomFactor * 0.8;
	if (key == 'i' || key == 'I')
		zoomFactor = zoomFactor / 0.8;

	if (key == 'p' || key == 'P') {
		cout << " PAN "; mouseSel = PAN;
	}
	if (key == 'r' || key == 'R') {
		cout << " ROTATE "; mouseSel = ROTATE;
	}
	if (key == '1') {
		cout << " refresh "; // do nothing, just refresh
	}

	if (key == 27) {
		//		exit(1);
		//		glutLeaveMainLoop();
		glutDestroyWindow(1);
		return;
	}

	display();

}


time_t clickTime1;
int clickX1, clickY1;

void mouseFunc(int wheel, int direction, int x, int y) {
	//cout << "\n wheel " << wheel << "  " <<   direction<< "  "  << x << "  " <<  y << " ";
	if ((wheel == 3) && (direction == 0))
		zoomFactor = zoomFactor / 0.8;
	if ((wheel == 4) && (direction == 0))
		zoomFactor = zoomFactor * 0.8;
	glutPostRedisplay();

	if ((wheel == 0) && (direction == 0)) {

		time_t clickTime2;
		time(&clickTime2);
		if ((clickTime2 - clickTime1 <= 1) && (abs(x - mouseX)<10.) && (abs(y - mouseY)<10.)) {
			clickTime1 = 0;
			getNearestNode(x, y);
		}

		mouseX = x; mouseY = y;
		clickTime1 = clickTime2;


	}

}


void mouseMoveFunc(int x, int y) {
	//		cout << " mouseMove " << x << " " << y << endl;
	int deltaX, deltaY;
	deltaX = x - mouseX;
	deltaY = y - mouseY;
	mouseX = x;
	mouseY = y;

	if (mouseSel == ROTATE) {
		// movimento del mouse in verticale, ruota attorno all'asse X
		if (abs(deltaY)>abs(deltaX)) {
			rotateX(1.*deltaY / 800 * 4.);
		}
		else {
			// movimento del mouse in orizzontale, ruota attorno all'asse Y
			rotateY(1.*deltaX / 800 * 4.);
		}
	}

	if (mouseSel == PAN) {
		xLocation = xLocation + 1.0*deltaX;
		yLocation = yLocation - 1.0*deltaY;
	}


	glutPostRedisplay();
}


// forward declaration for this utility function
void printVector(Vector3d* f);

void getNearestNode(int mouseX, int mouseY) {
	//cout << "\n getNearestNode  " << mouseX << " " << mouseY;

	// displView : vettore differenza, su schermo
	glm::vec4 displView(1. / zoomFactor *(mouseX - (xLocation + winWidth / 2)), 1. / zoomFactor *((winHeight - mouseY - 1) - (yLocation + winHeight / 2)), 0., 0.);

	// zVect : vettore z, su schermo
	glm::vec4 zVect(0., 0., 1., 0.);

	// inverse rotation
	glm::mat4 mRotInv = inverse(mRotation);

	// displView : vettore differenza, on model
	glm::vec4 displ = mRotInv * displView;
	//cout << "\n displ  " << displ[0] << " " << displ[1] << " " << displ[2] << " " ;

	// z2: z vector bringed to model
	glm::vec4 z2 = mRotInv * zVect;

	Vector3d zVersor;
	zVersor << z2[0], z2[1], z2[2];
	zVersor.normalize();

	// one point of the "Z" axis
	Vector3d p0 = Vector3d(centerOfRotation[0] + displ[0], centerOfRotation[1] + displ[1], centerOfRotation[2] + displ[2]);

	double minDist = 10e50;
	Vector3d p;
	Vector3d oldCenterRotation = centerOfRotation;
	Node *selectedNode;

	// LOOPS over nodes, search the one with minimum distance
	BOOST_FOREACH(mapElement_Node n1, mod->nodes) {
		Node *n = n1.second;

		p = *n->position;

		// projection
		Vector3d projection = zVersor * zVersor.dot(p - p0);
		// distance to the "Z" rotated axis
		double dist = ((p - p0) - projection).norm();

		if (dist < minDist) {
			minDist = dist;
			centerOfRotation = p;
			selectedNode = n;
		}

	}


	cout << endl;
	cout << endl << "Selected Node ID: " << selectedNode->ID;
	cout << endl << "Mass of Node:     " << selectedNode->massNode;
	cout << endl << "Stiffness:        " << selectedNode->totalStiffness;
	cout << endl << "Position of Node: "; printVector(selectedNode->position);
	cout << endl << "Force on Node:    "; printVector(&wm->fI[selectedNode->ID]);
	cout << endl << "Enabled:         "; if (selectedNode->enabled) cout << "TRUE"; else cout << "FALSE";
	cout << endl << endl;
	//cout << endl << "Position I: " ; printVector(&wm->posI[selectedNode->ID]);
	//cout << endl << "Position I-1: " ; printVector(&wm->posIm1[selectedNode->ID]);
	//cout << endl << "Position I-2: " ; printVector(&wm->posIm2[selectedNode->ID]);


	// updates the xLocation and yLocation
	Vector3d diff1 = centerOfRotation - oldCenterRotation;
	diff1 = diff1 * zoomFactor;
	glm::vec4 disp1(diff1[0], diff1[1], diff1[2], 0.);
	glm::vec4 dispScreen1 = mRotation * disp1;

	xLocation += dispScreen1[0];
	yLocation += dispScreen1[1];

}

