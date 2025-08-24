#include "TitleScene.h"

void TitleScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TitleScene::Initialize() {
	debugCamera_->SetInput(&ctx_->input);
	camera_->SetPosition({ 4.0f,6.0f,1.6f });
	camera_->SetRotate({ 0.3f,-0.4f,0.0f });

	gamePad_ = &ctx_->gamePad;

	titleUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/title1.png", { 1280.0f,720.0f });
	titleUI_->SetPosition({ 640.0f,360.0f });

	buildingA_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-a.obj");
	buildingB_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-b.obj");
	buildingC_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-c.obj");
	buildingD_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-d.obj");
	buildingE_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-e.obj");
	buildingF_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-f.obj");
	buildingG_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-g.obj");
	buildingH_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-h.obj");
	buildingI_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-i.obj");
	buildingJ_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-j.obj");
	buildingK_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-k.obj");
	buildingL_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-l.obj");
	buildingA_->SetSRT({ 2.0f,2.0f ,2.0f }, { 0.0f,-1.56f,0.0f }, { -4.93f,0.0f,10.6f });
	buildingB_->SetSRT({ 4.0f,2.0f ,2.0f }, { 0.0f,-1.56f,0.0f }, { -4.88f,0.0f,17.06f });
	buildingC_->SetSRT({ 4.0f,3.0f ,4.0f }, { 0.0f,-1.56f,0.0f }, { -6.51f,0.0f,22.49f });
	buildingD_->SetSRT({ 6.0f,4.0f ,4.0f }, { 0.0f,-1.56f,0.0f }, { -5.91f,0.0f,26.99f });
	buildingE_->SetSRT({ 3.0f,3.0f ,3.0f }, { 0.0f,1.57f,0.0f }, { 2.14f,0.0f,8.57f });
	buildingF_->SetSRT({ 2.0f,2.0f ,2.0f }, { 0.0f,1.57f,0.0f }, { 1.66f,0.0f,14.01f });
	buildingG_->SetSRT({ 3.0f,3.0f ,3.0f }, { 0.0f,1.57f,0.0f }, { 2.82f,0.0f,17.6f });
	buildingH_->SetSRT({ 5.0f,6.0f ,3.0f }, { 0.0f,1.57f,0.0f }, { 2.25f,0.0f,26.2f });
	buildingI_->SetSRT({ 8.5f,10.0f ,3.0f }, { 0.0f,1.57f,0.0f }, { 2.5f,0.0f,35.0f });
	buildingJ_->SetSRT({ 40.0f,10.0f ,40.0f }, { 0.0f,1.57f,0.0f }, { 20.0f,0.0f,40.0f });
	buildingK_->SetSRT({ 10.0f,7.5f ,3.0f }, { 0.0f,-1.56f,0.0f }, { -9.0f,0.0f,18.0f });
	buildingL_->SetSRT({ 6.0f,6.0f ,5.0f }, { 0.0f,-1.56f,0.0f }, { -6.4f,0.0f,33.5f });

	load_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-crossing.obj");
	load2_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-slant-curve.obj");
	load_->SetSRT({ 20.0f,1.0f,5.0f }, { 0.0f,1.58f,0.0f }, { -1.2f,0.0f,17.5f });
	load2_->SetSRT({ 5.0f,7.0f,5.0f }, { 0.0f,1.58f,0.0f }, { -1.06f,-0.13f,32.53f });

	skydome_->Initialize(&ctx_->dxCommon, "resources/object3d/skydome.obj");
	skydome_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	player_->Initialize(&ctx_->dxCommon, "resources/object3d/player/player.obj");
	player_->SetSRT({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f }, { -1.25f,0.5f,12.0f });

	engineFire_->Initialize(&ctx_->dxCommon, "resources/image/particle/circle.png", 1);
	engineFire_->UseEmitter(true);

	engineFireEmitter_.count = 5;
	engineFireEmitter_.radius = 0.05f;
	engineFireEmitter_.spawnTime = 0.01f;
	engineFireEmitter_.isMove = true;

	engineFireRange_.minScale = { 0.1f,0.1f,0.0f };
	engineFireRange_.maxScale = { 0.5f,0.5f,0.0f };
	engineFireRange_.minVelocity = { 0.0f,0.0f,-0.3f };
	engineFireRange_.maxVelocity = { 0.0f,0.0f,0.0f };
	engineFireRange_.minColor = { 0.9f,0.0f,0.0f };
	engineFireRange_.maxColor = { 1.0f,0.5f,0.0f };
	engineFireRange_.minLifeTime = 0.1f;
	engineFireRange_.maxLifeTime = 0.2f;

	engineFire_->SetEmitterValue(engineFireEmitter_);
	engineFire_->SetEmitterRange(engineFireRange_);

	fade_->Initialize(&ctx_->dxCommon);
	fade_->SetFadeInSpeed(0.25f);
	fade_->SetFadeOutSpeed(1.0f);

	ranking_->Initialize(&ctx_->dxCommon);

	pModelMove_ = false;
	isStart_ = false;
	startTimer_ = 0.0f;

	state_ = kPlay;
}
// { 0.3f,-0.4f,0.0f }
// { 4.0f,6.0f,1.6f }
void TitleScene::Update() {

	if (isStart_ && !pModelMove_) {
		Vector3 cR = camera_->GetRotate();
		cR.x -= 0.15f * deltaTime_;
		cR.y += 0.4f * deltaTime_;
		cR.x = std::clamp(cR.x, 0.15f, 0.3f);
		cR.y = std::clamp(cR.y, -0.4f, 0.0f);
		camera_->SetRotate(cR);

		Vector3 cT = camera_->GetTranslate();
		cT.x -= 5.25f * deltaTime_;
		cT.y -= 4.0f * deltaTime_;
		cT.z += 3.4f * deltaTime_;
		cT.x = std::clamp(cT.x, -1.25f, 4.0f);
		cT.y = std::clamp(cT.y, 2.0f, 6.0f);
		cT.z = std::clamp(cT.x, 5.0f, 5.0f);
		camera_->SetPosition(cT);

		startTimer_ += deltaTime_;
		if (startTimer_ >= 1.0f){
			startTimer_ = 0.0f;
			pModelMove_ = true;
		}
	}

	if (pModelMove_) {

		Vector3 cR = camera_->GetRotate();
		cR.x -= 0.1f * deltaTime_;
		camera_->SetRotate(cR);

		Vector3 pT = player_->GetTranslate();
		pT.y += 1.0f * deltaTime_;
		pT.z += 7.5f * deltaTime_;
		player_->SetTranslate(pT);

		Vector3 pR = player_->GetRotate();
		pR.x -= 0.1f * deltaTime_;
		player_->SetRotate(pR);
	}

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	SceneController();

	titleUI_->Update();

	buildingA_->Update(*useCamera_);
	buildingB_->Update(*useCamera_);
	buildingC_->Update(*useCamera_);
	buildingD_->Update(*useCamera_);
	buildingE_->Update(*useCamera_);
	buildingF_->Update(*useCamera_);
	buildingG_->Update(*useCamera_);
	buildingH_->Update(*useCamera_);
	buildingI_->Update(*useCamera_);
	buildingJ_->Update(*useCamera_);
	buildingK_->Update(*useCamera_);
	buildingL_->Update(*useCamera_);

	load_->Update(*useCamera_);
    load2_->Update(*useCamera_);

	skydome_->Update(*useCamera_);

	engineFire_->SetEmitterPosition(player_->GetTranslate());
	engineFire_->Update(*useCamera_);

	player_->Update(*useCamera_);

	ranking_->UpdateRanking();

	UpdateFade();
}

void TitleScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	skydome_->Draw();

	buildingA_->Draw();
	buildingB_->Draw();
	buildingC_->Draw();
	buildingD_->Draw();
	buildingE_->Draw();
	buildingF_->Draw();
	buildingG_->Draw();
	buildingH_->Draw();
	buildingI_->Draw();
	buildingJ_->Draw();
	buildingK_->Draw();
	buildingL_->Draw();

	load_->Draw();
	load2_->Draw();

	player_->Draw();
	engineFire_->Draw();

	titleUI_->Draw();

	ranking_->DrawRanking();

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

	//debugCamera_->DrawImgui();
	//ranking_->DrawImGui();
	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TitleScene::UpdateFade() {

	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		if (state_ == kPlay) {
			IScene::sceneNo = GAME;
		} else if (state_ == kTutorial) {
			IScene::sceneNo = TUTORIAL;
		}
	}

	fade_->Update();
}

void TitleScene::SceneController() {
	if (!fade_->GetIsFade() && fade_->GetFadeAlpha() <= 0.0f) {
		switch (state_)
		{
		case TitleScene::kPlay:

			titleUI_->SetTexture("resources/image/UI/title1.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				state_ = kTutorial;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				fade_->SetIsFade(true);
				isStart_ = true;
			}

			break;
		case TitleScene::kTutorial:

			titleUI_->SetTexture("resources/image/UI/title2.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_DOWN) || gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				state_ = kQuit;
			}
			if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				state_ = kPlay;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				fade_->SetIsFade(true);
				isStart_ = true;
			}

			break;
		case TitleScene::kQuit:

			titleUI_->SetTexture("resources/image/UI/title3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_UP) || gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				state_ = kTutorial;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				IScene::sceneNo = -1;
			}

			break;
		}
	}
}

void TitleScene::CameraController() {
	if (ctx_->input.TriggerKey(DIK_SPACE)) {
		if (isDebugCamera_) {
			isDebugCamera_ = false;
		}
		else {
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