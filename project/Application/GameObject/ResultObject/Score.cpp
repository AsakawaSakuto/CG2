#include "Score.h"

void Score::Initialize(DirectXCommon* dxCommon,float score) {
	dxCommon_ = dxCommon;
	score_ = score;
	// 数字モデルの初期化
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i] = make_unique<Model>();
	}
	textModel_[0]->Initialize(dxCommon_, "resultLogo/a.obj");
	textModel_[1]->Initialize(dxCommon_, "resultLogo/tu.obj");
	textModel_[2]->Initialize(dxCommon_, "resultLogo/me.obj");
	textModel_[3]->Initialize(dxCommon_, "resultLogo/ta.obj");
	textModel_[4]->Initialize(dxCommon_, "resultLogo/o.obj");
	textModel_[5]->Initialize(dxCommon_, "resultLogo/ka.obj");
	textModel_[6]->Initialize(dxCommon_, "resultLogo/si.obj");
	textModel_[7]->Initialize(dxCommon_, "resultLogo/ha.obj");
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->SetTexture("resources/image/0.png");
	}

	for (int i = 0; i < textTransform_.size(); i++) {
		textTransform_[i].scale = { 0.5f,0.5f,0.5f };
		textTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	textTransform_[0].translate = { -7.0f,4.5f,0.0f };
	textTransform_[1].translate = { -6.35f,4.5f,0.0f };
	textTransform_[2].translate = { -5.65f,4.5f,0.0f };
	textTransform_[3].translate = { -4.94f,4.5f,0.0f };
	textTransform_[4].translate = { -4.21f,4.5f,0.0f };
	textTransform_[5].translate = { -3.52f,4.5f,0.0f };
	textTransform_[6].translate = { -2.73f,4.5f,0.0f };
	textTransform_[7].translate = { -2.15f,4.5f,0.0f };

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i] = make_unique<Model>();
	}
	scoreModel_[0]->Initialize(dxCommon_, "number/0.obj");
	scoreModel_[1]->Initialize(dxCommon_, "number/1.obj");
	scoreModel_[2]->Initialize(dxCommon_, "number/2.obj");
	scoreModel_[3]->Initialize(dxCommon_, "number/3.obj");
	scoreModel_[4]->Initialize(dxCommon_, "number/4.obj");
}

void Score::Update() {
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->SetTransform(textTransform_[i]);
		textModel_[i]->Update();
	}
}

void Score::Draw(Camera camera) {
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->Draw(camera);
	}
}

void Score::DrawImGui() {
	ImGui::Begin("text");

	ImGui::DragFloat3("t0", &textTransform_[0].translate.x,0.01f);
	ImGui::DragFloat3("t1", &textTransform_[1].translate.x,0.01f);
	ImGui::DragFloat3("t2", &textTransform_[2].translate.x,0.01f);
	ImGui::DragFloat3("t3", &textTransform_[3].translate.x,0.01f);
	ImGui::DragFloat3("t4", &textTransform_[4].translate.x,0.01f);
	ImGui::DragFloat3("t5", &textTransform_[5].translate.x,0.01f);
	ImGui::DragFloat3("t6", &textTransform_[6].translate.x,0.01f);
	ImGui::DragFloat3("t7", &textTransform_[7].translate.x,0.01f);

	ImGui::End();
}