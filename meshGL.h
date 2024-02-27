#pragma once
#include "Shader.h"
#include "GL/glew.h"

struct Mesh;

struct MeshGl
{
	friend struct Mesh;
private:
	unsigned int VAO, VBO, EBO;
	unsigned int indexCount;

public:
	void draw(const Shader& shader) const;
	void deleteBuffers();

	~MeshGl();


};