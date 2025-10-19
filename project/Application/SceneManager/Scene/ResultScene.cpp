#include "ResultScene.h"

void ResultScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
	// 切り替え後に初期化して描画用変数へコピー
	lastScore_ = ctx_->lastScore;
	lastShotCount_ = ctx_->lastShotCount;
	lastStunCount_ = ctx_->lastStunCount;
}

void ResultScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 0.0f, -10.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	// 背景スプライト
	spriteBG_->Initialize(&ctx_->dxCommon, "resources/image/white16x16.png");
	spriteBG_->SetPosition({600.0f, 340.0f});
	spriteBG_->SetScale({76, 43});
	spriteBG_->SetColor({0, 0, 0, 1});

	// スコア用のスプライト集
	for (int i = 0; i < spriteNumCollection_.size(); ++i) {
		spriteNumCollection_[i] = "resources/image/UI/Number" + std::to_string(i) + "UI.png";
	}

	// スコア用スプライト5桁分
	for (int i = 0; i < spriteScore_.size(); ++i) {
		spriteScore_[i] = make_unique<Sprite>();
		spriteScore_[i]->Initialize(&ctx_->dxCommon, "resources/image/number/0.png");
		spriteScore_[i]->SetPosition({450.0f + i * 120.0f, 400.0f});
		spriteScore_[i]->SetScale({2, 2});
	}

	// 描画用のスコア
	drawScore_ = 0.0f;

	// 集めたお菓子の個数テキストスプライト
	spriteCollectedSweets_->Initialize(&ctx_->dxCommon, "resources/image/UI/CollectedCandyCountUI.png");
	spriteCollectedSweets_->SetPosition({300.0f, 200.0f});
	spriteCollectedSweets_->SetScale({10.0f * deltaTime_, 10.0f * deltaTime_});

	// お菓子降ってくる演出用モデル
	for (int i = 0; i < sweetModels_.size(); ++i) {
		sweetModels_[i] = make_unique<Model>();
		sweetModels_[i]->Initialize(&ctx_->dxCommon, "Candy/Candy.obj");
		sweetModels_[i]->SetTranslate({-500.0f, 0.0f});
		sweetModels_[i]->SetScale({1, 1});
	}

	// 「リトライ」テキスト
	spriteRetry_->Initialize(&ctx_->dxCommon, "resources/image/UI/RetryUI.png");
	spriteRetry_->SetPosition({360.0f, 560.0f});
	spriteRetry_->SetScale({0.4f, 0.4f});

	// 「タイトルへ」テキスト
	spriteBackToTitle_->Initialize(&ctx_->dxCommon, "resources/image/UI/BackToTitleUI.png");
	spriteBackToTitle_->SetPosition({920.0f, 560.0f});
	spriteBackToTitle_->SetScale({0.4f, 0.4f});

	// スクリーン切り替えフラグ
	isScreenChange_ = false;

	// カーソルスプライト
	spriteCursol_->Initialize(&ctx_->dxCommon, "resources/image/UI/Cursol.png");
	spriteCursol_->SetPosition({135.0f, 565.0f});
	spriteCursol_->SetScale({0.5f, 0.5f});

	// 「スタン回数」スプライト
	spriteStunCount_->Initialize(&ctx_->dxCommon, "resources/image/UI/StunCountUI.png");
	spriteStunCount_->SetPosition({440.0f, 170.0f});
	spriteStunCount_->SetScale({0.3f, 0.3f});

	// 「ショット」スプライト
	spriteShotCount_->Initialize(&ctx_->dxCommon, "resources/image/UI/ShotCountUI.png");
	spriteShotCount_->SetPosition({425.0f, 300.0f});
	spriteShotCount_->SetScale({0.3f, 0.3f});

	// 現在のスクリーン
	currentScreen_ = Screen::FIRST;

	// 現在のメニュー
	currentMenu_ = Menu::RETRY;

	// お菓子降ってくる演出用モデル更新
	for (int i = 0; i < sweetModels_.size(); ++i) {
		sweetModels_[i]->Update();
	}

	// ショット数用スプライト2桁分
	for (int i = 0; i < spriteShot_.size(); ++i) {
		spriteShot_[i] = make_unique<Sprite>();
		spriteShot_[i]->Initialize(&ctx_->dxCommon, "resources/image/number/0.png");
		spriteShot_[i]->SetPosition({700.0f + i * 60.0f, 300.0f});
		spriteShot_[i]->SetScale({1, 1});
	}

	// スタン数用スプライト2桁分
	for (int i = 0; i < spriteStun_.size(); ++i) {
		spriteStun_[i] = make_unique<Sprite>();
		spriteStun_[i]->Initialize(&ctx_->dxCommon, "resources/image/number/0.png");
		spriteStun_[i]->SetPosition({700.0f + i * 60.0f, 170.0f});
		spriteStun_[i]->SetScale({1, 1});
	}
}

