/*
*	Title: LetterPartitioning
*	Developer: Bret Oplinger
*	Last Modified: April 2016
*	Description: This project divides a letter into 10 different partitions.
*				 The partitioning is performed by generating sites inside a letter.
*				 Those sites are used to generate a Voronoi diagram. Each site has
*				 its own cell. These cells are the partitions desired.
*
*				 This project was enacted to fix an issue with the PTHaptics application.
*				 There was a need to find a beter way to partition a letter. The previous
*				 iteration just made use of a grid to divide a letter. This led to too many
*				 polygons being displayed. 
*
*	Credits: I got the source code to generate a Voronoi diagram from a set of points 
*			 from http://blog.ivank.net/fortunes-algorithm-and-implementation.html
*
*/
#include <windows.h>
#include <GL/gl.h> // Include the GLEW header file
#include <GL/glut.h> // Include the GLUT header file
#include <iostream>
#include <math.h>
#include <algorithm>
#include <time.h>

#include "Voronoi.h"
#include "VPoint.h"
#include "Letter.h"
#include "FitnessValue.h"

void display(void);
void initializeCamera();
void initializeLetter();
VPoint* findPointViaFitness();
std::vector<VPoint*> generateSamplePoints();
bool compareXValuesInVertices(VPoint* p);
bool compareYValuesInVertices(VPoint* p);
bool isGreater(FitnessValue val1, FitnessValue val2);

vor::Voronoi * v;
vor::Vertices * ver; 
vor::Edges * edg;	 

Letter* lowercase_c = nullptr;

int w = 100;
const int NUMBER_OF_SITES = 10;
const int NUMBER_OF_SAMPLES = 1000;

int main(int argc, char **argv)
{
	using namespace vor;

	v = new Voronoi();
	ver = new Vertices();

	initializeLetter();

	srand(time(NULL));

	for (int i = 0; i < NUMBER_OF_SITES; i++)
	{
		ver->push_back(findPointViaFitness());
	}

	edg = v->GetEdges(ver, w, w);

	glutInit(&argc, argv); // Initialize GLUT
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // Set up a basic display buffer (only single buffered for now)
	glutInitWindowSize(1024, 768); // Set the width and height of the window
	glutInitWindowPosition(100, 100); // Set the position of the window
	glutCreateWindow("Letter Partitioning"); // Set the title for the window

	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering

	initializeCamera();

	glutMainLoop(); // Enter GLUT's main loop

	return 0;
}

VPoint* findPointViaFitness() {
	std::vector<VPoint*> samples = generateSamplePoints();
	std::vector<FitnessValue> fitnessValues;
	
	for (std::size_t i = 0; i < samples.size(); ++i) {
		FitnessValue fitnessValue = FitnessValue(0.0, i);

		double distanceToNearestPoint = 10000000000.0;
		for (auto j = ver->begin(); j != ver->end(); ++j) {
			double distance = (*j)->calculateDistance(samples[i]);
			if (distanceToNearestPoint > distance) distanceToNearestPoint = distance;
		}
		
		double distanceToNearestWall = 10000000000.0;
		std::vector<double> distancesToWallFromSamples = lowercase_c->calculateDistancesToWalls(samples[i]);
		for (std::size_t j = 0; j < distancesToWallFromSamples.size(); ++j) {
			if (distanceToNearestWall > distancesToWallFromSamples[j]) {
				distanceToNearestWall = distancesToWallFromSamples[j];
			}
		}
		fitnessValue.fitness = distanceToNearestPoint + 3*distanceToNearestWall;
		fitnessValues.push_back(fitnessValue);
	}

	std::sort(fitnessValues.begin(), fitnessValues.end(), isGreater);
	return samples[fitnessValues[0].indexOfSample];
}

