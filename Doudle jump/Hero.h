#pragma once
#include "Entity.h"
class Hero: public Entity
{
	int dir;
	bool isLive;
	float speed;
	bool isMovePlatform;
	bool isShoot;
	const WCHAR* shootImage;

public:
	Hero(float x, float y, float width, float height, const WCHAR* image, const WCHAR* shootImage);
	int getDir() { return dir; }
	float getSpeed() { return speed; }
	bool getIsLive() { return isLive; }
	bool getIsMovePlatform() { return isMovePlatform; }
	bool getIsShoot() { return isShoot; }
	const WCHAR* getShootImage() { return shootImage; }
	void setDir(int dir) { this->dir = dir; }
	void setIsLive(bool isLive) { this->isLive = isLive; }
	void setIsShoot(bool isShoot) { this->isShoot = isShoot; }
	void setIsMovePlatform(bool isMovePlatform) { this->isMovePlatform = isMovePlatform; }
	void setSpeed(float speed) { this->speed = speed; }
	void setShootImage(const WCHAR* shootImage) { this->shootImage = shootImage; }
	void moveRect(float x, float y);
};

