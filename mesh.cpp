#include "mesh.h"
#include <glm/glm.hpp>

Mesh Mesh::interpolate(float t) const
{
    Mesh result;

    result.f = f;
    

    result.v.resize(v.size());
    for (int i = 0; i < v.size(); i++)
    {
        result.v[i].pos = glm::mix(
            this->v[i].pos, 
            glm::vec3(this->v[i].uv, 0.0), 
            t
        );
        result.v[i].uv = this->v[i].uv;
    }

    return result;
}
