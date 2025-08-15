#include"TutorialScene.h"

void TutorialScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	player_->Initialize(&ctx_->dxCommon);

	skyBox_->Initialize(&ctx_->dxCommon);
	skyBox_->SetTWallexture("resources/image/wall.png");

	gamePad_ = &ctx_->gamePad;
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
	if (gamePad_->TriggerButton(GamePad::A)) {
		goTitle_ = true;
	}

	CameraController();

	player_->Update(useCamera_);
	skyBox_->Update(useCamera_);
}

void TutorialScene::Draw() {
	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	skyBox_->Draw();
	player_->Draw();

	//ground_->Draw();

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