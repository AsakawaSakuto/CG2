#include "TestScene.h"

void TestScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TestScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({ 0.0f,0.0f,-30.0f });
	normalCamera_->SetRotate({ 0.0f, 0.0f,0.0f });

	// オブジェクトの初期化
	model_->Initialize(&ctx_->dxCommon, "resources/model/bg.obj");
	sprite_->Initialize(&ctx_->dxCommon, "resources/image/uvChecker.png", { 128.0f,128.0f });
	sprite_->SetPosition({ 640.0f, 360.0f });

	player_->Initialize(&ctx_->dxCommon);
	player_->SetInputSystem(&ctx_->input);

	// 汎用機能
	gameTimer_.Start(2.0f, true);
}

void TestScene::Update() {

	// カメラ切り替え&更新
	CameraController();

	// オブジェクトの更新
	model_->Update();
	sprite_->Update();

    player_->Update();

	// カメラの座標Yをプレイヤーの座標Yに合わせる
	UpdateCameraToPlayer();

	// 汎用機能の更新
	gameTimer_.Update();
}

void TestScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	player_->Draw(*useCamera_);

	model_->Draw(*useCamera_);
	//sprite_->Draw();

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

	//model_->DrawImGui("Model");
	//sprite_->DrawImGui("Sprite");

	debugCamera_->DrawImgui();

	// プレイヤーのImGui
	player_->DrawImgui();

	DrawSceneName();

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void TestScene::UpdateCameraToPlayer() {
	// カメラの座標Yをプレイヤーの座標Yに合わせる
	Vector3 pPos = player_->GetPosition();
	normalCamera_->SetPosition({0.0f, pPos.y + player_->CameraOffset(), -30.0f});
}

void TestScene::CameraController() {
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