#include "GameScene.h"

void GameScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void GameScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	cameraRx_ = -0.5f;
	camera_->SetRotate({ cameraRx_,0.0f,0.0f });

	player_->Initialize(&ctx_->dxCommon);
	player_->UseGamePad(false);

	bossTy_ = 50.0f;
	boss_->UseFire(false);
	boss_->Initialize(&ctx_->dxCommon);

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon);

	pauseBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause1.png", { 1280.0f,720.0f });
	pauseBG_->SetPosition({ 640.0f,360.0f });

	pauseUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause2.png", { 1280.0f,720.0f });
	pauseUI_->SetPosition({ 640.0f,360.0f });

	ground_->Initialize(&ctx_->dxCommon, "resources/object3d/ground.obj");
	ground_->SetTranslate({ 0.0f,-20.0f,200.0f });
	ground_->SetScale({ 5.0f,1.0f,5.0f });
	ground_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	skydome_->Initialize(&ctx_->dxCommon, "resources/object3d/skydome.obj");
	skydome_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	InitBuilding();

	isStart = false;
	startTimer_ = 0.0f;

	isPause_ = false;
	pause_ = kBack;

	state_ = kStart;
}

void GameScene::Update() {
	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	UpdatePause();

	switch (state_) {
	case GameScene::kStart:
		if (!isPause_) {
			if (!isStart) {
				startTimer_ += deltaTime_;

				cameraRx_ += deltaTime_ * cameraRxSpeed_;
				cameraRx_ = std::clamp(cameraRx_, -0.5f, 0.0f);
				camera_->SetRotate({ cameraRx_,0.0f,0.0f });

				bossTy_ -= deltaTime_ * bossTySpeed_;
				boss_->SetBodyTlansrate({ 0.0f,bossTy_ ,50.0f });
				if (startTimer_ >= startTime_) {
					startTimer_ = 0.0f;
					isStart = true;
					boss_->UseFire(true);
					boss_->SetIsStart(true);
					player_->UseGamePad(true);
					state_ = kPlay;
				}
			}
			player_->Update(useCamera_);
			boss_->Update(useCamera_);
		}
		break;
	case GameScene::kPlay:

		if (!isPause_) {
			player_->Update(useCamera_);
			boss_->SetPlayerPos(player_->GetWorldPosition());
			boss_->Update(useCamera_);
		}

		break;
	}

	UpdateBuilding();
	builA_->Update(*useCamera_);
	builB_->Update(*useCamera_);
	builC_->Update(*useCamera_);
	builD_->Update(*useCamera_);
	builE_->Update(*useCamera_);
	builF_->Update(*useCamera_);
	builG_->Update(*useCamera_);
	builH_->Update(*useCamera_);
	builI_->Update(*useCamera_);
	builJ_->Update(*useCamera_);
	builK_->Update(*useCamera_);
	builL_->Update(*useCamera_);

	ground_->Update(*useCamera_);
	skydome_->Update(*useCamera_);
	UpdateFade();
}

void GameScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	ground_->Draw();
	skydome_->Draw();

	builA_->Draw();
	builB_->Draw();
	builC_->Draw();
	builD_->Draw();
	builE_->Draw();
	builF_->Draw();
	builG_->Draw();
	builH_->Draw();
	builI_->Draw();
	builJ_->Draw();
	builK_->Draw();
	builL_->Draw();

	player_->Draw();
	boss_->Draw();

	if (isPause_) {
		pauseBG_->Draw();
		pauseUI_->Draw();
	}

	fade_->Draw();

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
	camera_->DrawImgui();

	boss_->DrawImGui();
	player_->DrawImGui();

	//ground_->DrawImGui("ground");
	skydome_->DrawImGui("skydome");
	
	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void GameScene::UpdateFade() {
	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		IScene::sceneNo = TITLE;
	}

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
				fade_->SetIsFade(true);
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

	pauseBG_->Update();
	pauseUI_->Update();
}

