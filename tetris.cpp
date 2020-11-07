#include "tetris.h"

Tetris::Tetris() {
	changeConsoleSize(WINDOW_WIDTH, WINDOW_HEIGHT); //Changing the console's size to 450x550 px.

	srand((unsigned int)time(NULL)); //Seeding the rand function so it doesn't give the same results for each game.
	this->displayMenu();
}

/*
This function was taken from here:
https://stackoverflow.com/questions/21238806/how-to-set-output-console-width-in-visual-studio
*/
void Tetris::changeConsoleSize(int width, int height) {
	HWND console = GetConsoleWindow();
	RECT r;

	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, width, height, TRUE);
}

/*
Destructor - removes the current block from the memory.
*/
Tetris::~Tetris() {
	delete this->currentBlock;
}

/*
This function displays the game's menu and handles keypresses for the menu's actions.
*/
void Tetris::displayMenu() {
	gotoxy(0, 0); //Going to the start of the console.

	cout << "Please select an option:" << endl;
	cout << "1) Start game" << endl;
	cout << "2) Pause / continue game" << endl;
	cout << "3) Increase the speed" << endl;
	cout << "4) Decrease the speed" << endl;
	cout << "5) Save game" << endl;
	cout << "6) Load game" << endl;
	cout << "9) Exit" << endl;

	this->displayGameDetails(); //Displaying the game's details such as score and amount of dropped blocks.
	this->waitForMenuAction(); //Handling the keypresses by the user.
}

/*
This function handles the menu button presses received from the user using a given key that was pressed and a boolean value indicating whether the function
was called from an ongoing game or from the waitForMenuAction function.
*/
bool Tetris::menuActionHandler(char keyPressed, bool arrivedFromOngoingGame) {
	if (!arrivedFromOngoingGame) {
		if (keyPressed == GAME_START_KEY) {
			this->startGame();
			return true;
		}
		else if (keyPressed == GAME_PAUSE_KEY) {
			this->continueGame();
			return true;
		}
	}
	else {
		if (keyPressed == GAME_PAUSE_KEY) {
			this->pauseGame();
			return true;
		}
	}

	if (keyPressed == GAME_INCREASE_SPEED_KEY) {
		this->increaseSpeed(GAME_SPEED_CHANGE_AMOUNT);

		return false;
	}
	else if (keyPressed == GAME_DECREASE_SPEED_KEY) {
		this->decreaseSpeed(GAME_SPEED_CHANGE_AMOUNT);

		return false;
	}
	else if (keyPressed == GAME_SAVE_KEY) {
		if (this->isStarted || this->isFailed) {
			this->saveToFile();
			this->showNotice("The game has been saved.");
		}
		else {
			this->showNotice("Cannot save a game that has not been started.");
		}

		return false;
	}
	else if (keyPressed == GAME_LOAD_KEY) {
		this->loadFromFile();
		this->paintBoard();

		if (this->currentBlock != nullptr) {
			this->currentBlock->paint();
			this->isStarted = true;
			this->isFailed = false;
		}
		else { //If the current block is null, then the game was saved after it was ended.
			this->isStarted = false;
		}

		this->updateGameDetails();
		this->showNotice("The game has been loaded from the file.");
		this->gameEngine();

		return true;
	}
	else if (keyPressed == GAME_EXIT_KEY) {
		this->exitGame();
		return true;
	}

	return false;
}

/*
This function handles the keypresses made by the user when the game is paused / was not started.
*/
void Tetris::waitForMenuAction() {
	char selectedOption = 0;

	while (true) {
		if (_kbhit()) { //Checking if a keypress was made and if so, getting the keypress.
			selectedOption = _getch();

			if (this->menuActionHandler(selectedOption, false)) {
				break;
			}

			selectedOption = 0;
		}

		Sleep(MENU_WAIT_FOR_ACTION_DELAY); //Waiting some time so we do not overload the CPU.
	}
}

/*
This function exits the game.
*/
void Tetris::exitGame() {
	delete this->currentBlock;
	this->currentBlock = nullptr;

	exit(0);
}

/*
This function displays a notice that the game was ended and waits for a menu action.
This function is only called when the game has ended.
*/
void Tetris::endGame() {
	delete this->currentBlock;
	this->currentBlock = nullptr;

	this->showNotice("The game was ended.");
	this->waitForMenuAction();
}

