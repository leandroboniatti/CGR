#include "Mesh.h"
#include <limits>

void Mesh::calculateBounds() {
    if (vertices.empty()) {
        min = max = glm::vec3(0.0f);
        return;
    }
    
    min = vertices[0];
    max = vertices[0];
    
    for (const auto& vertex : vertices) {
        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);
        
        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }
}