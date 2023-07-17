#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema_camera.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Wall.h"
#include "Labirint.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        int getMatrCoord(int center, int axis);
        void setCenterPlayer();
        bool gameWon(int coordOxMatrix, int coordOzMatrix);
        bool collisionPlayerEnemy(float coordPlayerX, float coordPlayerZ, Enemy enemy);
        int findEnemy(int coordOz, int coordOx);
        Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        bool checkCoordsInMap(int coordOx, int coordOz);

    protected:
        implemented::CameraTema* camera;
        implemented::CameraTema* camera_ortho;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        float fov;
        float deltaOrtho;
        float moveForward = 0;

        // to delete (no more)
        float rotateOx = 0, rotateOy = 0;
    
        float centerPlayerZ = 0;
        float centerPlayerX = 0;
        float thetaAux = 0;

        float x_test;
        float y_test;

        int noProjectiles;
        glm::vec3 distanceBullet;
        std::vector<Projectile> projectiles;
        std::vector<Enemy> enemies;
        std::vector<Wall> walls;

        // for recovering from first person mode to third person mode and vice-versa
        bool movedToFirst = false;
        
        int widthMap = 9;
        int heightMap = 9;
        int cell_length = 4;

        // MAZE
        int **matrixMaze;

        unsigned int startCoordMatrOx = 0;
        unsigned int startCoordMatrOz = 1;

        float timeDisplacement = 0;
        bool forDebug = false;

        float deltaTimeSecondsShooting;

        unsigned int gameState;
        unsigned int noLives = 2;
        float timeGame;
    };
}   // namespace m1
