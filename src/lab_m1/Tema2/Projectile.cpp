#include "Projectile.h"

Projectile::Projectile(glm::vec3 initialPos, glm::vec3 direction, glm::vec3 right, float radius) {
    this->crtPos = initialPos;
    this->initialPos = initialPos;
    this->direction = direction;
    this->right = right;
    this->radius = radius;
}

void Projectile::setDirection(glm::vec3 direction) {
    this->direction = direction;
}

glm::vec3 Projectile::getDirection() {
    return direction;
}

void Projectile::setCrtPos(glm::vec3 initialPos) {
    this->crtPos = initialPos;
}

glm::vec3 Projectile::getCrtPos() {
    return crtPos;
}

glm::vec3 Projectile::getInitialPos() {
    return initialPos;
}

float Projectile::getMaxDistance() {
    return maxDistance;
}

glm::vec3 Projectile::getRight() {
    return right;
}

float Projectile::getRadius() {
    return radius;
}