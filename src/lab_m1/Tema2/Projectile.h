#include <glm/glm.hpp>

class Projectile {
private:
    glm::vec3 crtPos;
    glm::vec3 initialPos;
    glm::vec3 direction;
    glm::vec3 right;
    float radius;
    float maxDistance = 8;
public:
    Projectile(glm::vec3 initialPos, glm::vec3 direction, glm::vec3 right, float radius);

    // Setter
    void setDirection(glm::vec3 direction);
    void setCrtPos(glm::vec3 crtPos);

    // Getter
    glm::vec3 getDirection();
    glm::vec3 getCrtPos();
    glm::vec3 getInitialPos();
    glm::vec3 getRight();
    float getMaxDistance();
    float getRadius();
};