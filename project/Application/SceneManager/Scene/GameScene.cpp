#include "GameScene.h"
#include "Application/GameObject/State/JsonState.h"
#include "Application/SceneManager/SceneManager.h"
#include "Engine/System/Audio/MasterVolume.h"
#include "Engine/System/DirectXCommon/ExeColor.h"

void GameScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

GameScene::~GameScene() {
	thorns_.clear();
}

void GameScene::Initialize() {
	// JSONからステータスを読み込み
	gameSceneState_ = JsonState::Load<GameSceneState>("Resources/Data/gameSceneState.json");

	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラのZ座標
	cameraPosisionZ_ = -38.0f;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 0.0f, cameraPosisionZ_});
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

	// オブジェクトの配置
	SpawnObjectsByMapChip(1.0f, player_->GetEndLine());

	// プレイヤーに他のゲームオブジェクトの情報を渡す
	player_->SetThrons(thorns_);

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	// 弾のゲージスプライト
	for (int i = 0; i < bulletGaugeSprite_.size(); ++i) {
		bulletGaugeSprite_[i].sprite = std::make_unique<Sprite>();
		bulletGaugeSprite_[i].sprite->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
		bulletGaugeSprite_[i].sprite->SetScale({2.0, 2.5f});
		bulletGaugeSprite_[i].sprite->SetPosition({1070.0f, 480.0f - (44.0f * i)});
		bulletGaugeSprite_[i].sprite->SetColor({0.0f, 0.0f, 1.0f, 1.0f});
		bulletGaugeSprite_[i].isActive = false;
	}

	bulletGaugeSprite_[0].sprite->SetTexture("resources/image/UI/bulletGaugeEdgeBottom.png");
	bulletGaugeSprite_[0].sprite->SetScale({2.0f, 2.5f});
	bulletGaugeSprite_[4].sprite->SetTexture("resources/image/UI/bulletGaugeEdge.png");
	bulletGaugeSprite_[4].sprite->SetScale({2.0f, 2.5f});

	// 画面両端の幕のスプライト
	for (int i = 0; i < curtainSprite_.size(); ++i) {
		curtainSprite_[i] = std::make_unique<Sprite>();
		curtainSprite_[i]->Initialize(&ctx_->dxCommon, "resources/image/UI/SidePanelUI.png");
		curtainSprite_[i]->SetScale({0.5f, 0.5f});
		curtainSprite_[i]->SetPosition({-62.0f + (i * 1404.0f), 620.0f});
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
		spriteScore_[i]->sprite.SetScale({0.5f, 0.4f});
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
	spriteRule_->Initialize(&ctx_->dxCommon, "resources/image/UI/GameRuleUI.png");
	spriteRule_->SetPosition({640.0f, -100.0f});
	spriteRule_->SetScale({0.25f, 0.25f});

	// ゲーム終了時に表示するスプライト
	spriteGameEnd_->Initialize(&ctx_->dxCommon, "resources/image/UI/FinishUI.png");
	spriteGameEnd_->SetPosition({640.0f, 360.0f});
	spriteGameEnd_->SetScale({1, 1});

	// 進行度ゲージスプライト
	spriteProgressLine_->Initialize(&ctx_->dxCommon, "resources/image/UI/altitudeGaugeUI.png");
	spriteProgressLine_->SetScale({0.2f, 0.3f});
	spriteProgressLine_->SetPosition({1230.0f, 360.0f});

	spriteProgressPlayer_->Initialize(&ctx_->dxCommon, "resources/image/UI/bearUI.png");
	spriteProgressPlayer_->SetScale({0.25f, 0.25f});
	spriteProgressPlayer_->SetPosition({988.0f, 360.0f});

	spriteProgressGoal_->Initialize(&ctx_->dxCommon, "resources/image/UI/flagUI02.png");
	spriteProgressGoal_->SetScale({0.5f, 0.5f});
	spriteProgressGoal_->SetPosition({1230.0f, 680.0f});

	spriteProgressMountaion_->Initialize(&ctx_->dxCommon, "resources/image/UI/mountainTopUI02.png");
	spriteProgressMountaion_->SetScale({0.25f, 0.25f});
	spriteProgressMountaion_->SetPosition({1225.0f, 50.0f});

	// ゲーム終了フラグ
	isActiveEndText_ = false;

	// 入力が無い時間をカウント
	noInputTimer_ = 0.0f;

	// スコア表示の後ろに配置するスプライト
	spriteCandyScore_->Initialize(&ctx_->dxCommon, "resources/image/UI/CandyUI.png");
	spriteCandyScore_->SetPosition({160.0f, 400.0f});
	spriteCandyScore_->SetScale({0.5f, 0.5f});
	spriteCandyScore_->SetColor({0.8f, 0.1f, 0.4f, 1.0f});

	// スコアの背景波紋
	spriteCandyEffect_->Initialize(&ctx_->dxCommon, "resources/image/UI/CandyUI.png");
	spriteCandyEffect_->SetPosition(spriteCandyScore_->GetPosition());
	candyEffectSize_ = 0.5f;
	spriteCandyEffect_->SetScale({candyEffectSize_, candyEffectSize_});
	spriteCandyEffect_->SetColor({0.8f, 0.1f, 0.4f, 1.0f});

	// 弾のゲージラムネUI
	spriteChargeUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/shotChargeGaugeUI.png");
	spriteChargeUI_->SetPosition({1070.0f, 350.0f});
	spriteChargeUI_->SetScale({0.4f, 0.4f});

	// ラムネの波紋
	spriteChargeUIEffect_->Initialize(&ctx_->dxCommon, "resources/image/UI/shotChargeGaugeUI.png");
	spriteChargeUIEffect_->SetPosition(spriteChargeUI_->GetPosition());
	chargeEffectSize_ = 0.4f;
	spriteChargeUIEffect_->SetScale({chargeEffectSize_, chargeEffectSize_});

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

	// 一定の時間入力がなかった時に減算されるタイマースプライト
	for (int i = 0; i < static_cast<int>(spriteNoInputCountDown_.size()); ++i) {
		spriteNoInputCountDown_[i] = make_unique<Sprite>();
		spriteNoInputCountDown_[i]->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
		spriteNoInputCountDown_[i]->SetPosition({1200.0f, 32.0f});
		spriteNoInputCountDown_[i]->SetScale({1, 1});
	}

	// SE
	startGameSE_->Initialize("resources/sound/SE/InGame/StartGameSE.mp3");
	countDownSE_->Initialize("resources/sound/SE/InGame/CountDownSE.mp3");
	clearSE_->Initialize("resources/sound/SE/InGame/ClearSE.mp3");

	// BGM
	gameSceneBGM01_->Initialize("resources/sound/BGM/InGameBGM01.mp3");
	gameSceneBGM02_->Initialize("resources/sound/BGM/InGameBGM02.mp3");
	gameSceneBGM01_->PlayAudio(BGM_Volume, true);

	// ○○個突破!スプライト
	spriteSnackCountOver_->Initialize(&ctx_->dxCommon, "resources/image/UI/CandyCountNotificationUI.png");
	spriteScoreCountOverPos_ = {-600.0f, 360.0f};
	spriteSnackCountOver_->SetPosition(spriteScoreCountOverPos_);
	spriteSnackCountOver_->SetScale({0.4f, 0.4f});
	spriteSnackCountOver_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// ○○個突破　スコア数　スプライト
	for (int i = 0; i < spriteScoreCountOver_.size(); ++i) {
		spriteScoreCountOver_[i] = make_unique<Sprite>();
		spriteScoreCountOver_[i]->Initialize(&ctx_->dxCommon, "resources/image/number/0.png");
		spriteScoreCountOver_[i]->SetPosition(spriteScoreCountOverPos_);
		spriteScoreCountOver_[i]->SetScale({1.0f, 1.0f});
		spriteScoreCountOver_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	}

	// push スプライト
	spritePush_->Initialize(&ctx_->dxCommon, "resources/image/UI/PushButton01UI.png");
	spritePush_->SetPosition({1068.0f, 600.0f});
	spritePush_->SetScale({0.2f, 0.2f});

	// 腕 スプライト
	spriteArm_->Initialize(&ctx_->dxCommon, "resources/image/UI/armUI.png");
	spriteArm_->SetPosition({450.0f, 75.0f});
	spriteArm_->SetScale({0.25f, 0.25f});

	// ピニャータ スプライト
	spriteThorn_->Initialize(&ctx_->dxCommon, "resources/image/UI/enemyUI.png");
	spriteThorn_->SetPosition({590.0f, 85.0f});
	spriteThorn_->SetScale({0.25f, 0.25f});

	// ○○個突破　1000の倍数
	nextMilestone_ = 0;

	// 振動の終了
	gamePad_->SetVibration(0.0f, 0.0f, 0.0f);

	// 地面モデル
	modelGround_->Initialize(&ctx_->dxCommon, "Ground/Ground5.obj");
	modelGround_->SetTexture("resources/model/Ground/Ground2.png");
	modelGround_->SetTranslate({0.0f, -250.0f, 1400.0f});
	modelGround_->SetScale({40.0f, 40.0f, 40.0f});

	// 曇モデル
	for (int i = 0; i < clouds_.size(); ++i) {
		clouds_[i] = make_unique<MoveModel>();
		clouds_[i]->model.Initialize(&ctx_->dxCommon, "Cloud/Cloud.obj");
		clouds_[i]->model.SetTexture("resources/image/0.png");
		clouds_[i]->model.SetScale({2.0f, 1.0f, 1.0f});
		clouds_[i]->model.SetColor({1.0f, 1.0f, 1.0f, 0.3f});
		clouds_[i]->direction = Direction::RIGHT;

		// 初期座標Y
		const float MIN_POS_Y = 150.0f;
		const float MAX_POS_Y = 400.0f;

		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_real_distribution<float> distr(MIN_POS_Y, MAX_POS_Y);

		// 初期座標X
		const float MIN_POS_X = -7.0f;
		const float MAX_POS_X = 7.0f;

		std::random_device rd2;
		std::mt19937 eng2(rd2());
		std::uniform_real_distribution<float> distr2(MIN_POS_X, MAX_POS_X);

		// 雲の進む向きを決める
		if (GetRandomDirection()) {
			clouds_[i]->model.SetTranslate({distr2(eng2), distr(eng), 20.0f});
			clouds_[i]->direction = Direction::LEFT;
		} else {
			clouds_[i]->model.SetTranslate({distr2(eng2), distr(eng), 20.0f});
			clouds_[i]->direction = Direction::RIGHT;
		}
	}

	// 雲のモデル座標リセットフラグ
	isCloudPosReset_ = false;

	// スコア加算時に使用するタイマー
	scoreUpTimer_ = 0.0f;

	maskBox_->Initialize(&ctx_->dxCommon, "resources/image/mask/box.png", { 640.0f,360.0f }, { 1.0f,1.0f });
	loadingUI_->Initialize(&ctx_->dxCommon, "resources/image/mask/loadingUI.png", { 1040.0f, 640.0f }, { 1.0f, 1.0f });
	loadingUI_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	loadingPlayer_->Initialize(&ctx_->dxCommon, "resources/image/mask/loadingPlayer.png", { 680.0f, 615.0f }, { 0.3f, 0.3f });

	maskType_ = static_cast<MaskType>(rand_.Int(0, 2));

	switch (maskType_)
	{
	case GameScene::MaskType::RAMA:
		mask_->Initialize(&ctx_->dxCommon, "resources/image/mask/view01.png", { 640.0f,360.0f }, { 1.0f,1.0f });
		maskStartScale_ = { 0.26f, 0.26f };
		maskEndScale_ = { 8.0f, 8.0f };
		break;
	case GameScene::MaskType::KUMA:
		mask_->Initialize(&ctx_->dxCommon, "resources/image/mask/view02.png", { 640.0f,360.0f }, { 1.0f,1.0f });
		maskStartScale_ = { 0.26f, 0.26f };
		maskEndScale_ = { 2.5f, 2.5f };
		break;
	case GameScene::MaskType::AME:
		mask_->Initialize(&ctx_->dxCommon, "resources/image/mask/view03.png", { 640.0f,360.0f }, { 1.0f,1.0f });
		maskStartScale_ = { 0.26f, 0.26f };
		maskEndScale_ = { 5.0f, 5.0f };
		break;
	}

	// マスクのアニメーション設定 - 画面外から中央へ
	maskStartPos_ = { 640.0f, 360.0f };  // 画面上部から開始
	maskEndPos_ = { 640.0f, 360.0f };    // 画面中央で終了

	maskTimer_.Start(1.0f, false);

	timerStarte_ = false;
	resultQuit_ = false;

	ExeColor = { 0.212f, 0.722f, 1.000f, 1.000f };

	ore_->Initialize(&ctx_->dxCommon, "ore.obj");
	ore_->SetUpdateFrustumCulling(false);
	ore_->SetUseLight(false);
	oreTransform_.translate = { 4.5f,450.5f,1.0f };
	oreTransform_.rotate = { -0.33f,-3.47f,0.66f };
	oreTransform_.scale = { 3.0f,3.0f,3.0f };
	ore_->SetTransform(oreTransform_);

	srarArea1_->Initialize(&ctx_->dxCommon);
	srarArea2_->Initialize(&ctx_->dxCommon);
	srarArea3_->Initialize(&ctx_->dxCommon);
	srarArea4_->Initialize(&ctx_->dxCommon);

	srarArea1_->LoadJson("starArea1");
	srarArea2_->LoadJson("starArea2");
	srarArea3_->LoadJson("starArea3");
	srarArea4_->LoadJson("starArea4");
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
	NoInputTitleBack();

	if (isBackToTitleScene_) {
		sceneFade_->StartFadeIn(1.0f);
		goSceneNum_ = SCENE::TITLE;
		isBackToTitleScene_ = false;

		// SEの解放
		ResetSE();
	}

	if (player_->GetIsGoal() && goSceneNum_ == 0) {
		sceneFade_->StartFadeIn(2.0f);
		goSceneNum_ = SCENE::RESULT;
		isActiveEndText_ = true; // 終了テキスト表示フラグオン

		maskTimer_.Start(1.5f, false);
		resultQuit_ = true;

		// ○○個突破スプライトの座標初期化
		spriteScoreCountOverPos_ = {-500.0f, 360.0f};

		// SE再生
		clearSE_->PlayAudio(SE_Volume);
	}

	// 終了テキストの更新
	UpdateEndText();

	if (sceneFade_->EndFadeIn()) {
		ctx_->lastScore = player_->GetScore();         // スコアを保存
		ctx_->lastShotCount = player_->GetShotCount(); // ショット数を保存
		ctx_->lastStunCount = player_->GetStunCount(); // スタン数を保存

		ChangeScene(goSceneNum_);
		goSceneNum_ = 0;

		// SEの解放
		ResetSE();
	}

	sceneFade_->Update();

	// カメラの座標Yをプレイヤーの座標Yに合わせる
	UpdateCameraToPlayer();

	// 曇モデル座標リセット
	if (player_->GetDirection() == PlayerDirection::DOWN && !isCloudPosReset_) {
		ResetCloudPos();
		isCloudPosReset_ = true;
	}

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

	// スコア増加アニメーション
	ScoreUpAnimation();

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

	// ○○個突破!スプライト更新
	spriteSnackCountOver_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	spriteSnackCountOver_->Update();

	// 進行度ゲージ更新
	UpdateProgressSprite();

	// クマのスプライト回転
	UpdateSpriteRotation();

	// ラムネの波紋
	UpdateSpriteChargeEffect();

	// 進行度ゲージスプライト更新
	spriteProgressLine_->Update();
	spriteProgressPlayer_->Update();
	spriteProgressGoal_->Update();
	spriteProgressMountaion_->Update();

	// ゲーム終了時に表示するスプライト更新
	spriteGameEnd_->Update();

	// スコアの背景波紋更新
	UpdateSpriteCandyEffect();

	// スコア表示の後ろに配置するスプライト更新
	spriteCandyScore_->Update();
	spriteCandyEffect_->Update();

	// ラムネゲージ
	spriteChargeUI_->Update();
	spriteChargeUIEffect_->Update();

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

	// オーディオ更新
	AudioUpdate();

	float currentScore = player_->GetScore();

	// 1000の倍数を突破したかチェック
	int lastThreshold = static_cast<int>(lastScoreChecked_ / 10000.0f);
	int currentThreshold = static_cast<int>(currentScore / 10000.0f);

	if (currentThreshold > lastThreshold) {
		StartSnackOverAnimation(); // アニメーション開始
	}

	lastScoreChecked_ = currentScore;

	// アニメーション関数
	AnimationSpriteSnackOver();

	// push スプライト
	spritePush_->Update();

	// push スプライトの切り替え
	PushSpriteChange();

	// ルール説明用のスプライトアニメーション
	UpdateRuleSprite();

	// 腕 スプライト
	spriteArm_->Update();

	// ピニャータ スプライト
	spriteThorn_->Update();

	// ○○個突破スコアの更新
	UpdateSpriteScoreOver();

	// ○○個突破スプライト
	for (auto& sprite : spriteScoreCountOver_) {
		sprite->Update();
	}

	// 地面モデル
	modelGround_->Update();

	// 雲モデル移動更新
	UpdateCloudModel();

	// 雲モデル
	for (auto& cloud : clouds_) {
		cloud->model.Update();
	}

	if (maskTimer_.IsActive()) {
		if (resultQuit_) {
			mask_->SetPosition({
				Easing::LerpVector2(maskEndPos_,maskStartPos_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskEndPos_,maskStartPos_,maskTimer_.GetProgress()).y });
			mask_->SetScale({
				Easing::LerpVector2(maskEndScale_,maskStartScale_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskEndScale_,maskStartScale_,maskTimer_.GetProgress()).y });
		}
		else {
			mask_->SetPosition({
				Easing::LerpVector2(maskStartPos_,maskEndPos_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskStartPos_,maskEndPos_,maskTimer_.GetProgress()).y });
			mask_->SetScale({
				Easing::LerpVector2(maskStartScale_,maskEndScale_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskStartScale_,maskEndScale_,maskTimer_.GetProgress()).y });
		}
	}
	else {
		if (resultQuit_) {
			mask_->SetPosition(maskStartPos_);
			mask_->SetScale(maskStartScale_);
		}
		else {
			mask_->SetPosition(maskEndPos_);
			mask_->SetScale(maskEndScale_);
		}
	}

	maskTimer_.Update();

	mask_->Update();
	maskBox_->Update();
	loadingUI_->Update();
	loadingPlayer_->Update();

	ore_->Update();

	srarArea1_->Update();
	srarArea2_->Update();
	srarArea3_->Update();
	srarArea4_->Update();
}

void GameScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	// 地面モデル
	modelGround_->Draw(*useCamera_);

	// 画面両端の幕のスプライト描画処理
	for (auto& curtain : curtainSprite_) {
		curtain->Draw();
	}

	// スコア表示の後ろに配置するスプライト描画
	spriteCandyScore_->Draw();
	spriteCandyEffect_->Draw();

	// プレイヤーの描画処理
	player_->Draw(*useCamera_);

	// 山のモデル描画
	/*for (auto& model : modelMountain_) {
	    model->Draw(*useCamera_);
	}*/

	// 雲モデル
	for (auto& cloud : clouds_) {
		cloud->model.Draw(*useCamera_);
	}

	// トゲの描画処理
	for (auto& thorn : thorns_) {
		if (thorn->GetIsAlive()) {
			thorn->Draw(*useCamera_);
		}
		thorn->DrawParticle(*useCamera_);
	}

	// ラムネゲージ
	spriteChargeUI_->Draw();
	spriteChargeUIEffect_->Draw();

	// 弾のゲージスプライト描画処理
	for (auto& gaugeInfo : bulletGaugeSprite_) {
		if (gaugeInfo.isActive) {
			gaugeInfo.sprite->Draw();
		}
	}

	// push スプライト
	if (player_->GetBulletGauge() > 0) { // 弾の数が1以上の時だけ描画する
		spritePush_->Draw();
	}

	// スコアスプライトの描画処理
	for (int i = 0; i < static_cast<int>(spriteScore_.size()); ++i) {
		if (spriteScore_[i]->isDraw) {
			spriteScore_[i]->sprite.Draw();
		}
	}

	// ルール説明用のスプライト描画
	spriteRule_->Draw();

	// 腕 スプライト
	spriteArm_->Draw();

	// ピニャータ スプライト
	spriteThorn_->Draw();

	// 進行度ゲージスプライト描画
	spriteProgressLine_->Draw();
	spriteProgressGoal_->Draw();
	spriteProgressMountaion_->Draw();
	spriteProgressPlayer_->Draw();

	// 入力なし　カウントダウン　スプライト 描画
	/*for (auto& sprite : spriteNoInputCountDown_) {
	    sprite->Draw();
	}*/

	// カウントダウン用のスプライト描画
	if (gameStartTimer_ >= 1 && gameStartTimer_ < 4.0f && player_->GetIsCameraSet()) {
		spriteNumber_->Draw();
	}

	// 「スタート!」スプライトの描画処理
	if (showStart_) {
		spriteStart_->Draw();
	}

	// ゲーム終了時に表示するスプライト描画
	if (isActiveEndText_) {
		spriteGameEnd_->Draw();
	}

	// ○○個突破!スプライト更新
	spriteSnackCountOver_->Draw();

	// ○○個突破スプライト
	for (auto& sprite : spriteScoreCountOver_) {
		sprite->Draw();
	}

	mask_->Draw();

	if (maskTimer_.IsFinished() && resultQuit_) {
		maskBox_->Draw();
		loadingUI_->Draw();
		loadingPlayer_->Draw();
	}

	ore_->Draw(*useCamera_);

	srarArea1_->Draw(*useCamera_);
	srarArea2_->Draw(*useCamera_);
	srarArea3_->Draw(*useCamera_);
	srarArea4_->Draw(*useCamera_);

	///
	/// ↑描画処理ここまで
	///

