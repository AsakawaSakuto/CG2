#include "ResultScene.h"

ResultScene::~ResultScene() {
	// シーンを抜ける際にランキングデータを保存
	if (score_) {
		score_->SaveRankingData();
	}
}

void ResultScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
	lastScore_ = ctx_->lastScore;
}

void ResultScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 2.0f, -20.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	score_->Initialize(&ctx_->dxCommon, 98765);
	score_->SetInput(&ctx_->input, &ctx_->gamePad);

	// mask の初期化 - 開始位置と終了位置を適切に設定
	mask_->Initialize(&ctx_->dxCommon, "resources/image/mask.png", { 640.0f,360.0f }, { 1.0f,1.0f });
	
	// マスクのアニメーション設定 - 画面外から中央へ
	maskStartPos_ = { 640.0f, 360.0f };  // 画面上部から開始
	maskEndPos_ = { 640.0f, 360.0f };     // 画面中央で終了
	maskStartScale_ = { 20.0f, 20.0f };   // 大きなスケールから開始
	maskEndScale_ = { 1.0f, 1.0f };       // 通常サイズで終了

	maskInTimer_.Start(1.0f, false);
	maskOutTimer_.Reset();

	timerStarte_ = false;
}

void ResultScene::Update() {

	if (score_->GoTitle() && !maskOutTimer_.IsActive() && !timerStarte_) {
		nextScene_ = 0;
		timerStarte_ = true;
		maskOutTimer_.Start(1.0f, false);
	}

	if (score_->GoResult() && !maskOutTimer_.IsActive() && !timerStarte_) {
		nextScene_ = 1;
		timerStarte_ = true;
		maskOutTimer_.Start(1.0f, false);
	}

	if (maskOutTimer_.IsFinished()) {
		if (nextScene_ == 0) {
			ChangeScene(TITLE);
		} else {
			ChangeScene(GAME);
		}
	}

	if (maskInTimer_.IsActive()) {
		mask_->SetPosition({
		Easing::Lerp(maskEndPos_.x,maskStartPos_.x,maskInTimer_.GetProgress(),Easing::Type::EaseInSine),
		Easing::Lerp(maskEndPos_.y,maskStartPos_.y,maskInTimer_.GetProgress(),Easing::Type::EaseInSine) });

		mask_->SetScale({
			Easing::Lerp(maskEndScale_.x,maskStartScale_.x,maskInTimer_.GetProgress(),Easing::Type::EaseInSine),
			Easing::Lerp(maskEndScale_.y,maskStartScale_.y,maskInTimer_.GetProgress(),Easing::Type::EaseInSine) });

	} else {
		if (maskInTimer_.IsFinished()) {
			mask_->SetPosition(maskStartPos_);
			mask_->SetScale(maskStartScale_);
		}
	}

	if (maskOutTimer_.IsActive()) {
		mask_->SetPosition({
		Easing::Lerp(maskStartPos_.x,maskEndPos_.x,maskOutTimer_.GetProgress(),Easing::Type::EaseInSine),
		Easing::Lerp(maskStartPos_.y,maskEndPos_.y,maskOutTimer_.GetProgress(),Easing::Type::EaseInSine) });

		mask_->SetScale({
			Easing::Lerp(maskStartScale_.x,maskEndScale_.x,maskOutTimer_.GetProgress(),Easing::Type::EaseInSine),
			Easing::Lerp(maskStartScale_.y,maskEndScale_.y,maskOutTimer_.GetProgress(),Easing::Type::EaseInSine) });

	} else {
		if (maskOutTimer_.IsFinished()) {
			mask_->SetPosition(maskEndPos_);
			mask_->SetScale(maskEndScale_);
		} else {
			mask_->SetPosition(maskStartPos_);
			mask_->SetScale(maskStartScale_);
		}
	}

	maskInTimer_.Update();
	maskOutTimer_.Update();

	score_->Update();

	mask_->Update();

	CameraController();
}

void ResultScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	score_->Draw(*useCamera_);

	//sceneFade_->Draw();

	mask_->Draw();

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

	debugCamera_->DrawImgui();

	score_->DrawImGui();

	mask_->DrawImGui("mask");

	DrawSceneName();

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