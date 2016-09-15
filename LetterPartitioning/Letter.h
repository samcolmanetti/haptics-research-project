#pragma once
#include <vector>
#include <math.h>

#include "VPoint.h"

using namespace std;

class Letter {
public:
	vector<VPoint*> polygon;

	Letter(const vector<VPoint*> points) {
		polygon = points;
	}
	
	bool isPointInLetter(VPoint* p) {
		int numVerts = polygon.size();
		bool pointIsInside = false;
		for (int i = 0, j = 1; i < numVerts - 1; i = j++) {
			if (((polygon[i]->y > p->y) != (polygon[j]->y > p->y))) {
				if (p->x < (polygon[j]->x - polygon[i]->x) * (p->y - polygon[i]->y) /
					(polygon[j]->y - polygon[i]->y) + polygon[i]->x) {
					pointIsInside = !pointIsInside;
				}
			}
		}
		return pointIsInside;
	}

	vector<double> calculateDistancesToWalls(VPoint* point) {
		vector<double> distances;
		for (size_t i = 0, j = 1; i < (polygon.size() - 1); i = j++) {
			double distance = 0.0;
			double diffX = polygon[j]->x - polygon[i]->x;
			double diffY = polygon[j]->y - polygon[i]->y;
			if ((diffX == 0) && (diffY == 0)) {
				diffX = point->x - polygon[i]->x;
				diffY = point->y - polygon[i]->y;
				distance = sqrt(pow(diffX, 2) + pow(diffY, 2));
			}
			else {
				double t = ((point->x - polygon[i]->x) * diffX + (point->y - polygon[i]->y) * diffY) 
							/ (pow(diffX, 2) + pow(diffY, 2));
				if (t < 0) {
					diffX = point->x - polygon[i]->x;
					diffY = point->y - polygon[i]->y;
				}
				else if (t > 1) {
					diffX = point->x - polygon[j]->x;
					diffY = point->y - polygon[j]->y;
				}
				else {
					diffX = point->x - (polygon[i]->x + t * diffX);
					diffY = point->y - (polygon[i]->y + t * diffY);
				}
				distance = sqrt(pow(diffX, 2) + pow(diffY, 2));
			}
			distances.push_back(distance);
		}
		return distances;
	}
};