#ifndef __BLOCKS_GENERATOR_H
#define __BLOCKS_GENERATOR_H

#include "general_block.h"
#include "bomb.h"
#include "joker.h"

class BlocksGenerator {
protected:
	constexpr static int SQUARE_CHANCE = 16;
	constexpr static int LINE_CHANCE = 32;
	constexpr static int SNAKE_CHANCE = 48;
	constexpr static int GAMMA_CHANCE = 64;
	constexpr static int PLUS_CHANCE = 80;
	constexpr static int JOKER_CHANCE = 90;
	constexpr static int BOMB_CHANCE = 100;

public:
	static Block * getRandomBlock();

	static bool isJoker(Block *block);
	static bool isBomb(Block *block);
};

#endif