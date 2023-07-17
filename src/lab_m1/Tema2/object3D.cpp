#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <iostream>


Mesh* object3D::CreateParallelepiped(
    const std::string& name, glm::vec3 leftBottomCorner,
    float width, float height, float depth,
    glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color),
        VertexFormat(corner + glm::vec3(0, 0, depth), color),
        VertexFormat(corner + glm::vec3(width, 0, depth), color),
        VertexFormat(corner + glm::vec3(width, height, depth), color),
        VertexFormat(corner + glm::vec3(0, height, depth), color),
    };

    Mesh* parallelepiped = new Mesh(name);
    std::vector<unsigned int> indices = { 
        0, 1, 2,
        0, 2, 3,
        1, 5, 6,
        1, 6, 2,
        3, 2, 6,
        3, 6, 7,
        0, 5, 1,
        0, 4, 5,
        4, 6, 5,
        4, 7, 6,
        0, 7, 4,
        0, 3, 7};

    parallelepiped->SetDrawMode(GL_TRIANGLES);

    parallelepiped->InitFromData(vertices, indices);
    return parallelepiped;
}

Mesh* object2D::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{


    std::vector<VertexFormat> vertices = {};
    Mesh* circle = new Mesh(name);
    std::vector<unsigned int> indices = {};
    /*{
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };*/

    if (fill) {
        vertices.push_back(VertexFormat(center, color));

        for (int i = 0; i < 361; i++) {
            double theta = M_PI * i / 180;
            vertices.push_back(VertexFormat(center + glm::vec3(radius * cos(theta), 0, radius * sin(theta)), color));
        }

        for (int i = 0; i < 360; i++) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
        }
    }
    else {
        circle->SetDrawMode(GL_LINE_LOOP);
        for (int a = 0; a < 179; a += 1)
        {
            vertices.push_back(center + glm::vec3(radius * cos(a / M_PI), radius * sin(a / M_PI), 0));
        }

        for (int i = 0; i < vertices.size(); i++)
            indices.push_back(i);
    }
    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* object2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    float width,
    glm::vec3 color,
    bool fill,
    bool horizontal)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices = {};

    if (horizontal) {
        vertices = 
        {
            VertexFormat(corner, color),
            VertexFormat(corner + glm::vec3(length, 0, 0), color),
            VertexFormat(corner + glm::vec3(length, 0, width), color),
            VertexFormat(corner + glm::vec3(0, 0, width), color)
        };
    }
    else {
        vertices =
        {
            VertexFormat(corner, color),
            VertexFormat(corner + glm::vec3(length, 0, 0), color),
            VertexFormat(corner + glm::vec3(length, width, 0), color),
            VertexFormat(corner + glm::vec3(0, width, 0), color)
        };
    }

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}