/*
This function handles the keypresses made by the user and displays new blocks if the block reached the bottom of the board.
This is the core of the game.
*/
void Tetris::gameEngine() {
	this->drawBoundaries(); //Drawing the board's boundaries.

	while (this->isStarted && !this->isFailed) { //Looping until the pause key was pressed or a block reached the end of the board.
		if (this->currentBlock == nullptr) { //If the current block that is dropping is null, then the previous block has reached the bottom of the board / the game has just started.
			this->addNewBlock(); //Adding a new block to the board.
			this->updateGameDetails(); //Updating the amount of blocks dropped.
		}
		else {
			char keyPressed = 0;
			if (_kbhit()) { //Checking if there's any keypress in the buffer.
				keyPressed = _getch(); //Getting the first keypress from the buffer.

				this->removeKeypressFromBuffer(); //Removing the rest of the keypresses from the buffer.

				if (keyPressed == MOVE_LEFT_KEY) {
					checkAndExplode(keyPressed); //Checking if the current block is a bomb, and if so we should explode if possible.
					this->moveBlockLeft(); //If the current block was a bomb and it has exploded, this function will not do anything.
				}
				else if (keyPressed == MOVE_DOWN_KEY) {
					this->moveBlockToBottom();
				}
				else if (keyPressed == MOVE_RIGHT_KEY) {
					checkAndExplode(keyPressed); //Checking if the current block is a bomb, and if so we should explode if possible.
					this->moveBlockRight(); //If the current block was a bomb and it has exploded, this function will not do anything.
				}
				else if (keyPressed == ROTATE_RIGHT_KEY) {
					this->rotateBlockRight();
				}
				else {
					this->menuActionHandler(keyPressed, true);
				}
			}

			checkAndExplode(); //Checking if the current block is a bomb, if it is and it hits a square beneath it, it will explode.

			//If the joker pause key was pressed and the current block is a joker block, then we should pause it.
			if (keyPressed == JOKER_PAUSE_KEY && this->currentBlock != nullptr && BlocksGenerator::isJoker(this->currentBlock)) {
				vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
				vector<Point *>::iterator itr = blockLocations.begin();

				this->setUsedPoints(); //Setting the joker's location in the usedPoints array.

				//Checking if we should remove the row the joker paused at.
				//If so, we should increase the score by 50.
				if (this->checkAndRemoveRow(((*itr)->getY() - Point::GAME_LOCATION_OFFSET_Y))) {
					this->increaseScore(JOKER_LINE_REMOVED_SCORE); //Increasing the score by 50.
					this->paintBoard(); //Painting the new board after the row was removed.
					this->updateGameDetails(); //Updating the score that is displayed to the user.
				}

				delete this->currentBlock; //Deleting the current block from the memory since we don't need it anymore.
				this->currentBlock = nullptr; //Setting the current block's pointer to null so we know we should add a new block.
			}
			else if (this->currentBlock != nullptr && this->canBlockMoveDown()) { //If the current block is not a joker / the joker key was not pressed and it can move down, we should move the block down in the console.
				this->moveBlockDown();
			}
			else if (this->currentBlock != nullptr) { //Checking if we still have a block in the instance (it may have been removed if it was a bomb and it has exploded).
				int removed = 0; //Indicating how many rows we have removed (if any).

				vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
				vector<Point *>::iterator itr = blockLocations.begin();
				vector<Point *>::iterator itrEnd = blockLocations.end();

				//Looping through each square in the block and checking if it fills a row and if it does, we remove it.
				for (; itr != itrEnd; ++itr) {
					if (this->checkAndRemoveRow(((*itr)->getY() - Point::GAME_LOCATION_OFFSET_Y))) {
						removed++;
					}
				}

				//Checking how many rows we have removed.
				switch (removed) {
				case 1: //If we removed 1 row, we should check if it was removed by a joker or not because it affects the score.
					if (BlocksGenerator::isJoker(this->currentBlock)) {
						this->increaseScore(JOKER_LINE_REMOVED_SCORE);
					}
					else {
						this->increaseScore(LINES_REMOVED_SCORE_1);
					}

					break;
				case 2:
					this->increaseScore(LINES_REMOVED_SCORE_2);
					break;
				case 3:
					this->increaseScore(LINES_REMOVED_SCORE_3);
					break;
				case 4:
					this->increaseScore(LINES_REMOVED_SCORE_4);
					break;
				}

				//If we removed at least one row, we should update the console's board and the displayed score.
				if (removed) {
					this->paintBoard();
					this->updateGameDetails();
				}

				delete this->currentBlock; //Deleting the current block from the memory since we don't need it anymore.
				this->currentBlock = nullptr; //Setting the current block's pointer to null so we know we should add a new block.
			}
		}

		Sleep(this->speed);
	}

	this->endGame(); //The game has ended so we should call the end game function.
}

