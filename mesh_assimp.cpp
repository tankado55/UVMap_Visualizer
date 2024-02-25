#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "mesh.h"

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
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
        else vertex.uv = glm::vec2(0.0f, 0.0f);

        output.v.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        
        if (face.mNumIndices == 3) {
            Face meshFace;
            meshFace.vi[0] = ++face.mIndices[0];
            meshFace.vi[1] = ++face.mIndices[1];
            meshFace.vi[2] = ++face.mIndices[2];

            output.f.push_back(meshFace);
        }
        else {
            std::cout << "warning, face.mNumIndices: " << face.mNumIndices << std::endl;
        }
    }
}

static void processNode(aiNode* node, const aiScene* scene, Mesh& mesh)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aim = scene->mMeshes[node->mMeshes[i]];
        convert(aim, mesh);
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


