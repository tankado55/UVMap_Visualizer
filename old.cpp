static glm::vec3 computeCentroid(glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
    glm::vec3 midAB = { (A.x + B.x) / 2.0, (A.y + B.y) / 2.0, (A.z + B.z) / 2.0 };
    glm::vec3 midBC = { (B.x + C.x) / 2.0, (B.y + C.y) / 2.0, (B.z + C.z) / 2.0 };
    glm::vec3 midCA = { (C.x + A.x) / 2.0, (C.y + A.y) / 2.0, (C.z + A.z) / 2.0 };

    glm::vec3 centroid;
    centroid.x = (midAB.x + midBC.x + midCA.x) / 3.0;
    centroid.y = (midAB.y + midBC.y + midCA.y) / 3.0;
    centroid.z = (midAB.z + midBC.z + midCA.z) / 3.0;
    return centroid;
}

static void addCentroids(aiMesh* meshAi, Mesh& mesh)
{
    for (unsigned int i = 0; i < meshAi->mNumFaces; i++)
    {
        aiFace face = meshAi->mFaces[i];
        if (face.mNumIndices == 3) {
            glm::vec3 A = mesh.v[face.mIndices[0]].pos;
            glm::vec3 B = mesh.v[face.mIndices[1]].pos;
            glm::vec3 C = mesh.v[face.mIndices[2]].pos;
            mesh.f[i].centroid3D = computeCentroid(A, B, C);

            A = glm::vec3(mesh.v[face.mIndices[0]].uv, 0.0);
            B = glm::vec3(mesh.v[face.mIndices[1]].uv, 0.0);
            C = glm::vec3(mesh.v[face.mIndices[2]].uv, 0.0);
            mesh.f[i].centroid2D = computeCentroid(A, B, C);

        }
    }
}

// centroids with volumes
static float SignedVolumeOfTetrahedron(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    return glm::dot(a, glm::cross(b, c)) / 6.0f;
}

static void setupCentroids(Mesh& mesh)
{
    //mesh3D
    glm::vec3 centroid = glm::vec3(0.0);
    float volume = 0;
    for (int i = 0; i < mesh.f.size(); i++)
    {
        Face face = mesh.f[i];
        glm::vec3 a = mesh.v[face.vi[0]].pos;
        glm::vec3 b = mesh.v[face.vi[1]].pos;
        glm::vec3 c = mesh.v[face.vi[2]].pos;

        float tetrahedronVolume = SignedVolumeOfTetrahedron(a, b, c);
        centroid += tetrahedronVolume * (a + b + c) / 4.0f;
        volume += tetrahedronVolume;
    }
    mesh.centroid3D = centroid / volume;

    //2D
    centroid = glm::vec3(0.0);
    volume = 0;
    for (int i = 0; i < mesh.f.size(); i++)
    {
        //mesh2D
        Face face = mesh.f[i];
        glm::vec3 a = glm::vec3(mesh.v[face.vi[0]].uv, 0.0);
        glm::vec3 b = glm::vec3(mesh.v[face.vi[1]].uv, 0.0);
        glm::vec3 c = glm::vec3(mesh.v[face.vi[2]].uv, 0.0);

        float tetrahedronVolume = SignedVolumeOfTetrahedron(a, b, c);
        centroid += tetrahedronVolume * (a + b + c) / 4.0f;
        volume += tetrahedronVolume;
    }
    //mesh.centroid2D = centroid / volume;
    mesh.centroid2D = glm::vec3(0.0);
}