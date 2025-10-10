#include "TitleScene.h"

void TitleScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TitleScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({ 0.0f,0.0f,-10.0f });
	normalCamera_->SetRotate({ 0.0f, 0.0f,0.0f });
	
	sceneFade_ = new SceneFade();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	particle_->Initialize(&ctx_->dxCommon, "circle", 2);
	particle_->LoadJson("temp");
}

void TitleScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneFade_->StartFadeIn(1.0f);
	}
	if (sceneFade_->EndFadeIn()) {
		ChangeScene(SCENE::GAME);
	}

	sceneFade_->Update();

	particle_->Update();

	// カメラ切り替え&更新
	CameraController();
}

void TitleScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw(); 

	///
	/// ↓描画処理ここから
	///

	particle_->Draw(*useCamera_);

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

	DrawSceneName();

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
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