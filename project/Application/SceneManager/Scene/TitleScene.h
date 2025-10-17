#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"
#include "Application/GameObject/Text3D/Text3D.h"
#include <array>

// タイトル画面のモード
enum class Screen
{
	FIRST,
	SECOND,
};

// タイトルのメニュー
enum class Menu
{
	IN_GAME,
	TUTORIAL,
	OPTION,
};

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TitleScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~TitleScene();

private:
	// メニューの状態切り替え(カーソルの更新)
	void UpdateMenu();

	// リソースクリーンアップメソッド
	void CleanupResources();

private:

	unique_ptr<Particles> particle_ = make_unique<Particles>();
	unique_ptr<Particles> particle2_ = make_unique<Particles>();

	unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Model> o_ = make_unique<Model>();

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ
	void CameraController();     

	// 3Dテキスト
	std::array<std::unique_ptr<Text3D>, 5> text3D_;

	// タイトル画面のモード
	Screen currentScreen_ = Screen::FIRST;

	// タイトルのメニュー
	Menu currentMenu_ = Menu::IN_GAME;

	// 背景スプライト
	unique_ptr<Sprite> spriteBG_ = make_unique<Sprite>();

	// インゲーム遷移スプライト
	unique_ptr<Sprite> spriteInGame_ = make_unique<Sprite>();

	// チュートリアルスプライト
	unique_ptr<Sprite> spriteTutorial_ = make_unique<Sprite>();

	// オプションスプライト
	unique_ptr<Sprite> spriteOption_ = make_unique<Sprite>();

	Vector2 testPos_{};
	Vector2 testScale_{};
};
