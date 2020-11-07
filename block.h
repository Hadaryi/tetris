#ifndef __BLOCK_H
#define __BLOCK_H

#include <iostream>
#include <vector>
#include "point.h"
#include "Gotoxy.h"
using namespace std;

class Block {
protected:
	char ch;

	vector<Point *> blockLocations; //This property saves the locations in the console for each square of the block.
	int rotatedAmount = 0;
	bool rotateable = true;
	int colAmount = 0;
	int rowAmount = 0;
	int maxColIndex = 0;
	int maxRowIndex = 0;

	void setBlockLocations(vector<Point *>& blockLocations);

public:
	constexpr static char NORMAL_SQUARE_CHAR = '#';
	constexpr static char BOMB_SQUARE_CHAR = '@';

	virtual ~Block();

	vector<Point *>& getBlockLocations();
	virtual vector<Point *> getRotatedPosition();
	virtual void rotateRight();
	bool isRotateable() const;

	void paint() const;
	void removeBlockFromConsole() const;

	void updateBlockProperties();
	void setRotatedAmount(int rotateAmount);
	int getRotatedAmount() const;

	static void clearVectorOfDynamicPoints(vector<Point *>& vec);
};

#endif