#ifdef DEVELOP_BUILD
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

#endif //  DEVELOP_BUILD

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void GameScene::UpdateCameraToPlayer() {
	if (player_->GetIsCameraSet()) {
		// カメラの座標Yをプレイヤーの座標Yに合わせる
		Vector3 pPos = player_->GetPosition();
		normalCamera_->SetPosition({0.0f + player_->GetShakeAmount().x, pPos.y + player_->CameraOffset() + player_->GetShakeAmount().y, cameraPosisionZ_});
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
				thorn->Spawn({static_cast<float>(x) * mag - 6.7f, static_cast<float>(y) * mag * -1.0f + mapHeight, 0.0f});
				thorns_.push_back(std::move(thorn));
			}
		}
	}
}

void GameScene::UpdateInput() {
	// 入力検知
	bool rightInput = input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D) || gamePad_->LeftStickX() >= 0.3f || gamePad_->PushButton(gamePad_->RIGHT_BOTTON); // 右入力
	bool leftInput = input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A) || gamePad_->LeftStickX() <= -0.3f || gamePad_->PushButton(gamePad_->LEFT_BOTTON);   // 左入力
	bool shotInput = input_->PushKey(DIK_SPACE) || gamePad_->PushButton(gamePad_->A);                                                                         // ショット入力

	if (rightInput || leftInput || shotInput) {
		isInput_ = true; // 入力があったらtrue
	} else {
		isInput_ = false; // 入力がなければfalse
	}
}

