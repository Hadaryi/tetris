#ifndef __POINT_H
#define __POINT_H

class Point {
public:
	constexpr static int GAME_LOCATION_OFFSET_X = 8;
	constexpr static int GAME_LOCATION_OFFSET_Y = 14;
	constexpr static int MIDDLE_X_POSITION = GAME_LOCATION_OFFSET_X + 4;
private:
	int x;
	int y;

public:
	Point(int x = MIDDLE_X_POSITION, int y = GAME_LOCATION_OFFSET_Y);

	void setX(int x);
	void setY(int y);

	int getX() const;
	int getY() const;
};

#endif