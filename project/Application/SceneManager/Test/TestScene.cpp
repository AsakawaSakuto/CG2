#include "TestScene.h"

void TestScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void TestScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 0.0f, -30.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	player_->Initialize(&ctx_->dxCommon);
	player_->SetInputSystem(&ctx_->input);
	player_->SetGamePadSystem(&ctx_->gamePad);

	// マップの初期化
	map_->Initialize();

	// 汎用機能
	gameTimer_.Start(2.0f, true);

	// Clear
	thorns_.clear();
	blocks_.clear();

	// オブジェクトの配置　上半分
	SpawnObjectsByMapChip(1.0f, player_->GetEndLine());

	// オブジェクトの配置　下半分
	SpawnObjectsByMapChip2(1.0f, 0.0f);

	// プレイヤーに他のゲームオブジェクトの情報を渡す
	player_->SetThrons(thorns_);
	player_->SetBlocks(blocks_);
}

void TestScene::Update() {

	// シーンのリセット
	if (input_->TriggerKey(DIK_R)) {
		Initialize();
	}

	// カメラ切り替え&更新
	CameraController();

	player_->Update();

	// カメラの座標Yをプレイヤーの座標Yに合わせる
	UpdateCameraToPlayer();

	// 汎用機能の更新
	gameTimer_.Update();

	// トゲの更新処理
	for (auto& thorn : thorns_) {
		if (thorn->GetIsAlive()) {
			thorn->Update();
		}
	}

	// ブロックの更新処理
	for (auto& block : blocks_) {
		if (block->GetIsAlive()) {
			block->Update();
		}
	}
}

void TestScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	player_->Draw(*useCamera_);

	// トゲの描画処理
	for (auto& thorn : thorns_) {
		if (thorn->GetIsAlive()) {
			thorn->Draw(*useCamera_);
		}
	}

	// ブロックの描画処理
	for (auto& block : blocks_) {
		if (block->GetIsAlive()) {
			block->Draw(*useCamera_);
		}
	}

	///
	/// ↑描画処理ここまで
	///

	// フレームの先頭でImguiにここからフレームが始まる旨を告げる
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	///
	/// ↓ImGuiここから
	///

	// プレイヤーのImGui
	player_->DrawImgui();

	DrawSceneName();

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void TestScene::UpdateCameraToPlayer() {
	// カメラの座標Yをプレイヤーの座標Yに合わせる
	Vector3 pPos = player_->GetPosition();
	normalCamera_->SetPosition({0.0f + player_->GetShakeAmount().x, pPos.y + player_->CameraOffset() + player_->GetShakeAmount().x, -30.0f});
}

void TestScene::SpawnObjectsByMapChip(float mag, float mapHeight) {
	for (int y = 0; y < map_->GetRowCount(); ++y) {
		for (int x = 0; x < map_->GetColumnCount(); ++x) {
			int tile = map_->GetMapData(y, x);

			// タイルごとの描画処理
			if (static_cast<TileType>(tile) == TileType::THORN) {
				// トゲの描画処理
				auto thorn = std::make_unique<Thorn>();
				thorn->Initialize(&ctx_->dxCommon);
				thorn->Spawn({static_cast<float>(x) * mag - 8.0f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				thorns_.push_back(std::move(thorn));
			}

			if (static_cast<TileType>(tile) == TileType::BLOCK) {
				// トゲの描画処理
				auto block = std::make_unique<Block>();
				block->Initialize(&ctx_->dxCommon);
				block->Spawn({static_cast<float>(x) * mag - 8.0f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				blocks_.push_back(std::move(block));
			}
		}
	}
}

void TestScene::SpawnObjectsByMapChip2(float mag, float mapHeight) {
	for (int y = 0; y < map_->GetRowCount2(); ++y) {
		for (int x = 0; x < map_->GetColumnCount2(); ++x) {
			int tile = map_->GetMapData2(y, x);

			// タイルごとの描画処理
			if (static_cast<TileType>(tile) == TileType::THORN) {
				// トゲの描画処理
				auto thorn = std::make_unique<Thorn>();
				thorn->Initialize(&ctx_->dxCommon);
				thorn->Spawn({static_cast<float>(x) * mag - 8.0f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				thorns_.push_back(std::move(thorn));
			}

			if (static_cast<TileType>(tile) == TileType::BLOCK) {
				// トゲの描画処理
				auto block = std::make_unique<Block>();
				block->Initialize(&ctx_->dxCommon);
				block->Spawn({static_cast<float>(x) * mag - 8.0f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				blocks_.push_back(std::move(block));
			}
		}
	}
}

void TestScene::CameraController() {
	if (useDebugCamera_) {
		if (debugCamera_ != nullptr) {
			debugCamera_->Update();
			useCamera_ = debugCamera_.get();
		}
	} else {
		if (normalCamera_ != nullptr) {
			normalCamera_->Update();
			useCamera_ = normalCamera_.get();
		}
	}
}