/*
This function starts a new game.
*/
void Tetris::startGame() {
	this->clearBoard(); //Clearing the board from the previous game.
	this->setScore(0); //Resetting the score.
	this->setBlocksDropped(0); //Resetting the amount of blocks dropped.
	this->showNotice(""); //Resetting the notice.

	this->isStarted = true; //Indicating that the game has started.
	this->isFailed = false; //Indicating that the game was not failed yet.

	this->gameEngine();
}

/*
This function displays a notice that the game was paused and waits for a menu action.
This function is only called when the game was paused.
*/
void Tetris::pauseGame() {
	this->showNotice("The game was paused.");
	this->waitForMenuAction();
}

/*
This function clears the game paused notice.
This function is only called when the continue game keypress was made.
*/
void Tetris::continueGame() {
	this->showNotice("");
}

/*
This function returns whether the current block can move down by 1 square.
*/
bool Tetris::canBlockMoveDown() {
	if (this->currentBlock == nullptr) { //Checking that the current block exists and it was not removed in a previous method.
		return false;
	}

	const vector<Point *> points = this->currentBlock->getBlockLocations();
	vector<Point *>::const_iterator itr = points.begin();
	Point *p;

	if (BlocksGenerator::isJoker(this->currentBlock)) {
		p = *itr;
		int pX = p->getX();
		int pY = p->getY();

		//Checking if the joker has room anywehre in the same column it's moving down at.
		for (int movedPosition = pY - Point::GAME_LOCATION_OFFSET_Y + 1; movedPosition < ROWS; movedPosition++) {
			if (this->usedPoints[movedPosition][pX - Point::GAME_LOCATION_OFFSET_X] == 0) {
				return true;
			}
		}

		//Setting the current's block location in the usedPoints array so we know the room was filled there because it cannot move down.
		this->setUsedPoints();
		return false;
	}

	vector<Point *>::const_iterator itrEnd = points.end();

	//If the block is not a joker, we handle it normally.
	for (; itr != itrEnd; ++itr) {
		p = *itr;

		if (p->getY() == ROWS + Point::GAME_LOCATION_OFFSET_Y - 1) { //Checking if the block reached the end of the board.
			//Setting the current's block locations in the usedPoints array so we know the room was filled there because it cannot move down.
			this->setUsedPoints();
			return false;
		}

		if (this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y + 1][p->getX() - Point::GAME_LOCATION_OFFSET_X] != 0) { //Checking if there is a part of another block beneath the current block.
			//Setting the current's block locations in the usedPoints array so we know the room was filled there because it cannot move down.
			this->setUsedPoints();
			return false;
		}
	}

	return true;
}

/*
This function returns whether the current block can move right.
*/
bool Tetris::canBlockMoveRight() {
	if (this->currentBlock == nullptr) { //Checking that the current block exists and it was not removed in a previous method.
		return false;
	}

	//If the current block is a joker, we assume it can move right because we handle it in the moveBlockRight method instead (to avoid code duplication).
	if (BlocksGenerator::isJoker(this->currentBlock)) {
		return true;
	}

	const vector<Point *> points = this->currentBlock->getBlockLocations();
	vector<Point *>::const_iterator itr = points.begin();
	vector<Point *>::const_iterator itrEnd = points.end();
	Point *p;

	for (; itr != itrEnd; ++itr) {
		p = *itr;

		if (p->getX() - Point::GAME_LOCATION_OFFSET_X == COLS - 1) { //Checking if the block reached the end of the board.
			return false;
		}

		if (this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X + 1] != 0) { //Checking if there is a part of another block to the right of the current block.
			return false;
		}
	}

	return true;
}

