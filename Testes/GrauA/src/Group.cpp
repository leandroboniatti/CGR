#include "Group.h"
#include <glad/glad.h>
#include <vector>

void Group::setupGL(const std::vector<glm::vec3>& vertices,
                   const std::vector<glm::vec3>& normals, 
                   const std::vector<glm::vec2>& texCoords) {
    
    std::vector<float> vertexData;
    std::vector<Vertex> processedVertices;
    
    // Process all faces and convert to triangles
    for (const auto& face : faces) {
        std::vector<Face> triangles = face.triangulate();
        
        for (const auto& tri : triangles) {
            for (int i = 0; i < 3; ++i) {
                Vertex vertex;
                
                // Position
                if (tri.verts[i] >= 0 && tri.verts[i] < vertices.size()) {
                    vertex.position = vertices[tri.verts[i]];
                }
                
                // Normal
                if (i < tri.norms.size() && tri.norms[i] >= 0 && tri.norms[i] < normals.size()) {
                    vertex.normal = normals[tri.norms[i]];
                } else {
                    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // default up
                }
                
                // Texture coordinate
                if (i < tri.texts.size() && tri.texts[i] >= 0 && tri.texts[i] < texCoords.size()) {
                    vertex.texCoord = texCoords[tri.texts[i]];
                } else {
                    vertex.texCoord = glm::vec2(0.0f, 0.0f); // default
                }
                
                processedVertices.push_back(vertex);
            }
        }
    }
    
    // Create interleaved vertex data
    for (const auto& vertex : processedVertices) {
        // Position
        vertexData.push_back(vertex.position.x);
        vertexData.push_back(vertex.position.y);
        vertexData.push_back(vertex.position.z);
        
        // Normal
        vertexData.push_back(vertex.normal.x);
        vertexData.push_back(vertex.normal.y);
        vertexData.push_back(vertex.normal.z);
        
        // Texture coordinate
        vertexData.push_back(vertex.texCoord.x);
        vertexData.push_back(vertex.texCoord.y);
    }
    
    vertexCount = processedVertices.size();
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), 
                 vertexData.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}