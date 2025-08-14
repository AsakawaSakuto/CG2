#include"AppContext.h"
#include"TitleScene.h"
#include"GameScene.h"

AppContext ctx;

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

	// // プロセスで1回だけ
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);                   // 追加
	// 追加
	ctx.winApp.Initialize(L"CG2_Window");                            // 追加
	ctx.winApp.EnableResize(true);                                   // 追加
	ctx.dxCommon.Initialize(&ctx.winApp);                            // 追加
	TextureManager::GetInstance()->Initialize(&ctx.dxCommon);        // 追加
	Logger::Initialize(); std::filesystem::create_directory("logs"); // 追加
	ctx.input.Initialize(&ctx.winApp);                               // 追加
	ctx.gamePad.Initialize();                                        // 追加

	// シーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene(&ctx);
	titleScene->Initialize();

	// ゲームループ
	MSG msg = {};
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) { goto APP_QUIT; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ChangeScene();

		// 更新
		UpdateScene();

		// 描画
		DrawScene();

	} // メインループ外

APP_QUIT:

	// 終了
	delete titleScene;
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;
	titleScene = nullptr;

	TextureManager::GetInstance()->Finalize();
	ctx.dxCommon.CloseFence();
	ctx.winApp.Finalize();
	CoUninitialize();

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
			gameScene = new GameScene(&ctx);
			gameScene->Initialize();
		}

		break;
	case Scene::kGame:

		if (gameScene->SceneChange()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene(&ctx);
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