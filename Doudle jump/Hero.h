#pragma once
#include "Entity.h"
class Hero: public Entity
{
	int dir;
	bool isLive;
	float speed;
	bool isMovePlatform;

public:
	Hero(float x, float y, float width, float height, const WCHAR* image);
	int getDir() { return dir; }
	float getSpeed() { return speed; }
	bool getIsLive() { return isLive; }
	bool getIsMovePlatform() { return isMovePlatform; }
	void setDir(int dir) { this->dir = dir; }
	void setIsLive(bool isLive) { this->isLive = isLive; }
	void setIsMovePlatform(bool isMovePlatform) { this->isMovePlatform = isMovePlatform; }
	void setSpeed(float speed) { this->speed = speed; }
	void moveRect(float x, float y);
};

