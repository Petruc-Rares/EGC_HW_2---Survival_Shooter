#include "Enemy.h"

Enemy::Enemy(glm::vec3 cornerLeft, float width, float height, float depth, int matrOz, int matrOx) {
	this->cornerLeft = cornerLeft;
	this->initialCornerLeft = cornerLeft;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->matrOz = matrOz;
	this->matrOx = matrOx;
}

// Setter
void Enemy::setCornerLeft(glm::vec3 cornerLeft) {
	this->cornerLeft = cornerLeft;
}

// Getter
glm::vec3 Enemy::getCornerLeft() {
	return cornerLeft;
}

float Enemy::getWidth() {
	return width;
}

float Enemy::getDepth() {
	return depth;
}

float Enemy::getHeight() {
	return height;
}

int Enemy::getMatrOz() {
	return matrOz;
}

int Enemy::getMatrOx() {
	return matrOx;
}

void Enemy::move(float deltaTimeSeconds, float cellLength) {
	int speed = 2;

	switch (case_movement) {
		case 0: {
			if (cornerLeft.x + (float) (deltaTimeSeconds * speed) + width < initialCornerLeft.x + cellLength) {
				cornerLeft.x += (float)deltaTimeSeconds * speed;
			}
			else {
				case_movement = 1;
			}
			break;
		}
		case 1: {
			if (cornerLeft.z - (float)(deltaTimeSeconds * speed) - depth > initialCornerLeft.z - cellLength) {
				cornerLeft.z -= (float)deltaTimeSeconds * speed;
			}
			else {
				case_movement = 2;
			}
			break;
		}
		case 2: {
			if (cornerLeft.x - (float)(deltaTimeSeconds * speed) > initialCornerLeft.x ) {
				cornerLeft.x -= (float)deltaTimeSeconds * speed;
			}
			else {
				case_movement = 3;
			}
			break;
		}
		case 3: {
			if (cornerLeft.z + (float)(deltaTimeSeconds * speed) < initialCornerLeft.z) {
				cornerLeft.z += (float)deltaTimeSeconds * speed;
			}
			else {
				case_movement = 0;
			}
			break;
		}
		default: {
			exit(1);
		}
	}
}