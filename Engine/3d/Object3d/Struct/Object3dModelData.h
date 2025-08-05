#pragma once
#include"Object3dMaterialData.h"
#include"Object3dVertexData.h"
#include"Object3dNode.h"
#include <vector>

struct Object3dModelData {
	std::vector<Object3dVertexData> vertices;
	Object3dMaterialData material;
	Object3dNode rootNode;
};