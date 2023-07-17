#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>


#include "lab_m1/Tema2/object3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

#define MAX_LIVES 2
#define MAX_TIME_TO_ESCAPE 120
#define AXIS_MATRIX_OZ 1
#define AXIS_MATRIX_OX 0
#define GAME_IN_PROGRESS 0
#define GAME_WON 1
#define GAME_LOST 2
#define GAME_DEVELOPMENT 120
#define MIN_FIRE_RATE 1

double maxDouble(double a, double b) {
    return (a > b) ? a : b;
}

double minDouble(double a, double b) {
    return (a < b) ? a : b;
}

int Tema2::getMatrCoord(int center, int axis) {
    int coord;
    unsigned int startCoord;
    if (axis == AXIS_MATRIX_OX) {
        startCoord = startCoordMatrOx;
    }
    else if (axis == AXIS_MATRIX_OZ) {
        startCoord = startCoordMatrOz;
    }

    if (center > 0) {
        coord = startCoord + ((int)center + cell_length / 2) / cell_length;
    }
    else {
        coord = startCoord + ((int)center - cell_length / 2) / cell_length;
    }
    return coord;
}

void Tema2::setCenterPlayer() {
    if (movedToFirst) {
        centerPlayerX = camera->position.x;
        centerPlayerZ = camera->position.z;
    }
    else {
        centerPlayerX = camera->GetTargetPosition().x;
        centerPlayerZ = camera->GetTargetPosition().z;
    }
}

bool Tema2::gameWon(int coordOxMatrix, int coordOzMatrix) {
    return ((coordOzMatrix >= heightMap) || (coordOzMatrix < 0)
        || (coordOxMatrix >= widthMap) || (coordOxMatrix < 0));
}


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data.

    // TODO(student): If you look closely in the `Init()` and `Update()`
    // functions, you will see that we have three objects which we load
    // and use in three different ways:
    // - LoadMesh   + LabShader (this lab's shader)
    // - CreateMesh + VertexNormal (this shader is already implemented)
    // - CreateMesh + LabShader (this lab's shader)
    // To get an idea about how they're different from one another, do the
    // following experiments. What happens if you switch the color pipe and
    // normal pipe in this function (but not in the shader)? Now, what happens
    // if you do the same thing in the shader (but not in this function)?
    // Finally, what happens if you do the same thing in both places? Why?

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));


    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

