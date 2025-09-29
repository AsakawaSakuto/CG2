#include "TestScene.h"

void TestScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TestScene::Initialize() {
	debugCamera_->SetInput(&ctx_->input);
	camera_->SetPosition({ 4.0f,6.0f,1.6f });
	camera_->SetRotate({ 0.3f,-0.4f,0.0f });

	gamePad_ = &ctx_->gamePad;

	pModelMove_ = false;
	isStart_ = false;
	startTimer_ = 0.0f;
}

void TestScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

}

void TestScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

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

	//titleLogo_->DrawImGui("f");
	//debugCamera_->DrawImgui();
	//engineFire_->DrawImGui("f");

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TestScene::CameraController() {
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