#include"TitleScene.h"
#include"GameScene.h"

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene {
	kTitle,
	kGame
};

Scene scene = Scene::kTitle;

void ChangeScene();

void UpdateScene();

void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// シーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// ゲームループ
	while (true) {

		ChangeScene();

		// 更新
		UpdateScene();

		// 描画
		DrawScene();

	} // メインループ外

	// 終了
	delete titleScene;
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;
	titleScene = nullptr;

	return 0;
}

void ChangeScene() {
	switch (scene)
	{
	case Scene::kTitle:

		if (titleScene->SceneChange()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
		}

		break;
	case Scene::kGame:

		if (gameScene->SceneChange()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}

		break;
	}
};

void UpdateScene() {
	switch (scene)
	{
	case Scene::kTitle:

		titleScene->Update();

		if (titleScene->IsEndRequst()) {
			break;
		}

		break;
	case Scene::kGame:

		gameScene->Update();

		if (gameScene->IsEndRequst()) {
			break;
		}

		break;
	}
};

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
};