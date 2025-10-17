#include "TitleScene.h"

void TitleScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void TitleScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// Audioの初期化
	//audio_->Initialize("project/resources/");

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 0.0f, -10.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	particle_->Initialize(&ctx_->dxCommon, 4);
	particle_->LoadJson("temp");

	particle2_->Initialize(&ctx_->dxCommon, 4);

	model_->Initialize(&ctx_->dxCommon, "cube.obj");
	model_->SetTexture("resources/image/test.png");

	// Text3Dの初期化
	for (auto& text : text3D_) {
		text = make_unique<Text3D>();
		text->Initialize(&ctx_->dxCommon);
	}

	// スプライトの初期化
	spriteBG_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteBG_->SetPosition({600.0f, 340.0f});
	spriteBG_->SetScale({76, 43});
	spriteBG_->SetColor({0, 0, 0, 1});

	spriteInGame_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteInGame_->SetPosition({750, 400});
	spriteInGame_->SetScale({20, 4});

	spriteTutorial_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteTutorial_->SetPosition({650.0f, 500.0f});
	spriteTutorial_->SetScale({20, 4});

	spriteOption_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteOption_->SetPosition({650.0f, 600.0f});
	spriteOption_->SetScale({20, 4});

	// スクリーンの状態
	currentScreen_ = Title::Screen::FIRST;
}

void TitleScene::Update() {
	// 決定ボタン
	bool enterInput = input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(gamePad_->A);

	// スプライトの更新
	if (currentScreen_ == Title::Screen::SECOND) {
		// メニューの切り替え更新
		UpdateMenu();

		spriteBG_->Update();
		spriteInGame_->Update();
		spriteTutorial_->Update();
		spriteOption_->Update();
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(SCENE::GAME); // シーン切り替え
	}

	if (enterInput) {
		currentScreen_ = Title::Screen::SECOND; // スクリーンの切り替え
	}

	sceneFade_->Update();

	model_->Update();
	particle_->Update();
	particle2_->Update();


	// カメラ切り替え&更新
	CameraController();

	// Text3Dの更新処理
	for (auto& text : text3D_) {
		text->MoveTextAnimation(0.0f, 10.0f, std::numbers::pi_v<float> * 2, 0.0f);
		text->Update();
	}

	/*spriteBG_->SetScale(testScale_);
	spriteBG_->SetPosition(testPos_);*/
}

void TitleScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	// スプライト描画
	if (currentScreen_ == Title::Screen::SECOND) {
		spriteBG_->Draw();
		spriteInGame_->Draw();
		spriteTutorial_->Draw();
		spriteOption_->Draw();
	}

	// 3Dテキストの描画処理
	for (auto& text : text3D_) {
		text->Draw(*useCamera_);
	}


	model_->Draw(*useCamera_);
	particle_->Draw(*useCamera_);
	particle2_->Draw(*useCamera_);

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

	particle_->DrawImGui("titleEffect");
	particle2_->DrawImGui("titleEffect2");

	DrawSceneName();

	model_->DrawImGui("test");

	ImGui::Begin("test");

	ImGui::DragFloat2("Scale", &testScale_.x, 1.0f);
	ImGui::DragFloat2("Pos", &testPos_.x, 1.0f);

	ImGui::End();

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void TitleScene::UpdateMenu() {
	bool upInput = input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W) || /*gamePad_->LeftStickY() >= 0.3f ||*/ gamePad_->TriggerButton(gamePad_->DPAD_UP);        // 上ボタン
	bool downInput = input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S) || /*gamePad_->LeftStickY() <= -0.3f ||*/ gamePad_->TriggerButton(gamePad_->DPAD_DOWN); // 下ボタン
	bool enterInput = input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(gamePad_->A);                                                                       // 決定ボタン

	switch (currentMenu_) {
	case Menu::IN_GAME:

		if (downInput) {
			currentMenu_ = Menu::TUTORIAL; // カーソルをチュートリアルへ
			spriteTutorial_->SetPosition({750, 500});
			spriteInGame_->SetPosition({650, 400});
		}

		if (enterInput) {
			if (currentScreen_ == Title::Screen::SECOND) {
				sceneFade_->StartFadeIn(1.0f); // フェード
			}
		}

		break;
	case Menu::TUTORIAL:

		if (upInput) {
			currentMenu_ = Menu::IN_GAME; // カーソルをインゲームへ
			spriteInGame_->SetPosition({750, 400});
			spriteTutorial_->SetPosition({650, 500});
		}

		if (downInput) {
			currentMenu_ = Menu::OPTION; // カーソルをオプションへ
			spriteOption_->SetPosition({750, 600});
			spriteTutorial_->SetPosition({650, 500});
		}

		if (enterInput) {
		}

		break;
	case Menu::OPTION:

		if (upInput) {
			currentMenu_ = Menu::TUTORIAL; // カーソルをチュートリアルへ
			spriteTutorial_->SetPosition({750, 500});
			spriteOption_->SetPosition({650, 600});
		}

		if (enterInput) {
		}

		break;
	}
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