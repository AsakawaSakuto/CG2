#include"AppContext.h"
#include"TitleScene.h"
#include"GameScene.h"

AppContext ctx;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene {
	kTitle,
	kGame,
	kTutorial,
	kResult
};

Scene scene = Scene::kTitle;

void ChangeScene();

void UpdateScene();

void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// // プロセスで1回だけ
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	ctx.winApp.Initialize(L"CG2_Window");
	ctx.winApp.EnableResize(true);
	ctx.dxCommon.Initialize(&ctx.winApp);
	TextureManager::GetInstance()->Initialize(&ctx.dxCommon);
	Logger::Initialize(); std::filesystem::create_directory("logs");
	ctx.input.Initialize(&ctx.winApp);
	ctx.gamePad.Initialize();

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

		if (titleScene->GoGameScene()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene(&ctx);
			gameScene->Initialize();
		}

		break;
	case Scene::kGame:

		if (gameScene->GoTitleScene()) {
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

		break;
	case Scene::kGame:

		gameScene->Update();

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