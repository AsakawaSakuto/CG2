#include"Ranking.h"

static int ranking[3] = { 600, 600, 600 };

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
	top1Num1_->SetPosition({ 205.0f,100.0f });
	top1Num2_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top1Num2_->SetPosition({ 275.0f,100.0f });
	top1Num3_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top1Num3_->SetPosition({ 315.0f,100.0f });
	top1Colon_->Initialize(dxCommon_, "resources/image/number/colon.png", { 32.0f,64.0f });
	top1Colon_->SetPosition({ 240.0f,100.0f });

	top2Num1_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top2Num1_->SetPosition({ 205.0f,220.0f });
	top2Num2_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top2Num2_->SetPosition({ 275.0f,220.0f });
	top2Num3_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top2Num3_->SetPosition({ 315.0f,220.0f });
	top2Colon_->Initialize(dxCommon_, "resources/image/number/colon.png", { 32.0f,64.0f });
	top2Colon_->SetPosition({ 240.0f,220.0f });

	top3Num1_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top3Num1_->SetPosition({ 205.0f,340.0f });
	top3Num2_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top3Num2_->SetPosition({ 275.0f,340.0f });
	top3Num3_->Initialize(dxCommon_, "resources/image/number/0.png", { 32.0f,64.0f });
	top3Num3_->SetPosition({ 315.0f,340.0f });
	top3Colon_->Initialize(dxCommon_, "resources/image/number/colon.png", { 32.0f,64.0f });
	top3Colon_->SetPosition({ 240.0f,340.0f });

	timer_ = 0.0f;
	seconds_ = 0.0f;
	minutes_ = 0;
	totalSeconds_ = 0;
}

void Ranking::Update() {
	// 経過時間を加算
	timer_ += deltaTime_;
	seconds_ += deltaTime_;

	// 秒を整数に変換（最大9分59秒＝599秒まで）
	totalSeconds_ = static_cast<int>(seconds_);
	if (totalSeconds_ >= 599) {
		totalSeconds_ = 599;
	}

	// 分と秒を正しく計算
	int minutes = totalSeconds_ / 60;
	int seconds = totalSeconds_ % 60;
	int secondsTens = seconds / 10;
	int secondsOnes = seconds % 10;

	// テクスチャ設定
	timerNum1_->SetTexture(numberTextures[minutes]);
	timerNum2_->SetTexture(numberTextures[secondsTens]);
	timerNum3_->SetTexture(numberTextures[secondsOnes]);

	// 60秒経過で分を加算する処理は不要なので削除

	// 更新
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
	if (ranking[0] == 600) {
		top1Num1_->SetTexture("resources/image/number/haifun.png");
		top1Num2_->SetTexture("resources/image/number/haifun.png");
		top1Num3_->SetTexture("resources/image/number/haifun.png");
	} else {
		int minutes = ranking[0] / 60;
		int seconds = ranking[0] % 60;
		int secondsTens = seconds / 10;
		int secondsOnes = seconds % 10;

		top1Num1_->SetTexture(numberTextures[minutes]);
		top1Num2_->SetTexture(numberTextures[secondsTens]);
		top1Num3_->SetTexture(numberTextures[secondsOnes]);
	}

	top1Num1_->Update();
	top1Num2_->Update();
	top1Num3_->Update();
	top1Colon_->Update();
	
	if (ranking[1] == 600) {
		top2Num1_->SetTexture("resources/image/number/haifun.png");
		top2Num2_->SetTexture("resources/image/number/haifun.png");
		top2Num3_->SetTexture("resources/image/number/haifun.png");
	} else {
		int minutes = ranking[1] / 60;
		int seconds = ranking[1] % 60;
		int secondsTens = seconds / 10;
		int secondsOnes = seconds % 10;

		top2Num1_->SetTexture(numberTextures[minutes]);
		top2Num2_->SetTexture(numberTextures[secondsTens]);
		top2Num3_->SetTexture(numberTextures[secondsOnes]);
	}

	top2Num1_->Update();
	top2Num2_->Update();
	top2Num3_->Update();
	top2Colon_->Update();

	if (ranking[2] == 600) {
		top3Num1_->SetTexture("resources/image/number/haifun.png");
		top3Num2_->SetTexture("resources/image/number/haifun.png");
		top3Num3_->SetTexture("resources/image/number/haifun.png");
	} else {
		int minutes = ranking[2] / 60;
		int seconds = ranking[2] % 60;
		int secondsTens = seconds / 10;
		int secondsOnes = seconds % 10;

		top3Num1_->SetTexture(numberTextures[minutes]);
		top3Num2_->SetTexture(numberTextures[secondsTens]);
		top3Num3_->SetTexture(numberTextures[secondsOnes]);
	}

	top3Num1_->Update();
	top3Num2_->Update();
	top3Num3_->Update();
	top3Colon_->Update();
}

void Ranking::DrawRanking() {
	top1Num1_->Draw();
	top1Num2_->Draw();
	top1Num3_->Draw();
	top1Colon_->Draw();

	top2Num1_->Draw();
	top2Num2_->Draw();
	top2Num3_->Draw();
	top2Colon_->Draw();

	top3Num1_->Draw();
	top3Num2_->Draw();
	top3Num3_->Draw();
	top3Colon_->Draw();
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

}