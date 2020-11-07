#ifndef __GENERAL_BLOCK_H
#define __GENERAL_BLOCK_H

#include "block.h"

class GeneralBlock : public Block {
public:
	GeneralBlock(vector<Point *>& blockLocations, char ch = Block::NORMAL_SQUARE_CHAR);
};

#endif