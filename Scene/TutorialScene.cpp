#include"TutorialScene.h"

void TutorialScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	player_->Initialize(&ctx_->dxCommon);

	skyBox_->Initialize(&ctx_->dxCommon);
	skyBox_->SetTWallexture("resources/image/wall.png");

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon, "resources/image/fade.png", { 1280.0f,720.0f });
	fade_->SetPosition({ 640.0f,360.0f });
	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });

	pauseBG_->Initialize(&ctx_->dxCommon, "resources/image/pauseUI1.png", { 1280.0f,720.0f });
	pauseBG_->SetPosition({ 640.0f,360.0f });

	pauseUI_->Initialize(&ctx_->dxCommon, "resources/image/pauseUI2.png", { 1280.0f,720.0f });
	pauseUI_->SetPosition({ 640.0f,360.0f });
}

void TutorialScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();

	if (gamePad_->TriggerButton(GamePad::X)) {
		player_->Heal();
	}
	if (gamePad_->TriggerButton(GamePad::Y)) {
		player_->Damage();
	}

	CameraController();

	if (!isPause_) {
		player_->Update(useCamera_);
		skyBox_->Update(useCamera_);
	}
	
	pauseBG_->Update();
	pauseUI_->Update();

	UpdatePause();

	UpdateFade();
}

void TutorialScene::Draw() {
	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	skyBox_->Draw();
	player_->Draw();

	if (isPause_) {
		pauseBG_->Draw();
		pauseUI_->Draw();
	}

	fade_->Draw();

	///
	/// ↑描画処理ここまで
	///

	///
	/// ↓ImGuiここから
	///

	// フレームの先頭でImguiにここからフレームが始まる旨を告げる
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// 開発用UIの処理、実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
	/*ImGui::ShowDemoWindow();*/

	debugCamera_->DrawImgui();

	player_->DrawImGui();

	//skyBox_->DrawImGui();

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TutorialScene::UpdateFade() {
	if (isFade_) {
		fadeAlpha_ += 0.5f * deltaTime_;
		if (fadeAlpha_ >= 1.0f) {
			goTitleScene_ = true;
		}
	} else {
		fadeAlpha_ -= 0.5f * deltaTime_;
	}
	fadeAlpha_ = std::clamp(fadeAlpha_, 0.0f, 1.0f);

	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });
	fade_->Update();
}

void TutorialScene::UpdatePause() {
	if (isPause_) {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = false;
		}

		switch (pause_)
		{
		case TutorialScene::kBack:
			pauseUI_->SetTexture("resources/image/pauseUI2.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				pause_ = kQuit;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isPause_ = false;
			}
			break;
		case TutorialScene::kQuit:
			pauseUI_->SetTexture("resources/image/pauseUI3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				pause_ = kBack;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isFade_ = true;
			}
			break;
		}

	} else {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = true;
		}
		pause_ = kBack;
	}
}

void TutorialScene::CameraController() {
	if (ctx_->input.TriggerKey(DIK_SPACE)) {
		if (isDebugCamera_) {
			isDebugCamera_ = false;
		}
		else {
			isDebugCamera_ = true;
		}
	}

	if (isDebugCamera_) {
		if (debugCamera_ != nullptr) {
			debugCamera_->Update();
			useCamera_ = debugCamera_.get();
		}
	}
	else {
		if (camera_ != nullptr) {
			camera_->Update();
			useCamera_ = camera_.get();
		}
	}
}