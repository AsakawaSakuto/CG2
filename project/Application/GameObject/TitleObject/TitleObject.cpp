#include "TitleObject.h"

void TitleObject::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	for (int i = 0; i < model_.size(); i++) {
		model_[i] = std::make_unique<Model>();
	}
	model_[0]->Initialize(dxCommon_, "Machine/Body.obj");
	model_[1]->Initialize(dxCommon_, "Machine/ArmL_01.obj");
	model_[2]->Initialize(dxCommon_, "Machine/ArmL_02.obj");
	model_[3]->Initialize(dxCommon_, "Machine/ArmR_01.obj");
	model_[4]->Initialize(dxCommon_, "Machine/ArmR_02.obj");
	model_[5]->Initialize(dxCommon_, "player/head/head.obj");
	model_[6]->Initialize(dxCommon_, "player/body/body.obj");
	model_[7]->Initialize(dxCommon_, "player/arm_L/arm_L.obj");
	model_[8]->Initialize(dxCommon_, "player/arm_R/arm_R.obj");
	model_[9]->Initialize(dxCommon_, "player/Leg_L/Leg_L.obj");
	model_[10]->Initialize(dxCommon_, "player/Leg_R/Leg_R.obj");

	transform_[0].scale = { 3.0f,3.0f,3.0f };
	transform_[0].rotate = { 0.0f,3.21f,0.0f };
	transform_[0].translate = { 3.0f, -10.0f, 0.0f };

	transform_[1].scale = { 3.0f,2.0f,3.0f };
	transform_[1].rotate = { 0.0f,0.0f,0.0f };
	transform_[1].translate = { 2.6f, -10.0f, 0.0f };

	transform_[2].scale = { 3.0f,3.0f,5.0f };
	transform_[2].rotate = { 0.0f,0.0f,-1.57f };
	transform_[2].translate = { 4.71f,-10.06f,0.0f };

	transform_[3].scale = { 3.0f,2.0f,3.0f };
	transform_[3].rotate = { 0.0f,0.0f,0.0f };
	transform_[3].translate = { 3.4f, -10.0f, 0.0f };

	transform_[4].scale = { 3.0f,3.0f,5.0f };
	transform_[4].rotate = { 0.0f,0.0f,1.57f };
	transform_[4].translate = { 1.3f, -10.06f, 0.0f };
}

void TitleObject::Update() {
	// モデルに座標情報を反映

	if (playerStartTimer_.IsActive()) {
		transform_[0].translate.y = Easing::Lerp(-10.0f, 0.0f, playerStartTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[1].translate.y = Easing::Lerp(-10.0f, 0.0f, playerStartTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[2].translate.y = Easing::Lerp(-10.0f, -0.06f, playerStartTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[3].translate.y = Easing::Lerp(-10.0f, 0.0f, playerStartTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[4].translate.y = Easing::Lerp(-10.0f, -0.06f, playerStartTimer_.GetProgress(), Easing::Type::EaseOutBack);
	}

	model_[0]->SetTransform(transform_[0]);
	model_[1]->SetTransform(transform_[1]);
	model_[2]->SetTransform(transform_[2]);
	model_[3]->SetTransform(transform_[3]);
	model_[4]->SetTransform(transform_[4]);
	for (int i = 0; i < model_.size(); i++) {
		//model_[i]->SetTransform(transform_[i]);
		model_[i]->Update();
	}

	playerStartTimer_.Update();
}

void TitleObject::Draw(Camera& useCamera) {
	for (int i = 0; i < model_.size(); i++) {
		model_[i]->Draw(useCamera);
	}
}

void TitleObject::DrawImGui() {
	ImGui::Begin("TitleObject");
	for (int i = 0; i < model_.size(); i++) {
		std::string label = "Model " + std::to_string(i);
		model_[i]->DrawImGui(label.c_str());
	}
	ImGui::End();
}