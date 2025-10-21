#include "GameScene.h"
#include "Application/GameObject/State/JsonState.h"
#include "Application/SceneManager/SceneManager.h"

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

	// オブジェクトの配置
	SpawnObjectsByMapChip(1.0f, player_->GetEndLine());

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
		curtainSprite_[i]->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
	}

	// カウントダウン用のスプライト集
	for (int i = 0; i <= 9; ++i) {
		spriteNumCollection_[i] = "resources/image/UI/Number" + std::to_string(i) + "UI.png";
	}

	// スコア用スプライト5桁分
	for (int i = 0; i < spriteScore_.size(); ++i) {
		spriteScore_[i] = make_unique<SpriteRender>();
		spriteScore_[i]->sprite.Initialize(&ctx_->dxCommon, "resources/image/number/0.png");
		spriteScore_[i]->sprite.SetPosition({100.0f + i * 32.0f, 400.0f});
		spriteScore_[i]->sprite.SetScale({32.0f * deltaTime_, 32.0f * deltaTime_});
	}

	// カウントダウン用のスプライト初期化
	spriteNumber_->Initialize(&ctx_->dxCommon, spriteNumCollection_[3]);
	spriteNumber_->SetScale({1, 1});
	spriteNumber_->SetPosition({640.0f, 360.0f});

	// ゲームスタートタイマー
	gameStartTimer_ = gameSceneState_.maxGameStartTimer;

	// 開始フラグ
	isGameStart_ = false;

	// 「スタート!」スプライト初期化
	spriteStart_->Initialize(&ctx_->dxCommon, "resources/image/UI/StartUI.png");
	spriteStart_->SetScale({1, 1});
	spriteStart_->SetPosition({640.0f, 360.0f});
	spriteStart_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});

	// ルール説明用のスプライト
	spriteRule_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteRule_->SetPosition({640.0f, -100.0f});
	spriteRule_->SetScale({20, 4});

	// ゲーム終了時に表示するスプライト
	spriteGameEnd_->Initialize(&ctx_->dxCommon, "resources/image/UI/FinishUI.png");
	spriteGameEnd_->SetPosition({640.0f, 360.0f});
	spriteGameEnd_->SetScale({1, 1});

	// 進行度ゲージスプライト
	spriteProgressLine_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteProgressLine_->SetScale({2.0f, 32.0f});
	spriteProgressLine_->SetPosition({988.0f, 360.0f});

	spriteProgressPlayer_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteProgressPlayer_->SetScale({3.0f, 2.0f});
	spriteProgressPlayer_->SetPosition({988.0f, 360.0f});
	spriteProgressPlayer_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});

	spriteProgressGoal_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteProgressGoal_->SetScale({2.0f, 2.0f});
	spriteProgressGoal_->SetPosition({988.0f, 670.0f});

	// ゲーム終了フラグ
	isActiveEndText_ = false;

	// 入力が無い時間をカウント
	noInputTimer_ = 0.0f;

	// スコア表示の後ろに配置するスプライト
	spriteCandyScore_->Initialize(&ctx_->dxCommon, "resources/image/UI/CandyCountUI.png");
	spriteCandyScore_->SetPosition({210.0f, 450.0f});
	spriteCandyScore_->SetScale({1, 1});

	// 弾のゲージラムネUI
	spriteChargeUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/ChargeGaugeUI.png");
	spriteChargeUI_->SetPosition({1200.0f, 450.0f});
	spriteChargeUI_->SetScale({1, 1});

	// 山のモデル初期化
	for (int i = 0; i < static_cast<int>(modelMountain_.size()); ++i) {
		modelMountain_[i] = make_unique<Model>();
		modelMountain_[i]->Initialize(&ctx_->dxCommon, "Mountain/Mountain.obj");
		modelMountain_[i]->SetColor({0.0f, 0.7f, 0.4f, 1.0f});
	}

	modelMountain_[0]->SetTranslate({-3.9f, -6.8f, 8.0f});
	modelMountain_[0]->SetScale({5.4f, 6.6f, 6.0f});
	modelMountain_[1]->SetTranslate({0, -8.8f, 22.0f});
	modelMountain_[1]->SetScale({8.6f, 14.0f, 6.9f});
	modelMountain_[2]->SetTranslate({3.2f, -6.8f, 8.0f});
	modelMountain_[2]->SetScale({6.6f, 4.6f, 5.0f});

	/////////////////////////////////////////////////
	for (int i = 0; i < 3; ++i) {
		testPos_[i] = modelMountain_[i]->GetTranslate();
		testScale_[i] = modelMountain_[i]->GetScale();
	}
	/////////////////////////////////////////////////

	// 一定の時間入力がなかった時に減算されるタイマースプライト
	for (int i = 0; i < static_cast<int>(spriteNoInputCountDown_.size()); ++i) {
		spriteNoInputCountDown_[i] = make_unique<Sprite>();
		spriteNoInputCountDown_[i]->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
		spriteNoInputCountDown_[i]->SetPosition({1200.0f, 32.0f});
		spriteNoInputCountDown_[i]->SetScale({1, 1});
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

	// プレイヤーから入力があるかどうか調べる
	UpdateInput();

	// プレイヤーから一定時間入力がなかった場合の処理
	//NoInputTitleBack();

	if (isBackToTitleScene_) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::TITLE;
		isBackToTitleScene_ = false;
	}

	if (player_->GetIsGoal() && goSceneNum_ == 0) {
		sceneFade_->StartFadeIn(2.0f);
		goSceneNum_ = SCENE::RESULT;
		isActiveEndText_ = true; // 終了テキスト表示フラグオン
	}

	// 終了テキストの更新
	UpdateEndText();

	if (sceneFade_->EndFadeIn()) {
		ctx_->lastScore = player_->GetScore();         // スコアを保存
		ctx_->lastShotCount = player_->GetShotCount(); // ショット数を保存
		ctx_->lastStunCount = player_->GetStunCount(); // スタン数を保存

		ChangeScene(goSceneNum_);
		goSceneNum_ = 0;
	}

	sceneFade_->Update();

	// シーンのリセット
	if (input_->TriggerKey(DIK_R)) {
		Initialize();
	}

	// カメラの座標Yをプレイヤーの座標Yに合わせる
	UpdateCameraToPlayer();

	// プレイヤーの更新
	player_->Update();

	// 汎用機能の更新
	gameTimer_.Update();

	// トゲの更新処理
	for (auto& thorn : thorns_) {
		if (thorn->GetIsAlive()) {
			thorn->Update();
		}
		thorn->UpdateParticle();
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

	// 演出用にスコアを加算していく
	SpriteScoreUpdate();

	// スコアスプライトの更新処理
	for (int i = 0; i < static_cast<int>(spriteScore_.size()); ++i) {
		spriteScore_[i]->sprite.Update();
	}

	// 「スタート!」スプライトの更新処理
	if (showStart_) {
		spriteStart_->Update();
	}

	AnimationRuleSprite();

	// ルール説明用のスプライト更新
	spriteRule_->Update();

	// 進行度ゲージ更新
	UpdateProgressSprite();

	// 進行度ゲージスプライト更新
	spriteProgressLine_->Update();
	spriteProgressPlayer_->Update();
	spriteProgressGoal_->Update();

	// ゲーム終了時に表示するスプライト更新
	spriteGameEnd_->Update();

	// スコア表示の後ろに配置するスプライト更新
	spriteCandyScore_->Update();

	// ラムネゲージ
	spriteChargeUI_->Update();

	// 山のモデル更新
	for (int i = 0; i < 3; ++i) { //////////////// 仮 //////////////
		/*modelMountain_[i]->SetTranslate({testPos_[i]});
		modelMountain_[i]->SetScale(testScale_[i]);*/
		modelMountain_[i]->Update();
	}

	// 入力なし　カウントダウン　スプライト 更新
	for (auto& sprite : spriteNoInputCountDown_) {
		sprite->Update();
	}
}

void GameScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	// カウントダウン用のスプライト描画
	if (gameStartTimer_ >= 1 && gameStartTimer_ < 4.0f && player_->GetIsCameraSet()) {
		spriteNumber_->Draw();
	}

	// プレイヤーの描画処理
	player_->Draw(*useCamera_);

	// トゲの描画処理
	for (auto& thorn : thorns_) {
		if (thorn->GetIsAlive()) {
			thorn->Draw(*useCamera_);
		}
		thorn->DrawParticle(*useCamera_);
	}

	// ゲージ用のスプライト(背景)の描画処理
	bulletGaugeSpriteBG_->Draw();

	// 弾のゲージスプライト描画処理
	for (auto& gaugeInfo : bulletGaugeSprite_) {
		if (gaugeInfo.isActive) {
			gaugeInfo.sprite->Draw();
		}
	}

	// スコア表示の後ろに配置するスプライト描画
	spriteCandyScore_->Draw();

	// スコアスプライトの描画処理
	for (int i = 0; i < static_cast<int>(spriteScore_.size()); ++i) {
		if (spriteScore_[i]->isDraw) {
			spriteScore_[i]->sprite.Draw();
		}
	}

	// 「スタート!」スプライトの描画処理
	if (showStart_) {
		spriteStart_->Draw();
	}

	// ルール説明用のスプライト描画
	spriteRule_->Draw();

	// 進行度ゲージスプライト描画
	spriteProgressLine_->Draw();
	spriteProgressPlayer_->Draw();
	spriteProgressGoal_->Draw();

	// ゲーム終了時に表示するスプライト描画
	if (isActiveEndText_) {
		spriteGameEnd_->Draw();
	}

	// ラムネゲージ
	// spriteChargeUI_->Draw();

	// 山のモデル描画
	for (auto& model : modelMountain_) {
		model->Draw(*useCamera_);
	}

	// 画面両端の幕のスプライト描画処理
	for (auto& curtain : curtainSprite_) {
		curtain->Draw();
	}

	// 入力なし　カウントダウン　スプライト 描画
	for (auto& sprite : spriteNoInputCountDown_) {
		sprite->Draw();
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

	//////////////////////////////////////////////////////////////////////////////////////
	ImGui::Begin("Test");

	const char* partNames[] = {"1", "2", "3"};

	for (int i = 0; i < 3; ++i) {
		if (ImGui::CollapsingHeader(partNames[i], ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::PushID(i); // 適切なIDスコープを設定

			ImGui::Text("Position");
			ImGui::DragFloat3("ModelPos", &testPos_[i].x, 0.1f);

			ImGui::Text("Scale");
			ImGui::DragFloat3("ModelScale", &testScale_[i].x, 0.1f);

			ImGui::Separator(); // 区切り線で視認性アップ

			ImGui::PopID();
		}
	}

	ImGui::End();
	//////////////////////////////////////////////////////////////////////////////////////

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void GameScene::UpdateCameraToPlayer() {
	if (player_->GetIsCameraSet()) {
		// カメラの座標Yをプレイヤーの座標Yに合わせる
		Vector3 pPos = player_->GetPosition();
		normalCamera_->SetPosition({0.0f + player_->GetShakeAmount().x, pPos.y + player_->CameraOffset() + player_->GetShakeAmount().x, -30.0f});
	}
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

void GameScene::UpdateInput() {
	// 入力検知
	bool rightInput = input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D) || gamePad_->LeftStickX() >= 0.3f || gamePad_->PushButton(gamePad_->DPAD_RIGHT); // 右入力
	bool leftInput = input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A) || gamePad_->LeftStickX() <= -0.3f || gamePad_->PushButton(gamePad_->DPAD_LEFT);   // 左入力
	bool shotInput = input_->PushKey(DIK_SPACE) || gamePad_->PushButton(gamePad_->A);                                                                       // ショット入力

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
	if (!player_->GetIsCameraSet())
		return;

	// カウントダウン
	gameStartTimer_ -= deltaTime_;
	if (gameStartTimer_ < 0.0f)
		gameStartTimer_ = 0.0f;

	// 表示する数字の算出
	int displayNumber = static_cast<int>(std::clamp(static_cast<int>(std::ceil(gameStartTimer_)), 0, 9));

	// 前フレームの表示と変わったらリセットするための静的保持変数
	static int prevDisplayNumber = -1;
	const Vector2 startScale = {0.5f, 0.5f};
	const Vector2 endScale = {1.5f, 1.5f};
	const float startAlpha = 1.0f;
	const float endAlpha = 0.0f;
	const float duration = 1.0f;

	// 表示が切り替わった瞬間はスケールと透明度をリセット
	if (displayNumber != prevDisplayNumber) {
		// 切り替え時にテクスチャも切り替える
		if (displayNumber >= 0 && displayNumber <= 9) {
			spriteNumber_->SetTexture(spriteNumCollection_[displayNumber]);
		}
		spriteNumber_->SetScale(startScale);
		spriteNumber_->SetColor({1.0f, 1.0f, 1.0f, startAlpha});
		prevDisplayNumber = displayNumber;
	}

	// 表示が1以上のときのみアニメーション
	if (displayNumber >= 1) {
		// 現在の数値区間内での進捗を変換
		float frac = gameStartTimer_ - std::floor(gameStartTimer_);
		float progress = 1.0f - frac;

		// イージングを適用
		float eased = Easing::Apply(std::clamp(progress / duration, 0.0f, 1.0f), Easing::Type::EaseOutCubic);

		// スケールと透明度を補間して設定
		Vector2 newScale = {std::lerp(startScale.x, endScale.x, eased), std::lerp(startScale.y, endScale.y, eased)};
		float newAlpha = std::lerp(startAlpha, endAlpha, eased);

		spriteNumber_->SetScale(newScale);
		spriteNumber_->SetColor({1.0f, 1.0f, 1.0f, newAlpha});
	}

	// カウントが0になった瞬間に「スタート!」スプライトのイージングを開始
	if (displayNumber == 1 && !showStart_) {
		showStart_ = true;
		startAnimTimer_ = 0.0f;
		// リセット状態にして表示開始
		spriteStart_->SetScale(startAnimStartScale_);
		spriteStart_->SetColor({1.0f, 1.0f, 1.0f, startAnimStartAlpha_});
	}

	// 「スタート!」スプライトのイージング処理
	if (showStart_) {
		startAnimTimer_ += deltaTime_;
		float t = std::clamp(startAnimTimer_ / startAnimDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseOutCubic);

		Vector2 newScale = {std::lerp(startAnimStartScale_.x, startAnimEndScale_.x, eased), std::lerp(startAnimStartScale_.y, startAnimEndScale_.y, eased)};
		float newAlpha = std::lerp(startAnimStartAlpha_, startAnimEndAlpha_, eased);

		spriteStart_->SetScale(newScale);
		spriteStart_->SetColor({1.0f, 1.0f, 1.0f, newAlpha});

		// アニメーション終了時の処理
		if (t >= 1.0f) {
			showStart_ = false;
			isGameStart_ = true;
		}
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

void GameScene::SpriteScoreUpdate() {
	int displayScore = static_cast<int>(player_->GetScore());

	for (int i = 0; i < digits.size(); ++i) {
		digits[digits.size() - 1 - i] = displayScore % 10;
		displayScore /= 10;
	}
	
	// 先頭を探索
	int firstNonZeroIndex = 0;
	while (firstNonZeroIndex < digits.size() - 1 && digits[firstNonZeroIndex] == 0) {
		++firstNonZeroIndex;
	}

	int visibleDigits = digits.size() - firstNonZeroIndex;
	float digitWidth = 32.0f;
	float totalWidth = digitWidth * visibleDigits;
	float startX = 100.0f + ((digitWidth * digits.size()) - totalWidth) / 2.0f;

	// 先頭のゼロを除く
	for (int i = 0; i < spriteScore_.size(); ++i) {
		if (i < firstNonZeroIndex) {
			spriteScore_[i]->isDraw = false;
		} else {
			int digit = digits[i];
			spriteScore_[i]->sprite.SetTexture(spriteNumCollection_[digit]);
			spriteScore_[i]->isDraw = true;

			// 中央詰めにする
			float x = startX + (i - firstNonZeroIndex) * digitWidth;
			spriteScore_[i]->sprite.SetPosition({x, 400.0f});
		}
	}
}

void GameScene::AnimationRuleSprite() {
	timerSpriteRule_ += deltaTime_;

	switch (ruleState_) {
	case RuleAnimState::Rising: {
		float t = std::clamp(timerSpriteRule_ / ruleDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseOutCubic);
		float posY = std::lerp(ruleStartPosY_, ruleEndPosY_, eased);
		spriteRule_->SetPosition({640.0f, posY});

		if (t >= 1.0f) {
			// 到達したら待機へ移行
			ruleState_ = RuleAnimState::Waiting;
			timerSpriteRule_ = 0.0f;
		}
		break;
	}
	case RuleAnimState::Waiting: {
		// そのまま指定秒だけ待つ
		if (timerSpriteRule_ >= ruleWaitDuration_) {
			ruleState_ = RuleAnimState::Falling;
			timerSpriteRule_ = 0.0f;
		}
		// 待機中は位置を終了位置に固定
		spriteRule_->SetPosition({640.0f, ruleEndPosY_});
		break;
	}
	case RuleAnimState::Falling: {
		float t = std::clamp(timerSpriteRule_ / ruleDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseInCubic);
		float posY = std::lerp(ruleEndPosY_, ruleStartPosY_, eased);
		spriteRule_->SetPosition({640.0f, posY});

		if (t >= 1.0f) {
			// 終了処理
			if (ruleLoop_) {
				ruleState_ = RuleAnimState::Rising;
			} else {
				ruleState_ = RuleAnimState::Done;
			}
			timerSpriteRule_ = 0.0f;
		}
		break;
	}
	case RuleAnimState::Done: {
		// 完了後は位置を開始位置に固定
		spriteRule_->SetPosition({640.0f, ruleStartPosY_});
		break;
	}
	}
}

void GameScene::UpdateProgressSprite() {
	// ラインの位置と表示高さを取得
	Vector2 linePos = spriteProgressLine_->GetPosition();
	Vector2 lineSize = {32.0f, 540.0f};
	float halfHeight = lineSize.y * 0.5f;
	float topY = linePos.y + halfHeight;
	float bottomY = linePos.y - halfHeight;

	// プレイヤーのY座標を取得
	float playerY = player_->GetPosition().y;

	// プレイヤーのスタート/エンドライン
	float startLine = player_->GetEndLine();
	float endLine = player_->GetStartLine();

	float t = 0.0f;
	if (endLine != startLine) {
		t = (playerY - startLine) / (endLine - startLine);
	}
	t = std::clamp(t, 0.0f, 1.0f);

	float mappedY = std::lerp(bottomY, topY, t);

	// プレイヤーの進行度スプライトのXはラインと合わせ、Yをマッピング結果にする
	spriteProgressPlayer_->SetPosition({linePos.x, mappedY});
}

void GameScene::UpdateEndText() {
	// 点滅用にタイマーを回す
	if (isActiveEndText_) {
		timerEndText_++;
	}

	if (timerEndText_ % 6 == 0) {
		spriteGameEnd_->SetColor({1.0f, 0.0f, 0.0f, 0.5f});
	} else {
		spriteGameEnd_->SetColor({1.0f, 1.0f, 1.0f, 0.5f});
	}
}