/*
This function returns whether the current block can move left.
*/
bool Tetris::canBlockMoveLeft() {
	if (this->currentBlock == nullptr) { //Checking that the current block exists and it was not removed in a previous method.
		return false;
	}

	//If the current block is a joker, we assume it can move right because we handle it in the moveBlockRight method instead (to avoid code duplication).
	if (BlocksGenerator::isJoker(this->currentBlock)) {
		return true;
	}

	const vector<Point *> points = this->currentBlock->getBlockLocations();
	vector<Point *>::const_iterator itr = points.begin();
	vector<Point *>::const_iterator itrEnd = points.end();
	Point *p;

	for (; itr != itrEnd; ++itr) {
		p = *itr;

		if (p->getX() == Point::GAME_LOCATION_OFFSET_X) { //Checking if the block reached the end of the board.
			return false;
		}

		if (this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X - 1] != 0) { //Checking if there is a part of another block to the left of the current block.
			return false;
		}
	}

	return true;
}

/*
This function receives an output parameter which is an array of rotated points of the current block 
and returns whether any of the rotated squares is overlapping an existing square in the board.
*/
bool Tetris::canBlockRotateRight() {
	if (this->currentBlock == nullptr) { //Checking that the current block exists and it was not removed in a previous method.
		return false;
	}

	//If the current block is not a line, then we cannot rotate it (because the other shapes cannot rotate).
	if (!this->currentBlock->isRotateable()) {
		return false;
	}

	vector<Point *> rotatedPoints = this->currentBlock->getRotatedPosition();
	vector<Point *>::iterator itr = rotatedPoints.begin();
	vector<Point *>::iterator itrEnd = rotatedPoints.end();
	bool ret = true;

	//Checking if any square of the rotated block is not in a legal location.
	for (; itr != itrEnd && ret; ++itr) {
		Point *p = *itr;

		if (p->getY() > ROWS + Point::GAME_LOCATION_OFFSET_Y - 1) { //Checking if the current square reached beyond the bottom of the board.
			ret = false;
		}
		else if (p->getX() < Point::GAME_LOCATION_OFFSET_X) { //Checking if the current square reached beyond the left side of the board.
			ret = false;
		}
		else if (p->getX() > COLS + Point::GAME_LOCATION_OFFSET_X - 1) { //Checking if the current square reached beyond the right of the board.
			ret = false;
		}
		else if (this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X] != 0) { //Checking if the current square is overlapping an existing square in the board.
			ret = false;
		}
	}

	Block::clearVectorOfDynamicPoints(rotatedPoints);

	return ret;
}

/*
This function moves the current block down by 1 square.
*/
void Tetris::moveBlockDown() {
	if (this->currentBlock == nullptr) { //Checking that the current block exists and it was not removed in a previous method.
		return;
	}

	this->currentBlock->removeBlockFromConsole(); //Removing the block from the console since we are going to paint it 1 square down from its current position.
	vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
	vector<Point *>::iterator itr = blockLocations.begin();

	if (BlocksGenerator::isJoker(this->currentBlock)) { //If the current block is a joker, we should try to find the first position it can fit into.
		Point *p = *itr;

		for (int movedPosition = p->getY() - Point::GAME_LOCATION_OFFSET_Y + 1; movedPosition < ROWS; movedPosition++) {
			if (this->usedPoints[movedPosition][p->getX() - Point::GAME_LOCATION_OFFSET_X] == 0) {
				p->setY(movedPosition + Point::GAME_LOCATION_OFFSET_Y);
				break;
			}
		}
	}
	else { //Moving all of the block's locations 1 square down.
		vector<Point *>::iterator itrEnd = blockLocations.end();

		for (; itr != itrEnd; ++itr) {
			(*itr)->setY((*itr)->getY() + 1);
		}

		this->currentBlock->updateBlockProperties();
	}

	//Painting the updated block to the console.
	this->currentBlock->paint();
}

/*
This function moves the current block 1 square to the right.
*/
void Tetris::moveBlockRight() {
	if (!this->canBlockMoveRight()) { //Validating that the block can move right before we move it.
		return;
	}

	this->currentBlock->removeBlockFromConsole(); //Removing the block from the console since we are going to paint it 1 square right from its current position.
	vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
	vector<Point *>::iterator itr = blockLocations.begin();

	//If the current block is a joker, we should try to find the first position it can fit into.
	if (BlocksGenerator::isJoker(this->currentBlock)) {
		Point *p = *itr;

		int pX = p->getX();
		int pY = p->getY();

		for (int movedPosition = pX - Point::GAME_LOCATION_OFFSET_X + 1; movedPosition < COLS; movedPosition++) {
			if (this->usedPoints[pY - Point::GAME_LOCATION_OFFSET_Y][movedPosition] == 0) {
				p->setX(movedPosition + Point::GAME_LOCATION_OFFSET_X);
				break;
			}
		}
	}
	else { //If the current block is not a joker, we should move all of its squares 1 square to the right.
		vector<Point *>::iterator itrEnd = blockLocations.end();

		for (; itr != itrEnd; ++itr) {
			(*itr)->setX((*itr)->getX() + 1);
		}

		this->currentBlock->updateBlockProperties();
	}

	//Painting the updated block to the console.
	this->currentBlock->paint();
}

