#pragma once
#include "Vector3.h"

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"
#endif

struct AABB {
	Vector3 center; // 中心点
	Vector3 size;   // 各辺の長さ

#ifdef USE_IMGUI
	/// <summary>
	/// AABBの値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {

		ImGui::Begin(name);

		ImGui::DragFloat3("center", &center.x, 0.01f);
		ImGui::DragFloat3("size", &size.x, 0.01f);
		
		if (ImGui::Button("Reset")) {
			center = { 0.0f,0.0f,0.0f };
			size = { 1.0f,1.0f,1.0f };
		}

		ImGui::End();
	}
#endif
};