#pragma once
#include <math.h>

/*
A structure that stores 2D point
*/

class VPoint
{
public:

	double x, y;

	/*
	Constructor for structure; x, y - coordinates
	*/

	VPoint(double nx, double ny)
	{
		x = nx;
		y = ny;
	}
	
	double calculateDistance(VPoint* point) {
		double distancex = pow((point->x - this->x),2);
		double distancey = pow((point->y - this->y),2);
		double result = sqrt(distancex + distancey);
		return result;
	}
};
