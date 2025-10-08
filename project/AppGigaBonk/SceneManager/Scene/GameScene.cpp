#include "GameScene.h"
#include "AppGigaBonk/GameUI/SceneFade/SceneFade.h"

void GameScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void GameScene::Initialize() {
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

	ground_->Initialize(&ctx_->dxCommon, "resources/gigabonk/model/ground.obj");
	player_->Initialize(&ctx_->dxCommon);
	player_->SetInputSystem(&ctx_->input);
}

void GameScene::Update() {

	if (input_->TriggerKey(DIK_ESCAPE)) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::TITLE;
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::RESULT;
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(goSceneNum_);
	}

	sceneFade_->Update();

	player_->Update();

	ground_->Update();

	// カメラ切り替え&更新
	CameraController();
}

void GameScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	player_->Draw(*useCamera_);

	ground_->Draw(*useCamera_);

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

	DrawSceneName();

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void GameScene::CameraController() {
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