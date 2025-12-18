#include "GameScene.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {

}

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	gameCamera_->Initialize();

	player_ = make_unique<Player>();
	player_->Initialize();

	enemyManager_->Initialize();

	// プレイヤーにEnemyManagerへの参照を設定
	player_->SetEnemyManager(enemyManager_.get());

	// CollisionManagerを初期化し、PlayerとEnemyManagerとWeaponManagerへの参照を設定
	collisionManager_->Initialize();
	collisionManager_->SetPlayer(player_.get());
	collisionManager_->SetEnemyManager(enemyManager_.get());
	collisionManager_->SetWeaponManager(player_->GetWeaponManager());

	testParticle_->Initialize();
	dustParticle_->Initialize();
	dustParticle_->LoadJson("dust");

	testLine_->Initialize();

	lv_ = make_unique<Sprite>();
	lv_->Initialize("UI/game/lv.png", { 70.0f, 110.0f }, { 0.7f, 0.7f });

	lvText_ = make_unique<Sprite>();
	lvText_->Initialize("UI/game/lvText.png", { 140.0f, 180.0f }, { 0.35f, 0.5f } );

	fireBallIcon_ = make_unique<Sprite>();
	fireBallIcon_->Initialize("icon/FireBall.png", { 275.0f, 175.0f }, { 1.5f, 1.5f });

	leaserIcon_ = make_unique<Sprite>();
	leaserIcon_->Initialize("icon/leaser.png", { 330.0f, 175.0f }, { 1.5f, 1.5f });

	runaIcon_ = make_unique<Sprite>();
	runaIcon_->Initialize("icon/Runa.png", { 385.0f, 175.0f }, { 1.5f, 1.5f });

	text_ = make_unique<Sprite>();
	text_->Initialize("UI/game/text.png", { 0.0f, 0.0f }, { 1.0f, 1.0f });
	textMoveTimer_.Start(2.0f, false);

	// ビットマップフォントの初期化
	timeFont_ = make_unique<BitmapFont>();
	timeFont_->Initialize("number/");
	timeFont_->SetScale({ 0.6f, 0.6f });
	timeFont_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 白色

	playerHPFont_ = make_unique<BitmapFont>();
	playerHPFont_->Initialize("number/");
	playerHPFont_->SetScale({ 0.4f, 0.4f });
	playerHPFont_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 赤色

	playerLv_ = make_unique<BitmapFont>();
	playerLv_->Initialize("number/");
	playerLv_->SetScale({ 0.4f, 0.4f });

	// HPゲージの初期化
	hpGauge_ = make_unique<Gauge>();
	hpGauge_->Initialize();
	hpGauge_->SetGaugeColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑色
	hpGauge_->SetBackgroundColor({ 0.3f, 0.0f, 0.0f, 1.0f }); // 暗い赤

	// 経験値ゲージの初期化
	expGauge_ = make_unique<Gauge>();
	expGauge_->Initialize();
	expGauge_->SetGaugeColor({ 0.0f, 0.8f, 1.0f, 1.0f }); // シアン色
	expGauge_->SetBackgroundColor({ 0.1f, 0.1f, 0.3f, 1.0f }); // 暗い青

	gameTime_ = 0.0f;
	score_ = 0;
}

void GameScene::Update() {
	if (MyInput::Trigger(Action::PAUSE)) {
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

	testLine_->AddGrid(100.0f, 20);

	if (textMoveTimer_.IsActive()) {
		text_->SetPosition({ Easing::Lerp(1780.0f, -500.0f, textMoveTimer_.GetProgress(),Easing::Type::EaseOutInSine), 360.0f });
		textMoveTimer_.Update();
	}

	text_->Update();
	lv_->Update();
	lvText_->Update();
	fireBallIcon_->Update();
	leaserIcon_->Update();
	runaIcon_->Update();

	// ゲーム時間を更新
	gameTime_ += 1.0f / 60.0f; // 仮に60FPSとして計算

	// スコアを更新（例：テスト用に毎フレーム1増加）
	// score_++;

	// ビットマップフォントの更新

	/*timeFont_->SetTime(gameTime_, { 1100.0f, 50.0f }, 60.0f);
	timeFont_->Update();

	playerHPFont_->SetNumber(player_->GetCurrentHP(), { 215.0f,50.0f }, 60.0f);
	playerHPFont_->Update();

	playerLv_->SetNumber(player_->GetLevel(), {122.0f,112.0f}, 55.0f);
	playerLv_->SetScale({0.5f,0.5f});
	playerLv_->Update();*/

	// ゲージの更新
	hpGauge_->Update(player_->GetCurrentHP(), player_->GetMaxHP());
	expGauge_->Update(player_->GetCurrentExp(), player_->GetExpToNextLevel());
}

void GameScene::Draw() {
	testLine_->Draw(camera_);

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	testParticle_->Draw(camera_);
	dustParticle_->Draw(camera_);

	collisionManager_->Draw(camera_);

	text_->Draw();
	lv_->Draw();
	lvText_->Draw();
	fireBallIcon_->Draw();
	leaserIcon_->Draw();
	runaIcon_->Draw();

	// ゲージの描画
	hpGauge_->Draw();
	expGauge_->Draw();

	// ビットマップフォントの描画
	timeFont_->Draw();
	playerHPFont_->Draw();
	playerLv_->Draw();
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