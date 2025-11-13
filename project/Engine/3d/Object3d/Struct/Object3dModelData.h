#pragma once
#include"Object3dMaterialData.h"
#include"Object3dVertexData.h"
#include"Object3dNode.h"
#include <vector>

struct ModelData {
	std::vector<ModelVertexData> vertices;
	ModeldMaterialData material;
	ModelNode rootNode;
};