void ResultScene::Update() {
	bool inputEnter = input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(gamePad_->A);

	// 1枚目のスクリーンの更新
	ScreenFirstUpdate();

	// 2枚目のスクリーンの更新
	ScreenSecondUpdate();

	sceneFade_->Update();

	// カメラ切り替え&更新
	CameraController();

	// 背景スプライトの更新処理
	spriteBG_->Update();

	// 描画用のスコアが実施のスコアと同じになったら
	if (isScreenChange_ && inputEnter) {
		currentScreen_ = Screen::SECOND;

		// ショットカウントスプライト更新
		SpriteShotCountUpdate();

		// スタンカウントスプライト更新
		SpriteStunCountUpdate();
	}
}

void ResultScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	// 背景スプライトの描画処理
	spriteBG_->Draw();

	sceneFade_->Draw();

	// 1枚目のスクリーンの描画
	ScreenFirstDraw();

	// 1枚目のスクリーンの描画
	ScreenSecondDraw();

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

	DrawSceneName();

	ImGui::Begin("Test");

	ImGui::DragFloat2("TestPos0", &testPos_[0].x, 1.0f);
	ImGui::DragFloat2("TestScale0", &testScale_[0].x, 0.1f);

	ImGui::DragFloat2("TestPos1", &testPos_[1].x, 1.0f);
	ImGui::DragFloat2("TestScale1", &testScale_[1].x, 0.1f);

	ImGui::End();

	///
	/// ↑ImGuiここまで
	///

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	// ここより下に描画処理を書かない
	ctx_->dxCommon.PostDraw();
}

