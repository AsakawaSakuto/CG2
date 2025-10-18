#include "TitleScene.h"

void TitleScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

TitleScene::~TitleScene() {
	CleanupResources();
}

void TitleScene::CleanupResources() {
	titleLogo_.reset();
	playUI_.reset();
	optionUI_.reset();
	cursolUI_.reset();
	sceneFade_.reset();
	titleTimer_.Reset();
	cursolTimer_.Reset();
}

void TitleScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	debugCamera_->SetPosition({ 0.0f, 0.0f, -15.0f });
	normalCamera_->SetPosition({0.0f, 0.0f, -15.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	titleLogo_->Initialize(&ctx_->dxCommon);

	uiAlpha_ = 0.0f;
	titleTimer_.Reset();
	cursolTimer_.Reset();

	playUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/PlayUI.png");
	playUI_->SetPosition({ 340.0f,445.0f });
	playUI_->SetScale({ 0.6f,0.6f });
	playUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	optionUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/optionUI.png");
	optionUI_->SetPosition({ 390.0f,565.0f });
	optionUI_->SetScale({ 0.6f,0.6f });
	optionUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	cursolUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/cursol.png");
	cursolUI_->SetPosition({ 210.0f,446.0f });
	cursolUI_->SetScale({ 0.3f,0.3f });
	cursolUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
}

void TitleScene::Update() {
	
	if (selectMenu_ == PLAY) {
		if (input_->TriggerKey(DIK_SPACE) && titleTimer_.IsFinished() ) {
			sceneFade_->StartFadeIn(1.0f);
		}
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(SCENE::GAME); // シーン切り替え
	}

	TitleLogoUiUpdate();
	MenuSelectUiUpdate();

	titleTimer_.Update();
	cursolTimer_.Update();

	sceneFade_->Update();
	titleLogo_->Update();
	playUI_->Update();
	optionUI_->Update();
	cursolUI_->Update();

	// カメラ切り替え&更新
	CameraController();
}

void TitleScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	titleLogo_->Draw(*useCamera_);

	sceneFade_->Draw();

	// UIの描画を有効化
	playUI_->Draw();
	optionUI_->Draw();
	cursolUI_->Draw();

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

	//titleLogo_->DrawImGui();

	debugCamera_->DrawImgui();

	//playUI_->DrawImGui("play");
	//optionUI_->DrawImGui("option");
	//cursolUI_->DrawImGui("ya");

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void TitleScene::CameraController() {
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

void TitleScene::TitleLogoUiUpdate() {
	// TitleLogoの演出が終了し、まだタイマーが開始されていない場合のみ開始
	if (titleLogo_->IsEnd() && !titleTimer_.IsActive() && !titleTimer_.IsFinished()) {
		titleTimer_.Start(2.0f, false);
	}
	// UIの透明度制御を修正
	if (titleTimer_.IsActive()) {
		// タイマーが動作中の場合、進行度に応じて透明度を上げる（0.0 → 1.0）
		uiAlpha_ = titleTimer_.GetProgress();
	} else if (titleTimer_.IsFinished()) {
		// タイマーが完了している場合は完全に不透明
		uiAlpha_ = 1.0f;
	}

	playUI_->SetColor({ 1.0f,1.0f,1.0f,uiAlpha_ });
	optionUI_->SetColor({ 1.0f,1.0f,1.0f,uiAlpha_ });
	cursolUI_->SetColor({ 1.0f,1.0f,1.0f,uiAlpha_ });
}

void TitleScene::MenuSelectUiUpdate() {
	// メニュー選択処理
	if (!cursolTimer_.IsActive() && titleTimer_.IsFinished()) {
		if (selectMenu_ == PLAY) {
			if (input_->TriggerKey(DIK_DOWN)) {
				selectMenu_ = OPTION;
				cursolTimer_.Start(0.25f, false);
			}
		} else if (selectMenu_ == OPTION) {
			if (input_->TriggerKey(DIK_UP)) {
				selectMenu_ = PLAY;
				cursolTimer_.Start(0.25f, false);
			}
		}
	}

	// カーソル位置の更新
	if (cursolTimer_.IsActive()) {
		// タイマーがアクティブな場合
		if (selectMenu_ == PLAY) {
			// OPTIONからPLAYへの移動
			float y = Easing::Lerp(563.0f, 446.0f, cursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			cursolUI_->SetPosition({ 210.0f, y });
		} else if (selectMenu_ == OPTION) {
			// PLAYからOPTIONへの移動
			float y = Easing::Lerp(446.0f, 563.0f, cursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			cursolUI_->SetPosition({ 210.0f, y });
		}
	} else {
		// タイマーが非アクティブな場合
		if (selectMenu_ == PLAY) {
			cursolUI_->SetPosition({ 210.0f, 446.0f });
		} else if (selectMenu_ == OPTION) {
			cursolUI_->SetPosition({ 210.0f, 563.0f });
		}
	}
}