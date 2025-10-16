#include "GameScene.h"
#include "Application/GameObject/State/JsonState.h"

void GameScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

GameScene::~GameScene() {
	thorns_.clear();
	blocks_.clear();
}

void GameScene::Initialize() {
	// JSONからステータスを読み込み
	gameSceneState_ = JsonState::Load<GameSceneState>("Resources/Data/gameSceneState.json");

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
	SpawnObjectsByMapChip2(0.5f, 0.0f);

	// プレイヤーに他のゲームオブジェクトの情報を渡す
	player_->SetThrons(thorns_);
	player_->SetBlocks(blocks_);

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	// ゲージ用のスプライト(背景)の初期化
	bulletGaugeSpriteBG_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	bulletGaugeSpriteBG_->SetScale({4, 25});
	bulletGaugeSpriteBG_->SetPosition({1200, 350});

	//testPos_ = {1200, 400};
	//testScale_ = {8, 40};

	// 弾のゲージスプライト
	for (int i = 0; i < bulletGaugeSprite_.size(); ++i) {
		bulletGaugeSprite_[i].sprite = std::make_unique<Sprite>();
		bulletGaugeSprite_[i].sprite->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
		bulletGaugeSprite_[i].sprite->SetScale({4, 5});
		bulletGaugeSprite_[i].sprite->SetPosition({1200.0f, 532.0f - (88 * i)});
		bulletGaugeSprite_[i].sprite->SetColor({0.0f, 1.0f, 0.0f, 1.0f});
		bulletGaugeSprite_[i].isActive = false;
	}

	// 画面両端の幕のスプライト
	for (int i = 0; i < curtainSprite_.size(); ++i) {
		curtainSprite_[i] = std::make_unique<Sprite>();
		curtainSprite_[i]->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
		curtainSprite_[i]->SetScale({20, 43});
		curtainSprite_[i]->SetPosition({160.0f + (i * 940.0f), 344.0f});
		curtainSprite_[i]->SetColor({0.4f, 0.4f, 0.4f, 1.0f});
	}

	// カウントダウン用のスプライト集
	for (int i = 0; i <= 9; ++i) {
		spriteNumCollection_[i] = "resources/image/number/" + std::to_string(i) + ".png";
	}

	// カウントダウン用のスプライト初期化
	spriteNumber_->Initialize(&ctx_->dxCommon, spriteNumCollection_[3]);
	spriteNumber_->SetScale({2, 2});
	spriteNumber_->SetPosition({640.0f, 360.0f});

	//priteNumber_->SetPosition();
	//spriteNumber_->SetScale();

	// ゲームスタートタイマー
	gameStartTimer_ = gameSceneState_.maxGameStartTimer;

	// 開始フラグ
	isGameStart_ = false;

	// プレイヤーから入力があるかどうか調べる
	UpdateInput();

	// プレイヤーから一定時間入力がなかった場合の処理
	NoInputTitleBack();

	if (isBackToTitleScene_) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::TITLE;
		isBackToTitleScene_ = false;
	}

	if (player_->GetIsGoal() && goSceneNum_ == 0) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::RESULT;
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(goSceneNum_);
		goSceneNum_ = 0;
	}

	sceneFade_->Update();

	// シーンのリセット
	if (input_->TriggerKey(DIK_R)) {
		Initialize();
	}

	// プレイヤーの更新処理
	player_->Update();

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

	// ゲージ用のスプライト(背景)の更新処理
	bulletGaugeSpriteBG_->Update();

	// 弾のゲージスプライト更新処理
	for (auto& gaugeInfo : bulletGaugeSprite_) {
		gaugeInfo.sprite->Update();
	}

	// 画面両端の幕のスプライト更新処理
	for (auto& curtain : curtainSprite_) {
		curtain->Update();
	}
}

void GameScene::Update() {
	// タイマーカウントダウン
	GameStartCount();

	// カメラ切り替え&更新
	CameraController();

	// カウントダウン用のスプライト更新
	spriteNumber_->Update();

	// カウントダウン用のスプライトの張り替え
	if (gameStartTimer_ >= 1) {
		spriteNumber_->SetTexture(spriteNumCollection_[static_cast<int>(gameStartTimer_)]);
	}

	// フラグが立つまで早期リターン
	if (!isGameStart_) {
		return;
	}

	// プレイヤーから入力があるかどうか調べる
	UpdateInput();

	// プレイヤーから一定時間入力がなかった場合の処理
	NoInputTitleBack();

	if (isBackToTitleScene_) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::TITLE;
		isBackToTitleScene_ = false;
	}

	if (player_->GetIsGoal() && goSceneNum_ == 0) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::RESULT;
	}

	if (sceneFade_->EndFadeIn()) {
		ChangeScene(goSceneNum_);
		goSceneNum_ = 0;
	}

	sceneFade_->Update();

	// シーンのリセット
	if (input_->TriggerKey(DIK_R)) {
		Initialize();
	}

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

	// ゲージ用のスプライト(背景)の更新処理
	bulletGaugeSpriteBG_->Update();

	// ゲージの描画を実際の弾のゲージに対応させる
	player_->SetBulletGaugeSprites(&bulletGaugeSprite_);

	// 弾のゲージスプライト更新処理
	for (auto& gaugeInfo : bulletGaugeSprite_) {
		gaugeInfo.sprite->Update();
	}

	// 画面両端の幕のスプライト更新処理
	for (auto& curtain : curtainSprite_) {
		curtain->Update();
	}
}

void GameScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	// sceneFade_->Draw();

	// カウントダウン用のスプライト描画
	if (gameStartTimer_ >= 1) {
		spriteNumber_->Draw();
	}

	// プレイヤーの描画処理
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

	// ゲージ用のスプライト(背景)の描画処理
	bulletGaugeSpriteBG_->Draw();

	// 弾のゲージスプライト描画処理
	for (auto& gaugeInfo : bulletGaugeSprite_) {
		if (gaugeInfo.isActive) {
			gaugeInfo.sprite->Draw();
		}
	}

	// 画面両端の幕のスプライト描画処理
	for (auto& curtain : curtainSprite_) {
		curtain->Draw();
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

	// ゲームシーン上で管理しているステータスのImGui
	GameSceneStateImGui();

	/*ImGui::Begin("Test");
	
	ImGui::DragFloat2("TestPos", &testPos_.x, 1.0f);
	ImGui::DragFloat2("TestScale", &testScale_.x, 1.0f);

	ImGui::End();*/

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void GameScene::UpdateCameraToPlayer() {
	// カメラの座標Yをプレイヤーの座標Yに合わせる
	Vector3 pPos = player_->GetPosition();
	normalCamera_->SetPosition({0.0f + player_->GetShakeAmount().x, pPos.y + player_->CameraOffset() + player_->GetShakeAmount().x, -30.0f});
}

void GameScene::SpawnObjectsByMapChip(float mag, float mapHeight) {
	for (int y = 0; y < map_->GetRowCount(); ++y) {
		for (int x = 0; x < map_->GetColumnCount(); ++x) {
			int tile = map_->GetMapData(y, x);

			// タイルごとの描画処理
			if (static_cast<TileType>(tile) == TileType::THORN) {
				// トゲの描画処理
				auto thorn = std::make_unique<Thorn>();
				thorn->Initialize(&ctx_->dxCommon);
				thorn->Spawn({static_cast<float>(x) * mag - 5.1f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				thorns_.push_back(std::move(thorn));
			}

			if (static_cast<TileType>(tile) == TileType::BLOCK) {
				// トゲの描画処理
				auto block = std::make_unique<Block>();
				block->Initialize(&ctx_->dxCommon);
				block->Spawn({static_cast<float>(x) * mag - 5.1f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				blocks_.push_back(std::move(block));
			}
		}
	}
}

void GameScene::SpawnObjectsByMapChip2(float mag, float mapHeight) {
	for (int y = 0; y < map_->GetRowCount2(); ++y) {
		for (int x = 0; x < map_->GetColumnCount2(); ++x) {
			int tile = map_->GetMapData2(y, x);

			// タイルごとの描画処理
			if (static_cast<TileType>(tile) == TileType::THORN) {
				// トゲの描画処理
				auto thorn = std::make_unique<Thorn>();
				thorn->Initialize(&ctx_->dxCommon);
				thorn->Spawn({static_cast<float>(x) * mag - 5.5f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				thorns_.push_back(std::move(thorn));
			}

			if (static_cast<TileType>(tile) == TileType::BLOCK) {
				// トゲの描画処理
				auto block = std::make_unique<Block>();
				block->Initialize(&ctx_->dxCommon);
				block->Spawn({static_cast<float>(x) * mag - 5.5f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				blocks_.push_back(std::move(block));
			}
		}
	}
}

void GameScene::UpdateInput() { 
	// 入力検知
	bool rightInput = input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D) || gamePad_->LeftStickX() >= 0.3f || gamePad_->PushButton(gamePad_->DPAD_RIGHT); // 右入力
	bool leftInput = input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A) || gamePad_->LeftStickX() <= -0.3f || gamePad_->PushButton(gamePad_->DPAD_LEFT); // 左入力
	bool shotInput = input_->PushKey(DIK_SPACE) || gamePad_->PushButton(gamePad_->A); // ショット入力

	if (rightInput || leftInput || shotInput) {
		isInput_ = true; // 入力があったらtrue
	} else {
		isInput_ = false; // 入力がなければfalse
	}
}

void GameScene::NoInputTitleBack() {
	if (isInput_) {
		noInputTimer_ = 0; // 入力があったらリセット
		return;
	} else {
		noInputTimer_ += 1.0f * deltaTime_; // タイマー加算

		// 5秒間入力がなかった場合
		if (noInputTimer_ >= gameSceneState_.maxNoInputTimer) {
			isBackToTitleScene_ = true;
			noInputTimer_ = 0;
		}
	}
}

void GameScene::GameSceneStateImGui() {
	ImGui::Begin("GameScene State");

	ImGui::Text("noInputTimer : %f", noInputTimer_);

	nlohmann::json jsonState = gameSceneState_;

	// JsonNo中身をImGuiで表示する
	player_->DrawImGuiForJson(jsonState);

	gameSceneState_ = jsonState.get<GameSceneState>();

	// --- JSONへ保存ボタン ---
	if (ImGui::Button("Save JSON")) {
		JsonState::Save("Resources/Data/gameSceneState.json", gameSceneState_);
	}

	ImGui::End();
}

void GameScene::GameStartCount() { 
	// カウントアップ
	gameStartTimer_ -= deltaTime_; 

	// 一定の値に達したらフラグをたてる
	if (gameStartTimer_ <= 1) {
		isGameStart_ = true;
	}
}

void GameScene::CameraController() {
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