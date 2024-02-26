#pragma once

struct Mesh;

struct MeshGL
{
	void draw() const;

	friend struct Mesh;
private:
	unsigned int VAO, VBO, EBO;
};