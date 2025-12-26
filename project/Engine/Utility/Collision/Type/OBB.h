#pragma once
#include "Math/Type/Vector3.h"
#include "Math/MatrixFunction/MatrixFunction.h"

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

struct OBB {
	Vector3 center = { 0.0f, 0.0f, 0.0f }; // ワールド中心
	Vector3 min = { -0.5f, -0.5f, -0.5f }; // centerからのローカルオフセット（回転前のローカル空間）
	Vector3 max = { 0.5f, 0.5f, 0.5f };    // centerからのローカルオフセット（回転前のローカル空間）
	Vector3 rotate = { 0.0f, 0.0f, 0.0f }; // 回転角
	Vector3 orientation[3];                // 座標軸 正規化 直交必須

	/// <summary>
	/// 回転角から座標軸を更新
	/// 注意: rotateが変更されたら必ずこの関数を呼ぶこと
	/// </summary>
	void UpdateOrientation() {
		Matrix4x4 rotateMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);

		// 回転行列から軸を抽出
		orientation[0].x = rotateMatrix.m[0][0];
		orientation[0].y = rotateMatrix.m[0][1];
		orientation[0].z = rotateMatrix.m[0][2];

		orientation[1].x = rotateMatrix.m[1][0];
		orientation[1].y = rotateMatrix.m[1][1];
		orientation[1].z = rotateMatrix.m[1][2];

		orientation[2].x = rotateMatrix.m[2][0];
		orientation[2].y = rotateMatrix.m[2][1];
		orientation[2].z = rotateMatrix.m[2][2];
	}

#ifdef USE_IMGUI
	/// <summary>
	/// OBBの値をImGuiで編集する
	/// </summary>
	/// <param name="name">適切な名前を入力</param>
	void DrawImGui(const char* name) {

		UpdateOrientation();

		ImGui::Begin(name);

		ImGui::DragFloat3("center", &center.x, 0.01f);
		ImGui::DragFloat3("min", &min.x, 0.01f);
		ImGui::DragFloat3("max", &max.x, 0.01f);
		ImGui::DragFloat3("rotate", &rotate.x, 0.01f);

		if (ImGui::Button("Reset")) {
			center = { 0.0f, 0.0f, 0.0f };
			min = { -0.5f, -0.5f, -0.5f };
			max = { 0.5f, 0.5f, 0.5f };
			rotate = { 0.0f, 0.0f, 0.0f };
			UpdateOrientation();
		}

		ImGui::End();
	}
#endif
};