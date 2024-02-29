#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <string>

struct MeshGl;

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv;
};

struct Face
{
	int vi[3];
	float uvScaling;
	glm::vec3 centroid3D;
	glm::vec3 centroid2D;
};

struct Mesh
{
	std::vector<Vertex> v;
	std::vector<Face> f;

	bool importOBJ(const char* fileName);
	void exportOBJ(std::string fileName);
	Mesh interpolate(float t) const;
	MeshGl bake();
};
