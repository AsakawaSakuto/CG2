#include "SceneManager.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <comdef.h>
#include <windows.h>

// Add missing includes
#include "TextureManager.h"
#include "Logger.h"

SceneManager::SceneManager() {
    // シーン配列は初期化時には空にする
    for (int i = 0; i < sceneNum; i++) {
        sceneArr_[i] = nullptr;
    }

    currentSceneNo_ = SCENE::TITLE;
    prevSceneNo_ = SCENE::TITLE;
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

std::unique_ptr<IScene> SceneManager::CreateScene(SCENE sceneNo) {
    #ifdef _DEBUG
    OutputDebugStringA(("Creating scene " + std::to_string(static_cast<int>(sceneNo)) + "\n").c_str());
    #endif

    try {
        switch (sceneNo) {
        case SCENE::TEST:
            return std::make_unique<TestScene>();
        case SCENE::TITLE:
            return std::make_unique<TitleScene>();
        case SCENE::GAME:
            return std::make_unique<GameScene>();
        case SCENE::RESULT:
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
    appContext_->winApp.Initialize(L"Engine");
    
    // exeのアイコン設定
    appContext_->winApp.SetIconFromTexture("resources/image/icon.png");
    
    appContext_->winApp.EnableResize(false);
    appContext_->dxCommon.Initialize(&appContext_->winApp);
    TextureManager::GetInstance()->Initialize(&appContext_->dxCommon);
    Logger::Initialize(); 
    std::filesystem::create_directory("logs");
    appContext_->input.Initialize(&appContext_->winApp);
    appContext_->gamePad.Initialize();

    // 初期シーンを作成
    sceneArr_[static_cast<int>(currentSceneNo_)] = CreateScene(currentSceneNo_);
    if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
        sceneArr_[static_cast<int>(currentSceneNo_)]->SetAppContext(appContext_.get());
        sceneArr_[static_cast<int>(currentSceneNo_)]->Initialize();
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
        SCENE nextSceneNo = SCENE::TEST;
        if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
            nextSceneNo = sceneArr_[static_cast<int>(currentSceneNo_)]->GetSceneNo();
        } else {
            running = false;
            break;
        }

        if (nextSceneNo != currentSceneNo_) {

            // シーン切り替え処理
            prevSceneNo_ = currentSceneNo_;
            
            // 現在のシーンを完全に削除
            if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
                sceneArr_[static_cast<int>(currentSceneNo_)].reset();
                sceneArr_[static_cast<int>(currentSceneNo_)] = nullptr;
                
                // ガベージコレクションを強制実行
                #ifdef _DEBUG
                // TextureManagerのリソース状況をチェック
                OutputDebugStringA("Forcing garbage collection...\n");
                #endif
            }
            
            // 新しいシーン番号を設定
            currentSceneNo_ = nextSceneNo;
            
            // 新しいシーンを作成
            sceneArr_[static_cast<int>(currentSceneNo_)] = CreateScene(currentSceneNo_);
            
            // 新しいシーンの初期化
            if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
                sceneArr_[static_cast<int>(currentSceneNo_)]->SetAppContext(appContext_.get());
                
                try {
                    sceneArr_[static_cast<int>(currentSceneNo_)]->Initialize();
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

        // 更新・描画
        if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
            try {
				// シーンの更新処理
                sceneArr_[static_cast<int>(currentSceneNo_)]->Update();

				// 描画前処理
                appContext_->dxCommon.PreDraw();

				// シーンの描画処理,SpriteやModel,Particle等
                sceneArr_[static_cast<int>(currentSceneNo_)]->Draw();

                #ifdef USE_IMGUI
                
                // フレームの先頭でImguiにここからフレームが始まる旨を告げる
                ImGui_ImplDX12_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();

                sceneArr_[static_cast<int>(currentSceneNo_)]->DrawImGui();

                // Imguiの内部コマンドを生成する
                ImGui::Render();

                #endif

				// 描画後処理
                appContext_->dxCommon.PostDraw();
            } catch (const std::exception& e) {
                #ifdef _DEBUG
                OutputDebugStringA(("Scene update/draw error: " + std::string(e.what()) + "\n").c_str());
                #endif
            }
        }
    }

    // 終了処理
    CleanupAllScenes();
    
    TextureManager::GetInstance()->Finalize();
    appContext_->dxCommon.CloseFence();
    appContext_->winApp.Finalize();
    CoUninitialize();

    // 振動のリセット
    appContext_->gamePad.SetVibration(0.0f, 0.0f, 0.0f);

    return 0;
}
