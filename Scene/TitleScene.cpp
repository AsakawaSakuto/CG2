#include "TitleScene.h"

void TitleScene::Initialize() {
	debugCamera_->SetInput(&ctx_->input);
	gamePad_ = &ctx_->gamePad;

	titleUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/title1.png", { 1280.0f,720.0f });
	titleUI_->SetPosition({ 640.0f,360.0f });

	loadingUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/loading.png", { 1280.0f,720.0f });
	loadingUI_->SetPosition({ 640.0f,360.0f });
	loadingUI_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });

	fade_->Initialize(&ctx_->dxCommon, "resources/image/fade.png", { 1280.0f,720.0f });
	fade_->SetPosition({ 640.0f,360.0f });
	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });
}

void TitleScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	SceneController();

	titleUI_->Update();

	UpdateFade();
}

void TitleScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	titleUI_->Draw();
	fade_->Draw();
	loadingUI_->Draw();

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
	fade_->DrawImGui("q");

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TitleScene::UpdateFade() {
	if (isFade_) {
		fadeAlpha_ += 0.5f * deltaTime_;
		if (fadeAlpha_ >= 1.0f) {
			if (state_ == kPlay) {
				goGameScene_ = true;
			}
			else if (state_ == kTutorial) {
				goTutorialScene_ = true;
			}
		}
	} else {
		fadeAlpha_ -= 0.5f * deltaTime_;
	}

	fadeAlpha_ = std::clamp(fadeAlpha_, 0.0f, 1.0f);

	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });
	fade_->Update();

	loadingUI_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });
	loadingUI_->Update();
}

void TitleScene::SceneController() {
	if (!isFade_) {
		switch (state_)
		{
		case TitleScene::kPlay:

			titleUI_->SetTexture("resources/image/UI/title1.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				state_ = kTutorial;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isFade_ = true;
			}

			break;
		case TitleScene::kTutorial:

			titleUI_->SetTexture("resources/image/UI/title2.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				state_ = kQuit;
			}
			if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				state_ = kPlay;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isFade_ = true;
			}

			break;
		case TitleScene::kQuit:

			titleUI_->SetTexture("resources/image/UI/title3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				state_ = kTutorial;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				goQuit_ = true;
			}

			break;
		}
	}
}

void TitleScene::CameraController() {
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