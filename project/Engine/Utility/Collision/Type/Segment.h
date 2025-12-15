#pragma once
#include "Math/Type/Vector3.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

// 線分
struct Segment {

	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル

#ifdef USE_IMGUI
	/// <summary>
	/// 線分の値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat3("origin", &origin.x, 0.01f);
		ImGui::DragFloat3("diff", &diff.x, 0.01f);

		if (ImGui::Button("Reset")) {
			origin = { 0.0f,0.0f,0.0f };
			diff = { 0.0f,0.0f,1.0f };
		}

		ImGui::End();
	}
#endif
};