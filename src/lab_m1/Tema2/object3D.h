#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object3D
{

    // Create parallelepiped with given bottom left corner, width, height and color
    Mesh* CreateParallelepiped(const std::string& name, glm::vec3 leftBottomCorner, float width, float height,
                                float depth, glm::vec3 color);
}

namespace object2D
{
    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill);

    // Create rectangle with given bottom left corner, length and with and color
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float length,
        float width, glm::vec3 color, bool fill = false, bool horizontal = true);
}