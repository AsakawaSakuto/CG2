#include "TempScene.h"

void TempScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TempScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({ 0.0f,0.0f,-10.0f });
	normalCamera_->SetRotate({ 0.0f, 0.0f,0.0f });
}

void TempScene::Update() {
	// inputSystemの更新
	gamePad_->Update();
	ctx_->input.Update();

	// カメラ切り替え&更新
	CameraController();
}

void TempScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

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

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void TempScene::CameraController() {
	if (useDebugCamera_) {
		if (debugCamera_ != nullptr) {
			debugCamera_->Update();
			useCamera_ = debugCamera_.get();
		}
	}
	else {
		if (normalCamera_ != nullptr) {
			normalCamera_->Update();
			useCamera_ = normalCamera_.get();
		}
	}
}