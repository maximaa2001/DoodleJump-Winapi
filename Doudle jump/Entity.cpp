#include "Entity.h"


Entity::Entity(float x, float y, float width, float height, const WCHAR* image) {
	rect.left = x;
	rect.right = x + width;
	rect.top = y;
	rect.bottom = y + height;
	this->image = image;
	this->width = width;
	this->height = height;
}
