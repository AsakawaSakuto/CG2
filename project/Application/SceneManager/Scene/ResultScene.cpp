#include "ResultScene.h"

void ResultScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void ResultScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 0.0f, -10.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	// 背景スプライト
	spriteBG_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteBG_->SetPosition({600.0f, 340.0f});
	spriteBG_->SetScale({76, 43});
	spriteBG_->SetColor({0, 0, 0, 1});

	// スコア用のスプライト集
	for (int i = 0; i < spriteNumCollection_.size(); ++i) {
		spriteNumCollection_[i] = "resources/image/number/" + std::to_string(i) + ".png";
	}

	// スコア用スプライト5桁分
	for (int i = 0; i < spriteScore_.size(); ++i) {
		spriteScore_[i] = make_unique<Sprite>();
		spriteScore_[i]->Initialize(&ctx_->dxCommon, "resources/image/number/0.png");
		spriteScore_[i]->SetPosition({500.0f + i * 70.0f, 400.0f});
		spriteScore_[i]->SetScale({1, 1});
	}

	// 描画用のスコア
	drawScore_ = 0.0f;
}

void ResultScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneFade_->StartFadeIn(1.0f);
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(SCENE::TITLE);
	}

	sceneFade_->Update();

	// カメラ切り替え&更新
	CameraController();

	// 背景スプライトの更新処理
	spriteBG_->Update();

	// スコアスプライトの更新処理
	for (int i = 0; i < spriteScore_.size(); ++i) {
		spriteScore_[i]->Update();
	}

	// 描画用のスコアカウント
	ScoreCountUpdate();

	// スプライトの切り替え
	SpriteScoreUpdate();
}

void ResultScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	// 背景スプライトの描画処理
	spriteBG_->Draw();

	// スコアスプライトの更新処理
	for (int i = 0; i < spriteScore_.size(); ++i) {
		spriteScore_[i]->Draw();
	}

	sceneFade_->Draw();

	///
	/// ↑描画処理ここまで
	///

	// フレームの先頭でImguiにここからフレームが始まる旨を告げる
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	///
	/// ↓ImGuiここから
	///

	DrawSceneName();

	/*ImGui::Begin("Test");

	ImGui::DragFloat2("TestPos", &testPos_.x, 1.0f);
	ImGui::DragFloat2("TestScale", &testScale_.x, 1.0f);

	ImGui::End();*/

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void ResultScene::CameraController() {
	if (useDebugCamera_) {
		if (debugCamera_ != nullptr) {
			debugCamera_->Update();
			useCamera_ = debugCamera_.get();
		}
	} else {
		if (normalCamera_ != nullptr) {
			normalCamera_->Update();
			useCamera_ = normalCamera_.get();
		}
	}
}

void ResultScene::ScoreCountUpdate() {
	// 加算
	drawScore_ = drawScore_ + (MAX_SCORE - drawScore_) * COUNT_SPEED;

	// 加算を打ち切り
	if (std::abs(drawScore_ - MAX_SCORE) < 0.01f) {
		drawScore_ = MAX_SCORE;
	}
}

void ResultScene::SpriteScoreUpdate() {
	int displayScore = static_cast<int>(drawScore_);

	for (int i = 0; i < digits.size(); ++i) {
		digits[digits.size() - 1 - i] = displayScore % 10;
		displayScore /= 10;
	}

	for (int i = 0; i < spriteScore_.size(); ++i) {
		int digit = digits[i];
		spriteScore_[i]->SetTexture(spriteNumCollection_[digit]);
	}
}
