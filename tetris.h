#ifndef __TETRIS_H
#define __TETRIS_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <Windows.h>
#include <conio.h>
using namespace std;

#include "Gotoxy.h"
#include "block.h"
#include "blocks_generator.h"

class Tetris {
public:
	//Definition of each keypress and what it does.
	enum eKeys {ROTATE_RIGHT_KEY = 'r', MOVE_LEFT_KEY = 'q', MOVE_DOWN_KEY = 'w', MOVE_RIGHT_KEY = 'e', JOKER_PAUSE_KEY = 's', GAME_START_KEY = '1', GAME_PAUSE_KEY = '2', GAME_INCREASE_SPEED_KEY = '3', GAME_DECREASE_SPEED_KEY = '4', GAME_SAVE_KEY = '5', GAME_LOAD_KEY = '6', GAME_EXIT_KEY = '9'};

	constexpr static int ROWS = 15;
	constexpr static int COLS = 10;
	constexpr static int WINDOW_WIDTH = 450;
	constexpr static int WINDOW_HEIGHT = 550;

	constexpr static int MENU_LINES_AMOUNT = 8;

	constexpr static int GAME_SPEED_CHANGE_AMOUNT = 50;
	constexpr static int MENU_WAIT_FOR_ACTION_DELAY = 200;

	constexpr static int MOVE_TO_BOTTOM_SCORE_MULTIPLIER = 2;
	constexpr static int JOKER_LINE_REMOVED_SCORE = 50;
	constexpr static int BOMB_EXPLODE_SCORE_PENALTY = 50;
	constexpr static int LINES_REMOVED_SCORE_1 = 100;
	constexpr static int LINES_REMOVED_SCORE_2 = 300;
	constexpr static int LINES_REMOVED_SCORE_3 = 500;
	constexpr static int LINES_REMOVED_SCORE_4 = 800;

	constexpr static int MAXIMUM_SPEED = 100;

	//Files constants.
	constexpr static char *FILE_NAME = "saved.bin";
	constexpr static char NO_BLOCK = -1;
	constexpr static char REGULAR_BLOCK = 0;
	constexpr static char JOKER_BLOCK = 1;
	constexpr static char BOMB_BLOCK = 2;

	Tetris();
	~Tetris();

private:
	bool isStarted = false; //This property saves whether the game has started or not.
	bool isFailed = false; //This property saves whether the blocks reached the end of the board.
	int speed = 350; //This property saves the game's speed in miliseconds.
	int usedPoints[ROWS][COLS] = {}; //This property saves the locations of the used points inside the board.
	Block *currentBlock; //This property saves the current block that is falling down.
	int score = 0;
	int blocksDropped = 0;

	int noticeCharactersWritten = 0; //This property saves the amount of characters written in the notice area for cleaning purposes.

	Tetris(const Tetris& other) = delete; //Removing the copy constructor since it's not needed.

	void changeConsoleSize(int width, int height);
	void displayMenu();

	bool menuActionHandler(char keyPressed, bool arrivedFromOngoingGame);
	void waitForMenuAction();
	void exitGame();
	void endGame();
	void showNotice(const string& notice);

	void displayGameDetails();
	void updateGameDetails();

	void gameEngine();
	void startGame();
	void continueGame();
	void pauseGame();

	void paintBoard();
	void clearBoard();
	void setUsedPoints();
	void setSpeed(int speed);
	void increaseSpeed(int speed);
	void decreaseSpeed(int speed);

	int getScore() const;
	int getNumOfBlocks() const;

	void addNewBlock();
	void setScore(int score);
	void increaseScore(int score);
	void decreaseScore(int score);
	void setBlocksDropped(int blocksDropped);
	void increaseNumOfBlocks();

	bool canBlockMoveDown();
	bool canBlockMoveRight();
	bool canBlockMoveLeft();
	bool canBlockRotateRight();

	void moveBlockDown();
	void moveBlockRight();
	void moveBlockLeft();
	void rotateBlockRight();
	void moveBlockToBottom();
	
	bool checkAndRemoveRow(int row);
	void checkAndExplode(char keyPressed = 0);
	void explode(Point p);
	void removeRow(int row); 

	void drawBoundaries() const;

	void removeKeypressFromBuffer() const;

	void saveToFile() const;
	void loadFromFile();
};

#endif