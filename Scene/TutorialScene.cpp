#include"TutorialScene.h"

void TutorialScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	player_->Initialize(&ctx_->dxCommon);

	skyBox_->Initialize(&ctx_->dxCommon);
	skyBox_->SetTWallexture("resources/image/wall.png");

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon);

	pauseBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause1.png", { 1280.0f,720.0f });
	pauseBG_->SetPosition({ 640.0f,360.0f });

	pauseUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause2.png", { 1280.0f,720.0f });
	pauseUI_->SetPosition({ 640.0f,360.0f });

	testUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/tutoUI1.png", { 1280.0f,128.0f });
	testUI_->SetPosition({ 640.0f,-128.0f });
}

void TutorialScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	UpdatePause();

	if (!isPause_) {
		player_->Update(useCamera_);
		skyBox_->Update(useCamera_);
	}
	
	UpdateTutorialTest();

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
	} else {
		testUI_->Draw();
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

	ImGui::Text("%d", &testState_);

	//skyBox_->DrawImGui();

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TutorialScene::UpdateFade() {
	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		goTitleScene_ = true;
	}
	
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
			pauseUI_->SetTexture("resources/image/UI/pause2.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				pause_ = kQuit;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isPause_ = false;
			}
			break;
		case TutorialScene::kQuit:
			pauseUI_->SetTexture("resources/image/UI/pause3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				pause_ = kBack;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				fade_->SetIsFade(true);
			}
			break;
		}

	} else {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = true;
		}
		pause_ = kBack;
	}

	pauseBG_->Update();
	pauseUI_->Update();
}

void TutorialScene::UpdateTutorialTest() {
	switch (testState_) {
	case Test1:
		lX_ = gamePad_->LeftStickX();
		lY_ = gamePad_->LeftStickY();

		if (testUIPos_.y >= 128.0f) {
			if (lX_ != 0.0f || lY_ != 0.0f) {
				testUIClear_ = true;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test2;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI2.png");
			}
		} else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test2:
		rX_ = gamePad_->RightStickX();
		rY_ = gamePad_->RightStickY();

		if (testUIPos_.y >= 128.0f) {
			if (rX_ != 0.0f || rY_ != 0.0f) {
				testUIClear_ = true;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test3;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI3.png");
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test3:
		if (testUIPos_.y >= 128.0f) {
			if (gamePad_->PushButton(GamePad::R)) {
				timer_ += deltaTime_;
				if (timer_ >= 0.25f) {
					testUIClear_ = true;
					timer_ = 0.0f;
				}
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test4;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI4.png");
				timer_ = 0.0f;
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test4:
		if (testUIPos_.y >= 128.0f) {
			if (gamePad_->PushButton(GamePad::L)) {
				timer_ += deltaTime_;
			}

			if (timer_ >= 1.5f && gamePad_->ReleaseButton(GamePad::L)) {
				testUIClear_ = true;
				timer_ = 0.0f;
			} else if (timer_ < 1.5f && gamePad_->ReleaseButton(GamePad::L)) {
				timer_ = 0.0f;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test5;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI5.png");
				timer_ = 0.0f;
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test5:
		if (testUIPos_.y >= 128.0f) {
			if (gamePad_->RightTrigger()>=1.0f|| gamePad_->LeftTrigger() >= 1.0f) {
				testUIClear_ = true;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test5;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI5.png");
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	}
	testUIPos_.y = std::clamp(testUIPos_.y, -128.0f, 128.0f);
	testUI_->SetPosition(testUIPos_);
	testUI_->Update();
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