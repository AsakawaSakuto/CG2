#pragma once
#include "Math/Type/Vector2.h"
#include "Math/Type/Vector4.h"
#include "Math/Type/Matrix4x4.h"

/// <summary>
/// Sprite専用の頂点データ（2D用、Normalなし）
/// </summary>
struct SpriteVertexData {
	Vector4 position;
	Vector2 texcoord;
};

/// <summary>
/// Sprite専用のマテリアルデータ（ライティングなし）
/// </summary>
struct SpriteMaterial {
	Vector4 color;
	Matrix4x4 uvTransformMatrix;
};

/// <summary>
/// Sprite専用の変換行列（WVPのみ）
/// </summary>
struct SpriteTransformationMatrix {
	Matrix4x4 WVP;
};
