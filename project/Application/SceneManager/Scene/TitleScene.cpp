#include "TitleScene.h"
#include "MasterVolume.h"

#include "Engine/System/DirectXCommon/ExeColor.h"

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

	useDebugCamera_ = false;

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	titleLogo_.reset();
	titleLogo_ = make_unique<Text3D>();
	titleLogo_->Initialize(&ctx_->dxCommon);

	titleObject_.reset();
	titleObject_ = make_unique<TitleObject>();
	titleObject_->Initialize(&ctx_->dxCommon);

	uiAlpha_ = 0.0f;
	titleTimer_.Reset();
	cursolTimer_.Reset();

	// cloudTimer_の初期化を追加
	cloudTimer_.Reset();
	cloudLineTimer_.Reset();

	SE_Volume = tyuVolumeSE_;
	BGM_Volume = tyuVolumeBGM_;

	InitSptite();

	pushStart_ = false;

	startGameSE_->Initialize("resources/sound/SE/Title/startGameSE.mp3");
	moveCursolSE_->Initialize("resources/sound/SE/Title/moveCursolSE.mp3");
	decideSE_->Initialize("resources/sound/SE/Title/DecideSE.mp3");
	titleSceneBGM_->Initialize("resources/sound/BGM/TitleBGM.mp3");
	titleSceneBGM_->PlayAudio(BGM_Volume, true);

	titleParticle_->Initialize(&ctx_->dxCommon, 2);
	titleParticle_->LoadJson("candy");
	titleParticle_->Stop();

	testParticle_->Initialize(&ctx_->dxCommon, 1);
	testParticle_->LoadJson("getScore");

	// 雲の初期化
	for (int i = 0; i < cloud_.size(); i++) {
		cloud_[i] = make_unique<Model>();	
		cloud_[i]->Initialize(&ctx_->dxCommon, "Cloud/Cloud.obj");
		cloud_[i]->SetTexture("resources/image/0.png");
		cloud_[i]->SetUpdateFrustumCulling(false);
		cloudIsActive_[i] = false;
		cloudTramsform_[i].scale = { 1.0f, 1.0f, 1.0f };  // スケールを大きく
		cloudTramsform_[i].rotate = { 0.0f, 0.0f, 0.0f };
		cloudTramsform_[i].translate = { 0.0f, 7.5f, 0.0f };  // 初期位置を上に
	}

	for (int i = 0; i < cloud_.size(); i++) {
		cloudLine_[i] = make_unique<Model>();
		cloudLine_[i]->Initialize(&ctx_->dxCommon, "Cloud/LineI.obj");
		cloudLine_[i]->SetTexture("resources/image/0.png");
		cloudLine_[i]->SetUpdateFrustumCulling(false);
		cloudLineIsActive_[i] = false;
		cloudLineTramsform_[i].scale = { 1.0f, 1.0f, 1.0f };  // スケールを大きく
		cloudLineTramsform_[i].rotate = { 0.0f, 0.0f, 0.0f };
		cloudLineTramsform_[i].translate = { 0.0f, 7.5f, 0.0f };  // 初期位置を上に
	}

	// 振動の終了
	gamePad_->SetVibration(0.0f, 0.0f, 0.0f);
}

