#ifndef __BOMB_H
#define __BOMB_H

#include "block.h"

class Bomb : public Block {
public:
	constexpr static int BOMB_SIZE = 1;

	Bomb(const Point& location, char ch = Block::NORMAL_SQUARE_CHAR);
};

#endif