/*
This function moves the current block 1 square to the right.
*/
void Tetris::moveBlockLeft() {
	if (!this->canBlockMoveLeft()) { //Validating that the block can move right before we move it.
		return;
	}

	this->currentBlock->removeBlockFromConsole(); //Removing the block from the console since we are going to paint it 1 square left from its current position.
	vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
	vector<Point *>::iterator itr = blockLocations.begin();
	
	//If the current block is a joker, we should try to find the first position it can fit into.
	if (BlocksGenerator::isJoker(this->currentBlock)) {
		int pX = (*itr)->getX();
		int pY = (*itr)->getY();

		for (int movedPosition = pX - Point::GAME_LOCATION_OFFSET_X - 1; movedPosition >= 0; movedPosition--) {
			if (this->usedPoints[pY - Point::GAME_LOCATION_OFFSET_Y][movedPosition] == 0) {
				(*itr)->setX(movedPosition + Point::GAME_LOCATION_OFFSET_X);
				break;
			}
		}
	}
	else { //If the current block is not a joker, we should move all of its squares 1 square to the left.
		vector<Point *>::iterator itrEnd = blockLocations.end();

		for (; itr != itrEnd; ++itr) {
			(*itr)->setX((*itr)->getX() - 1);
		}

		this->currentBlock->updateBlockProperties();
	}

	//Painting the updated block to the console.
	this->currentBlock->paint();
}

/*
This function rotates the current block to the right if possible.
*/
void Tetris::rotateBlockRight() {
	if (!this->canBlockRotateRight()) { //Checking if the current block can be rotated to the right.
		return;
	}

	this->currentBlock->removeBlockFromConsole(); //Removing the block from the console.
	this->currentBlock->rotateRight(); //Rotating the block to the right.
	this->currentBlock->paint(); //Painting the updated block to the console.
}

/*
This function moves the current block to the last available position at the bottom of the board and increases the score for each square moved.
*/
void Tetris::moveBlockToBottom() {
	int counter = 0;

	while (this->canBlockMoveDown()) {
		this->moveBlockDown();
		counter++;
	}

	this->increaseScore(counter * MOVE_TO_BOTTOM_SCORE_MULTIPLIER);
}

/*
This function randomly adds a new block to the top of the board and sets it as the current block.
*/
void Tetris::addNewBlock() {
	int randNum = (rand() % 100) + 1; //Calculating a number between 1 and 100.

	this->currentBlock = BlocksGenerator::getRandomBlock();

	const vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
	vector<Point *>::const_iterator itr = blockLocations.begin();
	vector<Point *>::const_iterator itrEnd = blockLocations.end();

	//Checking if we have created the block on top of another block and if so we should indicate the game has ended (using the isFailed property).
	for (; itr != itrEnd; ++itr) {
		Point *p = *itr;

		if (this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X] != 0) {
			this->isFailed = true;
			return;
		}
	}

	this->currentBlock->paint(); //Painting the new block to the console.
	this->increaseNumOfBlocks(); //Increasing the number of blocks used.
}

/*
This function receives a parameter score and sets the game's score to the given parameter.
*/
void Tetris::setScore(int score) {
	this->score = score;
}

/*
This function receives a parameter score and increases the game's score by the given parameter.
*/
void Tetris::increaseScore(int score) {
	this->score += score;
}

/*
This function receives a parameter score and decreases the game's score by the given parameter.
*/
void Tetris::decreaseScore(int score) {
	this->score -= score;

	if (this->score < 0) {
		this->score = 0;
	}
}

/*
This function receives a parameter blocksDropped and sets the game's amount of blocks dropped to the given parameter.
*/
void Tetris::setBlocksDropped(int blocksDropped) {
	this->blocksDropped = blocksDropped;
}

/*
This function increases the number of blocks dropped by 1.
*/
void Tetris::increaseNumOfBlocks() {
	this->blocksDropped++;
}