void TitleScene::Update() {
	
	if (selectMenu_ == PLAY) {
		if ((input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) && titleTimer_.IsFinished() && !pushStart_) {
			sceneFade_->StartFadeIn(2.0f);
			titleObject_->PlayerEnd();
			startGameSE_->PlayAudio(SE_Volume);
			pushStart_ = true;
		}
	}

	if (selectMenu_ == QUIT) {
		if ((input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) && titleTimer_.IsFinished() && !pushStart_) {
			Quit();
		}
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(SCENE::GAME); // シーン切り替え
		startGameSE_->Reset();
		moveCursolSE_->Reset();
		decideSE_->Reset();
		titleSceneBGM_->Reset();
	}

	TitleLogoUpdate();
	CloudUpdate();
	SelectUIUpdate();
	OptionUIUpdate();

	SpriteUpdate();
	AudioUpdate();

	titleTimer_.Update();
	cursolTimer_.Update();
	optionTimer_.Update();
	optionCursolTimer_.Update();
	fullScreenTimer_.Update();

	cloudTimer_.Update();
	cloudLineTimer_.Update();

	sceneFade_->Update();

	titleLogo_->Update();
	titleObject_->Update();

	titleParticle_->SetBlendMode(BlendMode::kBlendModeNone);
	titleParticle_->Update();

	testParticle_->SetBlendMode(BlendMode::kBlendModeNormal);
	testParticle_->Update();

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

	titleParticle_->Draw(*useCamera_);
	testParticle_->Draw(*useCamera_);

	for (int i = 0; i < cloud_.size(); i++) {
		//if (cloudIsActive_[i]) {
			cloud_[i]->Draw(*useCamera_);
		//}
	}

	for (int i = 0; i < cloudLine_.size(); i++) {
		//if (cloudLineIsActive_[i]) {
		cloudLine_[i]->Draw(*useCamera_);
		//}
	}

	// UIの描画
	playUI_->Draw();
	optionUI_->Draw();
	quitUI_->Draw();
	cursolUI_->Draw();

	optionBG_->Draw();
	uiBoxUI_->Draw();
	optionCursolUI_->Draw();
	fullScreenUI_->Draw();

	OptionBearUI_->Draw();

	parenthesesUI1_->Draw();
	parenthesesUI2_->Draw();
	parenthesesUI3_->Draw();
	parenthesesUI4_->Draw();
	parenthesesUI5_->Draw();
	parenthesesUI6_->Draw();

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

	//DrawSceneName();

	//titleLogo_->DrawImGui();

	//OptionBearUI_->DrawImGui("bear");
	//uiBoxUI_->DrawImGui("uiBox");

	//titleParticle_->DrawImGui("titleParticle");

	testParticle_->DrawImGui("testParticle");

	/*parenthesesUI1_->DrawImGui("pare1");
	parenthesesUI2_->DrawImGui("pare2");
	parenthesesUI3_->DrawImGui("pare3");
	parenthesesUI4_->DrawImGui("pare4");
	parenthesesUI5_->DrawImGui("pare5");
	parenthesesUI6_->DrawImGui("pare6");*/


	//debugCamera_->DrawImgui();

	//cloud_->DrawImGui("cloud");

	//titleObject_->DrawImGui();

	//ImGui::Begin("exe");

	//ImGui::ColorEdit4("color", &ExeColor.x);

	//ImGui::End();

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
	//quitUI_->DrawImGui("quit");
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
		cloudTimer_.Start(1.0f, false);
		cloudLineTimer_.Start(0.5f, false);
		titleObject_->PlayerStart();
		titleParticle_->Play();
	}
	
	// UIの透明度制御を修正 - フェードイン効果の実装
	float targetAlpha = 1.0f;
	if (titleTimer_.IsActive()) {
		// タイマーが動作中の場合、進行度に応じて透明度を上げる（0.0 → 1.0）
		uiAlpha_ = titleTimer_.GetProgress();
	} else if (titleTimer_.IsFinished()) {
		// タイマーが完了している場合は完全に不透明
		uiAlpha_ = 1.0f;
	} else {
		// タイマーが開始されていない場合は透明
		uiAlpha_ = 0.0f;
	}

	// UIの色を設定（RGB値は1.0を維持し、アルファのみ変更）
	playUI_->SetColor({ 1.0f, 1.0f, 1.0f, uiAlpha_ });
	optionUI_->SetColor({ 1.0f, 1.0f, 1.0f, uiAlpha_ });
	quitUI_->SetColor({ 1.0f, 1.0f, 1.0f, uiAlpha_ });
	cursolUI_->SetColor({ 1.0f, 1.0f, 1.0f, uiAlpha_ });
}

