#pragma once
#include "Entity.h"
class Platform : public Entity
{

public:
	Platform(float x, float y, float width, float height, const WCHAR* image);
};

