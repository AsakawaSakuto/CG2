//#include "TestScene.h"
//
//TestScene::~TestScene() {
//    CleanupResources();
//}
//
//void TestScene::CleanupResources() {
//    // Reset unique_ptr resources
//    if (player_) {
//        player_.reset();
//    }
//    if (map_) {
//        map_.reset();
//    }
//    if (model_) {
//        model_.reset();
//    }
//    if (sprite_) {
//        sprite_.reset();
//    }
//    
//    // Reset cameras
//    if (debugCamera_) {
//        debugCamera_.reset();
//    }
//    if (normalCamera_) {
//        normalCamera_.reset();
//    }
//    
//    // Reset timer
//    gameTimer_.Reset();
//}
//
//void TestScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }
//
//void TestScene::Initialize() {
//    // Initialize before cleanup to ensure clean state
//    CleanupResources();
//    
//    // inputSystemの初期化
//    gamePad_ = &ctx_->gamePad;
//    input_ = &ctx_->input;
//
//    // カメラの初期化
//    if (!debugCamera_) {
//        debugCamera_ = std::make_unique<DebugCamera>();
//    }
//    if (!normalCamera_) {
//        normalCamera_ = std::make_unique<Camera>();
//    }
//    
//    debugCamera_->SetInput(&ctx_->input);
//    normalCamera_->SetPosition({0.0f, 0.0f, -30.0f});
//    normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});
//
//    if (!player_) {
//        player_ = std::make_unique<Player>();
//    }
//    player_->Initialize(&ctx_->dxCommon);
//    player_->SetInputSystem(&ctx_->input);
//    player_->SetGamePadSystem(&ctx_->gamePad);
//
//    // マップの初期化
//    if (!map_) {
//        map_ = std::make_unique<Map>();
//    }
//    map_->Initialize();
//
//    // Model and Sprite initialization
//    if (!model_) {
//        model_ = std::make_unique<Model>();
//    }
//    if (!sprite_) {
//        sprite_ = std::make_unique<Sprite>();
//    }
//
//    // 汎用機能
//    gameTimer_.Start(2.0f, true);
//
//    // オブジェクトの配置　上半分
//    SpawnObjectsByMapChip(1.0f, player_->GetEndLine());
//}
//
//void TestScene::Update() {
//
//    // シーンのリセット
//    if (input_->TriggerKey(DIK_R)) {
//        Initialize();
//    }
//
//    // カメラ切り替え&更新
//    CameraController();
//
//    // 汎用機能の更新
//    gameTimer_.Update();
//}
//
//void TestScene::Draw() {
//
//    // ここより上に描画処理を書かない
//    ctx_->dxCommon.PreDraw();
//
//    ///
//    /// ↓描画処理ここから
//    ///
//
//    ///
//    /// ↑描画処理ここまで
//    ///
//
//    // フレームの先頭でImguiにここからフレームが始まる旨を告げる
//    ImGui_ImplDX12_NewFrame();
//    ImGui_ImplWin32_NewFrame();
//    ImGui::NewFrame();
//
//    ///
//    /// ↓ImGuiここから
//    ///
//
//    // プレイヤーのImGui
//    if (player_) {
//        player_->DrawImgui();
//    }
//
//    DrawSceneName();
//
//    ///
//    /// ↑ImGuiここまで
//    ///
//
//    // Imguiの内部コマンドを生成する
//    ImGui::Render();
//
//    // ここより下に描画処理を書かない
//    ctx_->dxCommon.PostDraw();
//}
//
//void TestScene::UpdateCameraToPlayer() {
//    if (player_ && normalCamera_) {
//        // カメラの座標Yをプレイヤーの座標Yに合わせる
//        Vector3 pPos = player_->GetPosition();
//        normalCamera_->SetPosition({0.0f + player_->GetShakeAmount().x, pPos.y + player_->CameraOffset() + player_->GetShakeAmount().x, -30.0f});
//    }
//}
//
//void TestScene::SpawnObjectsByMapChip(float mag, float mapHeight) {
//    if (!map_) return;
//    
//    for (int y = 0; y < map_->GetRowCount(); ++y) {
//        for (int x = 0; x < map_->GetColumnCount(); ++x) {
//            int tile = map_->GetMapData(y, x);
//
//            // タイルごとの描画処理
//            if (static_cast<TileType>(tile) == TileType::THORN) {
//                // トゲの描画処理
//                auto thorn = std::make_unique<Thorn>();
//                thorn->Initialize(&ctx_->dxCommon);
//                thorn->Spawn({static_cast<float>(x) * mag - 8.0f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
//                //thorns_.push_back(std::move(thorn));
//            }
//        }
//    }
//}
//
//void TestScene::CameraController() {
//    if (useDebugCamera_) {
//        if (debugCamera_ != nullptr) {
//            debugCamera_->Update();
//            useCamera_ = debugCamera_.get();
//        }
//    } else {
//        if (normalCamera_ != nullptr) {
//            normalCamera_->Update();
//            useCamera_ = normalCamera_.get();
//        }
//    }
//}