#include "Wall.h"

Wall::Wall(glm::vec3 cornerLeft, float width) {
    this->cornerLeft = cornerLeft;
    this->width = width;
}

void Wall::setCornerLeft(glm::vec3 cornerLeft) {
    this->cornerLeft = cornerLeft;
}

void Wall::setWidth(float width) {
    this->width = width;
}

glm::vec3 Wall::getCornerLeft() {
    return cornerLeft;
}

float Wall::getWidth() {
    return width;
}