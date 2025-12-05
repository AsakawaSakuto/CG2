#include "SceneManager.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <comdef.h>
#include <windows.h>
#include "TextureManager.h"
#include "Logger.h"
#include "Engine/System/Utility/GameTimer/DeltaTime.h" // パス修正
#include "Engine/3d/Model/Model.h" // Model::Finalize用

SceneManager::SceneManager() {
    // シーン配列は初期化時には空にする
    for (int i = 0; i < sceneNum; i++) {
        sceneArr_[i] = nullptr;
    }

    currentSceneNo_ = SCENE::TEST;
    prevSceneNo_ = currentSceneNo_;
}

SceneManager::~SceneManager() {
}

int SceneManager::Run() {

    // 初期化処理
    Initialize();

    // 更新と描画
    Update();

    // 終了処理
    Finalize();

    return 0;
}

void SceneManager::Initialize() {
    // COM初期化
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // ヒープ上にAppContextを生成
    appContext_ = std::make_unique<AppContext>();
    winApp_ = std::make_unique<WinApp>();

    // 各種初期化
    winApp_->Initialize(L"LE2A_01_アサカワ_サクト");

    // exeのアイコン設定
    winApp_->SetIconFromTexture("resources/image/icon.png");

    winApp_->EnableResize(false);
    appContext_->dxCommon.Initialize(winApp_.get());
    TextureManager::GetInstance()->Initialize(&appContext_->dxCommon);
    Logger::Initialize();
    std::filesystem::create_directory("logs");
    appContext_->input.Initialize(winApp_.get());
    appContext_->gamePad.Initialize();

    // 初期シーンを作成
    sceneArr_[static_cast<int>(currentSceneNo_)] = CreateScene(currentSceneNo_);
    if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
        sceneArr_[static_cast<int>(currentSceneNo_)]->SetAppContext(appContext_.get());
        sceneArr_[static_cast<int>(currentSceneNo_)]->Initialize();
    }
}

void SceneManager::Update() {
    MSG msg = {};
    bool running = true;

    while (running) {
        // メッセージ処理
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;

                // シェーダーキャッシュを削除
                //appContext_->dxCommon.ClearShaderCache();

                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) break;

        // 入力更新（共通処理）
        appContext_->input.Update();
        appContext_->gamePad.Update();

        Shortcut();

        // シーン切り替えチェック
        auto curIndex = static_cast<int>(currentSceneNo_);
        SCENE nextSceneNo = sceneArr_[curIndex]->GetSceneNo();

        if (nextSceneNo != currentSceneNo_) {

            // 古いシーン破棄
            prevSceneNo_ = currentSceneNo_;
            sceneArr_[curIndex].reset();

            // 新しいシーン作成
            currentSceneNo_ = nextSceneNo;
            curIndex = static_cast<int>(currentSceneNo_);
            sceneArr_[curIndex] = CreateScene(currentSceneNo_);

            // 新しいシーン初期化
            if (sceneArr_[curIndex]) {
                sceneArr_[curIndex]->SetAppContext(appContext_.get());
                sceneArr_[curIndex]->Initialize();
            } else {
                // 生成失敗したら落とすなりエラー処理
                running = false;
                OutputDebugStringA("Failed to generate new scene\n");
                break;
            }
        }

        // ここからは毎フレーム必ず通す処理
        curIndex = static_cast<int>(currentSceneNo_);

        // 更新
        sceneArr_[curIndex]->Update();

        // 描画前
        appContext_->dxCommon.PreDraw();

        // 描画
        sceneArr_[curIndex]->Draw();

#ifdef USE_IMGUI
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //ImGui::DockSpaceOverViewport();

        sceneArr_[curIndex]->DrawSceneName();
        sceneArr_[curIndex]->DrawImGui();

        // FPS表示（ImGui）
        ImGui::Begin("Performance");
        ImGui::Text("FPS: %.1f", GetFPS());
        ImGui::Text("DeltaTime: %.4f ms", GetDeltaTime() * 1000.0f);
        ImGui::End();

        ImGui::Render();
#endif

        // 描画後
        appContext_->dxCommon.PostDraw();

        if (curIndex == 2) { sceneArr_[curIndex]->PostFrameCleanup(); }
    }
}

