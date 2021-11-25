#pragma once
#include "Entity.h"
class Platform : public Entity
{
	bool isInGame;

public:
	Platform(float x, float y, float width, float height, const WCHAR* image);
	bool getIsInGame() { return isInGame; }
	void setIsInGame(bool isInGame) { this->isInGame = isInGame; }
};

