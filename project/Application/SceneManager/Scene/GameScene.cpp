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

	testParticle_->Initialize(&ctx_->dxCommon, 200);
	dustParticle_->Initialize(&ctx_->dxCommon);
	dustParticle_->LoadJson("dust");

	testLine_->Initialize(&ctx_->dxCommon);

	text_ = make_unique<Sprite>();
	text_->Initialize(&ctx_->dxCommon, "UI/game/text.png", { 0.0f, 0.0f }, { 1.0f, 1.0f });
	textMoveTimer_.Start(2.0f, false);
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

	testParticle_->Update();
	dustParticle_->Update();

	if (collisionManager_->GetGoResult() || ctx_->gamePad.TriggerButton(GamePad::X)) {
		//ChangeScene(SCENE::RESULT);
	}

	testLine_->AddGrid(100.0f, 20);

	if (textMoveTimer_.IsActive()) {
		text_->SetPosition({ Easing::Lerp(1780.0f, -500.0f, textMoveTimer_.GetProgress(),Easing::Type::EaseOutInSine), 360.0f });
		textMoveTimer_.Update();
	}
	text_->Update();
}

void GameScene::Draw() {
	testLine_->Draw(camera_);

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	testParticle_->Draw(camera_);
	dustParticle_->Draw(camera_);

	collisionManager_->Draw(camera_);

	text_->Draw();
}

void GameScene::DrawImGui() {
#ifdef USE_IMGUI
	//auto postEffect = ctx_->dxCommon.GetPostEffectManager();
	//postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	//postEffect->DrawImGui();
#endif // USE_IMGUI

	//gridModel_->DrawImGui("grid");
	//player_->DrawImGui();
	//enemyManager_->DrawImGui();

	testParticle_->DrawImGui("TestParticle");
}

void GameScene::PostFrameCleanup() {
	player_->PostFrameCleanup();
}