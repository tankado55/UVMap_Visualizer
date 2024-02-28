#include "meshGL.h"
#include "mesh.h"

MeshGl::~MeshGl()
{
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
}

void MeshGl::draw(const Shader& shader) const
{
    shader.Bind();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void MeshGl::updateGeometry(const Mesh& mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.v.size() * sizeof(Vertex), &mesh.v[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshGl::deleteBuffers()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

