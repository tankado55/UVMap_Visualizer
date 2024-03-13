#pragma once

#include "directionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec):
	direction(dir),
	ambient(amb),
	diffuse(diff),
	specular(spec)
{

}

void DirectionalLight::setUniform(const Shader& shader) const
{
	shader.SetUniformVec3f("u_DirLight.direction", direction);
	shader.SetUniformVec3f("u_DirLight.ambient", ambient);
	shader.SetUniformVec3f("u_DirLight.diffuse", diffuse);
	shader.SetUniformVec3f("u_DirLight.specular", specular);
}
