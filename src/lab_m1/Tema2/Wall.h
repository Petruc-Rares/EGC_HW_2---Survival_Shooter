#include <glm/glm.hpp>

class Wall {
private:
    glm::vec3 cornerLeft;
    float width;
public:
    Wall(glm::vec3 cornerLeft, float width);

    // Setter
    void setCornerLeft(glm::vec3 cornerLeft);
    void setWidth(float width);

    // Getter
    glm::vec3 getCornerLeft();
    float getWidth();
};