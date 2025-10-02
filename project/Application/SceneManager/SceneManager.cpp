#include "SceneManager.h"

SceneManager::SceneManager() {
	// シーン初期化、Enumと配列の順番を合わせること
    sceneArr_[0] = std::make_unique<TempScene>();
    sceneArr_[1] = std::make_unique<TitleScene>();
    sceneArr_[2] = std::make_unique<GameScene>();

    currentSceneNo_ = TEMP;
    prevSceneNo_ = 0;
}

SceneManager::~SceneManager() {
    // 明示的な解放処理がなければ空でもOK
}

int SceneManager::Run() {
    // COM初期化
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // ヒープ上にAppContextを生成
    appContext_ = std::make_unique<AppContext>();

    // 各種初期化
    appContext_->winApp.Initialize(L"Engine");
    appContext_->winApp.EnableResize(false);
    appContext_->dxCommon.Initialize(&appContext_->winApp);
    TextureManager::GetInstance()->Initialize(&appContext_->dxCommon);
    Logger::Initialize(); std::filesystem::create_directory("logs");
    appContext_->input.Initialize(&appContext_->winApp);
    appContext_->gamePad.Initialize();

    // 各シーンにAppContextを渡す
    for (int i = 0; i < static_cast<int>(sceneNum); i++) {
        sceneArr_[i]->SetAppContext(appContext_.get());
    }

    // 初期シーンのInitialize
    sceneArr_[currentSceneNo_]->Initialize();

    MSG msg = {};
    bool running = true;

    while (running) {
        // メッセージ処理
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) break;

        // 入力更新（共通処理）
        //appContext_->input.Update();
        //appContext_->gamePad.Update();

        if (GetAsyncKeyState(VK_F11) & 1) {
            if (!appContext_->winApp.IsFullscreen()) {
                appContext_->winApp.EnterBorderlessFullscreen();
            } else {
                appContext_->winApp.ExitBorderlessFullscreen();
            }
            appContext_->dxCommon.ResizeToWindow();
        }

        // シーン切り替えチェック
        int nextSceneNo = sceneArr_[currentSceneNo_]->GetSceneNo();

        if (nextSceneNo == -1) {
            running = false;
            break;
        }

        if (nextSceneNo != currentSceneNo_) {
            prevSceneNo_ = currentSceneNo_;
            currentSceneNo_ = nextSceneNo;
            sceneArr_[currentSceneNo_]->SetAppContext(appContext_.get());
            sceneArr_[currentSceneNo_]->Initialize();
        }

        // 更新・描画
        sceneArr_[currentSceneNo_]->Update();
        if (sceneArr_[currentSceneNo_]) {
            sceneArr_[currentSceneNo_]->Draw();
        }
    }

    // 終了処理
    TextureManager::GetInstance()->Finalize();
    appContext_->dxCommon.CloseFence();
    appContext_->winApp.Finalize();
    CoUninitialize();

    return 0;
}
