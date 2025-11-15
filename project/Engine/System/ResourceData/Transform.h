#pragma once
#include"../Quaternion/Quaternion.h"
#include"Vector3.h"
#include"Vector2.h"

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"
#endif

struct Transform {
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	Vector3 translate = { 0.0f,0.0f,0.0f };

#ifdef USE_IMGUI
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat3("translate", &translate.x, 0.01f);
		ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &scale.x, 0.01f);

		if (ImGui::Button("Reset")) {
			translate = { 0.0f,0.0f,0.0f };
			rotate = { 0.0f,0.0f,0.0f };
			scale = { 1.0f,1.0f,1.0f };
		}

		ImGui::End();
	}
#endif
};

struct Transform2D {
	Vector2 scale = { 1.0f,1.0f };
	float rotate = 0.0f;
	Vector2 translate = { 0.0f,0.0f };
#ifdef USE_IMGUI
	void DrawImGui(const char* name) {
		ImGui::Begin(name);

		ImGui::DragFloat2("translate", &translate.x, 1.0f);
		ImGui::DragFloat("rotate", &rotate, 0.01f);
		ImGui::DragFloat2("scale", &scale.x, 0.01f);

		if (ImGui::Button("Reset")) {
			translate = { 0.0f,0.0f };
			rotate = { 0.0f };
			scale = { 1.0f,1.0f };
		}

		ImGui::End();
	}
#endif
};

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};