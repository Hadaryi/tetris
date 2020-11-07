#include "general_block.h"

GeneralBlock::GeneralBlock(vector<Point *>& blockLocations, char ch) {
	this->blockLocations = blockLocations;
	this->ch = ch;

	this->updateBlockProperties();
}