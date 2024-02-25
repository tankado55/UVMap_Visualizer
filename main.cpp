#include "mesh.h"

int main() {

	Mesh mesh;
	mesh.importOBJ("models/Die-OBJ/Die-OBJ.obj");
	mesh.exportOBJ("test.obj");
	return 0;
}