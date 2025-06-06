#include"GameScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	GameScene gameScene;

	// シーンの初期化
	gameScene.Initialize();

	// ゲームループ
	while (true) {
		// 更新
		gameScene.Update();

		if (gameScene.IsEndRequst()) {
			break;
		}
		// 描画
		gameScene.Draw();

	} // メインループ外

	// 終了
	gameScene.Finalize();

	return 0;
}
