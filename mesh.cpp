#include "mesh.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "meshGL.h"


Mesh Mesh::interpolate(float t) const
{
    Mesh result;

    result.f = f;
    
    result.v.resize(v.size());
    for (int i = 0; i < v.size(); i++)
    {
        int faceIndex = i / 3;
        float uvScale = f[faceIndex].uvScaling;
        result.v[i].pos = glm::mix(
            this->v[i].pos, 
            glm::vec3(this->v[i].uv, 0.0) * uvScale, 
            t
        );
        result.v[i].uv = this->v[i].uv;
    }

    return result;
}

MeshGl Mesh::bake()
{
    MeshGl result;

    glGenVertexArrays(1, &result.VAO);
    glGenBuffers(1, &result.VBO);
    glGenBuffers(1, &result.EBO);

    glBindVertexArray(result.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, result.VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(Vertex), &v[0], GL_DYNAMIC_DRAW);

    std::vector<int> indexes;
    for (int i = 0; i < f.size(); i++)
    {
        int i0 = f[i].vi[0];
        int i1 = f[i].vi[1];
        int i2 = f[i].vi[2];
        indexes.push_back(i0);
        indexes.push_back(i1);
        indexes.push_back(i2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(int), &indexes[0], GL_STATIC_DRAW);
    result.indexCount = f.size() * 3;

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);

    return result;
}
