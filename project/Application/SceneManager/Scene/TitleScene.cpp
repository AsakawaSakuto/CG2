#include "TitleScene.h"
#include "MasterVolume.h"

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
	titleObject_->Initialize(&ctx_->dxCommon);

	uiAlpha_ = 0.0f;
	titleTimer_.Reset();
	cursolTimer_.Reset();

	playUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/PlayUI.png", { 340.0f,445.0f }, { 0.6f,0.6f });
	playUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	optionUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/optionUI.png",{ 390.0f,565.0f }, { 0.6f,0.6f });
	optionUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	cursolUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/cursol.png", { 210.0f,446.0f }, { 0.3f,0.3f });
	cursolUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	optionBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/optionBG.png", { 604.0f,340.0f }, { 0.0f,0.0f });
	optionBG_->SetColor({ 0.0f,0.0f,0.0f,0.8f });

	optionCursolUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/cursol.png", { 150.0f,156.0f }, { 0.0f,0.0f });
	optionCursolUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	fullScreenUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/fullscreenModeUI.png", { 791.0f,151.0f }, { 0.4f,0.4f });
	onUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/onUI.png", { 1068.0f,151.0f }, { 0.4f,0.4f });
	offUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/offUI.png", { 1070.0f,151.0f }, { 0.4f,0.4f });
	seUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/seUI.png", { 656.0f,237.0f }, { 0.4f,0.4f });
	bgmUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/bgmUI.png", { 670.0f,323.0f }, { 0.4f,0.4f });
	daiUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/daiUI.png", { 1069.0f,235.0f }, { 0.4f,0.4f });
	dai2UI_->Initialize(&ctx_->dxCommon, "resources/image/UI/daiUI.png", { 1069.0f,321.0f }, { 0.4f,0.4f });
	tyuUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/tyuUI.png", { 1070.0f,235.0f }, { 0.4f,0.4f });
	tyu2UI_->Initialize(&ctx_->dxCommon, "resources/image/UI/tyuUI.png", { 1070.0f,321.0f }, { 0.4f,0.4f });
	syouUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/syouUI.png", { 1068.0f,236.0f }, { 0.4f,0.4f });
	syou2UI_->Initialize(&ctx_->dxCommon, "resources/image/UI/syouUI.png", { 1068.0f,321.0f }, { 0.4f,0.4f });
	backUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/backUI.png", { 674.0f,403.0f }, { 0.4f,0.4f });

	startGameSE_->Initialize("resources/sound/startGameSE.mp3");
	moveCursolSE_->Initialize("resources/sound/moveCursolSE.mp3");
	decideSE_->Initialize("resources/sound/DecideSE.mp3");
	titleSceneBGM_->Initialize("resources/sound/titleSceneBGM.mp3");
	//titleSceneBGM_->PlayAudio(BGM_Volume, true);

	cloud_->Initialize(&ctx_->dxCommon, "Cloud/Cloud.obj");
	cloud_->SetTexture("resources/image/0.png");
}

