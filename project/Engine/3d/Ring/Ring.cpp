#include "3d/Ring/Ring.h"

#include <numbers>
#include <cmath>

void Ring::Create(uint32_t divideCount, float outerRadius, float innerRadius) {
	divideCount_ = divideCount;
	outerRadius_ = outerRadius;
	innerRadius_ = innerRadius;

	vertices_.clear();
	indices_.clear();

	// 1分割あたりのラジアン
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / static_cast<float>(divideCount);

	// 各分割ごとに4頂点・6インデックス（四角形＝三角形×2）を追加する
	// 頂点インデックス配置:
	//   ① 外側（現在） ... uv(u,    0.0f)
	//   ② 外側（次）   ... uv(uNext, 0.0f)
	//   ③ 内側（現在） ... uv(u,    1.0f)
	//   ④ 内側（次）   ... uv(uNext, 1.0f)
	//
	// 三角形: ①②③ と ③②④ （時計回り）
	for (uint32_t index = 0; index < divideCount; ++index) {
		const float sin     = std::sin(static_cast<float>(index)      * radianPerDivide);
		const float cos     = std::cos(static_cast<float>(index)      * radianPerDivide);
		const float sinNext = std::sin(static_cast<float>(index + 1)  * radianPerDivide);
		const float cosNext = std::cos(static_cast<float>(index + 1)  * radianPerDivide);

		const float u     = static_cast<float>(index)     / static_cast<float>(divideCount);
		const float uNext = static_cast<float>(index + 1) / static_cast<float>(divideCount);

		// ① 外側（現在）
		vertices_.push_back({ { -sin     * outerRadius, cos     * outerRadius, 0.0f }, { u,     0.0f } });
		// ② 外側（次）
		vertices_.push_back({ { -sinNext * outerRadius, cosNext * outerRadius, 0.0f }, { uNext, 0.0f } });
		// ③ 内側（現在）
		vertices_.push_back({ { -sin     * innerRadius, cos     * innerRadius, 0.0f }, { u,     1.0f } });
		// ④ 内側（次）
		vertices_.push_back({ { -sinNext * innerRadius, cosNext * innerRadius, 0.0f }, { uNext, 1.0f } });

		// 時計回りに三角形を2つ構成して四角形（ポリゴン）を形成する
		const uint32_t base = index * 4;
		// 三角形1: ①②③
		indices_.push_back(base + 0);
		indices_.push_back(base + 1);
		indices_.push_back(base + 2);
		// 三角形2: ③②④
		indices_.push_back(base + 2);
		indices_.push_back(base + 1);
		indices_.push_back(base + 3);
	}
}