void ResultScene::CameraController() {
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

void ResultScene::ScoreCountUpdate() {
	// 加算
	drawScore_ = drawScore_ + (lastScore_ - drawScore_) * COUNT_SPEED;

	// 加算を打ち切り
	if (std::abs(drawScore_ - lastScore_) < 0.1f) {
		drawScore_ = lastScore_;
		isScreenChange_ = true; // スクリーン切り替えフラグオン
	}
}

void ResultScene::SpriteScoreUpdate() {
	int displayScore = static_cast<int>(drawScore_);

	for (int i = 0; i < digits.size(); ++i) {
		digits[digits.size() - 1 - i] = displayScore % 10;
		displayScore /= 10;
	}

	for (int i = 0; i < spriteScore_.size(); ++i) {
		int digit = digits[i];
		spriteScore_[i]->SetTexture(spriteNumCollection_[digit]);
	}
}

ResultScene::~ResultScene() { CleanupResources(); }

void ResultScene::CleanupResources() {
	// 背景スプライトのクリーンアップ
	if (spriteBG_) {
		spriteBG_.reset();
	}

	// スコアスプライトのクリーンアップ
	for (auto& sprite : spriteScore_) {
		if (sprite) {
			sprite.reset();
		}
	}
	if (spriteShotCount_) {
		spriteCollectedSweets_.reset();
	}

	if (spriteShotCount_) {
		spriteRetry_.reset();
	}

	if (spriteShotCount_) {
		spriteBackToTitle_.reset();
	}

	if (spriteShotCount_) {
		spriteCursol_.reset();
	}

	if (spriteShotCount_) {
		spriteStunCount_.reset();
	}

	if (spriteShotCount_) {
		spriteShotCount_.reset();
	}

	for (auto& sprite : spriteShot_) {
		if (sprite) {
			sprite.reset();
		}
	}

	for (auto& sprite : spriteStun_) {
		if (sprite) {
			sprite.reset();
		}
	}
}

void ResultScene::ScreenFirstUpdate() {
	if (currentScreen_ == Screen::FIRST) {
		// スコアスプライトの更新処理
		for (int i = 0; i < spriteScore_.size(); ++i) {
			spriteScore_[i]->Update();
		}

		// 描画用のスコアカウント
		ScoreCountUpdate();

		// スプライトの切り替え
		SpriteScoreUpdate();

		// 集めたお菓子の個数テキストスプライト更新
		spriteCollectedSweets_->Update();

		// お菓子降ってくる演出用モデル更新
		for (int i = 0; i < sweetModels_.size(); ++i) {
			sweetModels_[i]->Update();
		}
	}
}

void ResultScene::ScreenSecondUpdate() {
	if (currentScreen_ == Screen::SECOND) {
		// メニュー切り替え
		UpdateMenu();

		// 「リトライ」テキスト更新
		spriteRetry_->Update();

		// 「タイトルへ」テキスト更新
		spriteBackToTitle_->Update();

		// カーソルスプライト更新
		spriteCursol_->Update();

		/*spriteStunCount_->SetPosition(testPos_[0]);
		spriteStunCount_->SetScale(testScale_[0]);
		spriteShotCount_->SetPosition(testPos_[1]);
		spriteShotCount_->SetScale(testScale_[1]);*/

		// 「スタン回数」スプライト更新
		spriteStunCount_->Update();

		// 「ショット回数」スプライト更新
		spriteShotCount_->Update();

		// スタン数用スプライト2桁分更新
		for (int i = 0; i < spriteStun_.size(); ++i) {
			spriteStun_[i]->Update();
		}

		// ショット数用スプライト2桁分更新
		for (int i = 0; i < spriteShot_.size(); ++i) {
			spriteShot_[i]->Update();
		}
	}
}

void ResultScene::ScreenFirstDraw() {
	if (currentScreen_ == Screen::FIRST) {
		// スコアスプライトの更新処理
		for (int i = 0; i < spriteScore_.size(); ++i) {
			spriteScore_[i]->Draw();
		}

		// 集めたお菓子の個数テキストスプライト更新
		spriteCollectedSweets_->Draw();

		// お菓子降ってくる演出用モデル描画
		for (int i = 0; i < sweetModels_.size(); ++i) {
			sweetModels_[i]->Draw(*useCamera_);
		}
	}
}

void ResultScene::ScreenSecondDraw() {
	if (currentScreen_ == Screen::SECOND) {
		// 「リトライ」テキスト描画
		spriteRetry_->Draw();

		// 「タイトルへ」テキスト描画
		spriteBackToTitle_->Draw();

		// カーソルスプライト描画
		spriteCursol_->Draw();

		// 「スタン回数」スプライト描画
		spriteStunCount_->Draw();

		// 「ショット回数」スプライト描画
		spriteShotCount_->Draw();

		// スタン数用スプライト2桁分描画
		for (int i = 0; i < spriteStun_.size(); ++i) {
			spriteStun_[i]->Draw();
		}

		// ショット数用スプライト2桁分描画
		for (int i = 0; i < spriteShot_.size(); ++i) {
			spriteShot_[i]->Draw();
		}
	}
}

void ResultScene::UpdateMenu() {
	bool rightInput = input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D) || /*gamePad_->LeftStickY() >= 0.3f ||*/ gamePad_->TriggerButton(gamePad_->DPAD_RIGHT); // 上ボタン
	bool leftInput = input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_A) || /*gamePad_->LeftStickY() <= -0.3f ||*/ gamePad_->TriggerButton(gamePad_->DPAD_LEFT);      // 下ボタン
	bool enterInput = input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(gamePad_->A);                                                                             // 決定ボタン

	// メニュー切り替え
	switch (currentMenu_) {
	case Menu::RETRY:
		if (rightInput) { // 右入力
			currentMenu_ = Menu::BACK_TO_TITLE;

			spriteCursol_->SetPosition({645.0f, 565.0f});
		}

		if (enterInput) { // 決定
			sceneFade_->StartFadeIn(1.0f);
		}

		if (sceneFade_->EndFadeIn()) {
			ChangeScene(SCENE::GAME); // ゲームシーンへ
		}

		break;

	case Menu::BACK_TO_TITLE:
		if (leftInput) { // 左入力
			currentMenu_ = Menu::RETRY;

			spriteCursol_->SetPosition({135.0f, 565.0f});
		}

		if (enterInput) { // 決定
			sceneFade_->StartFadeIn(1.0f);
		}

		if (sceneFade_->EndFadeIn()) {
			ChangeScene(SCENE::TITLE); // タイトルへ
		}

		break;
	}
}

void ResultScene::SpriteShotCountUpdate() {
	int displayShotCount = lastShotCount_;

	for (int i = 0; i < shotCountDigits.size(); ++i) {
		shotCountDigits[shotCountDigits.size() - 1 - i] = displayShotCount % 10;
		displayShotCount /= 10;
	}

	for (int i = 0; i < spriteShot_.size(); ++i) {
		int digit = shotCountDigits[i];
		spriteShot_[i]->SetTexture(spriteNumCollection_[digit]);
	}
}

void ResultScene::SpriteStunCountUpdate() {
	int displayStunCount = lastStunCount_;

	for (int i = 0; i < stunCountDigits.size(); ++i) {
		stunCountDigits[stunCountDigits.size() - 1 - i] = displayStunCount % 10;
		displayStunCount /= 10;
	}

	for (int i = 0; i < spriteStun_.size(); ++i) {
		int digit = stunCountDigits[i];
		spriteStun_[i]->SetTexture(spriteNumCollection_[digit]);
	}
}
