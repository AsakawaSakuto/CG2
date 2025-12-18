#pragma once
#include "EngineSystem.h"
#include "SceneManager/IScene.h"
#include "GameObject/Player/player.h"
#include "GameCameraCntroller/GameCameraController.h"
#include "GameObject/EnemyManager/EnemyManager.h"
#include "CollisionManager/CollisionManager.h"
#include "2d/BitmapFont/BitmapFont.h"
#include "2d/Gauge/Gauge.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class GameScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void PostFrameCleanup() override;
	~GameScene();
private:
	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	
	Camera camera_;
	DebugCamera debugCamera_;

	unique_ptr<Model> gridModel_ = make_unique<Model>();
	Transform gridTransform_;

	// プレイヤー関連のクラス
	unique_ptr<Player> player_;
	unique_ptr<GameCameraController> gameCamera_ = make_unique<GameCameraController>();

	unique_ptr<EnemyManager> enemyManager_ = make_unique<EnemyManager>();

	// 衝突判定マネージャー
	unique_ptr<CollisionManager> collisionManager_ = make_unique<CollisionManager>();

	unique_ptr<Particles> testParticle_ = make_unique<Particles>();

	unique_ptr<Particles> dustParticle_ = make_unique<Particles>();

	unique_ptr<Sprite> text_;
	unique_ptr<Sprite> lv_;
	unique_ptr<Sprite> lvText_;
	unique_ptr<Sprite> fireBallIcon_;
	unique_ptr<Sprite> leaserIcon_;
	unique_ptr<Sprite> runaIcon_;

	GameTimer textMoveTimer_;

	unique_ptr<Line3d> testLine_ = make_unique<Line3d>();

	// ビットマップフォント（数字表示用）
	unique_ptr<BitmapFont> timeFont_;
	unique_ptr<BitmapFont> playerHPFont_;
	unique_ptr<BitmapFont> playerLv_;

	float gameTime_ = 0.0f;
	int score_ = 0;
	GameTimer gameTimer_;

	// ゲージ（HP・経験値表示用）
	unique_ptr<Gauge> hpGauge_;
	unique_ptr<Gauge> expGauge_;
};