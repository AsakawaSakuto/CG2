#include"Ranking.h"

static int ranking[3] = { 599, 599, 599 };

void Ranking::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	timerNum1_->Initialize(dxCommon_, "resources/image/number/0.png", { 64.0f,128.0f });
	timerNum1_->SetPosition({ 996.0f,64.0f });
	timerNum2_->Initialize(dxCommon_, "resources/image/number/0.png", { 64.0f,128.0f });
	timerNum2_->SetPosition({ 1136.0f,64.0f });
	timerNum3_->Initialize(dxCommon_, "resources/image/number/0.png", { 64.0f,128.0f });
	timerNum3_->SetPosition({ 1216.0f,64.0f });
	timerColon_->Initialize(dxCommon_, "resources/image/number/colon.png", { 64.0f,128.0f });
	timerColon_->SetPosition({ 1066.0f,64.0f });

	top1Num1_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top1Num2_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top1Num3_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top1Colon_->Initialize(dxCommon_, "resources/image/number/colon.png", { 32.0f,64.0f });

	timer_ = 0.0f;
	seconds_ = 0.0f;
	minutes_ = 0;
	totalSeconds_ = 0;
}

void Ranking::Update() {

	// 経過時間を加算
	timer_ += deltaTime_;
	seconds_ += deltaTime_;

	// 秒を整数に変換
	totalSeconds_ = (int)seconds_;
	if (totalSeconds_ >= 599) {
		totalSeconds_ = 599;
	}

	// 10の位と1の位を取得
	int secondsTens = (totalSeconds_ / 10) % 6; // 0～5
	int secondsOnes = totalSeconds_ % 10;       // 0～9

	// 60秒経過したらリセット
	if (seconds_ >= 60.0f) {
		seconds_ = 0.0f;
		minutes_++;
	}

	if (minutes_ >= 9) {
		minutes_ = 9;
	}

	// 1の位
	timerNum3_->SetTexture(numberTextures[secondsOnes]);
	// 10の位
	timerNum2_->SetTexture(numberTextures[secondsTens]);
	// 分の位
	timerNum1_->SetTexture(numberTextures[minutes_]);

	timerNum1_->Update();
	timerNum2_->Update();
	timerNum3_->Update();
	timerColon_->Update();
}

void Ranking::Draw() {
	timerNum1_->Draw();
	timerNum2_->Draw();
	timerNum3_->Draw();
	timerColon_->Draw();
}

void Ranking::UpdateRanking() {
	int seconds1 = ranking[0] / 60;
	int seconds2 = (ranking[0] / 10) % 6; // 0～5
	int seconds3 = ranking[0] % 10;       // 0～9

	top1Num1_->SetTexture(numberTextures[seconds1]);
	top1Num2_->SetTexture(numberTextures[seconds2]);
	top1Num3_->SetTexture(numberTextures[seconds3]);

	top1Num1_->Update();
	top1Num2_->Update();
	top1Num3_->Update();
	top1Colon_->Update();
}

void Ranking::DrawRanking() {
	top1Num1_->Draw();
	top1Num2_->Draw();
	top1Num3_->Draw();
	top1Colon_->Draw();
}

void Ranking::IsEndGame() {
	// totalSeconds_ をランキングに入れる
	ranking[2] = totalSeconds_;

	// 昇順ソート（バブルソート）
	for (int i = 0; i < 3 - 1; i++) {
		for (int j = i + 1; j < 3; j++) {
			if (ranking[i] > ranking[j]) {
				int temp = ranking[i];
				ranking[i] = ranking[j];
				ranking[j] = temp;
			}
		}
	}
}

void Ranking::DrawImGui() {
	ImGui::Begin("ranking");
	ImGui::Text("ran0=%d", ranking[0]);
	ImGui::Text("ran1=%d", ranking[1]);
	ImGui::Text("ran2=%d", ranking[2]);
	ImGui::End();

	top1Num1_->DrawImGui("1");
	top1Num2_->DrawImGui("2");
	top1Num3_->DrawImGui("3");
	top1Colon_->DrawImGui("colon");
}