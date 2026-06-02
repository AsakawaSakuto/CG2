#include "TestScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"

#define WHITE {1.0f,1.0f,1.0f,1.0f}
#define RED   {1.0f,0.0f,0.0f,1.0f}

TestScene::~TestScene() {
    CleanupResources();
}

void TestScene::CleanupResources() {

}

void TestScene::Initialize() {
	nowSceneName_ = SCENE::TEST;

	CleanupResources();

	debugCamera_.SetInput(MyInput::GetInput());

	// SkyBoxの初期化
	skyBox_.Initialize("resources/Image/rostock_laage_airport_4k.dds");
	skyBox_.SetScale(1000.0f);

	model_.Initialize("ball.obj");
	model_.SetEnvironmentMap("resources/Image/rostock_laage_airport_4k.dds");

	// --- Effect 初期化 ---
	hitEffect_   = EffectFactory::CreateHitEffect();
	hitEffect_->Initialize();

	slashEffect_ = EffectFactory::CreateSlashEffect();
	slashEffect_->Initialize();

	// --- Ring 初期化 ---
	// テクスチャの AddressV は Sprite RootSignature で CLAMP 設定済み
	ring_.Initialize("resources/image/white16x16.png", 32, 1.0f, 0.3f);
	ring_.GetTransform().translate = { 0.0f, 0.0f, 0.0f };
	ring_.GetTransform().scale     = { 1.0f, 1.0f, 1.0f };
}

void TestScene::Update() {
	debugCamera_.Update();

	const float deltaTime = ::GetDeltaTime();

	// Space : HitEffect 再生
	if (MyInput::GetInput()->TriggerKey(DIK_SPACE)) {
		hitEffect_->Play(effectSpawnPos_);
	}
	// F : SlashEffect 再生
	if (MyInput::GetInput()->TriggerKey(DIK_F)) {
		slashEffect_->Play(effectSpawnPos_);
	}

	hitEffect_->Update(deltaTime);
	slashEffect_->Update(deltaTime);

	// --- Ring UVスクロール更新 ---
	uvScrollTimer_ += deltaTime * uvScrollSpeed_;
	ring_.SetUvScrollX(uvScrollTimer_);

	MyDebugLine::AddGrid(100.0f, 10, WHITE);
}

void TestScene::Draw() {
	// SkyBoxを最初に描画（遠景）
	//skyBox_.Draw(debugCamera_);

	//model_.Draw(debugCamera_);

	// --- Ring 描画 ---
	ring_.Draw(debugCamera_);

	// --- Effect 描画（加算合成のため不透明パスに追増） ---
	hitEffect_->Draw(debugCamera_);
	slashEffect_->Draw(debugCamera_);

	MyDebugLine::Draw(debugCamera_);
}

void TestScene::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Effect Test");
	ImGui::Text("[SPACE] Play HitEffect");
	ImGui::Text("[F]     Play SlashEffect");
	ImGui::Separator();
	ImGui::DragFloat3("SpawnPos", &effectSpawnPos_.x, 0.1f);
	ImGui::Separator();
	ImGui::Text("HitEffect   : %s", hitEffect_->IsPlaying()   ? "Playing" : "Stopped");
	ImGui::Text("SlashEffect : %s", slashEffect_->IsPlaying() ? "Playing" : "Stopped");
	ImGui::Separator();
	ImGui::DragFloat("UV Scroll Speed", &uvScrollSpeed_, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	ring_.DrawImGui("Ring");

	hitEffect_->DrawImGui("HitEffect Params");
	slashEffect_->DrawImGui("SlashEffect Params");
#endif
}

void TestScene::PostFrameCleanup() {

}