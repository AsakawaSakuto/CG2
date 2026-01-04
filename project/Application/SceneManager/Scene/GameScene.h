#pragma once
#include "EngineSystem.h"
#include "SceneManager/IScene.h"
#include "GameObject/Player/player.h"
#include "GameCameraCntroller/GameCameraController.h"
#include "GameObject/EnemyManager/EnemyManager.h"
#include "CollisionManager/CollisionManager.h"
#include "2d/BitmapFont/BitmapFont.h"
#include "2d/Gauge/Gauge.h"
#include "Map/Map3D.h"
#include "Map/JarManager/JarManager.h"
#include "Map/ChestManager/ChestManager.h"
#include "Map/TreeManager/TreeManager.h"
#include "GameUI/GameSceneUI/GameSceneUI.h"

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

	void TempMap();

	void JarUpdate();
	
	void ChestUpdate();

	void UIUpdate();

	bool cameraDebugMode_ = false;
private:
	int dangerLv_ = 0;

	Camera camera_;
	DebugCamera debugCamera_;

	GameTimer playTimer_;

	unique_ptr<GameSceneUI> gameSceneUI_;

	// プレイヤー関連のクラス
	unique_ptr<Player> player_;
	unique_ptr<GameCameraController> gameCamera_ = make_unique<GameCameraController>();

	// 敵管理クラス
	unique_ptr<EnemyManager> enemyManager_ = make_unique<EnemyManager>();

	// 衝突判定マネージャー
	unique_ptr<CollisionManager> collisionManager_ = make_unique<CollisionManager>();

	// 3Dマップ
	unique_ptr<Map3D> map3D_;

	// 壺管理クラス
	unique_ptr<JarManager> jarManager_ = make_unique<JarManager>();

	// 宝箱管理クラス
	unique_ptr<ChestManager> chestManager_ = make_unique<ChestManager>();

	// 木管理クラス
	unique_ptr<TreeManager> treeManager_ = make_unique<TreeManager>();

	bool useFog_ = false;

	std::unique_ptr<Model> wall_;
};