void SceneManager::Finalize() {
    // 振動のリセット
    appContext_->gamePad.SetVibration(0.0f, 0.0f, 0.0f);

    // シーンのリセット（最初にシーンを解放してGPUリソースを減らす）
    for (int i = 0; i < sceneNum; i++) {
        if (sceneArr_[i]) {
            sceneArr_[i].reset();
        }
    }

    // GPUの完了を待機（リソース解放前に）
    appContext_->dxCommon.WaitForGPU();

    // Modelの静的キャッシュを解放
    Model::Finalize();

    // TextureManagerのリソースを解放
    TextureManager::GetInstance()->Finalize();
    
    // フェンスイベントを閉じる（デストラクタでも行うが念のため）
    appContext_->dxCommon.CloseFence();

    // appContext_はunique_ptrなので、ここでデストラクタが呼ばれる前に
    // 明示的にリセットして、DirectXCommonのデストラクタを呼び出す
    appContext_.reset();

    // WinAppの終了処理
    winApp_->Finalize();
    
    // COM終了
    CoUninitialize();
}

void SceneManager::Shortcut() {
    // ESCキー : 終了
    if (GetAsyncKeyState(VK_ESCAPE) & 1) {
        PostQuitMessage(0);
    }

    // F11キー : フルスクリーン切替
    if (GetAsyncKeyState(VK_F11) & 1) {
        if (!winApp_->IsFullscreen()) {
            winApp_->EnterBorderlessFullscreen();
        } else {
            winApp_->ExitBorderlessFullscreen();
        }
        appContext_->dxCommon.ResizeToWindow();
    }

    // F10キー : SRV使用状況をデバッグログに出力
    if (GetAsyncKeyState(VK_F10) & 1) {
        uint32_t totalUsed = appContext_->dxCommon.GetTotalUsedSRVCount();
        char buffer[256];
        sprintf_s(buffer, "=== SRV Usage Report ===\n");
        OutputDebugStringA(buffer);

        sprintf_s(buffer, "Total SRVs Used: %u / %u\n", totalUsed, DirectXCommon::kMaxSRVCount_);
        OutputDebugStringA(buffer);

        // テクスチャ詳細
        uint32_t textureCount = static_cast<uint32_t>(TextureManager::GetInstance()->GetTextureCount());
        sprintf_s(buffer, "  - Textures: %u (Range: %u-%u)\n",
            textureCount,
            DirectXCommon::kTextureSRVBegin,
            DirectXCommon::kTextureSRVEnd);
        OutputDebugStringA(buffer);

        // パーティクル詳細
        auto& particleAlloc = appContext_->dxCommon.GetParticleAlloc();
        sprintf_s(buffer, "  - Particles: %u / %u (Range: %u-%u)\n",
            particleAlloc.GetUsedCount(),
            particleAlloc.GetCapacity(),
            DirectXCommon::kParticleSRVBegin,
            DirectXCommon::kParticleSRVEnd);
        OutputDebugStringA(buffer);

        // モデル詳細
        auto& modelAlloc = appContext_->dxCommon.GetModelAlloc();
        sprintf_s(buffer, "  - Models (Skinning): %u / %u (Range: %u-%u)\n",
            modelAlloc.GetUsedCount(),
            modelAlloc.GetCapacity(),
            DirectXCommon::kModelSRVBegin,
            DirectXCommon::kModelSRVEnd);
        OutputDebugStringA(buffer);

        sprintf_s(buffer, "========================\n");
        OutputDebugStringA(buffer);
    }
}

std::unique_ptr<IScene> SceneManager::CreateScene(SCENE sceneNo) {
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
        return nullptr;
    }
}