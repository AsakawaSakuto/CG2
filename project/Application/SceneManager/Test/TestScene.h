#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"

#include"Application/GameObject/Player/Player.h"
#include"Application/Map/Map.h"
#include <Application/GameObject/Thorn/Thorn.h>
#include <Application/GameObject/Block/Block.h>

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TestScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	// カメラの座標Xをプレイヤーに合わせる
	void UpdateCameraToPlayer();

	// マップチップに基づいてオブジェクトを配置する
	void SpawnObjectsByMapChip(float mag, float mapHeight);

	void SpawnObjectsByMapChip2(float mag, float mapHeight);

private:
	// AppContext
	AppContext* ctx_ = nullptr;

	// InputSystem
	GamePad* gamePad_ = nullptr;
	Input* input_ = nullptr;

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = false;                                       // デバッグカメラ使用フラグ
	void CameraController();

	// Objects
	unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Sprite> sprite_ = make_unique<Sprite>();
	unique_ptr<Player> player_ = make_unique<Player>();
	/*std::vector<std::shared_ptr<Thorn>> thorns_;
	std::vector<std::shared_ptr<Block>> blocks_;*/

	// UtilSystem
	GameTimer gameTimer_;

	// Map
	unique_ptr<Map> map_ = make_unique<Map>();

	//
	Vector3 startPos_ = { 0.0f,0.0f,0.0f };
	Vector3 endPos_ = { 10.0f,0.0f,0.0f };
};
