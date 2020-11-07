#include "point.h"

/*
Constructor - initializes a new point with given x and y parameters in the console.
*/
Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

/*
This function receives a parameter x and sets the x property of the current point to the given parameter.
*/
void Point::setX(int x) {
	this->x = x;
}

/*
This function receives a parameter y and sets the y property of the current point to the given parameter.
*/
void Point::setY(int y) {
	this->y = y;
}

/*
This function returns the x property of the current point.
*/
int Point::getX() const {
	return this->x;
}

/*
This function returns the y property of the current point.
*/
int Point::getY() const {
	return this->y;
}