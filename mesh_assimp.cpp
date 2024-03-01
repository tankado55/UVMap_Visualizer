#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "mesh.h"
#include "Utils.h"

static void convert(aiMesh* mesh, Mesh& output)
{
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            vertex.uv.x = glm::fract(mesh->mTextureCoords[0][i].x);
            vertex.uv.y = glm::fract(mesh->mTextureCoords[0][i].y);
            if (vertex.uv.x > 1.0 || vertex.uv.y > 1.0)
            {
                std::cout << "warning" << std::endl;
            }
        }
        else vertex.uv = glm::vec2(0.0f, 0.0f);

        output.v.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        
        if (face.mNumIndices == 3) {
            Face meshFace;
            meshFace.vi[0] = face.mIndices[0];
            meshFace.vi[1] = face.mIndices[1];
            meshFace.vi[2] = face.mIndices[2];

            output.f.push_back(meshFace);
        }
        else {
            std::cout << "warning, face.mNumIndices: " << face.mNumIndices << std::endl;
        }
    }
}

static void computeUVScaling(aiMesh* meshAi, Mesh& mesh)
{
    for (unsigned int i = 0; i < meshAi->mNumFaces; i++)
    {
        aiFace face = meshAi->mFaces[i];
        if (face.mNumIndices == 3) {
            //scaling UV
            glm::vec3 v1 = mesh.v[face.mIndices[0]].pos;
            glm::vec3 v2 = mesh.v[face.mIndices[1]].pos;
            glm::vec3 v3 = mesh.v[face.mIndices[2]].pos;

            float areaMesh = Utils::ComputeArea(v1, v2, v3);

            v1 = glm::vec3(mesh.v[face.mIndices[0]].uv, 0.0f);
            v2 = glm::vec3(mesh.v[face.mIndices[1]].uv, 0.0f);
            v3 = glm::vec3(mesh.v[face.mIndices[2]].uv, 0.0f);

            float areaUV = Utils::ComputeArea(v1, v2, v3);

            mesh.f[i].uvScaling = sqrt(areaMesh / areaUV);
        }
    }
}

static float SignedVolumeOfTetrahedron(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    return glm::dot(a, glm::cross(b, c)) / 6.0f;
}

static void addMeshCentroids(Mesh& mesh)
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

static glm::mat3 computeInitRotation(Mesh& mesh)
{
    glm::mat3 result = glm::mat3();
    for (int i = 0; i < mesh.f.size(); i++)
    {
        Face face = mesh.f[i];
        glm::vec3 vi = glm::vec3(0.0);
        glm::vec3 wi = glm::vec3(0.0);

        vi = mesh.v[face.vi[0]].pos - mesh.centroid3D;
        wi = glm::vec3(mesh.v[face.vi[0]].uv, 0.0) - mesh.centroid2D;
        glm::mat3 outer = glm::outerProduct(vi, wi);
        result += outer;
        vi = mesh.v[face.vi[1]].pos - mesh.centroid3D;
        wi = glm::vec3(mesh.v[face.vi[1]].uv, 0.0) - mesh.centroid2D;
        outer = glm::outerProduct(vi, wi);
        result += outer;
        vi = mesh.v[face.vi[2]].pos - mesh.centroid3D;
        wi = glm::vec3(mesh.v[face.vi[2]].uv, 0.0) - mesh.centroid2D;
        outer = glm::outerProduct(vi, wi);
        result += outer;
    }
    result = result / static_cast<float>(mesh.f.size() * 3);
    glm::mat3 transpose = glm::transpose(result);
    glm::mat3 inverseTranspose = glm::inverse(glm::transpose(result));

    result = (result + inverseTranspose) / 2.0f;

    return result;
}

static void processNode(aiNode* node, const aiScene* scene, Mesh& mesh)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aim = scene->mMeshes[node->mMeshes[i]];
        convert(aim, mesh);
        computeUVScaling(aim, mesh);
        addMeshCentroids(mesh);
        mesh.bestRotation = computeInitRotation(mesh);
        break;
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, mesh);
    }
}

bool Mesh::importOBJ(const char* fileName)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    processNode(scene->mRootNode , scene, *this);
    return true;
}


