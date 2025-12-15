#pragma once
#include "Math/Type/Vector3.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

// 球体
struct Sphere {
	Vector3 center = { 0.0f,0.0f,0.0f }; // 中心点
	float radius = 1.0f;                 // 半径

#ifdef USE_IMGUI
	/// <summary>
	/// 球体の値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat3("center", &center.x, 0.01f);
		ImGui::DragFloat("radius", &radius, 0.01f);

		if (ImGui::Button("Reset")) {
			center = { 0.0f,0.0f,0.0f };
			radius = 1.0f;
		}

		ImGui::End();
	}
#endif
};