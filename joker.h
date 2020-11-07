#ifndef __JOKER_H
#define __JOKER_H

#include "block.h"

class Joker : public Block {
public:
	constexpr static int JOKER_SIZE = 1;

	Joker(const Point& location, char ch = Block::NORMAL_SQUARE_CHAR);
};

#endif