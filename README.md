# Tetris
A Tetris console game built using C++.  
The game was created by Elad Cohen ([eladcn](https://www.github.com/eladcn)) and Hadar Yitzhaki ([Hadaryi](https://www.github.com/Hadaryi)).

## Game Keys
Move left - q
Move to bottom - w
Move right - e
Rotate right - r
Pause joker - s

## Score
Block moves to bottom - 2 * amount of lines moved.  
1 line removed - 100.  
2 lines removed - 300.  
3 lines removed - 500.  
4 lines removed - 800.  
Removed line with Joker - 50.  
Bomb explodes - (-50) * (number of removed squares).
	
## Special blocks
### Joker
* The joker can pass through blocks as long as there's room for it.
* If there is no empty space that the joker can fill while moving down, the joker will pause at its last position.
* If the Joker fills a row, the player will get 50 points instead of 100.
### Bomb
* If the bomb hits a square, it will explode and remove any square in 3x3 range and the player will lose from the score 50 points for every removed square.
* If the bomb doesn't hit anything, it will not explode and will disappear from the board.
