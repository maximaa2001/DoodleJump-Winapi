#include "Hero.h"

Hero::Hero(float x, float y, float width, float height, const WCHAR* image):Entity(x, y, width, height, image){
	this->dir = 1;
	this->isLive = true;
	this->speed = 21;
	this->isMovePlatform = false;
}

void Hero::moveRect(float x, float y)
{
	setX(x);
	setY(y);
}

