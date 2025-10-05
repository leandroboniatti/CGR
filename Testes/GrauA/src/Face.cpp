#include "Face.h"

std::vector<Face> Face::triangulate() const {
    std::vector<Face> triangles;
    
    if (verts.size() < 3) return triangles;
    
    // Fan triangulation for n-gons
    for (size_t i = 1; i < verts.size() - 1; ++i) {
        Face triangle;
        
        // Vertices
        triangle.verts.push_back(verts[0]);
        triangle.verts.push_back(verts[i]);
        triangle.verts.push_back(verts[i + 1]);
        
        // Normals (if available)
        if (!norms.empty() && norms.size() >= verts.size()) {
            triangle.norms.push_back(norms[0]);
            triangle.norms.push_back(norms[i]);
            triangle.norms.push_back(norms[i + 1]);
        }
        
        // Texture coordinates (if available)
        if (!texts.empty() && texts.size() >= verts.size()) {
            triangle.texts.push_back(texts[0]);
            triangle.texts.push_back(texts[i]);
            triangle.texts.push_back(texts[i + 1]);
        }
        
        triangles.push_back(triangle);
    }
    
    return triangles;
}