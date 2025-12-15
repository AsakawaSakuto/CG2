#pragma once
#include "Math/Type/Vector3.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

// 線分
struct Line {

	Vector3 start; // 始点
	Vector3 end;   // 終点

#ifdef USE_IMGUI
	/// <summary>
	/// 線分の値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat3("start", &start.x, 0.01f);
		ImGui::DragFloat3("end", &end.x, 0.01f);

		if (ImGui::Button("Reset")) {
			start = { 0.0f,0.0f,0.0f };
			end = { 0.0f,0.0f,1.0f };
		}

		ImGui::End();
	}
#endif
};