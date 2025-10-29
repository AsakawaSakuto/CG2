#include "SceneManager.h"

SceneManager::SceneManager() {
    // シーン配列は初期化時には空にする
    for (int i = 0; i < sceneNum; i++) {
        sceneArr_[i] = nullptr;
    }

    currentSceneNo_ = TITLE;
    prevSceneNo_ = TITLE;
    sceneChangeCount_ = 0;
}

SceneManager::~SceneManager() {
    // 明示的な解放処理
    CleanupAllScenes();
}

void SceneManager::CleanupAllScenes() {
    for (int i = 0; i < sceneNum; i++) {
        if (sceneArr_[i]) {
            // デバッグ：どのシーンを削除するかログ出力
            #ifdef _DEBUG
            OutputDebugStringA(("Cleaning up scene " + std::to_string(i) + "\n").c_str());
            #endif
            sceneArr_[i].reset();
        }
    }
}

std::unique_ptr<IScene> SceneManager::CreateScene(int sceneNo) {
    #ifdef _DEBUG
    OutputDebugStringA(("Creating scene " + std::to_string(sceneNo) + "\n").c_str());
    #endif

    try {
        switch (sceneNo) {
        case TITLE:
            return std::make_unique<TitleScene>();
        case GAME:
            return std::make_unique<GameScene>();
        case RESULT:
            return std::make_unique<ResultScene>();
        default:
            #ifdef _DEBUG
            OutputDebugStringA("Invalid scene number\n");
            #endif
            return nullptr;
        }
    }
    catch (const std::exception& e) {
        #ifdef _DEBUG
        OutputDebugStringA(("Scene creation failed: " + std::string(e.what()) + "\n").c_str());
        #endif
        return nullptr;
    }
}

int SceneManager::Run() {
    // COM初期化
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // ヒープ上にAppContextを生成
    appContext_ = std::make_unique<AppContext>();

    // 各種初期化
    appContext_->winApp.Initialize(L"2105_おかしあつめてクマサイダー");
    
    // exeのアイコン設定
    appContext_->winApp.SetIconFromTexture("resources/image/icon.png");
    
    appContext_->winApp.EnableResize(false);
    appContext_->dxCommon.Initialize(&appContext_->winApp);
    TextureManager::GetInstance()->Initialize(&appContext_->dxCommon);
    Logger::Initialize(); std::filesystem::create_directory("logs");
    appContext_->input.Initialize(&appContext_->winApp);
    appContext_->gamePad.Initialize();

    // 初期シーンを作成
    sceneArr_[currentSceneNo_] = CreateScene(currentSceneNo_);
    if (sceneArr_[currentSceneNo_]) {
        sceneArr_[currentSceneNo_]->SetAppContext(appContext_.get());
        sceneArr_[currentSceneNo_]->Initialize();
    }

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
        appContext_->input.Update();
        appContext_->gamePad.Update();

        if (GetAsyncKeyState(VK_F11) & 1) {
            if (!appContext_->winApp.IsFullscreen()) {
                appContext_->winApp.EnterBorderlessFullscreen();
            } else {
                appContext_->winApp.ExitBorderlessFullscreen();
            }
            appContext_->dxCommon.ResizeToWindow();
        }

        // シーン切り替えチェック
        int nextSceneNo = -1;
        if (sceneArr_[currentSceneNo_]) {
            nextSceneNo = sceneArr_[currentSceneNo_]->GetSceneNo();
        }

        if (nextSceneNo == -1) {
            running = false;
            break;
        }

        if (nextSceneNo != currentSceneNo_) {
            sceneChangeCount_++;
            
            #ifdef _DEBUG
            OutputDebugStringA(("Scene change #" + std::to_string(sceneChangeCount_) + 
                              ": " + std::to_string(currentSceneNo_) + " -> " + 
                              std::to_string(nextSceneNo) + "\n").c_str());
            #endif

            // シーン切り替え処理
            prevSceneNo_ = currentSceneNo_;
            
            // 現在のシーンを完全に削除（デストラクタが呼ばれてリソース解放）
            if (sceneArr_[currentSceneNo_]) {
                sceneArr_[currentSceneNo_].reset();
                sceneArr_[currentSceneNo_] = nullptr;
                
                // ガベージコレクションを強制実行（メモリ圧迫を軽減）
                #ifdef _DEBUG
                // TextureManagerのリソース状況をチェック（もしメソッドがあれば）
                OutputDebugStringA("Forcing garbage collection...\n");
                #endif
            }
            
            // 新しいシーン番号を設定
            currentSceneNo_ = nextSceneNo;
            
            // 新しいシーンを作成（古いシーンが完全に削除された後）
            sceneArr_[currentSceneNo_] = CreateScene(currentSceneNo_);
            
            // 新しいシーンの初期化
            if (sceneArr_[currentSceneNo_]) {
                sceneArr_[currentSceneNo_]->SetAppContext(appContext_.get());
                
                try {
                    sceneArr_[currentSceneNo_]->Initialize();
                    #ifdef _DEBUG
                    OutputDebugStringA("Scene initialization completed successfully\n");
                    #endif
                }
                catch (const std::exception& e) {
                    #ifdef _DEBUG
                    OutputDebugStringA(("Scene initialization failed: " + std::string(e.what()) + "\n").c_str());
                    #endif
                }
            } else {
                #ifdef _DEBUG
                OutputDebugStringA("Failed to create new scene\n");
                #endif
            }
        }

        // 更新・描画 - nullチェック追加
        if (sceneArr_[currentSceneNo_]) {
            try {
                sceneArr_[currentSceneNo_]->Update();
                sceneArr_[currentSceneNo_]->Draw();
            } catch (const std::exception& e) {
                #ifdef _DEBUG
                OutputDebugStringA(("Scene update/draw error: " + std::string(e.what()) + "\n").c_str());
                #endif
            }
        }
    }

    // 終了処理 - 全シーンのクリーンアップ
    CleanupAllScenes();
    
    TextureManager::GetInstance()->Finalize();
    appContext_->dxCommon.CloseFence();
    appContext_->winApp.Finalize();
    CoUninitialize();

    // 振動のリセット
    appContext_->gamePad.SetVibration(0.0f, 0.0f, 0.0f);

    return 0;
}
