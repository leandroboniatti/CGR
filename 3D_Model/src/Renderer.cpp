#include "Renderer.h"
#include <glad/gl.h>

void Renderer::draw(const std::vector<Mesh>& meshes, const Shader& shader) const {
    for (auto& mesh : meshes) {
        shader.setVec3("material.Ka", mesh.material.Ka);
        shader.setVec3("material.Kd", mesh.material.Kd);
        shader.setVec3("material.Ks", mesh.material.Ks);
        shader.setFloat("material.Ns", mesh.material.Ns);
        shader.setInt("material.hasDiffuseTex", mesh.material.diffuseTex >=0);
        if (mesh.material.diffuseTex >=0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, (unsigned int)mesh.material.diffuseTex);
        }
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}
