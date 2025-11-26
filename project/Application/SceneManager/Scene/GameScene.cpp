#include "GameScene.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {

}

void GameScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	gameCamera_->Initialize(ctx_);
	player_->Initialize(ctx_);

	enemyManager_->Initialize(ctx_);

	// プレイヤーにEnemyManagerへの参照を設定
	player_->SetEnemyManager(enemyManager_.get());

	// CollisionManagerを初期化し、PlayerとEnemyManagerとWeaponManagerへの参照を設定
	collisionManager_->Initialize(ctx_);
	collisionManager_->SetPlayer(player_.get());
	collisionManager_->SetEnemyManager(enemyManager_.get());
	collisionManager_->SetWeaponManager(player_->GetWeaponManager());

	testParticle_->Initialize(&ctx_->dxCommon);

	gridModel_->Initialize(&ctx_->dxCommon, "grid/grid.obj");
	gridModel_->SetTexture("resources/image/uvChecker.png");
	gridTransform_.translate = { 0.0f,-0.5f,0.0f };
	gridModel_->SetColor4({ 1.0f,1.0f,1.0f,0.05f });
}

void GameScene::Update() {
	if (ctx_->gamePad.TriggerButton(GamePad::START)) {
		ChangeScene(SCENE::TITLE);
	}

	player_->Update();

	gameCamera_->SetTarget(player_->GetPosition());
	gameCamera_->Update();

	// CollisionManagerで衝突判定を実行
	collisionManager_->Update();

	enemyManager_->SetTargetPosition(player_->GetPosition());
	enemyManager_->Update();

	camera_ = gameCamera_->GetCamera();
	camera_.Update();

	gridModel_->Update();

	testParticle_->Update();

	if (collisionManager_->GetGoResult() || ctx_->gamePad.TriggerButton(GamePad::X)) {
		//ChangeScene(SCENE::RESULT);
	}
}

void GameScene::Draw() {
	gridModel_->Draw(camera_, gridTransform_);

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	testParticle_->Draw(camera_);

	collisionManager_->Draw(camera_);
}

void GameScene::DrawImGui() {

	auto postEffect = ctx_->dxCommon.GetPostEffectManager();
	postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	postEffect->DrawImGui();

	gridModel_->DrawImGui("grid");

	player_->DrawImGui();

	enemyManager_->DrawImGui();

	testParticle_->DrawImGui("fire");
}

void GameScene::PostFrameCleanup() {
	player_->PostFrameCleanup();
}