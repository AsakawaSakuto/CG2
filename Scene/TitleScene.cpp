#include "TitleScene.h"

void TitleScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TitleScene::Initialize() {
	debugCamera_->SetInput(&ctx_->input);
	camera_->SetPosition({ 0.0f,0.0f,0.0f });

	gamePad_ = &ctx_->gamePad;

	titleUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/title1.png", { 1280.0f,720.0f });
	titleUI_->SetPosition({ 640.0f,360.0f });

	building_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-h.obj");

	fade_->Initialize(&ctx_->dxCommon);

	state_ = kPlay;
}

void TitleScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	SceneController();

	titleUI_->Update();

	building_->Update(*useCamera_);

	UpdateFade();
}

void TitleScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	building_->Draw();

	titleUI_->Draw();
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
	building_->DrawImGui("b");
	camera_->DrawImgui();
	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TitleScene::UpdateFade() {

	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		if (state_ == kPlay) {
			IScene::sceneNo = GAME;
		} else if (state_ == kTutorial) {
			IScene::sceneNo = TUTORIAL;
		}
	}

	fade_->Update();
}

void TitleScene::SceneController() {
	if (!fade_->GetIsFade() && fade_->GetFadeAlpha() <= 0.0f) {
		switch (state_)
		{
		case TitleScene::kPlay:

			titleUI_->SetTexture("resources/image/UI/title1.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				state_ = kTutorial;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				fade_->SetIsFade(true);
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
				fade_->SetIsFade(true);
			}

			break;
		case TitleScene::kQuit:

			titleUI_->SetTexture("resources/image/UI/title3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				state_ = kTutorial;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				IScene::sceneNo = -1;
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