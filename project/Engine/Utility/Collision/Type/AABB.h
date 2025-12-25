#pragma once
#include "Math/Type/Vector3.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

struct AABB {
	Vector3 center = { 0.0f, 0.0f, 0.0f }; // 中心点
	Vector3 min = { -0.5f, -0.5f, -0.5f }; // 最小点（ローカルオフセット）
	Vector3 max = { 0.5f, 0.5f, 0.5f };    // 最大点（ローカルオフセット）

	/// <summary>
	/// ワールド空間での最小点を取得
	/// </summary>
	Vector3 GetWorldMin() const {
		return center + min;
	}

	/// <summary>
	/// ワールド空間での最大点を取得
	/// </summary>
	Vector3 GetWorldMax() const {
		return center + max;
	}

#ifdef USE_IMGUI
	/// <summary>
	/// AABBの値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {

		ImGui::Begin(name);

		ImGui::DragFloat3("center", &center.x, 0.01f);
		ImGui::DragFloat3("min", &min.x, 0.01f);
		ImGui::DragFloat3("max", &max.x, 0.01f);
		
		if (ImGui::Button("Reset")) {
			center = { 0.0f, 0.0f, 0.0f };
			min = { -0.5f, -0.5f, -0.5f };
			max = { 0.5f, 0.5f, 0.5f };
		}

		ImGui::End();
	}
#endif
};