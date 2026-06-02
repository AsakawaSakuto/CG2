#pragma once
#include <vector>
#include <cstdint>

#include "Math/Type/Vector2.h"
#include "Math/Type/Vector3.h"

/// <summary>
/// XY平面上のリング（ドーナツ形）プリミティブを生成するクラス。
///
/// テクスチャサンプラーの注意:
///   UV の V 方向（0.0 = 外径, 1.0 = 内径）が半径方向に対応する。
///   中心部に意図しない白い丸（アーティファクト）が出るのを防ぐため、
///   サンプラーの AddressV には必ず CLAMP を設定すること。
///     例) staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
/// </summary>
class Ring {
public:
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct Vertex {
		Vector3 pos;
		Vector2 uv;
	};

	Ring()  = default;
	~Ring() = default;

	/// <summary>
	/// パラメータを設定して頂点データを生成する
	/// </summary>
	/// <param name="divideCount">分割数。大きいほど滑らかな円になる</param>
	/// <param name="outerRadius">外径（外側の円の半径）</param>
	/// <param name="innerRadius">内径（内側の円の半径）</param>
	void Create(uint32_t divideCount, float outerRadius, float innerRadius);

	/// <summary>
	/// 生成された頂点リストを取得する
	/// </summary>
	const std::vector<Vertex>& GetVertices() const { return vertices_; }

	/// <summary>
	/// 生成されたインデックスリストを取得する
	/// </summary>
	const std::vector<uint32_t>& GetIndices() const { return indices_; }

	uint32_t GetDivideCount()  const { return divideCount_; }
	float    GetOuterRadius()  const { return outerRadius_; }
	float    GetInnerRadius()  const { return innerRadius_; }

private:
	uint32_t divideCount_ = 32;
	float    outerRadius_ = 1.0f;
	float    innerRadius_ = 0.2f;

	std::vector<Vertex>   vertices_;
	std::vector<uint32_t> indices_;
};
