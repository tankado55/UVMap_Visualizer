#include "mesh.h"
#include <iostream>
#include <fstream>

void Mesh::exportOBJ(std::string fileName)
{
	std::ofstream file(fileName);

	for (int i = 0; i < v.size(); ++i)
	{
		file << "v " << v[i].pos.x << " " << v[i].pos.y << " " << v[i].pos.z << std::endl;
	}

	for (int i = 0; i < v.size(); ++i)
	{
		file << "vt " << v[i].uv.x << " " << v[i].uv.y << std::endl;
	}

	for (int i = 0; i < f.size(); ++i)
	{
		int v0 = f[i].vi[0];
		int v1 = f[i].vi[1];
		int v2 = f[i].vi[2];
		file << "f " << v0 << " " << v1 << " " << v2 << std::endl;
	}
}