void GameScene::NoInputTitleBack() {
	//if (isInput_) {
	//	noInputTimer_ = 0; // 入力があったらリセット
	//	return;
	//} else {
	//	noInputTimer_ += 1.0f * deltaTime_; // タイマー加算

	//	// 5秒間入力がなかった場合
	//	if (noInputTimer_ >= gameSceneState_.maxNoInputTimer) {
	//		isBackToTitleScene_ = true;
	//		noInputTimer_ = 0;

	//		ResetSE(); // SEの解放
	//		player_->AudioReset();
	//	}
	//}
}

void GameScene::GameSceneStateImGui() {
	ImGui::Begin("GameScene State");

	ImGui::Text("noInputTimer : %f", noInputTimer_);

	nlohmann::json jsonState = gameSceneState_;

	// JsonNo中身をImGuiで表示する
	player_->DrawImGuiForJson(jsonState, 0.01f);

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

		if (displayNumber >= 1) {
			// SE再生
			countDownSE_->PlayAudio(SE_Volume);
		}
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
		player_->SetIsCountDownZero(true);
		showStart_ = true;
		startAnimTimer_ = 0.0f;
		// リセット状態にして表示開始
		spriteStart_->SetScale(startAnimStartScale_);
		spriteStart_->SetColor({1.0f, 1.0f, 1.0f, startAnimStartAlpha_});

		// SE再生
		startGameSE_->PlayAudio(SE_Volume);
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

	if (displayScore < 0.0f)
		return;

	for (int i = 0; i < digits.size(); ++i) {
		digits[digits.size() - 1 - i] = displayScore % 10;
		displayScore /= 10;
	}

	// 先頭を探索
	int firstNonZeroIndex = 0;
	while (firstNonZeroIndex < digits.size() - 1 && digits[firstNonZeroIndex] == 0) {
		++firstNonZeroIndex;
	}

	int visibleDigits = static_cast<int>(digits.size()) - firstNonZeroIndex;
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
		spriteArm_->SetPosition({spriteArm_->GetPosition().x, posY});
		spriteThorn_->SetPosition({spriteThorn_->GetPosition().x, posY});

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
		spriteArm_->SetPosition({spriteArm_->GetPosition().x, ruleEndPosY_});
		spriteThorn_->SetPosition({spriteThorn_->GetPosition().x, ruleEndPosY_});
		break;
	}
	case RuleAnimState::Falling: {
		float t = std::clamp(timerSpriteRule_ / ruleDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseInCubic);
		float posY = std::lerp(ruleEndPosY_, ruleStartPosY_, eased);
		spriteRule_->SetPosition({640.0f, posY});
		spriteArm_->SetPosition({spriteArm_->GetPosition().x, posY});
		spriteThorn_->SetPosition({spriteThorn_->GetPosition().x, posY});

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
		spriteArm_->SetPosition({spriteArm_->GetPosition().x, ruleStartPosY_});
		spriteThorn_->SetPosition({spriteThorn_->GetPosition().x, ruleStartPosY_});
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
	spriteProgressPlayer_->SetPosition({linePos.x - 4.0f, mappedY});
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

void GameScene::AudioUpdate() {
	startGameSE_->SetVolume(SE_Volume);
	countDownSE_->SetVolume(SE_Volume);
	clearSE_->SetVolume(SE_Volume);
	gameSceneBGM01_->SetVolume(BGM_Volume);
	gameSceneBGM02_->SetVolume(BGM_Volume);

	startGameSE_->Update();
	countDownSE_->Update();
	clearSE_->Update();
	gameSceneBGM01_->Update();
	gameSceneBGM02_->Update();
}

void GameScene::UpdateSpriteScoreOver() {
	if (player_->GetScore() >= nextMilestone_) {
		// 数字を桁ごとに分解
		std::vector<int> digits;

		int value = static_cast<int>(nextMilestone_);

		while (value > 0) {
			digits.push_back(value % 10);
			value /= 10;
		}
		std::reverse(digits.begin(), digits.end());

		// スプライトを並べる
		float basePosX = 100;
		int digitWidth = 32;

		for (size_t i = 0; i < digits.size(); ++i) {
			int digit = digits[i];
			spriteScoreCountOver_[i]->SetTexture(spriteNumCollection_[digit]);
			spriteScoreCountOver_[i]->SetPosition({-600.0f, spriteScoreCountOver_[i]->GetPosition().y});
			spriteScoreCountOver_[i]->SetColor({1.0f, 1.0f, 1.0f, 0.8f});
		}

		nextMilestone_ += 10000;
	}
}

void GameScene::AnimationSpriteSnackOver() {
	timerSnackCountOver_ += deltaTime_;

	// スコアと○○個突破の余白
	float offsetX = 300.0f;

	if (player_->GetScore() >= 10000.0f) {
		offsetX = 350.0f;
	}

	switch (snackCountOverAnimationState_) {
	case RuleAnimState::Rising: {
		float t = std::clamp(timerSnackCountOver_ / ruleDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseOutCubic);
		float posX = std::lerp(kScoreOverStartPosX_, kScoreOverMiddlePosX_, eased);
		spriteScoreCountOverPos_.x = posX;
		spriteSnackCountOver_->SetPosition(spriteScoreCountOverPos_);

		for (int i = 0; i < spriteScoreCountOver_.size(); ++i) {
			spriteScoreCountOver_[i]->SetPosition({spriteScoreCountOverPos_.x + i * 64.0f - offsetX, spriteScoreCountOver_[i]->GetPosition().y});
		}

		if (t >= 1.0f) {
			// 到達したら待機へ移行
			snackCountOverAnimationState_ = RuleAnimState::Waiting;
			timerSnackCountOver_ = 0.0f;
		}
		break;
	}
	case RuleAnimState::Waiting: {
		// そのまま指定秒だけ待つ
		if (timerSnackCountOver_ >= kScoreOverWaitDuration_) {
			snackCountOverAnimationState_ = RuleAnimState::Falling;
			timerSnackCountOver_ = 0.0f;
		}
		// 待機中は位置を終了位置に固定
		spriteScoreCountOverPos_.x = kScoreOverMiddlePosX_;
		spriteSnackCountOver_->SetPosition(spriteScoreCountOverPos_);

		for (int i = 0; i < spriteScoreCountOver_.size(); ++i) {
			spriteScoreCountOver_[i]->SetPosition({spriteScoreCountOverPos_.x + i * 64.0f - offsetX, spriteScoreCountOver_[i]->GetPosition().y});
		}
		break;
	}
	case RuleAnimState::Falling: {
		float t = std::clamp(timerSnackCountOver_ / ruleDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseInCubic);
		float posX = std::lerp(kScoreOverMiddlePosX_, kScoreOverEndPosX_, eased);
		spriteScoreCountOverPos_.x = posX;
		spriteSnackCountOver_->SetPosition(spriteScoreCountOverPos_);

		for (int i = 0; i < spriteScoreCountOver_.size(); ++i) {
			spriteScoreCountOver_[i]->SetPosition({spriteScoreCountOverPos_.x + i * 64.0f - offsetX, spriteScoreCountOver_[i]->GetPosition().y});
		}

		if (t >= 1.0f) {
			// 終了処理
			if (ruleLoop_) {
				snackCountOverAnimationState_ = RuleAnimState::Rising;
			} else {
				snackCountOverAnimationState_ = RuleAnimState::Done;
			}
			timerSnackCountOver_ = 0.0f;
		}
		break;
	}
	case RuleAnimState::Done: {
		// 完了後は位置を開始位置に固定
		spriteScoreCountOverPos_.x = kScoreOverEndPosX_;
		spriteSnackCountOver_->SetPosition(spriteScoreCountOverPos_);

		for (int i = 0; i < spriteScoreCountOver_.size(); ++i) {
			spriteScoreCountOver_[i]->SetPosition({spriteScoreCountOverPos_.x + i * 64.0f - offsetX, spriteScoreCountOver_[i]->GetPosition().y});
		}
		break;
	}
	}
}

void GameScene::StartSnackOverAnimation() {
	snackCountOverAnimationState_ = RuleAnimState::Rising;
	timerSnackCountOver_ = 0.0f;
}

void GameScene::CameraStateImGui() {
	ImGui::Begin("Camera State");

	ImGui::DragFloat("PositionZ", &cameraPosisionZ_, 0.01f);

	ImGui::DragFloat2("positionArm", &spriteArm_->GetPosition().x);
	ImGui::DragFloat2("positionThorn", &spriteThorn_->GetPosition().x);

	ImGui::End();
}

void GameScene::PushSpriteChange() {
	pushSpriteTimer_++;

	if ((pushSpriteTimer_ / 8) % 2 == 0) {
		spritePush_->SetTexture("resources/image/UI/PushButton01UI.png");
	} else {
		spritePush_->SetTexture("resources/image/UI/PushButton02UI.png");
	}
}

void GameScene::UpdateSpriteRotation() {
	float maxTiltRad = std::numbers::pi_v<float> / 10.0f;
	float speed = 0.05f;

	float angleRad = std::sin(frameCount_ * speed) * maxTiltRad;

	// 角度を適用
	spriteProgressPlayer_->SetRotate(angleRad); // 進行度ゲージ　クマ
	spriteCandyScore_->SetRotate(angleRad);     // スコアの背景
	spriteCandyEffect_->SetRotate(angleRad);

	frameCount_++;
}

void GameScene::UpdateSpriteChargeEffect() {
	// 弾の数に応じてサイズの変更速度を変える
	float changeSpeed = 0.0f;
	int gauge = player_->GetBulletGauge();

	if (gauge >= 1 && gauge <= 5) {
		changeSpeed = gaugeSizeSpeeds[gauge - 1];
	}

	// サイズと透明度を変更
	chargeEffectAlpha_ -= changeSpeed;
	chargeEffectSize_ += changeSpeed;

	if (chargeEffectAlpha_ <= 0.0f) {
		chargeEffectAlpha_ = 1.0f;
		chargeEffectSize_ = 0.4f;
	}

	spriteChargeUIEffect_->SetScale({chargeEffectSize_, chargeEffectSize_});
	spriteChargeUIEffect_->SetColor({1.0f, 1.0f, 1.0f, chargeEffectAlpha_});
}

void GameScene::UpdateSpriteCandyEffect() {
	// スコアに応じてサイズの変更速度を変える
	float changeSpeed = 0.0f;

	if (player_->GetScore() >= 9000.0f) {
		changeSpeed = candySizeSpeeds[4];
	} else if (player_->GetScore() >= 7000.0f) {
		changeSpeed = candySizeSpeeds[3];
	} else if (player_->GetScore() >= 5000.0f) {
		changeSpeed = candySizeSpeeds[2];
	} else if (player_->GetScore() >= 3000.0f) {
		changeSpeed = candySizeSpeeds[1];
	} else if (player_->GetScore() >= 1000.0f) {
		changeSpeed = candySizeSpeeds[0];
	}

	// サイズと透明度を変更
	candyEffectAlpha_ -= 0.05f;
	candyEffectSize_ += changeSpeed;

	if (candyEffectAlpha_ <= 0.0f) {
		candyEffectAlpha_ = 1.0f;
		candyEffectSize_ = 0.5f;
	}

	spriteCandyEffect_->SetScale({candyEffectSize_, candyEffectSize_});
	spriteCandyEffect_->SetColor({spriteCandyEffect_->GetColor().x, spriteCandyEffect_->GetColor().y, spriteCandyEffect_->GetColor().z, candyEffectAlpha_});
}

void GameScene::ResetSE() {
	startGameSE_->Reset();
	countDownSE_->Reset();
	clearSE_->Reset();
	gameSceneBGM01_->Reset();
	gameSceneBGM02_->Reset();
}

void GameScene::ScoreUpAnimation() {
	if (player_->GetIsScoreUpAnimation()) {
		scoreUpTimer_ += deltaTime_;

		float t = std::clamp(scoreUpTimer_ / scoreUpDuration_, 0.0f, 1.0f);
		float eased = Easing::Apply(t, Easing::Type::EaseOutElastic);

		Vector2 scale;
		const float kStartScale = 0.4f;
		const float kEndScale = 0.8f;

		scale.x = Lerp(kStartScale, kEndScale, eased);
		scale.y = Lerp(kStartScale, kEndScale, eased);

		// スコア用スプライト5桁分
		for (int i = 0; i < spriteScore_.size(); ++i) {
			spriteScore_[i]->sprite.SetScale(scale);
		}

		if (t >= 1.0f) {
			scoreUpTimer_ = 0.0f;
			player_->SetIsScoreUpAnimation(false);
		}
	}
}

void GameScene::UpdateRuleSprite() {
	static float time = 0.0f;
	time += deltaTime_;

	float amplitude = std::numbers::pi_v<float> / 12.0f; // 揺れ幅
	float frequency = 32.0f;                             // 揺れの速さ

	float angle = std::sin(time * frequency) * amplitude;

	// 回転
	spriteArm_->SetRotate(angle);
	spriteThorn_->SetRotate(-angle);

	// スケール
	float scale = 0.25f + std::sin(time * frequency) * 0.05f;
	spriteArm_->SetScale({scale, scale});
	spriteThorn_->SetScale({scale, scale});
}

float GameScene::RandomFloat(float min, float max) {
	// 二つの値からランダムに値を返す
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(rng);
}

void GameScene::UpdateCloudModel() {
	if (player_->GetPosition().y < 150.0f)
		return;

	for (auto& model : clouds_) {
		// 雲の移動
		if (model->direction == Direction::LEFT) {
			model->model.GetTranslate().x -= 0.01f;
		} else {
			model->model.GetTranslate().x += 0.01f;
		}
	}
}

int GameScene::GetRandomDirection() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<int> dist(0, 1);

	return dist(gen);
}

void GameScene::ResetCloudPos() {
	for (int i = 0; i < clouds_.size(); ++i) {
		// 初期座標X
		const float MIN_POS_X = -7.0f;
		const float MAX_POS_X = 7.0f;

		std::random_device rd2;
		std::mt19937 eng2(rd2());
		std::uniform_real_distribution<float> distr2(MIN_POS_X, MAX_POS_X);

		// 座標リセット
		clouds_[i]->model.SetTranslate({distr2(eng2), clouds_[i]->model.GetTranslate().y, 10.0f});
	}
}
