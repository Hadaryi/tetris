#include "joker.h"

/*
Constructor - receives the initial location of where the block should be created and the char that represents the block.
*/
Joker::Joker(const Point& location, char ch) {
	this->ch = ch;
	this->rotateable = false;
	this->blockLocations.reserve(JOKER_SIZE);

	//Setting the joker's locations according to the initial location of the joker.
	this->blockLocations.push_back(new Point(location));
}