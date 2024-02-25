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
		int vx = f[i].vi[0];
		int vy = f[i].vi[1];
		int vz = f[i].vi[2];
		file << "f " << vx << "/" << vx << "/" << vx;
		file << " " << vy << "/" << vy << "/" << vy;
		file << " " << vz << "/" << vz << "/" << vz << std::endl;
	}
}
