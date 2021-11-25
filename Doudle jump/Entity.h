#pragma once
#include <Windows.h>
#include <iostream>
#include <gdiplus.h>
using namespace std;
using namespace Gdiplus;
class Entity
{
	RECT rect;
	const WCHAR* image;
	float width;
	float height;

public:
	Entity (float x, float y, float width, float height, const WCHAR* image);
	float getX() { return rect.left; }
	float getY() { return rect.top; }
	float getWidth() { return width; }
	float getHeight() { return height; }
	const WCHAR* getImage() { return image; }
	const RECT getRect() { return rect; }
	void setX(float x) { rect.left = x; rect.right = rect.left + width; }
	void setY(float y) { rect.top = y; rect.bottom = rect.top + height; }
};

