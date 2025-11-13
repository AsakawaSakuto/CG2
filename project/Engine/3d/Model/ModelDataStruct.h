#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include <string>
#include <vector>
#include <assimp/scene.h>

struct ModelVertexData {
    Vector4 position; // 16 bytes
    Vector2 texcoord; // 8 bytes
    Vector3 normal;   // 12 bytes
    float pad[2];     // 8 bytes (align to 16)
    float pad2;       // 4 bytes (align to 16 again)
};

struct ModelMaterial {
    Vector4 color;
    int32_t enableLighting;
    float pad1[3];
    Matrix4x4 uvTransformMatrix;
    float shininess;
    float pad2[3];
};

struct ModeldMaterialData {
    std::string textureFilePath;
};

struct ModelTransformationMatrix {
    Matrix4x4 WVP;
    Matrix4x4 World;
    Matrix4x4 WorldInverseTranspose;
};

struct ModelSubMesh {
	uint32_t indexStart;
	uint32_t indexCount;
	uint32_t materialIndex;
};

struct ModelNode {
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<ModelNode> children;

    ModelNode ReadNode(aiNode* node) {
        ModelNode result;
        aiMatrix4x4 aiLocalMatrix = node->mTransformation; // nodeのlocalMatrixを取得
        aiLocalMatrix.Transpose(); // 列ベクトル形式を行ベクトル形式に転置
        for (uint32_t i = 0; i < 4; i++)
        {
            for (uint32_t j = 0; j < 4; j++)
            {
                result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
            }
        }
        result.name = node->mName.C_Str(); // Node名を格納
        result.children.resize(node->mNumChildren); // 子供の数だけ確保
        for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
            // 再帰的に読んで階層構造を作っていく
            result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
        }
        return result;
    }
};

struct ModelData {
    std::vector<ModelVertexData> vertices;
    ModeldMaterialData material;
    ModelNode rootNode;
};