void TitleScene::Update() {
	
	if (selectMenu_ == PLAY) {
		if (input_->TriggerKey(DIK_SPACE) && titleTimer_.IsFinished() ) {
			sceneFade_->StartFadeIn(1.0f);
			startGameSE_->PlayAudio(SE_Volume);
		}
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(SCENE::GAME); // シーン切り替え
	}

	TitleLogoUpdate();
	SelectUIUpdate();
	OptionUIUpdate();

	SpriteUpdate();
	AudioUpdate();

	titleTimer_.Update();
	cursolTimer_.Update();
	optionTimer_.Update();
	optionCursolTimer_.Update();

	sceneFade_->Update();

	titleLogo_->Update();
	titleObject_->Update();

	cloud_->Update();

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
	titleObject_->Draw(*useCamera_);

	//cloud_->Draw(*useCamera_);

	sceneFade_->Draw();

	// UIの描画
	playUI_->Draw();
	optionUI_->Draw();
	cursolUI_->Draw();

	optionBG_->Draw();
	optionCursolUI_->Draw();
	fullScreenUI_->Draw();

	if (!ctx_->winApp.IsFullscreen()) {
		offUI_->Draw();
	} else {
		onUI_->Draw();
	}

	seUI_->Draw();
	bgmUI_->Draw();

	switch (seVolume_)
	{
	case DAI:
		daiUI_->Draw();
		break;
	case TYU:
		tyuUI_->Draw();
		break;
	case SYOU:
		syouUI_->Draw();
		break;
	}

	switch (bgmVolume_)
	{
	case DAI:
		dai2UI_->Draw();
		break;
	case TYU:
		tyu2UI_->Draw();
		break;
	case SYOU:
		syou2UI_->Draw();
		break;
	}

	backUI_->Draw();

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

	//cloud_->DrawImGui("cloud");

	titleObject_->DrawImGui();

	//optionCursolUI_->DrawImGui("optionCursol");
	//optionBG_->DrawImGui("optionBG");
	//fullScreenUI_->DrawImGui("full");
	//onUI_->DrawImGui("on");
	//offUI_->DrawImGui("off");
	//seUI_->DrawImGui("se");
	//bgmUI_->DrawImGui("bgm");
	//daiUI_->DrawImGui("dai");
	//dai2UI_->DrawImGui("dai2");
	//tyuUI_->DrawImGui("tyu");
	//tyu2UI_->DrawImGui("tyu2");
	//syouUI_->DrawImGui("syou");
	//syou2UI_->DrawImGui("syou2");
	//backUI_->DrawImGui("back");

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

void TitleScene::TitleLogoUpdate() {
	// TitleLogoの演出が終了し、まだタイマーが開始されていない場合のみ開始
	if (titleLogo_->IsEnd() && !titleTimer_.IsActive() && !titleTimer_.IsFinished()) {
		titleTimer_.Start(1.0f, false);
		titleObject_->PlayerStart();
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

void TitleScene::SelectUIUpdate() {
	// メニュー選択処理
	if (!cursolTimer_.IsActive() && titleTimer_.IsFinished() && selectOptionMenu_ == OptionMenu::NONE) {
		if (selectMenu_ == PLAY) {
			if (input_->TriggerKey(DIK_DOWN)) {
				selectMenu_ = OPTION;
				cursolTimer_.Start(0.25f, false);

				moveCursolSE_->PlayAudio(SE_Volume);
			}
		} else if (selectMenu_ == OPTION) {
			if (input_->TriggerKey(DIK_UP)) {
				selectMenu_ = PLAY;
				cursolTimer_.Start(0.25f, false);

				moveCursolSE_->PlayAudio(SE_Volume);
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

void TitleScene::OptionUIUpdate() {
	if (selectMenu_ == Menu::OPTION && selectOptionMenu_ == OptionMenu::NONE) {
		if (input_->TriggerKey(DIK_SPACE) && titleTimer_.IsFinished() && !optionTimer_.IsActive()) {
			optionTimer_.Start(0.5f, false);
			optionOpen_ = true;
			selectOptionMenu_ = OptionMenu::FULLSCREEN;
			optionCursolUI_->SetPosition({ 590.0f, 151.0f });

			decideSE_->PlayAudio(SE_Volume);
		}
	}

	if (selectOptionMenu_ == OptionMenu::BACK && input_->TriggerKey(DIK_SPACE) && !optionTimer_.IsActive()) {
		optionTimer_.Start(0.5f, false);
		optionOpen_ = false;
		selectOptionMenu_ = OptionMenu::NONE;

		decideSE_->PlayAudio(SE_Volume);
	}

	switch (selectOptionMenu_)
	{
	case NONE:
		break;
	case FULLSCREEN:

		if (optionCursolTimer_.IsActive()) {
			float y = Easing::Lerp(optionCursolStart_, optionCursolEnd_, optionCursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			optionCursolUI_->SetPosition({ 590.0f, y });
		} else {
			optionCursolUI_->SetPosition({ 590.0f, 151.0f });
		}

		if (!optionTimer_.IsActive() && input_->TriggerKey(DIK_SPACE)) {
			if (!ctx_->winApp.IsFullscreen()) {
				ctx_->winApp.EnterBorderlessFullscreen();
			} else {
				ctx_->winApp.ExitBorderlessFullscreen();
			}

			decideSE_->PlayAudio(SE_Volume);
		}

		if (input_->TriggerKey(DIK_DOWN)) {
			selectOptionMenu_ = OptionMenu::SE;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 151.0f;
			optionCursolEnd_ = 237.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}
		break;
	case SE:

		if (optionCursolTimer_.IsActive()) {
			float y = Easing::Lerp(optionCursolStart_, optionCursolEnd_, optionCursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			optionCursolUI_->SetPosition({ 590.0f, y });
		} else {
			optionCursolUI_->SetPosition({ 590.0f, 237.0f });
		}

		if (input_->TriggerKey(DIK_UP)) {
			selectOptionMenu_ = OptionMenu::FULLSCREEN;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 237.0f;
			optionCursolEnd_ = 151.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		if (input_->TriggerKey(DIK_DOWN)) {
			selectOptionMenu_ = OptionMenu::BGM;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 237.0f;
			optionCursolEnd_ = 321.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		switch (seVolume_)
		{
		case DAI:
			if (input_->TriggerKey(DIK_SPACE)) {
				seVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT)) {
				seVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT)) {
				seVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			SE_Volume = daiVolumeSE_;
			break;
		case TYU:
			if (input_->TriggerKey(DIK_SPACE)) {
				seVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT)) {
				seVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT)) {
				seVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			SE_Volume = tyuVolumeSE_;
			break;
		case SYOU:
			if (input_->TriggerKey(DIK_SPACE)) {
				seVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT)) {
				seVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT)) {
				seVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			SE_Volume = syouVolumeSE_;
			break;
		}

		break;
	case BGM:

		if (optionCursolTimer_.IsActive()) {
			float y = Easing::Lerp(optionCursolStart_, optionCursolEnd_, optionCursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			optionCursolUI_->SetPosition({ 590.0f, y });
		} else {
			optionCursolUI_->SetPosition({ 590.0f, 321.0f });
		}

		if (input_->TriggerKey(DIK_UP)) {
			selectOptionMenu_ = OptionMenu::SE;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 321.0f;
			optionCursolEnd_ = 237.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		if (input_->TriggerKey(DIK_DOWN)) {
			selectOptionMenu_ = OptionMenu::BACK;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 321.0f;
			optionCursolEnd_ = 402.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		switch (bgmVolume_)
		{
		case DAI:
			if (input_->TriggerKey(DIK_SPACE)) {
				bgmVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT)) {
				bgmVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT)) {
				bgmVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			BGM_Volume = daiVolumeBGM_;
			break;
		case TYU:
			if (input_->TriggerKey(DIK_SPACE)) {
				bgmVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT)) {
				bgmVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT)) {
				bgmVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			BGM_Volume = tyuVolumeBGM_;
			break;
		case SYOU:
			if (input_->TriggerKey(DIK_SPACE)) {
				bgmVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT)) {
				bgmVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT)) {
				bgmVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			BGM_Volume = syouVolumeBGM_;
			break;
		}

		break;
	case BACK:

		if (optionCursolTimer_.IsActive()) {
			float y = Easing::Lerp(optionCursolStart_, optionCursolEnd_, optionCursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			optionCursolUI_->SetPosition({ 590.0f, y });
		} else {
			optionCursolUI_->SetPosition({ 590.0f, 402.0f });
		}

		if (input_->TriggerKey(DIK_UP)) {
			selectOptionMenu_ = OptionMenu::BGM;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 402.0f;
			optionCursolEnd_ = 321.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}
		break;
	}

	// オプションBGのスケール制御
	if (optionTimer_.IsActive()) {
		if (optionOpen_) {
			optionBGScale_.x = Easing::Lerp(0.0f, 65.0f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			optionBGScale_.y = Easing::Lerp(0.0f, 35.0f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);

			normalUIScale_.x = Easing::Lerp(0.0f, 0.4f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			normalUIScale_.y = Easing::Lerp(0.0f, 0.4f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);

			optionCursolUIScale_.x = Easing::Lerp(0.0f, 0.2f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			optionCursolUIScale_.y = Easing::Lerp(0.0f, 0.2f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
		} else {
			optionBGScale_.x = Easing::Lerp(65.0f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			optionBGScale_.y = Easing::Lerp(35.0f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);

			normalUIScale_.x = Easing::Lerp(0.4f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			normalUIScale_.y = Easing::Lerp(0.4f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);

			optionCursolUIScale_.x = Easing::Lerp(0.2f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			optionCursolUIScale_.y = Easing::Lerp(0.2f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
		}
	} else {
		// タイマーが非アクティブな場合の適切なスケール設定
		if (optionOpen_) {
			// オープン状態で完了：最大サイズに設定
			optionBGScale_ = { 65.0f, 35.0f };
			normalUIScale_ = { 0.4f,0.4f };
			optionCursolUIScale_ = { 0.2f,0.2f };
		} else {
			// クローズ状態で完了：0サイズに設定
			optionBGScale_ = { 0.0f, 0.0f };
			normalUIScale_ = { 0.0f, 0.0f }; 
			optionCursolUIScale_ = { 0.0f,0.0f };
		}
	}

	optionBG_->SetScale(optionBGScale_);
	fullScreenUI_->SetScale(normalUIScale_);
	onUI_->SetScale(normalUIScale_);
	offUI_->SetScale(normalUIScale_);
	seUI_->SetScale(normalUIScale_);
	bgmUI_->SetScale(normalUIScale_);
	daiUI_->SetScale(normalUIScale_);
	dai2UI_->SetScale(normalUIScale_);
	tyuUI_->SetScale(normalUIScale_);
	tyu2UI_->SetScale(normalUIScale_);
	syouUI_->SetScale(normalUIScale_);
	syou2UI_->SetScale(normalUIScale_);
	backUI_->SetScale(normalUIScale_);
	optionCursolUI_->SetScale(optionCursolUIScale_);
}

void TitleScene::SpriteUpdate() {
	playUI_->Update();
	optionUI_->Update();
	cursolUI_->Update();

	optionBG_->Update();
	optionCursolUI_->Update();
	fullScreenUI_->Update();
	onUI_->Update();
	offUI_->Update();
	seUI_->Update();
	bgmUI_->Update();
	daiUI_->Update();
	dai2UI_->Update();
	tyuUI_->Update();
	tyu2UI_->Update();
	syouUI_->Update();
	syou2UI_->Update();
	backUI_->Update();
}

void TitleScene::AudioUpdate() {
	startGameSE_->SetVolume(SE_Volume);
	moveCursolSE_->SetVolume(SE_Volume);
	decideSE_->SetVolume(SE_Volume);
	titleSceneBGM_->SetVolume(BGM_Volume);

	startGameSE_->Update();
	moveCursolSE_->Update();
	decideSE_->Update();
	titleSceneBGM_->Update();
}