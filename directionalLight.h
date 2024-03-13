#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

class DirectionalLight
{
    glm::vec3 direction;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

public:
    DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
    void setUniform(const Shader& shader) const;

};