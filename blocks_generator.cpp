#include "blocks_generator.h"

/*
This function returns a random block.
*/
Block * BlocksGenerator::getRandomBlock() {
	//Getting the default values for a general point in the game.
	int xLocation = Point::MIDDLE_X_POSITION;
	int yLocation = Point::GAME_LOCATION_OFFSET_Y;

	vector<Point *> blockLocations;

	int randNum = (rand() % 100) + 1; //Calculating a number between 1 and 100.

	//Checking which block the random number is applicable to.
	if (randNum <= SQUARE_CHANCE) {
		//Setting the square's locations according to the initial location of the square.
		blockLocations.reserve(4);
		blockLocations.push_back(new Point(xLocation, yLocation));
		blockLocations.push_back(new Point(xLocation + 1, yLocation));
		blockLocations.push_back(new Point(xLocation, yLocation + 1));
		blockLocations.push_back(new Point(xLocation + 1, yLocation + 1));
	}
	else if (randNum <= LINE_CHANCE) {
		//Setting the line's locations according to the initial location of the line.
		blockLocations.reserve(4);
		blockLocations.push_back(new Point(xLocation, yLocation));
		blockLocations.push_back(new Point(xLocation + 1, yLocation));
		blockLocations.push_back(new Point(xLocation + 2, yLocation));
		blockLocations.push_back(new Point(xLocation + 3, yLocation));
	}
	else if (randNum <= SNAKE_CHANCE) {
		//Setting the snake's locations according to the initial location of the snake.
		blockLocations.reserve(4);
		blockLocations.push_back(new Point(xLocation, yLocation));
		blockLocations.push_back(new Point(xLocation + 1, yLocation));
		blockLocations.push_back(new Point(xLocation + 1, yLocation + 1));
		blockLocations.push_back(new Point(xLocation + 2, yLocation + 1));
	}
	else if (randNum <= GAMMA_CHANCE) {
		//Setting the gammas's locations according to the initial location of the gamma.
		blockLocations.reserve(4);
		blockLocations.push_back(new Point(xLocation, yLocation));
		blockLocations.push_back(new Point(xLocation, yLocation + 1));
		blockLocations.push_back(new Point(xLocation + 1, yLocation + 1));
		blockLocations.push_back(new Point(xLocation + 2, yLocation + 1));
	}
	else if (randNum <= PLUS_CHANCE) {
		//Setting the plus' locations according to the initial location of the plus.
		blockLocations.reserve(4);
		blockLocations.push_back(new Point(xLocation, yLocation));
		blockLocations.push_back(new Point(xLocation - 1, yLocation + 1));
		blockLocations.push_back(new Point(xLocation, yLocation + 1));
		blockLocations.push_back(new Point(xLocation + 1, yLocation + 1));
	}
	else {
		if (randNum <= JOKER_CHANCE) {
			return (Block *)new Joker(Point());
		}
		else {
			return (Block *)new Bomb(Point());
		}
	}
	
	return (Block *)new GeneralBlock(blockLocations);
}

/*
This function receives a block and returns whether its type is Joker.
*/
bool BlocksGenerator::isJoker(Block *block) {
	Joker *joker = dynamic_cast<Joker *>(block);
	if (joker != nullptr) {
		return true;
	}

	return false;
}

/*
This function receives a block and returns whether its type is Bomb.
*/
bool BlocksGenerator::isBomb(Block *block) {
	Bomb *bomb = dynamic_cast<Bomb *>(block);
	if (bomb != nullptr) {
		return true;
	}

	return false;
}