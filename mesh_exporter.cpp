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
		file << "f";
		for (int j = 0; j < 3; j++)
		{
			int v = f[i].vi[j];
			file << " " << v << "/" << v;
		}
		file << std::endl;
	}
}