void TitleScene::SelectUIUpdate() {
	// メニュー選択処理
	if (/*!cursolTimer_.IsActive() && */titleTimer_.IsFinished() && selectOptionMenu_ == OptionMenu::NONE) {
		if (selectMenu_ == PLAY) {
			if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON) || gamePad_->TriggerLeftStick(GamePad::DOWN_STICK)) {
				selectMenu_ = OPTION;
				cursolTimer_.Start(0.25f, false);
				startCursolY_ = 386.0f;
				endCursolY_ = 487.0f;

				moveCursolSE_->PlayAudio(SE_Volume);
			}
		} else if (selectMenu_ == OPTION) {
			if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W) || gamePad_->TriggerButton(GamePad::UP_BOTTON) || gamePad_->TriggerLeftStick(GamePad::UP_STICK)) {
				selectMenu_ = PLAY;
				cursolTimer_.Start(0.25f, false);
				startCursolY_ = 487.0f;
				endCursolY_ = 386.0f;

				moveCursolSE_->PlayAudio(SE_Volume);
			} else if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON) || gamePad_->TriggerLeftStick(GamePad::DOWN_STICK)) {
				selectMenu_ = QUIT;
				cursolTimer_.Start(0.25f, false);
				startCursolY_ = 487.0f;
				endCursolY_ = 584.0f;

				moveCursolSE_->PlayAudio(SE_Volume);
			}
		} else if (selectMenu_ == QUIT) {
			if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W) || gamePad_->TriggerButton(GamePad::UP_BOTTON) || gamePad_->TriggerLeftStick(GamePad::UP_STICK)) {
				selectMenu_ = OPTION;
				cursolTimer_.Start(0.25f, false);
				startCursolY_ = 584.0f;
				endCursolY_ = 487.0f;

				moveCursolSE_->PlayAudio(SE_Volume);
			}
		}
	}

	// カーソル位置の更新
	if (cursolTimer_.IsActive()) {
		// タイマーがアクティブな場合
		if (selectMenu_ == PLAY) {
			// OPTIONからPLAYへの移動
			float y = Easing::Lerp(startCursolY_, endCursolY_, cursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			cursolUI_->SetPosition({ 210.0f, y });
		} else if (selectMenu_ == OPTION) {
			// PLAYからOPTIONへの移動
			float y = Easing::Lerp(startCursolY_, endCursolY_, cursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			cursolUI_->SetPosition({ 210.0f, y });
		} else if (selectMenu_ == QUIT) {
			float y = Easing::Lerp(startCursolY_, endCursolY_, cursolTimer_.GetProgress(), Easing::Type::EaseInOutQuint);
			cursolUI_->SetPosition({ 210.0f, y });
		}
	} else {
		// タイマーが非アクティブな場合
		if (selectMenu_ == PLAY) {
			cursolUI_->SetPosition({ 210.0f, 384.0f });
		} else if (selectMenu_ == OPTION) {
			cursolUI_->SetPosition({ 210.0f, 488.0f });
		} else if (selectMenu_ == QUIT) {
			cursolUI_->SetPosition({ 210.0f, 581.0f });
		}
	}
}

void TitleScene::OptionUIUpdate() {
	if (selectMenu_ == Menu::OPTION && selectOptionMenu_ == OptionMenu::NONE) {
		if ((input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) && titleTimer_.IsFinished() && !optionTimer_.IsActive()) {
			optionTimer_.Start(0.5f, false);
			optionOpen_ = true;
			selectOptionMenu_ = OptionMenu::FULLSCREEN;
			optionCursolUI_->SetPosition({ 590.0f, 151.0f });

			decideSE_->PlayAudio(SE_Volume);
		}
	}

	if (selectOptionMenu_ == OptionMenu::BACK && (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) && !optionTimer_.IsActive()) {
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

		if (!fullScreenTimer_.IsActive() && !optionTimer_.IsActive() && (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A) || input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_D) || input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_LEFT) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK))) {
			if (!ctx_->winApp.IsFullscreen()) {
				ctx_->winApp.EnterBorderlessFullscreen();
			} else {
				ctx_->winApp.ExitBorderlessFullscreen();
			}

			fullScreenTimer_.Start(0.5f, false);

			decideSE_->PlayAudio(SE_Volume);
		}

		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON) || gamePad_->TriggerLeftStick(GamePad::DOWN_STICK)) {
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

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W) || gamePad_->TriggerButton(GamePad::UP_BOTTON) || gamePad_->TriggerLeftStick(GamePad::UP_STICK)) {
			selectOptionMenu_ = OptionMenu::FULLSCREEN;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 237.0f;
			optionCursolEnd_ = 151.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON) || gamePad_->TriggerLeftStick(GamePad::DOWN_STICK)) {
			selectOptionMenu_ = OptionMenu::BGM;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 237.0f;
			optionCursolEnd_ = 321.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		switch (seVolume_)
		{
		case DAI:
			if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				seVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK)) {
				seVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK)) {
				seVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			SE_Volume = daiVolumeSE_;
			break;
		case TYU:
			if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				seVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK)) {
				seVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK)) {
				seVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			SE_Volume = tyuVolumeSE_;
			break;
		case SYOU:
			if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				seVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK)) {
				seVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK)) {
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

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W) || gamePad_->TriggerButton(GamePad::UP_BOTTON) || gamePad_->TriggerLeftStick(GamePad::UP_STICK)) {
			selectOptionMenu_ = OptionMenu::SE;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 321.0f;
			optionCursolEnd_ = 237.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON) || gamePad_->TriggerLeftStick(GamePad::DOWN_STICK)) {
			selectOptionMenu_ = OptionMenu::BACK;
			optionCursolTimer_.Start(0.25f, false);

			optionCursolStart_ = 321.0f;
			optionCursolEnd_ = 402.0f;

			moveCursolSE_->PlayAudio(SE_Volume);
		}

		switch (bgmVolume_)
		{
		case DAI:
			if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				bgmVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK)) {
				bgmVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK)) {
				bgmVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			BGM_Volume = daiVolumeBGM_;
			break;
		case TYU:
			if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				bgmVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK)) {
				bgmVolume_ = SoundVolume::DAI;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK)) {
				bgmVolume_ = SoundVolume::SYOU;

				decideSE_->PlayAudio(SE_Volume);
			}

			BGM_Volume = tyuVolumeBGM_;
			break;
		case SYOU:
			if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				bgmVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::RIGHT_STICK)) {
				bgmVolume_ = SoundVolume::TYU;

				decideSE_->PlayAudio(SE_Volume);
			}

			if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerLeftStick(GamePad::LEFT_STICK)) {
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

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W) || gamePad_->TriggerButton(GamePad::UP_BOTTON) || gamePad_->TriggerLeftStick(GamePad::UP_STICK)) {
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
			optionBGScale_.x = Easing::Lerp(0.0f, 1.55f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			optionBGScale_.y = Easing::Lerp(0.0f, 0.85f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);

			normalUIScale_.x = Easing::Lerp(0.0f, 0.4f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			normalUIScale_.y = Easing::Lerp(0.0f, 0.4f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);

			optionCursolUIScale_.x = Easing::Lerp(0.0f, 0.2f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			optionCursolUIScale_.y = Easing::Lerp(0.0f, 0.2f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);

			parenthesesUIScale_.x = Easing::Lerp(0.0f, 0.25f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);
			parenthesesUIScale_.y = Easing::Lerp(0.0f, 0.25f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack);

			OptionBearUI_->SetScale({ Easing::Lerp(0.0f, 0.6f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack) ,Easing::Lerp(0.0f, 0.6f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack) });
			uiBoxUI_->SetScale({ Easing::Lerp(0.0f, 0.85f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack) ,Easing::Lerp(0.0f, 0.75f, optionTimer_.GetProgress(), Easing::Type::EaseOutBack) });
		} else {
			optionBGScale_.x = Easing::Lerp(1.55f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			optionBGScale_.y = Easing::Lerp(0.85f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);

			normalUIScale_.x = Easing::Lerp(0.4f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			normalUIScale_.y = Easing::Lerp(0.4f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);

			optionCursolUIScale_.x = Easing::Lerp(0.2f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			optionCursolUIScale_.y = Easing::Lerp(0.2f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);

			parenthesesUIScale_.x = Easing::Lerp(0.25f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);
			parenthesesUIScale_.y = Easing::Lerp(0.25f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack);

			OptionBearUI_->SetScale({ Easing::Lerp(0.6f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack) ,Easing::Lerp(0.6f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack) });
			uiBoxUI_->SetScale({ Easing::Lerp(0.85f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack) ,Easing::Lerp(0.75f, 0.0f, optionTimer_.GetProgress(), Easing::Type::EaseInBack) });
		}
	} else {
		// タイマーが非アクティブな場合の適切なスケール設定
		if (optionOpen_) {
			// オープン状態で完了：最大サイズに設定
		    optionBGScale_ = { 1.55f, 0.85f };
			normalUIScale_ = { 0.4f,0.4f };
			optionCursolUIScale_ = { 0.2f,0.2f };
			parenthesesUIScale_ = { 0.25f,0.25f };
			OptionBearUI_->SetScale({ 0.6f,0.6f });
			uiBoxUI_->SetScale({ 0.85f,0.75f });
		} else {
			// クローズ状態で完了：0サイズに設定
			optionBGScale_ = { 0.0f, 0.0f };
			normalUIScale_ = { 0.0f, 0.0f }; 
			optionCursolUIScale_ = { 0.0f,0.0f };
			parenthesesUIScale_ = { 0.0f,0.0f };
			OptionBearUI_->SetScale({ 0.0f,0.0f });
			uiBoxUI_->SetScale({ 0.0f,0.0f });
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

	parenthesesUI1_->SetScale(parenthesesUIScale_);
	parenthesesUI2_->SetScale(parenthesesUIScale_);
	parenthesesUI3_->SetScale(parenthesesUIScale_);
	parenthesesUI4_->SetScale(parenthesesUIScale_);
	parenthesesUI5_->SetScale(parenthesesUIScale_);
	parenthesesUI6_->SetScale(parenthesesUIScale_);
}

void TitleScene::CloudUpdate() {

	// 雲の生成処理を修正
	if (cloudTimer_.IsFinished()) {
		// 雲生成処理
		for (int i = 0; i < cloud_.size(); i++) {
			if (!cloudIsActive_[i]) {
				cloudIsActive_[i] = true;
				// X座標をランダムに設定
				cloudTramsform_[i].translate.x = random_.Float(-10.0f, 10.0f);
				// Y座標を画面上部に設定
				cloudTramsform_[i].translate.y = 7.5f;
				// Z座標をランダムに設定（奥行き）
				cloudTramsform_[i].translate.z = random_.Float(-3.0f, 10.0f);
				break;
			}
		}
		// 次の雲生成のためにタイマーを再開始
		cloudTimer_.Start(2.5f, false);  // 間隔を少し長くして確認しやすくする
	}

	// 雲の更新処理
	for (int i = 0; i < cloud_.size(); i++) {
		if (cloudIsActive_[i]) {
			// 雲を下に移動
			cloudTramsform_[i].translate.y -= 2.0f * deltaTime_;
			//cloudTramsform_[i].translate.x -= 3.0f * deltaTime_;

			// 画面下部まで来たら非アクティブにする
			if (cloudTramsform_[i].translate.y < -7.0f) {
				cloudIsActive_[i] = false;
			}
		}
		// 雲の色と透明度を設定
		cloud_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });
		cloud_[i]->SetTransform(cloudTramsform_[i]);
		cloud_[i]->Update();
	}

	// 雲の生成処理を修正
	if (cloudLineTimer_.IsFinished()) {
		// 雲生成処理
		for (int i = 0; i < cloudLine_.size(); i++) {
			if (!cloudLineIsActive_[i]) {
				cloudLineIsActive_[i] = true;
				// X座標をランダムに設定
				cloudLineTramsform_[i].translate.x = random_.Float(-10.0f, 10.0f);
				// Y座標を画面上部に設定
				cloudLineTramsform_[i].translate.y = 7.5f;
				// Z座標をランダムに設定（奥行き）
				cloudLineTramsform_[i].translate.z = random_.Float(-3.0f, 10.0f);
				break;
			}
		}
		// 次の雲生成のためにタイマーを再開始
		cloudLineTimer_.Start(0.5f, false);  // 間隔を少し長くして確認しやすくする
	}

	// 雲の更新処理
	for (int i = 0; i < cloudLine_.size(); i++) {
		if (cloudLineIsActive_[i]) {
			// 雲を下に移動
			cloudLineTramsform_[i].translate.y -= 10.0f * deltaTime_;
			//cloudTramsform_[i].translate.x -= 3.0f * deltaTime_;

			// 画面下部まで来たら非アクティブにする
			if (cloudLineTramsform_[i].translate.y < -7.0f) {
				cloudLineIsActive_[i] = false;
			}
		}
		// 雲の色と透明度を設定
		cloudLine_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });
		cloudLine_[i]->SetTransform(cloudLineTramsform_[i]);
		cloudLine_[i]->Update();
	}
}

void TitleScene::SpriteUpdate() {
	playUI_->Update();
	optionUI_->Update();
	cursolUI_->Update();
	quitUI_->Update();

	optionBG_->Update();
	uiBoxUI_->Update();
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

	parenthesesUI1_->Update();
	parenthesesUI2_->Update();
	parenthesesUI3_->Update();
	parenthesesUI4_->Update();
	parenthesesUI5_->Update();
	parenthesesUI6_->Update();

	float bearRotate = OptionBearUI_->GetRotate();
	bearRotate -= 1.0f * deltaTime_;
	OptionBearUI_->SetRotate(bearRotate);
	OptionBearUI_->Update();
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

void TitleScene::InitSptite() {
	playUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/PlayUI.png", { 342.0f,384.0f }, { 0.6f,0.6f });
	playUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	optionUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/optionUI.png", { 390.0f,488.0f }, { 0.6f,0.6f });
	optionUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	quitUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/quitUI.png", { 347.0f,581.0f }, { 0.6f,0.6f });
	quitUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	cursolUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/cursol.png", { 210.0f,386.0f }, { 0.3f,0.3f });
	cursolUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	optionBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/UIBoxUI.png", { 640.0f,355.0f }, { 0.0f,0.0f });
	optionBG_->SetColor({ 0.0f,0.0f,0.0f,0.85f });

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
	OptionBearUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/optionBearUI.png", { 320.0f,350.0f }, { 0.6f,0.6f });
	uiBoxUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/uiBoxUI.png", { 860.0f,350.0f }, { 0.0f,0.0f });
	uiBoxUI_->SetColor({ 0.5f,0.5f,0.5f,0.5f });
	parenthesesUI1_->Initialize(&ctx_->dxCommon, "resources/image/UI/parenthesesUI.png", { 1115.0f,151.0f });
	parenthesesUI2_->Initialize(&ctx_->dxCommon, "resources/image/UI/parenthesesUI.png", { 1023.0f,151.0f });
	parenthesesUI3_->Initialize(&ctx_->dxCommon, "resources/image/UI/parenthesesUI.png", { 1115.0f,234.0f });
	parenthesesUI4_->Initialize(&ctx_->dxCommon, "resources/image/UI/parenthesesUI.png", { 1023.0f,234.0f });
	parenthesesUI5_->Initialize(&ctx_->dxCommon, "resources/image/UI/parenthesesUI.png", { 1115.0f,323.0f });
	parenthesesUI6_->Initialize(&ctx_->dxCommon, "resources/image/UI/parenthesesUI.png", { 1023.0f,323.0f });
	parenthesesUI2_->SetRotate(3.16f);
	parenthesesUI4_->SetRotate(3.16f);
	parenthesesUI6_->SetRotate(3.16f);
}