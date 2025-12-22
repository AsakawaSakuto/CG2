#include "LineManager.h"
#include <variant>

LineManager* LineManager::GetInstance() {
	static LineManager instance;
	return &instance;
}

void LineManager::Initialize() {
	line_ = std::make_unique<Line3d>();
	line_->Initialize();
}

void LineManager::AddShape(const Shape& shape, const Vector4& color) {

	// std::visit を使って型に応じた処理を実行
	std::visit([this, &color](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, AABB>) {
			line_->AddBox(arg, color);
		} else if constexpr (std::is_same_v<T, OBB>) {
			line_->AddBox(arg, color);
		} else if constexpr (std::is_same_v<T, Sphere>) {
			line_->AddSphere(arg, color);
		} else if constexpr (std::is_same_v<T, OvalSphere>) {
			line_->AddOvalSphere(arg, color);
		} else if constexpr (std::is_same_v<T, Plane>) {
			line_->AddPlane(arg, 10, color); // divisions = 10 をデフォルト
		} else if constexpr (std::is_same_v<T, Segment>) {
			line_->AddSegment(arg, color);
		} else if constexpr (std::is_same_v<T, Line>) {
			line_->AddLine(arg, color);
		} else if constexpr (std::is_same_v<T, Circle>) {
			line_->AddCircle(arg, color);
		}
		}, shape);
}

void LineManager::AddGrid(float size, int divisions, const Vector4& color) {
	line_->AddGrid(size, divisions, color);
}

void LineManager::Draw(Camera& camera) {
#ifdef USE_IMGUI
	// 描画
	line_->Draw(camera);
#endif // USE_IMGUI

}
