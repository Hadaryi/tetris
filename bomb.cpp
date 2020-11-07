#include "bomb.h"

/*
Constructor - receives the initial location of where the block should be created and the char that represents the block.
*/
Bomb::Bomb(const Point& location, char ch) {
	this->ch = BOMB_SQUARE_CHAR;
	this->rotateable = false;
	this->blockLocations.reserve(BOMB_SIZE);

	//Setting the bomb's location according to the initial location of the bomb.
	this->blockLocations.push_back(new Point(location));
}