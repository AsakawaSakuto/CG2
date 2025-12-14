#pragma once
#include "Math/Type/Vector3.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

// 平面を表す構造体
struct Plane {
	Vector3 normal;   // 平面の法線
	float distance;   // 原点からの距離

#ifdef USE_IMGUI
	/// <summary>
	/// 平面の値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat3("normal", &normal.x, 0.01f);
		ImGui::DragFloat("distance", &distance, 0.01f);

		if (ImGui::Button("Reset")) {
			normal = { 0.0f,1.0f,0.0f };
			distance = 0.0f;
		}

		ImGui::End();
	}
#endif
};