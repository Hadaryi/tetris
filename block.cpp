#include "block.h"

/*
Destructor - clears the memory used for the points in the block locations vector.
*/
Block::~Block() {
	Block::clearVectorOfDynamicPoints(this->blockLocations);
}

/*
This function clears the memory used for a vector of dynamic points.
*/
void Block::clearVectorOfDynamicPoints(vector<Point *>& vec) {
	vector<Point *>::iterator itr = vec.begin();
	vector<Point *>::iterator itrEnd = vec.end();

	for (; itr != itrEnd; ++itr) {
		delete *itr;
	}
}

/*
This function returns the block's locations in the console.
The class Tetris is allowed to change this property, therefore it is not a const function.
*/
vector<Point *>& Block::getBlockLocations() {
	return this->blockLocations;
}

/*
This function returns whether the block is rotateable.
*/
bool Block::isRotateable() const {
	return this->rotateable;
}

/*
This function sets the block's squares locations in the console.
*/
void Block::setBlockLocations(vector<Point *>& blockLocations) {
	Block::clearVectorOfDynamicPoints(this->blockLocations);
	this->blockLocations = blockLocations;
	this->updateBlockProperties();
}

/*
This function paints the block into the console.
*/
void Block::paint() const {
	vector<Point *>::const_iterator itr = this->blockLocations.begin();
	vector<Point *>::const_iterator itrEnd = this->blockLocations.end();

	for (; itr != itrEnd; ++itr) {
		gotoxy((*itr)->getX(), (*itr)->getY());
		cout << this->ch;
	}
}

/*
This function prints spaces instead of the block in the console.
*/
void Block::removeBlockFromConsole() const {
	vector<Point *>::const_iterator itr = this->blockLocations.begin();
	vector<Point *>::const_iterator itrEnd = this->blockLocations.end();

	for (; itr != itrEnd; ++itr) {
		gotoxy((*itr)->getX(), (*itr)->getY());
		cout << " ";
	}
}

/*
This function rotates the block to the right.
*/
void Block::rotateRight() {
	if (this->rotateable) {
		this->setBlockLocations(this->getRotatedPosition());
		this->rotatedAmount++; //Increasing the amount of times the block was rotated.
	}
}

/*
Rotating the block according to a formula we've discovered.
*/
vector<Point *> Block::getRotatedPosition() {
	vector<Point *> rotatedPosition;
	
	vector<Point *>::iterator itr = this->blockLocations.begin();
	vector<Point *>::iterator itrEnd = this->blockLocations.end();

	for (; itr != itrEnd; ++itr) {
		int newX, newY, x, y, relativeX, relativeY;

		x = (*itr)->getX();
		y = (*itr)->getY();

		relativeX = abs(this->maxColIndex - x - this->colAmount); //Getting the relative position of X according to the rest of the points.
		relativeY = abs(this->maxRowIndex - y - this->rowAmount); //Getting the relative position of Y according to the rest of the points.

		newY = relativeX; //Setting the newY to the relative value of x according to the other points in the vector.
		
		if (this->rotatedAmount % 2 == 0) {
			newX = abs(relativeY - this->colAmount); //Getting the new relative value of x according to the relative value of y.
			newX += this->maxColIndex - this->colAmount; //Returning newX to the real value in the board.
		}
		else {
			newX = abs(relativeY - this->rowAmount); //Getting the new relative value of x according to the relative value of y.
			newX += this->maxColIndex - this->rowAmount; //Returning newX to the real value in the board.
		}

		newY += this->maxRowIndex - this->rowAmount; //Returning newY to the real value in the board.

		rotatedPosition.push_back(new Point(newX, newY));
	}

	return rotatedPosition;
}

/*
This function updates the maxRowIndex, maxColIndex, colAmount and rowAmount properties of the block.
*/
void Block::updateBlockProperties() {
	int minColIndex, minRowIndex;
	Point *p;

	vector<Point *>::iterator itr = this->blockLocations.begin();
	vector<Point *>::iterator itrEnd = this->blockLocations.end();
	p = *itr;

	minColIndex = this->maxColIndex = p->getX();
	minRowIndex = this->maxRowIndex = p->getY();

	//Setting the maxColIndex and maxRowIndex properties for the block.
	for (++itr; itr != itrEnd; ++itr) {
		p = *itr;
		int x = p->getX();
		int y = p->getY();

		if (x > this->maxColIndex) {
			this->maxColIndex = x;
		}

		if (x < minColIndex) {
			minColIndex = x;
		}

		if (y > this->maxRowIndex) {
			this->maxRowIndex = y;
		}

		if (y < minRowIndex) {
			minRowIndex = y;
		}
	}

	this->colAmount = this->maxColIndex - minColIndex;
	this->rowAmount = this->maxRowIndex - minRowIndex;
}

/*
Setting the amount of times the block was rotated - used for loading the block from the file.
*/
void Block::setRotatedAmount(int rotateAmount) {
	this->rotatedAmount = rotateAmount;
}

/*
Getting the amount of times the block was rotated - used for saving the block to the file.
*/
int Block::getRotatedAmount() const {
	return this->rotatedAmount;
}