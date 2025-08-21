#include"TutorialScene.h"

void TutorialScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TutorialScene::Initialize() {

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon);
}

void TutorialScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();
}

void TutorialScene::Draw() {

	if (!ctx_) {
		OutputDebugStringA("ctx_ is nullptr\n");
		return;
	}

	if (!ctx_->dxCommon.GetCommandList()) {
		OutputDebugStringA("commandList_ is nullptr\n");
		return;
	}

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

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