void GameScene::InitBuilding() {
	builA_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-a.obj");
	builA_->SetSRT({ 20.0f,20.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -40.0f,-20.0f,25.0f });
	builB_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-b.obj");
	builB_->SetSRT({ 22.0f,25.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -40.0f,-20.0f,73.0f });
	builC_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-c.obj");
	builC_->SetSRT({ 22.0f,40.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -44.0f,-20.0f,120.0f });
	builD_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-d.obj");
	builD_->SetSRT({ 25.0f,23.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -42.0f,-20.0f,150.0f });
	builE_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-e.obj");
	builE_->SetSRT({ 20.0f,35.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -40.0f,-20.0f,180.0f });
	builF_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-f.obj");
	builF_->SetSRT({ 25.0f,30.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -40.0f,-20.0f,210.0f });
	builG_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-g.obj");
	builG_->SetSRT({ 22.0f,30.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -44.0f,-20.0f,265.0f });
	builH_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-h.obj");
	builH_->SetSRT({ 30.0f,35.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -42.0f,-20.0f,290.0f });
	builI_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-i.obj");
	builI_->SetSRT({ 45.0f,40.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -40.0f,-20.0f,335.0f });
	builJ_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-j.obj");
	builJ_->SetSRT({ 35.0f,25.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -40.0f,-20.0f,382.0f });
	builK_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-k.obj");
	builK_->SetSRT({ 33.0f,40.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -42.0f,-20.0f,440.0f });
	builL_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-l.obj");
	builL_->SetSRT({ 32.0f,35.0f ,20.0f }, { 0.0f,-1.6f,0.0f }, { -42.0f,-20.0f,485.0f });
}

void GameScene::UpdateBuilding() {
	Vector3 translateA = builA_->GetTranslate();
	translateA.z -= builSpeed_ * deltaTime_;
	if (translateA.z <= backLine_) {
		translateA.z = 500.0f;
	}

	builA_->SetTranslate(translateA);
	Vector3 translateB = builB_->GetTranslate();
	translateB.z -= builSpeed_ * deltaTime_;
	if (translateB.z <= backLine_) {
		translateB.z = 500.0f;
	}
	builB_->SetTranslate(translateB);

	Vector3 translateC = builC_->GetTranslate();
	translateC.z -= builSpeed_ * deltaTime_;
	if (translateC.z <= backLine_) {
		translateC.z = 500.0f;
	}
	builC_->SetTranslate(translateC);

	Vector3 translateD = builD_->GetTranslate();
	translateD.z -= builSpeed_ * deltaTime_;
	if (translateD.z <= backLine_) {
		translateD.z = 500.0f;
	}
	builD_->SetTranslate(translateD);

	Vector3 translateE = builE_->GetTranslate();
	translateE.z -= builSpeed_ * deltaTime_;
	if (translateE.z <= backLine_) {
		translateE.z = 500.0f;
	}

	builE_->SetTranslate(translateE);

	Vector3 translateF = builF_->GetTranslate();
	translateF.z -= builSpeed_ * deltaTime_;
	if (translateF.z <= backLine_) {
		translateF.z = 500.0f;
	}
	builF_->SetTranslate(translateF);

	Vector3 translateG = builG_->GetTranslate();
	translateG.z -= builSpeed_ * deltaTime_;
	if (translateG.z <= backLine_) {
		translateG.z = 500.0f;
	}
	builG_->SetTranslate(translateG);

	Vector3 translateH = builH_->GetTranslate();
	translateH.z -= builSpeed_ * deltaTime_;
	if (translateH.z <= backLine_) {
		translateH.z = 500.0f;
	}
	builH_->SetTranslate(translateH);

	Vector3 translateI = builI_->GetTranslate();
	translateI.z -= builSpeed_ * deltaTime_;
	if (translateI.z <= backLine_) {
		translateI.z = 500.0f;
	}
	builI_->SetTranslate(translateI);

	Vector3 translateJ = builJ_->GetTranslate();
	translateJ.z -= builSpeed_ * deltaTime_;
	if (translateJ.z <= backLine_) {
		translateJ.z = 500.0f;
	}
	builJ_->SetTranslate(translateJ);

	Vector3 translateK = builK_->GetTranslate();
	translateK.z -= builSpeed_ * deltaTime_;
	if (translateK.z <= backLine_) {
		translateK.z = 500.0f;
	}
	builK_->SetTranslate(translateK);

	Vector3 translateL = builL_->GetTranslate();
	translateL.z -= builSpeed_ * deltaTime_;
	if (translateL.z <= backLine_) {
		translateL.z = 500.0f;
	}
	builL_->SetTranslate(translateL);
};

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