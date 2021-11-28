#include "Enemy.h"

Enemy::Enemy(float x, float y, float width, float height, const WCHAR* image, int countLife, bool isCanFly) :Entity(x, y, width, height, image) {
	this->countLife = countLife;
	this->isCanFly = isCanFly;
	if (isCanFly) {
		this->dir = 1;
	}
	else {
		this->dir = 0;
	}
}

void Enemy::decrementLife() {
	this->countLife = this->countLife--;
}