/*
This function takes the usedPoints array and displaying the board according to the filled usedPoints.
*/
void Tetris::paintBoard() {
	for (int i = 0; i < ROWS; i++) {
		gotoxy(Point::GAME_LOCATION_OFFSET_X, i + Point::GAME_LOCATION_OFFSET_Y);

		for (int j = 0; j < COLS; j++) {
			if (this->usedPoints[i][j] != 0) {
				cout << Block::NORMAL_SQUARE_CHAR;
			}
			else {
				cout << " ";
			}
		}
	}
}

/*
This function resets the usedPoints array and removes the squares from the console.
*/
void Tetris::clearBoard() {
	for (int i = 0; i < ROWS; i++) {
		gotoxy(Point::GAME_LOCATION_OFFSET_X, i + Point::GAME_LOCATION_OFFSET_Y);

		for (int j = 0; j < COLS; j++) {
			cout << " ";
			this->usedPoints[i][j] = 0;
		}
	}

	delete this->currentBlock;
	this->currentBlock = nullptr;
}

/*
This function receives a block and sets the usedPoints array used locations according to the received block's locations in the console.
This function is called when the block should pause (when it doesn't move any further).
*/
void Tetris::setUsedPoints() {
	const vector<Point *> points = this->currentBlock->getBlockLocations();
	vector<Point *>::const_iterator itr = points.begin();
	vector<Point *>::const_iterator itrEnd = points.end();

	for (; itr != itrEnd; ++itr) {
		Point *p = *itr;

		this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X] = 1;
	}
}

/*
This function receives a parameter speed and sets the game's speed according to the given parameter.
*/
void Tetris::setSpeed(int speed) {
	this->speed = speed;
}

/*
This function receives a parameter speed and increases the game's speed by the given parameter as long as the speed after the change is not faster than 100 miliseconds.
*/
void Tetris::increaseSpeed(int speed) {
	if (this->speed - speed >= MAXIMUM_SPEED) {
		this->speed -= speed;
		this->showNotice("The speed has been increased.");
	}
	else {
		this->showNotice("The speed has reached the maximum speed.");
	}
}

/*
This function receives a parameter speed and decreases the game's speed by the given parameter.
*/
void Tetris::decreaseSpeed(int speed) {
	this->speed += speed;
	this->showNotice("The speed has been decreased.");
}

/*
This function returns the current game's score.
*/
int Tetris::getScore() const {
	return this->score;
}

/*
This function returns the current game's amount of blocks dropped.
*/
int Tetris::getNumOfBlocks() const {
	return this->blocksDropped;
}

/*
This function receives a row index, checks if it's full and if it is - removes it from the usedPoints array.
*/
bool Tetris::checkAndRemoveRow(int row) {
	for (int i = 0; i < COLS; i++) {
		if (usedPoints[row][i] == 0) {
			return false;
		}
	}

	removeRow(row);
	
	return true;
}

/*
This function receives a row index and removes it from the usedPoints array.
*/
void Tetris::removeRow(int row) {
	for (int i = row; i > 0; i--) {		
		for (int j = 0; j < COLS; j++) {
			this->usedPoints[i][j] = this->usedPoints[i - 1][j];
		}
	}
	
	//Clearing the top row in the board since it cannot be full and we do not accomplish that in the previous loop.
	for (int j = 0; j < COLS; j++) {
		this->usedPoints[0][j] = 0;
	}
}