void Tema2::Init()
{
    deltaTimeSecondsShooting = 0;
    timeGame = MAX_TIME_TO_ESCAPE;
    srand(time(0));
    walls = {};
    enemies = {};
    projectiles = {};

    camera_ortho = new implemented::CameraTema();
    //camera->Set(glm::vec3(0, 0.8, 2), glm::vec3(0, 0.8, -2), glm::vec3(0, 1, 0));
    camera_ortho->Set(glm::vec3(0, 0.8, 2), glm::vec3(0, 0.8, -2), glm::vec3(0, 1, 0));

    matrixMaze = new int* [heightMap];
    for (int i = 0; i < heightMap; ++i)
        matrixMaze[i] = new int[widthMap];

    Labirint lab = Labirint(heightMap, widthMap);
    lab.printLabirint();
    int *maze = lab.getMaze();
    startCoordMatrOz = lab.startPos.i;
    startCoordMatrOx = lab.startPos.j;

    gameState = GAME_IN_PROGRESS;

    renderCameraTarget = true;

    camera = new implemented::CameraTema();
    //camera->Set(glm::vec3(0, 0.8, 2), glm::vec3(0, 0.8, -2), glm::vec3(0, 1, 0));
    camera->Set(glm::vec3(0, 0.8, 2), glm::vec3(0, 0.8, -2), glm::vec3(0, 1, 0));
    noProjectiles = 0;

    for (int i = 0; i < heightMap; i++) {
        for (int j = 0; j < widthMap; j++) {
            matrixMaze[i][j] = maze[i * widthMap + j];

            if (maze[i * widthMap + j]) {
                int currOx = (j - startCoordMatrOx) * cell_length - cell_length / 2;
                int currOz = (i - startCoordMatrOz) * cell_length + cell_length / 2;

                if (maze[i * widthMap + j] == 1) {
                    walls.push_back(Wall(glm::vec3(currOx, 0, currOz), cell_length));
                }
                else if (maze[i * widthMap + j] == 2) {
                    enemies.push_back(Enemy(glm::vec3(currOx, 0, currOz), 2.5, 1.3, 1.5, i, j));
                }
            }
        }
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("body", glm::vec3(-0.5, -0.2, 0.5), 1, 0.7, -1, glm::vec3((float)152 / 255, (float)251 / 255, (float)152 / 255));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("leg_left", glm::vec3(-0.5, -0.8, 0.5), 0.42, 0.6, -1, glm::vec3((float)93 / 255, (float)173 / 255, (float)236 / 255));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("leg_right", glm::vec3(0.08, -0.8, 0.5), 0.42, 0.6, -1, glm::vec3((float) 93/255, (float) 173/255, (float) 236/255));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("arm_left", glm::vec3(-0.65, -0.05, 0.5), 0.15, 0.55, -1, glm::vec3((float) 152/255, (float)251/255, (float)152/255));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("arm_right", glm::vec3(0.5, -0.05, 0.5), 0.15, 0.55, -1, glm::vec3((float) 152 / 255, (float)251 / 255, (float)152 / 255));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("forearm_left", glm::vec3(-0.65, -0.2, 0.5), 0.15, 0.15, -1, glm::vec3(1, 0.807f, 0.705f));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("forearm_right", glm::vec3(0.5, -0.2, 0.5), 0.15, 0.15, -1, glm::vec3(1, 0.807f, 0.705f));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("head", glm::vec3(-0.3, 0.5, 0.5), 0.6, 0.40, -1, glm::vec3(1, 0.807f, 0.705f));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object2D::CreateCircle("hitcircle_player", glm::vec3(0, -0.8, 0), sqrt(2)/2, glm::vec3(1, 0, 0), true);
        AddMeshToList(mesh);
    }

    // for enemy

    {
        Mesh* mesh = object3D::CreateParallelepiped("body_enemy", glm::vec3(0, 0.4, 0), 2.5, 1.3, -1.5, glm::vec3(255, 255, 0));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("leg_left_front_enemy", glm::vec3(0, 0, 0), 0.2, 0.4, -0.2, glm::vec3(0, 255, 0));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("leg_right_front_enemy", glm::vec3(2.5-0.2, 0, 0), 0.2, 0.4, -0.2, glm::vec3(0, 255, 0));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("leg_left_back_enemy", glm::vec3(0, 0, -1.5 + 0.2), 0.2, 0.4, -0.2, glm::vec3(0, 255, 0));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("leg_right_back_enemy", glm::vec3(2.5 - 0.2, 0, -1.5 + 0.2), 0.2, 0.4, -0.2, glm::vec3(0, 255, 0));
        AddMeshToList(mesh);
    }

    // for maze
    {
        Mesh* mesh = object3D::CreateParallelepiped("wall", glm::vec3(0, 0, 0), cell_length, cell_length, -cell_length, glm::vec3((float)70/255, (float) 130/255, (float)180/255));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object2D::CreateRectangle("floor", glm::vec3(-cell_length * widthMap / 2, 0, cell_length * heightMap/2)
            ,widthMap * cell_length, -cell_length * heightMap, glm::vec3((float) 139/255, 0, (float) 139/255), true);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("cubeWin", glm::vec3(-cell_length * widthMap / 2, 0, cell_length * heightMap / 2)
            , widthMap * cell_length, cell_length, -cell_length * heightMap, glm::vec3(0, 1, 0));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object3D::CreateParallelepiped("cubeLost", glm::vec3(-cell_length * widthMap / 2, 0, cell_length * heightMap / 2)
            , widthMap * cell_length, cell_length, -cell_length * heightMap, glm::vec3(1, 0, 0));
        AddMeshToList(mesh);
    }


    {
        Mesh* mesh = object2D::CreateRectangle("healthbar_wireframe", glm::vec3(0, 2, 0), 1, 1, glm::vec3(0, 1, 0), false, false);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object2D::CreateRectangle("healthbar_solid", glm::vec3(0, 2, 0), 1, 1, glm::vec3(0, 1, 0), true, false);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object2D::CreateRectangle("time_elapsed_wireframe", glm::vec3(0.6, -1.4, 0), 0.3, 3, glm::vec3(1, 0, 0), false, false);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = object2D::CreateRectangle("time_elapsed_solid", glm::vec3(0.6, -1.4, 0), 0.3, 3, glm::vec3(0, 0, 1), true, false);
        AddMeshToList(mesh);
    }

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    fov = 45.0f;
    deltaOrtho = 0;

    // decisive test
        // Create a simple cube
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

    //    CreateMesh("cube2", vertices, indices);
        
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("DisplacementShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and 4 buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


bool Tema2::collisionPlayerEnemy(float coordPlayerX, float coordPlayerZ, Enemy enemy) {
    // it is enough to check whether the collision in xOz occurs, because both are having y = 0;
    if (enemy.canKill) {

        double x = maxDouble(enemy.getCornerLeft().x, minDouble(coordPlayerX,(double) enemy.getCornerLeft().x + enemy.getWidth()));
        double z = maxDouble((double) enemy.getCornerLeft().z - enemy.getDepth(), minDouble(coordPlayerZ, enemy.getCornerLeft().z));

        double distance = sqrt((x - coordPlayerX) * (x - coordPlayerX) +
            (z - coordPlayerZ) * (z - coordPlayerZ));

        return (distance < sqrt(2) / 2);
    }

    return false;
}

int Tema2::findEnemy(int coordOz, int coordOx) {
    for (int i = 0; i < enemies.size(); i++) {
        if ((enemies[i].getMatrOz() == coordOz) && (enemies[i].getMatrOx() == coordOx)) {
            return i;
        }
    }
    return -1;
}

bool collisionProjectileEnemy(Projectile projectile, Enemy enemy) {
    double x = maxDouble(enemy.getCornerLeft().x, minDouble(projectile.getCrtPos().x, (double) enemy.getCornerLeft().x + enemy.getWidth()));
    // + 0.4, inaltimea picioarelor inamicului
    double y = maxDouble(enemy.getCornerLeft().y, minDouble(projectile.getCrtPos().y, (double) enemy.getCornerLeft().y + enemy.getHeight() + 0.4));
    double z = maxDouble((double) enemy.getCornerLeft().z - enemy.getDepth(), minDouble(projectile.getCrtPos().z, (double) enemy.getCornerLeft().z));

    // this is the same as isPointInsideSphere
    double distance = sqrt((x - projectile.getCrtPos().x) * (x - projectile.getCrtPos().x) +
         (y - projectile.getCrtPos().y) * (y - projectile.getCrtPos().y) +
         (z - projectile.getCrtPos().z) * (z - projectile.getCrtPos().z));

    return distance < projectile.getRadius();
}

bool Tema2::checkCoordsInMap(int coordOx, int coordOz) {
    return ((coordOx >= 0) && (coordOx < widthMap) && (coordOz >= 0) && (coordOz < heightMap));
}

void Tema2::Update(float deltaTimeSeconds)
{
    deltaTimeSecondsShooting += deltaTimeSeconds;

    if (gameState == GAME_DEVELOPMENT) {
        timeDisplacement += deltaTimeSeconds;
        timeDisplacement = (timeDisplacement < 3) ? timeDisplacement : 0;

        
        glm::mat4 modelMatrix = glm::mat4(1);
        projectionMatrix = glm::ortho(-2.f, 1.f, -4.0f, 2.5f, 0.0f, 10.0f);
        RenderMesh(meshes["healthbar_wireframe"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["time_elapsed_wireframe"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::scale(modelMatrix, glm::vec3((float) noLives/MAX_LIVES, 1, 1));
        RenderMesh(meshes["healthbar_solid"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, (float) timeGame / MAX_TIME_TO_ESCAPE, 1));
        RenderMesh(meshes["time_elapsed_solid"], shaders["VertexColor"], modelMatrix);
        
        // RESTORE PREVIOUS PROJECTION MATRIX
        projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
        
        
        //modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5));
        //RenderMesh(meshes["body_enemy"], shaders["DisplacementShader"], modelMatrix);
    } else if (gameState == GAME_IN_PROGRESS) {
        timeGame -= deltaTimeSeconds;

        if ((noLives <= 0) || (timeGame < 0)) {
            cout << "GAME OVER" << endl;
            gameState = GAME_LOST;
        }

        setCenterPlayer();

        // check enemies that might die
        {
            for (int i = 0; i < enemies.size(); i++) {
                if (!enemies[i].canKill) {
                    enemies[i].elapsedAgonizingTime += deltaTimeSeconds;
                    if (enemies[i].elapsedAgonizingTime > enemies[i].agonizingTime) {
                        enemies.erase(std::next(enemies.begin(), i));
                        i--;
                        continue;
                    }
                }
            }
        }

        // check collision player - enemy
        {
            glm::vec3 North = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            glm::vec3 aux_v = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
            North += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixN, coordOzMatrixN;

            coordOxMatrixN = getMatrCoord(North.x, AXIS_MATRIX_OX);
            coordOzMatrixN = getMatrCoord(North.z, AXIS_MATRIX_OZ);

            bool coordsInside = checkCoordsInMap(coordOxMatrixN, coordOzMatrixN);

            if ((coordsInside) && (matrixMaze[coordOzMatrixN][coordOxMatrixN] == 2)) {
                // player is inside an enemy cell, let's check collision;
                int indexEnemy = findEnemy(coordOzMatrixN, coordOxMatrixN);

                // maybe the enemy was killed, so he is no more in that cell
                if (indexEnemy != -1) {

                    if (collisionPlayerEnemy(centerPlayerX, centerPlayerZ, enemies[indexEnemy])) {
                        /*enemies[indexEnemy].canKill = false;*/
                        enemies.erase(std::next(enemies.begin(), indexEnemy));
                        noLives--;

                    }
                }
            }

            glm::vec3 South = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            aux_v = glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
            South += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixS, coordOzMatrixS;

            coordOxMatrixS = getMatrCoord(South.x, AXIS_MATRIX_OX);
            coordOzMatrixS = getMatrCoord(South.z, AXIS_MATRIX_OZ);

            coordsInside = checkCoordsInMap(coordOxMatrixS, coordOzMatrixS);

            if ((coordsInside) && (matrixMaze[coordOzMatrixS][coordOxMatrixS] == 2)) {
                // player is inside an enemy cell, let's check collision;
                int indexEnemy = findEnemy(coordOzMatrixS, coordOxMatrixS);

                // maybe the enemy was killed, so he is no more in that cell
                if (indexEnemy != -1) {
                    if (collisionPlayerEnemy(centerPlayerX, centerPlayerZ, enemies[indexEnemy])) {
                        //enemies[indexEnemy].canKill = false;
                        enemies.erase(std::next(enemies.begin(), indexEnemy));
                        noLives--;
                    }
                }
            }

            glm::vec3 East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
            East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixE, coordOzMatrixE;

            coordOxMatrixE = getMatrCoord(East.x, AXIS_MATRIX_OX);
            coordOzMatrixE = getMatrCoord(East.z, AXIS_MATRIX_OZ);

            coordsInside = checkCoordsInMap(coordOxMatrixE, coordOzMatrixE);

            if ((coordsInside) && (matrixMaze[coordOzMatrixE][coordOxMatrixE] == 2)) {
                // player is inside an enemy cell, let's check collision;
                int indexEnemy = findEnemy(coordOzMatrixE, coordOxMatrixE);

                // maybe the enemy was killed, so he is no more in that cell
                if (indexEnemy != -1) {
                    if (collisionPlayerEnemy(centerPlayerX, centerPlayerZ, enemies[indexEnemy])) {
                        //enemies[indexEnemy].canKill = false;
                        enemies.erase(std::next(enemies.begin(), indexEnemy));
                        noLives--;
                    }
                }
            }

            glm::vec3 West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            aux_v = glm::normalize(glm::vec3(-camera->right.x, 0, -camera->right.z));
            West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixW, coordOzMatrixW;

            coordOxMatrixW = getMatrCoord(West.x, AXIS_MATRIX_OX);
            coordOzMatrixW = getMatrCoord(West.z, AXIS_MATRIX_OZ);

            coordsInside = checkCoordsInMap(coordOxMatrixW, coordOzMatrixW);

            if ((coordsInside) && (matrixMaze[coordOzMatrixW][coordOxMatrixW] == 2)) {
                // player is inside an enemy cell, let's check collision;
                int indexEnemy = findEnemy(coordOzMatrixW, coordOxMatrixW);

                // maybe the enemy was killed, so he is no more in that cell
                if (indexEnemy != -1) {
                    if (collisionPlayerEnemy(centerPlayerX, centerPlayerZ, enemies[indexEnemy])) {
                        //enemies[indexEnemy].canKill = false;
                        enemies.erase(std::next(enemies.begin(), indexEnemy));
                        noLives--;
                    }
                }
            }

        }

        // test for displacement
        {
            //glm::mat4 modelMatrix = glm::mat4(1);
            //modelMatrix = glm::translate(modelMatrix, glm::vec3(1));
            //RenderMesh(meshes["cube2"], shaders["DisplacementShader"], modelMatrix);
        }

        // Render the floor
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-((int)startCoordMatrOx - widthMap / 2) * cell_length, 0, -((int)startCoordMatrOz - heightMap / 2) * cell_length));
            RenderMesh(meshes["floor"], shaders["VertexColor"], modelMatrix);

        }

        for (int i = 0; i < walls.size(); i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, walls[i].getCornerLeft());
            RenderMesh(meshes["wall"], shaders["VertexColor"], modelMatrix);
        }

        for (int i = 0; i < enemies.size(); i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, enemies[i].getCornerLeft());

            if (enemies[i].canKill == true) {
                RenderMesh(meshes["body_enemy"], shaders["VertexColor"], modelMatrix);
                RenderMesh(meshes["leg_left_front_enemy"], shaders["VertexColor"], modelMatrix);
                RenderMesh(meshes["leg_right_front_enemy"], shaders["VertexColor"], modelMatrix);
                RenderMesh(meshes["leg_left_back_enemy"], shaders["VertexColor"], modelMatrix);
                RenderMesh(meshes["leg_right_back_enemy"], shaders["VertexColor"], modelMatrix);
            }
            else {
                timeDisplacement = enemies[i].elapsedAgonizingTime;
                RenderMesh(meshes["body_enemy"], shaders["DisplacementShader"], modelMatrix);
                RenderMesh(meshes["leg_left_front_enemy"], shaders["DisplacementShader"], modelMatrix);
                RenderMesh(meshes["leg_right_front_enemy"], shaders["DisplacementShader"], modelMatrix);
                RenderMesh(meshes["leg_left_back_enemy"], shaders["DisplacementShader"], modelMatrix);
                RenderMesh(meshes["leg_right_back_enemy"], shaders["DisplacementShader"], modelMatrix);
            }
            if (enemies[i].canKill) {
                enemies[i].move(deltaTimeSeconds, cell_length);
            }
        }

        // Render the camera target. This is useful for understanding where
        // the rotation point is, when moving in third-person camera mode.
        if (renderCameraTarget)
        {
            glm::mat4 modelMatrix = glm::mat4(1);

            modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
            modelMatrix = glm::rotate(modelMatrix, rotateOy, glm::vec3(0, 1, 0));

            RenderMesh(meshes["body"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["leg_left"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["leg_right"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["arm_left"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["arm_right"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["forearm_left"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["forearm_right"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["head"], shaders["VertexColor"], modelMatrix);
        }

        // projectiles rendering
        {
            for (int i = 0; i < noProjectiles; i++) {
                if (projectiles[i].getMaxDistance() < glm::l2Norm(projectiles[i].getCrtPos(), projectiles[i].getInitialPos())) {
                    projectiles.erase(std::next(projectiles.begin(), i));
                    i--;
                    noProjectiles--;
                    continue;
                }

                // advance projectile
                projectiles[i].setCrtPos(projectiles[i].getCrtPos() + projectiles[i].getDirection() * 6.f * deltaTimeSeconds);

                if (projectiles[i].getCrtPos().y <= 0) {
                    projectiles.erase(std::next(projectiles.begin(), i));
                    i--;
                    noProjectiles--;
                    continue;
                }

                // check for collision for each projectile with enemy or wall

                // first check for forward (north)
                glm::vec3 North = glm::vec3(projectiles[i].getCrtPos().x, 0, projectiles[i].getCrtPos().z);
                glm::vec3 aux_v = glm::normalize(glm::vec3(projectiles[i].getDirection().x, 0, projectiles[i].getDirection().z));
                North += glm::vec3(aux_v.x * projectiles[i].getRadius(), 0, aux_v.z * projectiles[i].getRadius());

                int coordOxMatrixN, coordOzMatrixN;

                coordOxMatrixN = getMatrCoord(North.x, AXIS_MATRIX_OX);
                coordOzMatrixN = getMatrCoord(North.z, AXIS_MATRIX_OZ);
              
                bool coordsInside = checkCoordsInMap(coordOxMatrixN, coordOzMatrixN);

                if ((coordsInside) && (matrixMaze[coordOzMatrixN][coordOxMatrixN] == 2)) {
                    int indexEnemy = findEnemy(coordOzMatrixN, coordOxMatrixN);

                    // maybe the enemy was killed, so he is no more in that cell
                    if (indexEnemy != -1) {
                        if ((enemies[indexEnemy].canKill) && (collisionProjectileEnemy(projectiles[i], enemies[indexEnemy]))) {
                            enemies[indexEnemy].canKill = false;
                            projectiles.erase(std::next(projectiles.begin(), i));
                            i--;
                            noProjectiles--;
                            continue;
                        }
                    }

                }
                else if ((coordsInside) && (matrixMaze[coordOzMatrixN][coordOxMatrixN] == 1)) {
                    projectiles.erase(std::next(projectiles.begin(), i));
                    i--;
                    noProjectiles--;
                    continue;
                }
                else {
                    // check margins of projectile (West and East)
                    // now test West and East
                    glm::vec3 West = glm::vec3(projectiles[i].getCrtPos().x, 0, projectiles[i].getCrtPos().z);
                    glm::vec3 aux_v = glm::normalize(glm::vec3(-projectiles[i].getRight().x, 0, -projectiles[i].getRight().z));
                    West += glm::vec3(aux_v.x * projectiles[i].getRadius(), 0, aux_v.z * projectiles[i].getRadius());



                    int coordOxMatrixW, coordOzMatrixW;

                    coordOxMatrixW = getMatrCoord(West.x, AXIS_MATRIX_OX);
                    coordOzMatrixW = getMatrCoord(West.z, AXIS_MATRIX_OZ);

                    bool coordsInsideW = checkCoordsInMap(coordOxMatrixW, coordOzMatrixW);

                    glm::vec3 East = glm::vec3(projectiles[i].getCrtPos().x, 0, projectiles[i].getCrtPos().z);
                    aux_v = glm::normalize(glm::vec3(projectiles[i].getRight().x, 0, projectiles[i].getRight().z));
                    East += glm::vec3(aux_v.x * projectiles[i].getRadius(), 0, aux_v.z * projectiles[i].getRadius());

                    int coordOxMatrixE, coordOzMatrixE;

                    coordOxMatrixE = getMatrCoord(East.x, AXIS_MATRIX_OX);
                    coordOzMatrixE = getMatrCoord(East.z, AXIS_MATRIX_OZ);

                    bool coordsInsideE = checkCoordsInMap(coordOxMatrixE, coordOzMatrixE);

                    if ((coordsInsideW) && (matrixMaze[coordOzMatrixW][coordOxMatrixW] == 2)) {
                        int indexEnemy = findEnemy(coordOzMatrixW, coordOxMatrixW);

                        // maybe the enemy was killed, so he is no more in that cell
                        if (indexEnemy != -1) {
                            if ((enemies[indexEnemy].canKill) && (collisionProjectileEnemy(projectiles[i], enemies[indexEnemy]))) {
                                enemies[indexEnemy].canKill = false;
                                projectiles.erase(std::next(projectiles.begin(), i));
                                i--;
                                noProjectiles--;
                                continue;
                            }
                        }
                    }
                    else if ((coordsInsideE) && (matrixMaze[coordOzMatrixE][coordOxMatrixE] == 2)) {
                        int indexEnemy = findEnemy(coordOzMatrixE, coordOxMatrixE);

                        // maybe the enemy was killed, so he is no more in that cell
                        if (indexEnemy != -1) {
                            if ((enemies[indexEnemy].canKill) && (collisionProjectileEnemy(projectiles[i], enemies[indexEnemy]))) {
                                enemies[indexEnemy].canKill = false;
                                projectiles.erase(std::next(projectiles.begin(), i));
                                i--;
                                noProjectiles--;
                                continue;
                            }
                        }
                    }
                    else if (((coordsInsideW) && (matrixMaze[coordOzMatrixW][coordOxMatrixW] == 1))
                        || ((coordsInsideE) && (matrixMaze[coordOzMatrixE][coordOxMatrixE] == 1))) {
                        projectiles.erase(std::next(projectiles.begin(), i));
                        i--;
                        noProjectiles--;
                        continue;
                    }
                    else {
                        // test n-w and n-e
                        glm::vec3 North_West = glm::vec3(projectiles[i].getCrtPos().x, 0, projectiles[i].getCrtPos().z);
                        glm::vec3 aux_v = glm::vec3(-projectiles[i].getRight().x, 0, -projectiles[i].getRight().z);
                        aux_v += glm::vec3(projectiles[i].getDirection().x, 0, projectiles[i].getDirection().z);
                        aux_v = glm::normalize(aux_v);
                        North_West += glm::vec3(aux_v.x * projectiles[i].getRadius(), 0, aux_v.z * projectiles[i].getRadius());


                        int coordOxMatrixNW, coordOzMatrixNW;

                        coordOxMatrixNW = getMatrCoord(North_West.x, AXIS_MATRIX_OX);
                        coordOzMatrixNW = getMatrCoord(North_West.z, AXIS_MATRIX_OZ);

                        bool coordsInsideNW = checkCoordsInMap(coordOxMatrixNW, coordOzMatrixNW);

                        glm::vec3 North_East = glm::vec3(projectiles[i].getCrtPos().x, 0, projectiles[i].getCrtPos().z);
                        aux_v = glm::normalize(glm::vec3(projectiles[i].getRight().x, 0, projectiles[i].getRight().z));
                        aux_v += glm::normalize(glm::vec3(projectiles[i].getDirection().x, 0, projectiles[i].getDirection().z));
                        aux_v = glm::normalize(aux_v);
                        North_East += glm::vec3(aux_v.x * projectiles[i].getRadius(), 0, aux_v.z * projectiles[i].getRadius());

                        int coordOxMatrixNE, coordOzMatrixNE;

                        coordOxMatrixNE = getMatrCoord(North_East.x, AXIS_MATRIX_OX);
                        coordOzMatrixNE = getMatrCoord(North_East.z, AXIS_MATRIX_OZ);

                        bool coordsInsideNE = checkCoordsInMap(coordOxMatrixNE, coordOzMatrixNE);

                        if (((coordsInsideNW) && (matrixMaze[coordOzMatrixNW][coordOxMatrixNW] == 1))
                            || ((coordsInsideNE) && (matrixMaze[coordOzMatrixNE][coordOxMatrixNE] == 1))) {
                            projectiles.erase(std::next(projectiles.begin(), i));
                            i--;
                            noProjectiles--;
                            continue;
                        }
                        glm::vec3 South = glm::vec3(projectiles[i].getCrtPos().x, 0, projectiles[i].getCrtPos().z);
                        aux_v = glm::normalize(glm::vec3(-projectiles[i].getDirection().x, 0, -projectiles[i].getDirection().z));
                        North += glm::vec3(aux_v.x * projectiles[i].getRadius(), 0, aux_v.z * projectiles[i].getRadius());

                        int coordOxMatrixS, coordOzMatrixS;

                        coordOxMatrixS = getMatrCoord(South.x, AXIS_MATRIX_OX);
                        coordOzMatrixS = getMatrCoord(South.z, AXIS_MATRIX_OZ);

                        bool coordsInsideS = checkCoordsInMap(coordOxMatrixS, coordOzMatrixS);

                        // test south for projectile vs enemy
                        if ((coordsInsideS) && (matrixMaze[coordOzMatrixS][coordOxMatrixS] == 2)) {
                            int indexEnemy = findEnemy(coordOzMatrixS, coordOxMatrixS);

                            // maybe the enemy was killed, so he is no more in that cell
                            if (indexEnemy != -1) {
                                if ((enemies[indexEnemy].canKill) && (collisionProjectileEnemy(projectiles[i], enemies[indexEnemy]))) {
                                    enemies[indexEnemy].canKill = false;
                                    projectiles.erase(std::next(projectiles.begin(), i));
                                    i--;
                                    noProjectiles--;
                                    continue;
                                }
                            }
                        }
                    }
                }
                glm::mat4 modelMatrix = glm::mat4(1);

                modelMatrix = glm::translate(modelMatrix, projectiles[i].getCrtPos());
                RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
            }
        }

       /* glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(centerPlayerX, 0.8, centerPlayerZ));
        RenderMesh(meshes["hitcircle_player"], shaders["VertexColor"], modelMatrix);*/

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            projectionMatrix = glm::ortho(-2.f, 1.f, -4.0f, 2.5f, 0.0f, 10.0f);
            RenderMesh(meshes["healthbar_wireframe"], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes["time_elapsed_wireframe"], shaders["VertexColor"], modelMatrix);

            modelMatrix = glm::scale(modelMatrix, glm::vec3((float)noLives / MAX_LIVES, 1, 1));
            RenderMesh(meshes["healthbar_solid"], shaders["VertexColor"], modelMatrix);

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, ((-(MAX_TIME_TO_ESCAPE - (float)timeGame) / 2) / MAX_TIME_TO_ESCAPE)*2.8f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, (float)timeGame / MAX_TIME_TO_ESCAPE, 1));
            RenderMesh(meshes["time_elapsed_solid"], shaders["VertexColor"], modelMatrix);
            projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
        }
    }
    else if (gameState == GAME_WON) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-((int)startCoordMatrOx - widthMap / 2) * cell_length, 0, -((int)startCoordMatrOz - heightMap / 2) * cell_length));
        RenderMesh(meshes["cubeWin"], shaders["VertexColor"], modelMatrix);
        //RenderMesh(meshes["hitcircle_player"], shaders["VertexColor"], modelMatrix);
    }
    else if (gameState == GAME_LOST) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-((int)startCoordMatrOx - widthMap / 2) * cell_length, 0, -((int)startCoordMatrOz - heightMap / 2) * cell_length));
        RenderMesh(meshes["cubeLost"], shaders["VertexColor"], modelMatrix);
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();

    if ((strcmp(mesh->GetMeshID(), "healthbar_wireframe") == 0) ||
        (strcmp(mesh->GetMeshID(), "healthbar_solid") == 0) ||
        (strcmp(mesh->GetMeshID(), "time_elapsed_wireframe") == 0) ||
        (strcmp(mesh->GetMeshID(), "time_elapsed_solid") == 0)) {

        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera_ortho->GetViewMatrix()));
        //glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    }
    else {
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    }
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));


    if (strcmp(shader->GetName(), "DisplacementShader") == 0) {
        int location_time = glGetUniformLocation(shader->program, "Time");
        glUniform1f(location_time, timeDisplacement);
    }

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */



void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    if ((!gameState) || (gameState == GAME_DEVELOPMENT)){
        float cameraSpeed = 1.5f;

        if (window->KeyHold(GLFW_KEY_W)) {
            camera->MoveForward(deltaTime * cameraSpeed);

            glm::vec3 North = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            glm::vec3 aux_v = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
            North += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixN, coordOzMatrixN;

            coordOxMatrixN = getMatrCoord(North.x, AXIS_MATRIX_OX);
            coordOzMatrixN = getMatrCoord(North.z, AXIS_MATRIX_OZ);

            if (gameWon(coordOxMatrixN, coordOzMatrixN)) {
                cout << "Felicitari!" << endl;
                gameState = GAME_WON;
            }
            else {
                if (matrixMaze[coordOzMatrixN][coordOxMatrixN] == 1) {
                    camera->MoveForward(-deltaTime * cameraSpeed);
                }
                else {
                    // now test West and East
                    glm::vec3 West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    glm::vec3 aux_v = glm::normalize(glm::vec3(-camera->right.x, 0, -camera->right.z));
                    West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixW, coordOzMatrixW;

                    coordOxMatrixW = getMatrCoord(West.x, AXIS_MATRIX_OX);
                    coordOzMatrixW = getMatrCoord(West.z, AXIS_MATRIX_OZ);

                    glm::vec3 East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
                    East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixE, coordOzMatrixE;

                    coordOxMatrixE = getMatrCoord(East.x, AXIS_MATRIX_OX);
                    coordOzMatrixE = getMatrCoord(East.z, AXIS_MATRIX_OZ);

                    if ((gameWon(coordOxMatrixE, coordOzMatrixE)) || (gameWon(coordOxMatrixW, coordOzMatrixW))) {
                        cout << "Felicitari!" << endl;
                        gameState = GAME_WON;
                    }
                    else {

                        if ((matrixMaze[coordOzMatrixW][coordOxMatrixW] == 1) || (matrixMaze[coordOzMatrixE][coordOxMatrixE] == 1)) {
                            camera->MoveForward(-deltaTime * cameraSpeed);
                        }
                        else {
                            // test n-w and n-e
                            glm::vec3 North_West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            glm::vec3 aux_v = glm::vec3(-camera->right.x, 0, -camera->right.z);
                            aux_v += glm::vec3(camera->forward.x, 0, camera->forward.z);
                            aux_v = glm::normalize(aux_v);
                            North_West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);


                            int coordOxMatrixNW, coordOzMatrixNW;

                            coordOxMatrixNW = getMatrCoord(North_West.x, AXIS_MATRIX_OX);
                            coordOzMatrixNW = getMatrCoord(North_West.z, AXIS_MATRIX_OZ);

                            glm::vec3 North_East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
                            aux_v += glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
                            aux_v = glm::normalize(aux_v);
                            North_East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                            int coordOxMatrixNE, coordOzMatrixNE;

                            coordOxMatrixNE = getMatrCoord(North_East.x, AXIS_MATRIX_OX);
                            coordOzMatrixNE = getMatrCoord(North_East.z, AXIS_MATRIX_OZ);

                            if ((gameWon(coordOxMatrixNE, coordOzMatrixNE)) || (gameWon(coordOxMatrixNW, coordOzMatrixNW))) {
                                cout << "Felicitari!" << endl;
                                gameState = GAME_WON;
                            }
                            else {
                                if ((matrixMaze[coordOzMatrixNW][coordOxMatrixNW] == 1) || (matrixMaze[coordOzMatrixNE][coordOxMatrixNE] == 1)) {
                                    camera->MoveForward(-deltaTime * cameraSpeed);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(-deltaTime * cameraSpeed);

            // first check west
            glm::vec3 West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            glm::vec3 aux_v = glm::normalize(glm::vec3(-camera->right.x, 0, -camera->right.z));
            West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixW, coordOzMatrixW;

            coordOxMatrixW = getMatrCoord(West.x, AXIS_MATRIX_OX);
            coordOzMatrixW = getMatrCoord(West.z, AXIS_MATRIX_OZ);

            if (gameWon(coordOxMatrixW, coordOzMatrixW)) {
                cout << "Felicitari!" << endl;
                gameState = GAME_WON;
            }
            else {

                if (matrixMaze[coordOzMatrixW][coordOxMatrixW] == 1) {
                    camera->TranslateRight(deltaTime * cameraSpeed);
                }
                else {
                    // now test North and South
                    glm::vec3 North = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    glm::vec3 aux_v = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
                    North += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixN, coordOzMatrixN;

                    coordOxMatrixN = getMatrCoord(North.x, AXIS_MATRIX_OX);
                    coordOzMatrixN = getMatrCoord(North.z, AXIS_MATRIX_OZ);

                    glm::vec3 South = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    aux_v = glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
                    South += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixS, coordOzMatrixS;

                    coordOxMatrixS = getMatrCoord(South.x, AXIS_MATRIX_OX);
                    coordOzMatrixS = getMatrCoord(South.z, AXIS_MATRIX_OZ);

                    if ((gameWon(coordOxMatrixN, coordOzMatrixN)) || (gameWon(coordOxMatrixS, coordOzMatrixS))) {
                        cout << "Felicitari!" << endl;
                        gameState = GAME_WON;
                    }
                    else {
                        if ((matrixMaze[coordOzMatrixN][coordOxMatrixN] == 1) || (matrixMaze[coordOzMatrixS][coordOxMatrixS] == 1)) {
                            camera->TranslateRight(deltaTime * cameraSpeed);
                        }

                        else {
                            // test n-w and s-w
                            glm::vec3 North_West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            glm::vec3 aux_v = glm::vec3(-camera->right.x, 0, -camera->right.z);
                            aux_v += glm::vec3(camera->forward.x, 0, camera->forward.z);
                            aux_v = glm::normalize(aux_v);
                            North_West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);


                            int coordOxMatrixNW, coordOzMatrixNW;

                            coordOxMatrixNW = getMatrCoord(North_West.x, AXIS_MATRIX_OX);
                            coordOzMatrixNW = getMatrCoord(North_West.z, AXIS_MATRIX_OZ);

                            glm::vec3 South_West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            aux_v = glm::normalize(glm::vec3(-camera->right.x, 0, -camera->right.z));
                            aux_v += glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
                            aux_v = glm::normalize(aux_v);
                            South_West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                            int coordOxMatrixSW, coordOzMatrixSW;

                            coordOxMatrixSW = getMatrCoord(South_West.x, AXIS_MATRIX_OX);
                            coordOzMatrixSW = getMatrCoord(South_West.z, AXIS_MATRIX_OZ);

                            if ((gameWon(coordOxMatrixNW, coordOzMatrixNW)) || (gameWon(coordOxMatrixSW, coordOzMatrixSW))) {
                                cout << "Felicitari!" << endl;
                                gameState = GAME_WON;
                            }
                            else {
                                if ((matrixMaze[coordOzMatrixNW][coordOxMatrixNW] == 1) || (matrixMaze[coordOzMatrixSW][coordOxMatrixSW] == 1)) {
                                    camera->TranslateRight(deltaTime * cameraSpeed);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->MoveForward(-deltaTime * cameraSpeed);

            // first test south
            glm::vec3 South = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            glm::vec3 aux_v = glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
            South += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixS, coordOzMatrixS;

            coordOxMatrixS = getMatrCoord(South.x, AXIS_MATRIX_OX);
            coordOzMatrixS = getMatrCoord(South.z, AXIS_MATRIX_OZ);

            if (gameWon(coordOxMatrixS, coordOzMatrixS)) {
                cout << "Felicitari!" << endl;
                gameState = GAME_WON;
            }
            else {
                if (matrixMaze[coordOzMatrixS][coordOxMatrixS] == 1) {
                    camera->MoveForward(deltaTime * cameraSpeed);
                }
                else {
                    // now test west and East
                    glm::vec3 West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    glm::vec3 aux_v = glm::normalize(glm::vec3(-camera->right.x, 0, -camera->right.z));
                    West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixW, coordOzMatrixW;

                    coordOxMatrixW = getMatrCoord(West.x, AXIS_MATRIX_OX);
                    coordOzMatrixW = getMatrCoord(West.z, AXIS_MATRIX_OZ);

                    glm::vec3 East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
                    East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixE, coordOzMatrixE;

                    coordOxMatrixE = getMatrCoord(East.x, AXIS_MATRIX_OX);
                    coordOzMatrixE = getMatrCoord(East.z, AXIS_MATRIX_OZ);

                    if ((gameWon(coordOxMatrixE, coordOzMatrixE)) || (gameWon(coordOxMatrixW, coordOzMatrixW))) {
                        cout << "Felicitari!" << endl;
                        gameState = GAME_WON;
                    }
                    else {

                        if ((matrixMaze[coordOzMatrixW][coordOxMatrixW] == 1) || (matrixMaze[coordOzMatrixE][coordOxMatrixE] == 1)) {
                            camera->MoveForward(deltaTime * cameraSpeed);
                        }
                        else {
                            // test s-w and s-e
                            glm::vec3 South_West = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            glm::vec3 aux_v = glm::vec3(-camera->right.x, 0, -camera->right.z);
                            aux_v += glm::vec3(-camera->forward.x, 0, -camera->forward.z);
                            aux_v = glm::normalize(aux_v);
                            South_West += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);


                            int coordOxMatrixSW, coordOzMatrixSW;

                            coordOxMatrixSW = getMatrCoord(South_West.x, AXIS_MATRIX_OX);
                            coordOzMatrixSW = getMatrCoord(South_West.z, AXIS_MATRIX_OZ);

                            glm::vec3 South_East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
                            aux_v += glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
                            aux_v = glm::normalize(aux_v);
                            South_East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                            int coordOxMatrixSE, coordOzMatrixSE;

                            coordOxMatrixSE = getMatrCoord(South_East.x, AXIS_MATRIX_OX);
                            coordOzMatrixSE = getMatrCoord(South_East.z, AXIS_MATRIX_OZ);

                            if ((gameWon(coordOxMatrixSW, coordOzMatrixSW)) || (gameWon(coordOxMatrixSE, coordOzMatrixSE))) {
                                cout << "Felicitari!" << endl;
                                gameState = GAME_WON;
                            }
                            else {
                                if ((matrixMaze[coordOzMatrixSW][coordOxMatrixSW] == 1) || (matrixMaze[coordOzMatrixSE][coordOxMatrixSE] == 1)) {
                                    camera->MoveForward(deltaTime * cameraSpeed);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(deltaTime * cameraSpeed);
            

            // first check east
            glm::vec3 East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
            glm::vec3 aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
            East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

            int coordOxMatrixE, coordOzMatrixE;

            coordOxMatrixE = getMatrCoord(East.x, AXIS_MATRIX_OX);
            coordOzMatrixE = getMatrCoord(East.z, AXIS_MATRIX_OZ);

            if (gameWon(coordOxMatrixE, coordOzMatrixE)) {
                cout << "Felicitari!" << endl;
                gameState = GAME_WON;
            }
            else {

                if (matrixMaze[coordOzMatrixE][coordOxMatrixE] == 1) {
                    camera->TranslateRight(-deltaTime * cameraSpeed);
                }
                else {
                    // now test North and South
                    glm::vec3 North = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    glm::vec3 aux_v = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
                    North += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixN, coordOzMatrixN;

                    coordOxMatrixN = getMatrCoord(North.x, AXIS_MATRIX_OX);
                    coordOzMatrixN = getMatrCoord(North.z, AXIS_MATRIX_OZ);

                    glm::vec3 South = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                    aux_v = glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
                    South += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                    int coordOxMatrixS, coordOzMatrixS;

                    coordOxMatrixS = getMatrCoord(South.x, AXIS_MATRIX_OX);
                    coordOzMatrixS = getMatrCoord(South.z, AXIS_MATRIX_OZ);

                    if ((gameWon(coordOxMatrixN, coordOzMatrixN)) || (gameWon(coordOxMatrixS, coordOzMatrixS))) {
                        cout << "Felicitari!" << endl;
                        gameState = GAME_WON;
                    }
                    else {
                        if ((matrixMaze[coordOzMatrixN][coordOxMatrixN] == 1) || (matrixMaze[coordOzMatrixS][coordOxMatrixS] == 1)) {
                            camera->TranslateRight(-deltaTime * cameraSpeed);
                        }
                        else {
                            // test n-e and s-e
                            glm::vec3 North_East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            glm::vec3 aux_v = glm::vec3(camera->right.x, 0, camera->right.z);
                            aux_v += glm::vec3(camera->forward.x, 0, camera->forward.z);
                            aux_v = glm::normalize(aux_v);
                            North_East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);


                            int coordOxMatrixNE, coordOzMatrixNE;

                            coordOxMatrixNE = getMatrCoord(North_East.x, AXIS_MATRIX_OX);
                            coordOzMatrixNE = getMatrCoord(North_East.z, AXIS_MATRIX_OZ);

                            glm::vec3 South_East = glm::vec3(centerPlayerX, 0, centerPlayerZ);
                            aux_v = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
                            aux_v += glm::normalize(glm::vec3(-camera->forward.x, 0, -camera->forward.z));
                            aux_v = glm::normalize(aux_v);
                            South_East += glm::vec3(aux_v.x * sqrt(2) / 2, 0, aux_v.z * sqrt(2) / 2);

                            int coordOxMatrixSE, coordOzMatrixSE;

                            coordOxMatrixSE = getMatrCoord(South_East.x, AXIS_MATRIX_OX);
                            coordOzMatrixSE = getMatrCoord(South_East.z, AXIS_MATRIX_OZ);

                            if ((gameWon(coordOxMatrixNE, coordOzMatrixNE)) || (gameWon(coordOxMatrixSE, coordOzMatrixSE))) {
                                cout << "Felicitari!" << endl;
                                gameState = GAME_WON;
                            }
                            else {
                                if ((matrixMaze[coordOzMatrixNE][coordOxMatrixNE] == 1) || (matrixMaze[coordOzMatrixSE][coordOxMatrixSE] == 1)) {
                                    camera->TranslateRight(-deltaTime * cameraSpeed);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (window->KeyHold(GLFW_KEY_LEFT_CONTROL)) {
            renderCameraTarget = false;

            if (!movedToFirst) {

                camera->Set(glm::vec3(camera->position.x, 0.8, camera->position.z), glm::vec3(centerPlayerX, 0.8, centerPlayerZ), camera->up);
                camera->MoveForward(camera->distanceToTarget);
                movedToFirst = true;
            }
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    if (!gameState) {
        if ((key == GLFW_KEY_SPACE) && (!renderCameraTarget) && (deltaTimeSecondsShooting > MIN_FIRE_RATE)) {
            deltaTimeSecondsShooting = 0;

            noProjectiles += 1;

            Projectile new_projectile = Projectile(camera->position, camera->forward, camera->right, 0.5f);
            projectiles.push_back(new_projectile);
            distanceBullet = camera->forward;
        }
    }
    // TODO(student): Switch projections
}


void Tema2::OnKeyRelease(int key, int mods)
{
    if (!gameState) {
        if (key == GLFW_KEY_LEFT_CONTROL) {
            if (movedToFirst == true) {
                camera->MoveForward(-camera->distanceToTarget);

                camera->Set(glm::vec3(camera->position.x, 0.8, camera->position.z), glm::vec3(centerPlayerX, 0.8, centerPlayerZ), camera->up);
                //camera->forward = glm::vec3(camera->forward.x, 0.8, camera->forward.z);
                //camera->position = glm::vec3(camera->position.x, 0.8, camera->position.z);
                movedToFirst = false;
            }
            renderCameraTarget = true;
        }
    }
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    if (!renderCameraTarget) {
        camera->RotateFirstPerson_OX(deltaY * (-4) * sensivityOX);
        camera->RotateFirstPerson_OY(deltaX * (-4) * sensivityOY);

        rotateOy += deltaX * (-4) * sensivityOY;
    } else {
        camera->RotateThirdPerson_OX(deltaY * (-1) * sensivityOX);
        camera->RotateThirdPerson_OY(deltaX * (-5) * sensivityOY); 
        
        rotateOy += deltaX * (-5) * sensivityOY;
        //rotateOy = 0;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
