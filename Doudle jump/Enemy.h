#pragma once
#include "Entity.h"
class Enemy: public Entity
{
	int countLife;
	bool isCanFly;
	int dir;
public:
	Enemy(float x, float y, float width, float height, const WCHAR* image, int countLife, bool isCanFly);
	int getCountLife() { return countLife; }
	bool getIsCanFly() { return isCanFly; }
	void setIsCanFly(bool isCanFly) {this->isCanFly = isCanFly;}
	int getDir() { return dir; }
	void setDir(int dir) {this->dir = dir;}
	void decrementLife();
};

