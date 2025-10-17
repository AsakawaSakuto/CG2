#include "Text3D.h"
#include <algorithm>

void Text3D::Initialize(DirectXCommon* dxCommon) { 
	dxCommon_ = dxCommon;

	for (int i = 0; i < model_.size(); i++) {
		model_[i] = std::make_unique<Model>();
	}

	model_[0]->Initialize(dxCommon_, "TitleLogo/o.obj");
	model_[1]->Initialize(dxCommon_, "TitleLogo/ka.obj");
	model_[2]->Initialize(dxCommon_, "TitleLogo/si.obj");
	model_[3]->Initialize(dxCommon_, "TitleLogo/a.obj");
	model_[4]->Initialize(dxCommon_, "TitleLogo/tu.obj");
	model_[5]->Initialize(dxCommon_, "TitleLogo/me.obj");
	model_[6]->Initialize(dxCommon_, "TitleLogo/te.obj");
	model_[7]->Initialize(dxCommon_, "TitleLogo/ku.obj");
	model_[8]->Initialize(dxCommon_, "TitleLogo/ma.obj");
	model_[9]->Initialize(dxCommon_, "TitleLogo/sa.obj");
	model_[10]->Initialize(dxCommon_, "TitleLogo/i.obj");
	model_[11]->Initialize(dxCommon_, "TitleLogo/da.obj");

	// Transform配列を初期化
	for (int i = 0; i < transform_.size(); i++) {
		model_[i]->SetTexture("resources/image/0.png");
		transform_[i].rotate = { 0.0f, 0.0f, 0.0f };

		timer_[i].Start(0.25f + (i * 0.15f), false);
		rotateTimer_[i].Start(0.25f + (i * 0.15f), false);
	}

	for (int i = 0; i < 7; i++) {
		transform_[i].scale = { 0.6f, 0.6f, 1.0f };
	}

	for (int i = 7; i < 11; i++) {
		transform_[i].scale = { 0.9f, 0.9f, 1.0f };
	}

	transform_[11].scale = { 0.85f, 0.85f, 1.0f };

	// 各文字の最終位置を設定
	transform_[0].translate = { -5.1f, 2.2f, 0.0f };
	transform_[1].translate = { -4.35f, 2.2f, 0.0f };
	transform_[2].translate = { -3.5f, 2.2f, 0.0f };
	transform_[3].translate = { -2.9f, 2.2f, 0.0f };
	transform_[4].translate = { -2.2f, 2.2f, 0.0f };
	transform_[5].translate = { -1.5f, 2.2f, 0.0f };
	transform_[6].translate = { -0.85f, 2.2f, 0.0f };
	transform_[7].translate = { -5.05f, 1.15f, 0.0f };
	transform_[8].translate = { -4.05f, 1.15f, 0.0f };
	transform_[9].translate = { -2.95f, 1.15f, 0.0f };
	transform_[10].translate = { -1.9f, 1.15f, 0.0f };
	transform_[11].translate = { -1.0f, 1.15f, 0.0f };
}

void Text3D::Update() {
	// 位置のアニメーション（各タイマーの進行度を使用）
	transform_[0].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[0].GetProgress(),Easing::Type::EaseInOutBounce);
	transform_[1].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[1].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[2].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[2].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[3].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[3].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[4].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[4].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[5].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[5].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[6].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[6].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[7].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[7].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[8].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[8].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[9].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[9].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[10].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[10].GetProgress(), Easing::Type::EaseInOutBounce);
	transform_[11].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[11].GetProgress(), Easing::Type::EaseInOutBounce);

	// 全タイマーの更新
	for (int i = 0; i < timer_.size(); i++) {
		timer_[i].Update();
		rotateTimer_[i].Update();
	}

	for (int i = 0; i < model_.size(); i++) {
		// 全ての文字が同じ回転進行度を使用して、最終的にrotate.yを0にする
		//transform_[i].rotate.y = Easing::Lerp(6.45f, 0.0f,rotateTimer_[i].GetProgress());

		model_[i]->SetTransform(transform_[i]);
		model_[i]->Update();
	}
}

void Text3D::Draw(Camera& useCamera) {
	for (int i = 0; i < model_.size(); i++) {
		model_[i]->Draw(useCamera);
	}
}

void Text3D::DrawImGui(){

	ImGui::Begin("titleLogo");

	for (int i = 0; i < transform_.size(); i++) {
		std::string label = "Transform " + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &transform_[i].translate.x, 0.01f);
	}

	ImGui::End();

	ImGui::Begin("titleLogo2");

	for (int i = 0; i < transform_.size(); i++) {
		std::string label = "rotate" + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &transform_[i].rotate.x, 0.01f);
	}

	ImGui::End();

	ImGui::Begin("titleLogo3");

	for (int i = 0; i < transform_.size(); i++) {
		std::string label = "scale" + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &transform_[i].scale.x, 0.01f);
	}

	ImGui::End();

}