/*
This function receives a keypress made by the user (or 0 if a keypress was not made), checks if the current block is a bomb
and if it is, checks whether it should explode and if so - it explodes.
*/
void Tetris::checkAndExplode(char keyPressed) {
	if (this->currentBlock == nullptr || !BlocksGenerator::isBomb(this->currentBlock)) //Checking that the current block exists and that it is a bomb.
		return;

	vector<Point *> blockLocations = this->currentBlock->getBlockLocations();
	vector<Point *>::iterator itr = blockLocations.begin();

	Point *p = *itr; //Getting the bomb's location.

	if (keyPressed == 0) { //If the user did not make any keypress we should check if the bomb touches a square beneath it.
		if (p->getY() - Point::GAME_LOCATION_OFFSET_Y == ROWS - 1) { //If the bomb reached the end of the board, we should remove it from the board.
			this->usedPoints[ROWS - 1][p->getX() - Point::GAME_LOCATION_OFFSET_X] = 0;
			gotoxy(p->getX(), p->getY());
			cout << " ";

			delete this->currentBlock;
			this->currentBlock = nullptr;
		}
		//Checking if the bomb is touching a square beneath it and if so - explode.
		else if (p->getY() - Point::GAME_LOCATION_OFFSET_Y >= 0 && this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y + 1][p->getX() - Point::GAME_LOCATION_OFFSET_X] != 0) { //Checking if there is a part of another block beneath the current block.
			this->explode(*p);
		}
	}
	else if (keyPressed == MOVE_LEFT_KEY) { //If the user pressed on the move left key, we should check whether the bomb touches a square to its left and if so - explode.
		if (p->getX() - Point::GAME_LOCATION_OFFSET_X > 0 && this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X - 1] != 0) {
			this->explode(*p);
		}
	}
	else if (keyPressed == MOVE_RIGHT_KEY) { //If the user pressed on the move right key, we should check whether the bomb touches a square to its right and if so - explode.
		if (p->getX() - Point::GAME_LOCATION_OFFSET_X < COLS - 1 && this->usedPoints[p->getY() - Point::GAME_LOCATION_OFFSET_Y][p->getX() - Point::GAME_LOCATION_OFFSET_X + 1] != 0) {
			this->explode(*p);
		}
	}
}

/*
This function receives a location in the console, and removes all squares in 3x3 range around it.
*/
void Tetris::explode(Point p) {
	int startX, startY;
	int amountJumpX = 3;
	int amountJumpY = 3;

	startX = p.getX() - Point::GAME_LOCATION_OFFSET_X - 1;
	startY = p.getY() - Point::GAME_LOCATION_OFFSET_Y - 1;

	if (startX < 0) { //If the startX location of the removal is exceeding the game's left border, we should only remove 2 squares to the right.
		startX = 0;
		amountJumpX = 2;
	}

	if (startY < 0) { //If the startY location of the removal is exceeding the game's top border, we should only remove 2 squares to the bottom.
		startY = 0;
		amountJumpY = 2;
	}

	for (int i = startY; i < startY + amountJumpY && i < ROWS; i++) {
		gotoxy(startX + Point::GAME_LOCATION_OFFSET_X, i + Point::GAME_LOCATION_OFFSET_Y);

		for (int j = startX; j < startX + amountJumpX && j < COLS; j++) {
			if (this->usedPoints[i][j] != 0) { //Removing the squares the bomb exploded at and removing 50 points for each square removed.
				this->decreaseScore(BOMB_EXPLODE_SCORE_PENALTY);
				this->usedPoints[i][j] = 0;
			}

			cout << " ";
		}
	}

	//Removing the current block's instance.
	delete this->currentBlock;
	this->currentBlock = nullptr;
}

/*
This function draws the boundaries of the game's board.
*/
void Tetris::drawBoundaries() const {
	//Drawing the top boundary.
	gotoxy(Point::GAME_LOCATION_OFFSET_X - 1, Point::GAME_LOCATION_OFFSET_Y - 1);
	for (int i = 0; i < COLS + 2; i++) {
		cout << "$";
	}

	//Drawing the bottom boundary.
	gotoxy(Point::GAME_LOCATION_OFFSET_X - 1, Point::GAME_LOCATION_OFFSET_Y + ROWS);
	for (int i = 0; i < COLS + 2; i++) {
		cout << "$";
	}

	//Drawing the left and right boundaries.
	for (int i = 0; i < ROWS; i++) {
		gotoxy(Point::GAME_LOCATION_OFFSET_X - 1, i + Point::GAME_LOCATION_OFFSET_Y);
		cout << "$";
		gotoxy(Point::GAME_LOCATION_OFFSET_X + COLS, i + Point::GAME_LOCATION_OFFSET_Y);
		cout << "$";
	}
}

/*
This function displays the game's details.
*/
void Tetris::displayGameDetails() {
	gotoxy(0, MENU_LINES_AMOUNT + 2);

	cout << "        Game's details " << endl;
	cout << "       ----------------" << endl;

	this->updateGameDetails();
}

/*
This function updates and displays the game's details to the console (such as score and the amount of dropped blocks). 
*/
void Tetris::updateGameDetails() {
	gotoxy(0, MENU_LINES_AMOUNT + 4);
	cout << "Score:" << this->getScore() << "   " << "Dropped blocks: " << this->getNumOfBlocks() << "         " << endl;
}

/*
This function removes all keypresses from the buffer.
*/
void Tetris::removeKeypressFromBuffer() const {
	while (_kbhit()) {
		_getch();
	}
}

