#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"
#include"Application/GameObject/Player/player.h"
#include"Application/GameCameraCntroller/GameCameraController.h"
#include"Application/GameObject/EnemyManager/EnemyManager.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class GameScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	~GameScene();
private:
	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	// AppContext
	AppContext* ctx_ = nullptr;

	Camera camera_;
	DebugCamera debugCamera_;

	unique_ptr<Model> testPlane_ = make_unique<Model>();
	unique_ptr<Model> testWall_ = make_unique<Model>();

	// プレイヤー関連のクラス
	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<GameCameraController> gameCamera_ = make_unique<GameCameraController>();

	unique_ptr<EnemyManager> enemyManager_ = make_unique<EnemyManager>();
};