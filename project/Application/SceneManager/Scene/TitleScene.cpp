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
	
	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	particle_->Initialize(&ctx_->dxCommon, "circle");
	particle_->LoadJson("temp");

	// Text3Dの初期化
	for (auto& text : text3D_) {
		text = make_unique<Text3D>();
		text->Initialize(&ctx_->dxCommon);
	}

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

	// Text3Dの更新処理
	for (auto& text : text3D_) {
		text->MoveTextAnimation(0.0f, 10.0f, std::numbers::pi_v<float> * 2, 0.0f);
		text->Update();
	}
}

void TitleScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw(); 

	///
	/// ↓描画処理ここから
	///

	// 3Dテキストの描画処理
	for (auto& text : text3D_) {
		text->Draw(*useCamera_);
	}

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