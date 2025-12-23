#pragma once
#include "Utility/Transform/Transform.h"
#include "Math/Type/Matrix4x4.h"
#include "Math/MatrixFunction/MatrixFunction.h"
#include "Math/Type/Vector4.h"
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector2.h"
#include <string>
#include <vector>
#include <map>
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
    QuaternionTransform transform;
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<ModelNode> children;

    ModelNode ReadNode(aiNode* node) {
        ModelNode result;
        aiVector3D scale, translate;
        aiQuaternion rotate;
        node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数を利用
        result.transform.scale = { scale.x, scale.y, scale.z }; // Scaleはそのまま
        result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; // x軸を反転、さらに回転方向が逆なので軸を反転させる
        result.transform.translate = { -translate.x, translate.y, translate.z }; // x軸を反転
        result.localMatrix = MakeAffineAnimationMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
        result.name = node->mName.C_Str(); // Node名を格納
        result.children.resize(node->mNumChildren); // 子供の数だけ確保
        for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
            // 再帰的に読んで階層構造を作っていく
            result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
        }
        return result;
    }
};

struct VertexWeightData {
    float weight;
    uint32_t vertexIndex;
};

struct JointWeightData {
    Matrix4x4 inverseBindPoseMatrix;
    std::vector<VertexWeightData> vertexWeights;
};

struct ModelData {
    std::map<std::string, JointWeightData> skinClusterData;
    std::vector<ModelVertexData> vertices;
    std::vector<uint32_t> indeces;
    ModeldMaterialData material;
    ModelNode rootNode;
};