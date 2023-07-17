#include <glm/glm.hpp>

class Enemy {
private:
    glm::vec3 cornerLeft;
    glm::vec3 initialCornerLeft;
    float width;
    float depth;
    float height;
    int case_movement = 0;
    int matrOx;
    int matrOz;

public:
    float agonizingTime = 3;
    float elapsedAgonizingTime = 0;
    bool canKill = true;

    Enemy(glm::vec3 cornerLeft, float width, float height, float depth, int matrOx, int matrOy);

    // Setter
    void setCornerLeft(glm::vec3 cornerLeft);

    // Getter
    glm::vec3 getCornerLeft();
    float getWidth();
    float getDepth();
    float getHeight();
    int getMatrOz();
    int getMatrOx();

    void move(float deltaTimeSeconds, float cellLength);
};