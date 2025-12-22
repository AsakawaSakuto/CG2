#pragma once
#include "Math/Type/Vector3.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

// 平円を表す構造体
struct Circle {
	Vector3 center; // 平円の中心座標
	float radius;   // 平円の半径
	Vector3 normal; // 平円の法線

#ifdef USE_IMGUI
	/// <summary>
	/// 平面の値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat3("center", &center.x, 0.01f);
		ImGui::DragFloat3("normal", &normal.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("size", &radius, 0.01f);

		if (ImGui::Button("Reset")) {
			normal = { 0.0f,1.0f,0.0f };
			center = { 0.0f,0.0f,0.0f };
			radius = 1.0f;
		}

		ImGui::End();
	}
#endif
};