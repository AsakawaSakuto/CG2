#include "ResultScene.h"

ResultScene::~ResultScene() {
	// シーンを抜ける際にランキングデータを保存
	// （Scoreクラスの初期化時に既にソート・更新されているが、念のため再保存）
	if (score_) {
		score_->SaveRankingData();
	}
}

void ResultScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
	lastScore_ = ctx_->lastScore;
}

void ResultScene::Initialize() {
	// inputSystemの初期化
	gamePad_ = &ctx_->gamePad;
	input_ = &ctx_->input;

	// カメラの初期化
	debugCamera_->SetInput(&ctx_->input);
	normalCamera_->SetPosition({0.0f, 2.0f, -20.0f});
	normalCamera_->SetRotate({0.0f, 0.0f, 0.0f});

	// Create SceneFade
	sceneFade_ = std::make_unique<SceneFade>();
	sceneFade_->Initialize(&ctx_->dxCommon);
	sceneFade_->StartFadeOut(1.0f);

	// ★Scoreクラスの初期化（ここで自動的にランキングがソートされて更新される）
	score_->Initialize(&ctx_->dxCommon, ctx_->lastScore);
	score_->SetInput(&ctx_->input, &ctx_->gamePad);

	// ★ランキング更新結果の確認（デバッグ用 - 必要に応じてコメントアウト）
	if (score_->IsNewRecord()) {
		// 新記録の場合の処理を追加可能
		// 例：特別なエフェクトの再生、音声の再生など
		int position = score_->GetRankingPosition();
		// position: 1=1st, 2=2nd, 3=3rd
	}

	maskBox_->Initialize(&ctx_->dxCommon, "resources/image/mask/box.png", { 640.0f,360.0f }, { 1.0f,1.0f });
	loadingUI_->Initialize(&ctx_->dxCommon, "resources/image/mask/loadingUI.png", { 1040.0f, 640.0f }, { 1.0f, 1.0f });
	loadingUI_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	loadingPlayer_->Initialize(&ctx_->dxCommon, "resources/image/mask/loadingPlayer.png", { 680.0f, 615.0f }, { 0.3f, 0.3f });

	maskType_ = static_cast<MaskType>(rand_.Int(0, 2));

	switch (maskType_)
	{
	case ResultScene::MaskType::RAMA:
		mask_->Initialize(&ctx_->dxCommon, "resources/image/mask/view01.png", { 640.0f,360.0f }, { 1.0f,1.0f });
		maskStartScale_ = { 0.26f, 0.26f };
		maskEndScale_ = { 8.0f, 8.0f };
		break;
	case ResultScene::MaskType::KUMA:
		mask_->Initialize(&ctx_->dxCommon, "resources/image/mask/view02.png", { 640.0f,360.0f }, { 1.0f,1.0f });
		maskStartScale_ = { 0.26f, 0.26f };
		maskEndScale_ = { 2.5f, 2.5f };
		break;
	case ResultScene::MaskType::AME:
		mask_->Initialize(&ctx_->dxCommon, "resources/image/mask/view03.png", { 640.0f,360.0f }, { 1.0f,1.0f });
		maskStartScale_ = { 0.26f, 0.26f };
		maskEndScale_ = { 5.0f, 5.0f };
		break;
	}

	// マスクのアニメーション設定 - 画面外から中央へ
	maskStartPos_ = { 640.0f, 360.0f };  // 画面上部から開始
	maskEndPos_ = { 640.0f, 360.0f };    // 画面中央で終了
	
	maskTimer_.Start(1.0f, false);
	quitTimer_.Start(30.0f, false);

	timerStarte_ = false;
	resultQuit_ = false;
}

void ResultScene::Update() {

	if (score_->GetNext() && !resultQuit_) {
		resultQuit_ = true;
		maskTimer_.Start(1.0f, false);
		score_->SaveRankingData();
	}

	if (maskTimer_.IsActive()) {
		if (resultQuit_) {
			mask_->SetPosition({
				Easing::LerpVector2(maskEndPos_,maskStartPos_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskEndPos_,maskStartPos_,maskTimer_.GetProgress()).y });
			mask_->SetScale({
				Easing::LerpVector2(maskEndScale_,maskStartScale_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskEndScale_,maskStartScale_,maskTimer_.GetProgress()).y });
		} else {
			mask_->SetPosition({
				Easing::LerpVector2(maskStartPos_,maskEndPos_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskStartPos_,maskEndPos_,maskTimer_.GetProgress()).y });
			mask_->SetScale({
				Easing::LerpVector2(maskStartScale_,maskEndScale_,maskTimer_.GetProgress()).x,
				Easing::LerpVector2(maskStartScale_,maskEndScale_,maskTimer_.GetProgress()).y });
		}
	} else {
		if (resultQuit_) {
			mask_->SetPosition(maskStartPos_);
			mask_->SetScale(maskStartScale_);
		} else {
			mask_->SetPosition(maskEndPos_);
			mask_->SetScale(maskEndScale_);
		}
	}

	if (maskTimer_.IsFinished() && resultQuit_) {
		if (quitTimer_.IsFinished()) {
			score_->ResetAudio();
			ChangeScene(TITLE);
		} else {
			if (score_->NextSceneNum() == 0) {
				score_->ResetAudio();
				ChangeScene(TITLE);
			} else {
				score_->ResetAudio();
				ChangeScene(GAME);
			}
		}
	}

	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_S) || input_->TriggerKey(DIK_D) ||
		gamePad_->TriggerButton(GamePad::A) || gamePad_->TriggerButton(GamePad::B) || gamePad_->TriggerButton(GamePad::X) || gamePad_->TriggerButton(GamePad::L) || gamePad_->TriggerButton(GamePad::UP_BOTTON) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON) || gamePad_->TriggerButton(GamePad::LEFT_BOTTON) || gamePad_->TriggerButton(GamePad::RIGHT_BOTTON)) {
		quitTimer_.Start(30.0f, false);
	}

	if (quitTimer_.IsFinished() && !resultQuit_) {
		resultQuit_ = true;
		maskTimer_.Start(1.0f, false);
		score_->SaveRankingData();
	}

	maskTimer_.Update();
	quitTimer_.Update();

	score_->Update();

	mask_->Update();
	maskBox_->Update();
	loadingUI_->Update();
	loadingPlayer_->Update();

	CameraController();
}

void ResultScene::Draw() {

	// ここより上に描画処理を書かない
	ctx_->dxCommon.PreDraw();

	///
	/// ↓描画処理ここから
	///

	score_->Draw(*useCamera_);

	//sceneFade_->Draw();

	mask_->Draw();

	if (maskTimer_.IsFinished() && resultQuit_) {
		maskBox_->Draw();
		loadingUI_->Draw();
		loadingPlayer_->Draw();
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

	score_->DrawImGui();

	DrawSceneName();

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