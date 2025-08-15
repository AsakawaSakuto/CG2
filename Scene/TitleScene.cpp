#include "TitleScene.h"

void TitleScene::Initialize() {
	debugCamera_->SetInput(&ctx_->input);
	gamePad_ = &ctx_->gamePad;
	test_->Initialize(&ctx_->dxCommon, "resources/image/1.png", { 128.0f,128.0f });
}

void TitleScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	switch (state_)
	{
	case TitleScene::kPlay:

		test_->SetTexture("resources/image/1.png");

		if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
			state_ = kTutorial;
		}

		if (gamePad_->TriggerButton(GamePad::A)) {
			goGameScene_ = true;
		}

		break;
	case TitleScene::kTutorial:

		test_->SetTexture("resources/image/2.png");

		if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
			state_ = kQuit;
		}
		if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
			state_ = kPlay;
		}

		if (gamePad_->TriggerButton(GamePad::A)) {
			goTutorialScene_ = true;
		}

		break;
	case TitleScene::kQuit:

		test_->SetTexture("resources/image/3.png");

		if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
			state_ = kTutorial;
		}

		if (gamePad_->TriggerButton(GamePad::A)) {
			goQuit_ = true;
		}

		break;
	}
	test_->Update();
}

void TitleScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	test_->Draw();

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

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TitleScene::Finalize() {
	
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