#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <string>

struct MeshGL;

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv;
};

struct Face
{
	int vi[3];
};

struct Mesh
{
	std::vector<Vertex> v;
	std::vector<Face> f;

	bool importOBJ(const char* fileName);

	Mesh interpolate(float t) const;
	void exportOBJ(std::string fileName);
	MeshGL bake();
};
