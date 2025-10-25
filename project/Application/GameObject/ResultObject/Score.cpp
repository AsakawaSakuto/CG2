#include "Score.h"

void Score::Initialize(DirectXCommon* dxCommon, float score) {
	dxCommon_ = dxCommon;
	score_ = score;

	InitTextModel();
	InitScoreModel();
	InitRankModel();
	InitPlayerModel();
}

void Score::Update() {
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->SetTransform(textTransform_[i]);
		textModel_[i]->Update();
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i]->SetTransform(scoreTransform_[i]);
		scoreModel_[i]->Update();
	}

	for (int i = 0; i < rankModel_.size(); ++i) {
		rankModel_[i]->SetTransform(rankTransform_[i]);
		rankModel_[i]->Update();
	}

	playerModel_->SetTransform(playerTransform_);
	playerModel_->Update();
	machineModel_->SetTransform(machineTransform_);
	machineModel_->Update();
}

void Score::Draw(Camera camera) {
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->Draw(camera);
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i]->Draw(camera);
	}

	for (int i = 0; i < rankModel_.size(); ++i) {
		rankModel_[i]->Draw(camera);
	}

	playerModel_->Draw(camera);
	machineModel_->Draw(camera);
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

	// スコア数字のTransform調整用
	ImGui::Separator();
	ImGui::Text("Score Digits");
	ImGui::DragFloat3("s0", &scoreTransform_[0].translate.x,0.01f);
	ImGui::DragFloat3("s1", &scoreTransform_[1].translate.x,0.01f);
	ImGui::DragFloat3("s2", &scoreTransform_[2].translate.x,0.01f);
	ImGui::DragFloat3("s3", &scoreTransform_[3].translate.x,0.01f);
	ImGui::DragFloat3("s4", &scoreTransform_[4].translate.x,0.01f);

	ImGui::Separator();
	ImGui::Text("rank Digits");
	ImGui::DragFloat3("r0", &rankTransform_[0].translate.x, 0.01f);
	ImGui::DragFloat3("r1", &rankTransform_[1].translate.x, 0.01f);
	ImGui::DragFloat3("rr0", &rankTransform_[0].rotate.x, 0.01f);
	ImGui::DragFloat3("rr1", &rankTransform_[1].rotate.x, 0.01f);
	ImGui::DragFloat3("rs0", &rankTransform_[0].scale.x, 0.01f);
	ImGui::DragFloat3("rs1", &rankTransform_[1].scale.x, 0.01f);

	ImGui::End();

	ImGui::Begin("playerTranslate");

	ImGui::DragFloat3("Pt", &playerTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("Pr", &playerTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Ps", &playerTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Mt", &machineTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("Mr", &machineTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Ms", &machineTransform_.scale.x, 0.01f);

	ImGui::End();
}

void Score::InitTextModel() {
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
	textTransform_[0].translate = { -6.97f,4.5f,0.0f };
	textTransform_[1].translate = { -6.36f,4.5f,0.0f };
	textTransform_[2].translate = { -5.72f,4.5f,0.0f };
	textTransform_[3].translate = { -5.1f,4.5f,0.0f };
	textTransform_[4].translate = { -4.5f,4.5f,0.0f };
	textTransform_[5].translate = { -3.91f,4.5f,0.0f };
	textTransform_[6].translate = { -3.22f,4.5f,0.0f };
	textTransform_[7].translate = { -2.72f,4.5f,0.0f };
}

void Score::InitScoreModel() {
	int scoreInt = static_cast<int>(score_);
	std::array<int, 5> digits = { 0, 0, 0, 0, 0 };

	for (int i = 0; i < 5; ++i) {
		digits[i] = scoreInt % 10;
		scoreInt /= 10;
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i] = make_unique<Model>();
		std::string modelPath = "number/" + std::to_string(digits[i]) + ".obj";
		scoreModel_[i]->Initialize(dxCommon_, modelPath);
		scoreModel_[i]->SetTexture("resources/image/0.png");
	}

	for (int i = 0; i < scoreTransform_.size(); i++) {
		scoreTransform_[i].scale = { 1.5f,1.5f,0.5f };
		scoreTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	scoreTransform_[0].translate = { 3.72f,4.75f,0.0f };
	scoreTransform_[1].translate = { 2.36f,4.75f,0.0f };
	scoreTransform_[2].translate = { 0.96f,4.75f,0.0f };
	scoreTransform_[3].translate = { -0.23f,4.75f,0.0f };
	scoreTransform_[4].translate = { -1.5f,4.75f,0.0f };
}

void Score::InitRankModel() {
	rankModel_[0] = make_unique<Model>();
	rankModel_[1] = make_unique<Model>();

	rankModel_[0]->Initialize(dxCommon_, "rank/s.obj");
	rankModel_[1]->Initialize(dxCommon_, "rank/rank.obj");

	rankModel_[0]->SetTexture("resources/image/0.png");
	rankModel_[1]->SetTexture("resources/image/0.png");

	rankTransform_[0].scale = { 2.5f,3.0f,0.5f };
	rankTransform_[1].scale = { 1.0f,1.0f,0.5f };
	rankTransform_[0].rotate = { 0.0f,-0.55f,0.0f };
	rankTransform_[1].rotate = { 0.0f,-0.55f,0.0f };
	rankTransform_[0].translate = { -5.65f,0.3f,0.0f };
	rankTransform_[1].translate = { -3.18f,-0.74f,1.8f };
}

void Score::InitPlayerModel() {
	playerModel_ = make_unique<Model>();
	playerModel_->Initialize(dxCommon_, "resultLogo/player.obj");
	playerTransform_.translate = { 4.4f,1.01f,1.16f };
	playerTransform_.rotate = { 1.35f,3.36f,0.65f };
	playerTransform_.scale = { 3.5f,3.5f,3.5f };
	machineModel_ = make_unique<Model>();
	machineModel_->Initialize(dxCommon_, "resultLogo/machine.obj");
	machineTransform_.translate = { 3.85f,0.67f,-0.06f };
	machineTransform_.rotate = { 1.5f,3.02f,1.11f };
	machineTransform_.scale = { 3.5f,3.5f,3.5f };
}