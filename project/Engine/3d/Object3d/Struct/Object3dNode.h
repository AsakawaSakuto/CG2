#pragma once

#include"Matrix4x4.h"
#include <string>
#include <vector>

#include <assimp/scene.h>

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