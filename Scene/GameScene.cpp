#include "GameScene.h"

void GameScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	player_->Initialize(&ctx_->dxCommon);
	skyBox_->Initialize(&ctx_->dxCommon);

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon, "resources/image/fade.png", { 1280.0f,720.0f });
	fade_->SetPosition({ 640.0f,360.0f });
	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });

	loadingUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/loading.png", { 1280.0f,720.0f });
	loadingUI_->SetPosition({ 640.0f,360.0f });
	loadingUI_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });

	pauseBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause1.png", { 1280.0f,720.0f });
	pauseBG_->SetPosition({ 640.0f,360.0f });

	pauseUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause2.png", { 1280.0f,720.0f });
	pauseUI_->SetPosition({ 640.0f,360.0f });
}

void GameScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	if (gamePad_->TriggerButton(GamePad::X)) {
		player_->Heal();
	}
	if (gamePad_->TriggerButton(GamePad::Y)) {
		player_->Damage();
	}

	UpdatePause();

	if (!isPause_) {
		player_->Update(useCamera_);
		skyBox_->Update(useCamera_);
	}

	pauseBG_->Update();
	pauseUI_->Update();

	UpdateFade();
}

void GameScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	skyBox_->Draw();
	player_->Draw();

	if (isPause_) {
		pauseBG_->Draw();
		pauseUI_->Draw();
	}

	fade_->Draw();
	loadingUI_->Draw();

	///
	/// ↑描画処理ここまで
	///

	///
	/// ↓ImGuiここから
	///

	// フレームの先頭でImguiにここからフレームが始まる旨を告げる
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// 開発用UIの処理、実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
	/*ImGui::ShowDemoWindow();*/

	DrawFPS_ImGui();

	debugCamera_->DrawImgui();

	player_->DrawImGui();

	pauseUI_->DrawImGui("ui");

	//skyBox_->DrawImGui();

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void GameScene::UpdateFade() {
	if (isFade_) {
		fadeAlpha_ += 0.5f * deltaTime_;
		if (fadeAlpha_ >= 1.0f) {
			goTitleScene_ = true;
		}
	} else {
		fadeAlpha_ -= 0.5f * deltaTime_;
	}
	fadeAlpha_ = std::clamp(fadeAlpha_, 0.0f, 1.0f);

	loadingUI_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });
	loadingUI_->Update();

	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });
	fade_->Update();
}

void GameScene::UpdatePause() {
	if (isPause_) {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = false;
		}

		switch (pause_)
		{
		case GameScene::kBack:
			pauseUI_->SetTexture("resources/image/UI/pause2.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				pause_ = kQuit;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isPause_ = false;
			}
			break;
		case GameScene::kQuit:
			pauseUI_->SetTexture("resources/image/UI/pause3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				pause_ = kBack;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isFade_ = true;
			}
			break;
		}

	}
	else {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = true;
		}
		pause_ = kBack;
	}
}

void GameScene::CameraController() {
	if (ctx_->input.TriggerKey(DIK_SPACE)) {
		if (isDebugCamera_) {
			isDebugCamera_ = false;
		} else {
			isDebugCamera_ = true;
		}
	}

	if (isDebugCamera_) {
		if (debugCamera_ != nullptr) {
			debugCamera_->Update();
			useCamera_ = debugCamera_.get();
		}
	}
	else {
		if (camera_ != nullptr) {
			camera_->Update();
			useCamera_ = camera_.get();
		}
	}
}

void GameScene::DrawFPS_ImGui() {
	static float fpsHistory[100] = {};
	static int frameCount = 0;

	float currentFPS = ImGui::GetIO().Framerate;
	fpsHistory[frameCount % IM_ARRAYSIZE(fpsHistory)] = currentFPS;
	frameCount++;

	ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - 20.0f, 20.0f); // 右上にオフセット付き
	ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f); // 原点を右上にする
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.8f); // 背景を少し透過

	ImGui::Begin("Client FPS", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	// FPSグラフと数値の描画
	ImGui::PlotLines("##FPSGraph", fpsHistory, IM_ARRAYSIZE(fpsHistory), 0, nullptr, 0.0f, 175.0f, ImVec2(0, 80));
	ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
	ImGui::Separator();
	ImGui::Text("LoadTexture Count: %zu", TextureManager::GetInstance()->GetTextureCount());
	ImGui::Text("Path-Index Map Size: %zu", TextureManager::GetInstance()->GetPathToIndexMapSize());
	ImGui::Text("Max SRV Slots: %u", DirectXCommon::kMaxSRVCount_);

	ImGui::End();
}