std::vector<VPoint*> generateSamplePoints() {
	std::vector<VPoint*> samples;
	for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
	{
		VPoint* point = NULL;
		bool foundRandomSiteWithUniqueY = true;
		while (foundRandomSiteWithUniqueY) {
			point = new VPoint(rand() % w, rand() % w);
			if (!compareXValuesInVertices(point) && 
				!compareYValuesInVertices(point) && 
				lowercase_c->isPointInLetter(point)) {
				foundRandomSiteWithUniqueY = false;
			}
		}
		samples.push_back(point);
	}
	return samples;
}

bool isGreater(FitnessValue val1, FitnessValue val2) {
	return val1.fitness > val2.fitness;
}

bool compareYValuesInVertices(VPoint* p) {
	for (auto it = ver->begin(); it != ver->end(); ++it) {
		if ((*it)->y == p->y) {
			return true;
		}
	}
	return false;
}

bool compareXValuesInVertices(VPoint* p) {
	for (auto it = ver->begin(); it != ver->end(); ++it) {
		if ((*it)->x == p->x) {
			return true;
		}
	}
	return false;
}

void drawVoronoi(){
	for (vor::Vertices::iterator i = ver->begin(); i != ver->end(); ++i){
		glBegin(GL_QUADS);
		//std::cout << (*i)->x << " , " << (*i)->y  << "\n";
		glColor3f(0.0, 1.0, 1.0);
		glVertex2f((*i)->x - 0.5, (*i)->y - 0.5);
		glVertex2f((*i)->x + 0.5, (*i)->y - 0.5);
		glVertex2f((*i)->x + 0.5, (*i)->y + 0.5);
		glVertex2f((*i)->x - 0.5, (*i)->y + 0.5);
		glEnd();
	}

	
	for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i){
		//cout << (*i)->start->x << ", " << (*i)->start->y << endl;
		//cout << (*i)->end->x << ", " << (*i)->end->y << endl;
		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 0.0);
		glVertex2f((*i)->start->x, (*i)->start->y);
		glVertex2f((*i)->end->x, (*i)->end->y);
		glEnd();

	}
}

void drawLetter() {
	for (std::size_t i = 0; i < lowercase_c->polygon.size() - 1; ++i) {
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 1.0);
		glVertex2f(lowercase_c->polygon[i]->x, lowercase_c->polygon[i]->y);
		glVertex2f(lowercase_c->polygon[i + 1]->x, lowercase_c->polygon[i + 1]->y);
		glEnd();
	}
}

void display(void)
{
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
	glClear(GL_COLOR_BUFFER_BIT);//Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the background of our window to red  

	drawVoronoi();
	drawLetter();
	glutSwapBuffers();

}

void initializeCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, w);
	glMatrixMode(GL_MODELVIEW);
}

void initializeLetter() {
	vector<VPoint*> points = getPoints("../dxf/lowercase_c.dxf"); 
	lowercase_c = new Letter(points);
	
	/*
	points.push_back(new VPoint(20, 80));
	points.push_back(new VPoint(20, 20));
	points.push_back(new VPoint(70, 20));
	points.push_back(new VPoint(70, 40));
	points.push_back(new VPoint(40, 40));
	points.push_back(new VPoint(40, 60));
	points.push_back(new VPoint(70, 60));
	points.push_back(new VPoint(70, 80));
	points.push_back(new VPoint(20, 80));
	*/
}
vector<VPoint*> getPoints (string filename){
    vector<VPoint*> points; 
    
    VPoint point; 
    ifstream input(filename);
    for (string line; getline(input, line);){
        VPoint point; 
        string tLine = trim(line);

        if (tLine.compare("0") == 1){
            getline(input, line);
            tLine = trim(line); 
            if (tLine.compare("10") == 1){ // start x
                getline(input, line); 
                tLine = trim(line); 
                point.x = stod(line); 
                
                getline(input, line); 
                tLine = trim(line); 
                if (tLine.compare("20") == 1){
                    getline(input, line); 
                    point.y = stod(trim(line)); 
                    
                    points.push_back(point); 
                    cout << "x=" << point.x << ", y=" << point.y << endl; 
                }
            }
        }
    }
    return points; 
}

