#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <Eigen/Dense>
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
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
            if (vertex.uv.x > 1.0 || vertex.uv.y > 1.0)
            {
                std::cout << "warning, uv > 1.0" << std::endl;
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
    float scalingSum = 0.0;
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
            if (areaUV > 0)
            {
                float ratio = sqrt(areaMesh / areaUV);
                mesh.f[i].uvScaling = ratio;
                scalingSum += ratio;
            }
        }
    }
    if (scalingSum != 0)
        mesh.averageScaling = scalingSum / meshAi->mNumFaces;
    else
        mesh.averageScaling = 1.0;
}

static void setupCentroids(Mesh& mesh)
{
    //mesh3D
    glm::vec3 centroid = glm::vec3(0.0);
    float areaSum = 0.0;
    for (int i = 0; i < mesh.f.size(); i++)
    {
        Face face = mesh.f[i];
        glm::vec3 a = mesh.v[face.vi[0]].pos;
        glm::vec3 b = mesh.v[face.vi[1]].pos;
        glm::vec3 c = mesh.v[face.vi[2]].pos;

        glm::vec3 center = (a + b + c) / 3.0f;
        float area = 0.5 * length(cross(b - a, c - a));

        centroid += area * center;
        areaSum += area;
    }
    mesh.centroid3D = centroid / areaSum;

    //2D
    centroid = glm::vec3(0.0);
    areaSum = 0.0;
    for (int i = 0; i < mesh.f.size(); i++)
    {
        Face face = mesh.f[i];
        glm::vec3 a = glm::vec3(mesh.v[face.vi[0]].uv, 0.0);
        glm::vec3 b = glm::vec3(mesh.v[face.vi[1]].uv, 0.0);
        glm::vec3 c = glm::vec3(mesh.v[face.vi[2]].uv, 0.0);

        glm::vec3 center = (a + b + c) / 3.0f;
        float area = 0.5 * length(cross(b - a, c - a));

        centroid += area * center;
        areaSum += area;
    }
    mesh.centroid2D = centroid / areaSum;
}

static Eigen::MatrixXd glmToEigen(const glm::mat3& glmMatrix) {
    Eigen::MatrixXd eigenMatrix(3, 3);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            eigenMatrix(i, j) = glmMatrix[i][j];
        }
    }
    return eigenMatrix;
}

static glm::mat3 eigenToGlm(const Eigen::MatrixXd& eigenMatrix) {
    glm::mat3 glmMatrix;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            glmMatrix[i][j] = eigenMatrix(i, j);
        }
    }
    return glmMatrix;
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

    // SVD
    Eigen::MatrixXd A = glmToEigen(result);
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);

    Eigen::Matrix3d R = svd.matrixU() * svd.matrixV().transpose();

    if (R.determinant() < 0) {
        R *= -1;
    }
    result = eigenToGlm(R);

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
        setupCentroids(mesh);
        mesh.bestRotation = computeInitRotation(mesh);
        mesh.updateBB();
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
    std::cout << "vertices: " << v.size() << std::endl;
    return true;
}