/*
This function displays a notice for the user (for example: when the game was paused or the game's speed has increased).
*/
void Tetris::showNotice(const string& notice) {
	int length = notice.length();

	gotoxy(0, MENU_LINES_AMOUNT); //Moving the cursor to the position of the notice (under the menu).
	cout << notice;

	if (noticeCharactersWritten - length > 0) { //Removing any characters left at the end of the notice (if left from the previous notice).
		cout << string(noticeCharactersWritten - length, ' ');
	}

	this->noticeCharactersWritten = length;
}

/*
This function saves the current game into a file.
*/
void Tetris::saveToFile() const {
	char blockType = REGULAR_BLOCK;
	ofstream outFile(FILE_NAME, ios::binary | ios::trunc);

	outFile.write((const char *)this->usedPoints, sizeof(int) * ROWS * COLS); //Writing the board to the file.
	outFile.write((const char *)&this->score, sizeof(int)); //Writing the score to the file.
	outFile.write((const char *)&this->blocksDropped, sizeof(int)); //Writing the amount of dropped blocks to the file.
	outFile.write((const char*)&this->speed, sizeof(int)); //Writing the current game's speed to the file.

	if (this->currentBlock == nullptr) {
		blockType = NO_BLOCK;
	}
	else {
		Joker *jokerBlock = dynamic_cast<Joker *>(this->currentBlock);
		if (jokerBlock != nullptr) {
			blockType = JOKER_BLOCK;
		}
		else {
			Bomb *bombBlock = dynamic_cast<Bomb *>(this->currentBlock);
			if (bombBlock != nullptr) {
				blockType = BOMB_BLOCK;
			}
		}
	}

	outFile.write((const char *)&blockType, sizeof(char)); //Writing the current block type to the file.

	if (blockType != NO_BLOCK) {
		int blockRotatedAmount = this->currentBlock->getRotatedAmount();

		outFile.write((const char *)&blockRotatedAmount, sizeof(int)); //Writing the amount of times the block was rotated.

		const vector<Point *> blockLocations = this->currentBlock->getBlockLocations();

		//Writing the current block's size to the file.
		int blockLocationsSize = blockLocations.size();
		outFile.write((const char *)&blockLocationsSize, sizeof(int));

		//Writing the points in the current block's locations to the file.
		vector<Point *>::const_iterator itr = blockLocations.begin();
		vector<Point *>::const_iterator itrEnd = blockLocations.end();
		for (; itr != itrEnd; ++itr) {
			outFile.write((const char *)*itr, sizeof(Point));
		}
	}

	outFile.close();
}

/*
This function loads the game from a file.
*/
void Tetris::loadFromFile() {
	char blockType;
	int blockSize;
	Point p;
	ifstream inFile(FILE_NAME, ios::binary);

	if (!inFile.is_open()) {
		return;
	}

	inFile.read((char *)this->usedPoints, sizeof(int) * ROWS * COLS); //Reading the board from the file.
	inFile.read((char *)&this->score, sizeof(int)); //Reading the score from the file.
	inFile.read((char *)&this->blocksDropped, sizeof(int)); //Reading the amount of blocks dropped from the file.
	inFile.read((char *)&this->speed, sizeof(int)); //Reading the game's speed from the file.
	inFile.read((char *)&blockType, sizeof(char)); //Reading the current block's type.

	if (blockType == NO_BLOCK) {
		this->currentBlock = nullptr;
	}
	else {
		int blockRotatedAmount;
		
		inFile.read((char *)&blockRotatedAmount, sizeof(int)); //Reading the amount of times the current block was rotated.
		inFile.read((char *)&blockSize, sizeof(int)); //Reading the current block's size.

		vector<Point *> blockLocations;
		blockLocations.reserve(blockSize);

		//Reading the current block's locations from the file.
		for (int i = 0; i < blockSize; i++) {
			inFile.read((char *)&p, sizeof(Point));
			blockLocations.push_back(new Point(p));
		}

		//Initializing the current block.
		if (blockType == REGULAR_BLOCK) {
			this->currentBlock = new GeneralBlock(blockLocations);
			this->currentBlock->setRotatedAmount(blockRotatedAmount);
		}
		else if (blockType == JOKER_BLOCK) {
			this->currentBlock = new Joker(p);
		}
		else {
			this->currentBlock = new Bomb(p);
		}
	}

	inFile.close();
}