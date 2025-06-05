#pragma once
#include"MaterialData.h"
#include"VertexData.h"
#include <vector>

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};