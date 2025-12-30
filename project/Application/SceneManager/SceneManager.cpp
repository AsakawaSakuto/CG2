#include "SceneManager.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <comdef.h>
#include <windows.h>
#include "Core/Logger/Logger.h"
#include "Core/TextureManager/TextureManager.h"
#include "Core/ServiceLocator/ServiceLocator.h"

#include "Audio/AudioManager.h"
#include "Input/InputManager.h"
#include "3d/Line/LineManager.h"

SceneManager::SceneManager() {
    // シーン配列は初期化時には空にする
    for (int i = 0; i < sceneNum; i++) {
        sceneArr_[i] = nullptr;
    }

    //currentSceneNo_ = SCENE::GAME;
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

	// WinAppの初期化
    winApp_ = std::make_unique<WinApp>();
    winApp_->Initialize(L"LE2A_01_アサカワ_サクト");

    // exeのアイコン設定
    winApp_->SetIconFromTexture("resources/image/icon.png");
    winApp_->EnableResize(false);

	dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Initialize(winApp_.get());
    
    // ServiceLocatorにDirectXCommonを登録
    ServiceLocator::Provide(dxCommon_.get());
    
    TextureManager::GetInstance()->Initialize(dxCommon_.get());
    Logger::Initialize();
    std::filesystem::create_directory("logs");

    // 入力関連の初期化
	input_ = std::make_unique<Input>();
    input_->Initialize(winApp_.get());

	gamePad_ = std::make_unique<GamePad>();
    gamePad_->Initialize();

    InputManager::GetInstance()->Initialize();
    InputManager::GetInstance()->SetInputDevices(input_.get(), gamePad_.get());

	AudioManager::GetInstance()->Initialize();

	LineManager::GetInstance()->Initialize();

    // 初期シーンを作成
	currentSceneNo_ = SCENE::GAME;
    sceneArr_[static_cast<int>(currentSceneNo_)] = CreateScene(currentSceneNo_);
    if (sceneArr_[static_cast<int>(currentSceneNo_)]) {
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
                dxCommon_->ClearShaderCache();

                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) break;

        // 入力更新（共通処理）
        input_->Update();
        gamePad_->Update();

		AudioManager::GetInstance()->Update();

        Shortcut();

        // シーン切り替えチェック
        auto curIndex = static_cast<int>(currentSceneNo_);
        SCENE nextSceneNo = sceneArr_[curIndex]->GetSceneNo();

        if (nextSceneNo != currentSceneNo_) {

            // GPU処理の完了を待機（重要！）
            dxCommon_->WaitForGPU();

            // 古いシーン破棄
            prevSceneNo_ = currentSceneNo_;
            sceneArr_[curIndex].reset();

            // リソース解放後、再度GPU待機
            dxCommon_->WaitForGPU();

            // 新しいシーン作成
            currentSceneNo_ = nextSceneNo;
            curIndex = static_cast<int>(currentSceneNo_);
            sceneArr_[curIndex] = CreateScene(currentSceneNo_);

            // 新しいシーン初期化
            if (sceneArr_[curIndex]) {
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
        dxCommon_->PreDraw();

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
        dxCommon_->PostDraw();

        // PostFrameCleanupの呼び出し（GAMEシーンの場合のみ）
        if (currentSceneNo_ == SCENE::GAME) {
            sceneArr_[curIndex]->PostFrameCleanup();
        }
    }
}

void SceneManager::Finalize() {
    // 振動のリセット
    gamePad_->SetVibration(0.0f, 0.0f, 0.0f);

    // シーンのリセット（最初にシーンを解放してGPUリソースを減らす）
    for (int i = 0; i < sceneNum; i++) {
        if (sceneArr_[i]) {
            sceneArr_[i].reset();
        }
    }

    // GPUの完了を待機（リソース解放前に）
    dxCommon_->WaitForGPU();

    // Modelの静的キャッシュを解放
    Model::Finalize();

    // TextureManagerのリソースを解放
    TextureManager::GetInstance()->Finalize();
    
	//
    AudioManager::GetInstance()->Finalize();

    // フェンスイベントを閉じる（デストラクタでも行うが念のため）
    dxCommon_->CloseFence();

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
        dxCommon_->ResizeToWindow();
    }

    // F10キー : SRV使用状況をデバッグログに出力
    if (GetAsyncKeyState(VK_F10) & 1) {
        uint32_t totalUsed = dxCommon_->GetTotalUsedSRVCount();
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
        auto& particleAlloc = dxCommon_->GetParticleAlloc();
        sprintf_s(buffer, "  - Particles: %u / %u (Range: %u-%u)\n",
            particleAlloc.GetUsedCount(),
            particleAlloc.GetCapacity(),
            DirectXCommon::kParticleSRVBegin,
            DirectXCommon::kParticleSRVEnd);
        OutputDebugStringA(buffer);

        // モデル詳細
        auto& modelAlloc = dxCommon_->GetModelAlloc();
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