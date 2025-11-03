#include "TestScene.h"

TestScene::~TestScene() {
    CleanupResources();
}

void TestScene::CleanupResources() {

}

void TestScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void TestScene::Initialize() {
    CleanupResources();
	testParticle_->Initialize(&ctx_->dxCommon);
	camera_.SetPosition({ 0.0f, 0.0f, -10.0f });
	debugCamera_.SetInput(&ctx_->input);
	debugCamera_.SetPosition({ 0.0f, 0.0f, -10.0f });
}

void TestScene::Update() {
	testParticle_->Update();

    camera_ = debugCamera_;
	camera_.Update();
	debugCamera_.Update();
}

void TestScene::Draw() {

    // ここより上に描画処理を書かない
    ctx_->dxCommon.PreDraw();

    ///
    /// ↓描画処理ここから
    ///

	testParticle_->Draw(camera_);

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

	testParticle_->DrawImGui("TestParticle");

    ///
    /// ↑ImGuiここまで
    ///

    // Imguiの内部コマンドを生成する
    ImGui::Render();

    // ここより下に描画処理を書かない
    ctx_->dxCommon.PostDraw();
}