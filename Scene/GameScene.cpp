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
	ranking_->Initialize(&ctx_->dxCommon);

	pauseBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause1.png", { 1280.0f,720.0f });
	pauseBG_->SetPosition({ 640.0f,360.0f });

	pauseUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause2.png", { 1280.0f,720.0f });
	pauseUI_->SetPosition({ 640.0f,360.0f });

	skydome_->Initialize(&ctx_->dxCommon, "resources/object3d/skydome.obj");
	skydome_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	keepOut1_->Initialize(&ctx_->dxCommon, "resources/image/UI/keepOut1.png", { 1280.0f,360.0f });
	keepOut2_->Initialize(&ctx_->dxCommon, "resources/image/UI/keepOut2.png", { 1280.0f,360.0f });
	keepOut1_->SetPosition({ 640.0f,180.0f });
	keepOut2_->SetPosition({ 640.0f,541.0f });

	InitLoad();

	InitBuilding();

	InitParticle();

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

			UpdateBuilding();
			UpdateLoad();

            #pragma region ObjectUpdate

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

			builM_->Update(*useCamera_);
			builN_->Update(*useCamera_);
			builO_->Update(*useCamera_);
			builP_->Update(*useCamera_);
			builQ_->Update(*useCamera_);
			builR_->Update(*useCamera_);
			builS_->Update(*useCamera_);
			builT_->Update(*useCamera_);
			builU_->Update(*useCamera_);
			builV_->Update(*useCamera_);
			builW_->Update(*useCamera_);
			builX_->Update(*useCamera_);

			loadA_->Update(*useCamera_);
			loadB_->Update(*useCamera_);
			loadC_->Update(*useCamera_);
			loadD_->Update(*useCamera_);
			loadE_->Update(*useCamera_);
			loadF_->Update(*useCamera_);
			loadEnd_->Update(*useCamera_);

#pragma endregion

			Vector2 uvT1 = keepOut1_->GetUvTranslate_();
			uvT1.x += keepOutSpeed_ * deltaTime_;
			keepOut1_->SetUvTranslate(uvT1);

			Vector4 cA1 = keepOut1_->GetColor();
			cA1.w -= keepOutDeltaTime_;
			keepOut1_->SetColor(cA1);

			Vector2 uvT2 = keepOut2_->GetUvTranslate_();
			uvT2.x -= keepOutSpeed_ * deltaTime_;
			keepOut2_->SetUvTranslate(uvT2);

			Vector4 cA2 = keepOut2_->GetColor();
			cA2.w -= keepOutDeltaTime_;
			keepOut2_->SetColor(cA2);

			keepOut1_->Update();
			keepOut2_->Update();

			skydome_->Update(*useCamera_);
		}
		break;
	case GameScene::kPlay:

		if (!isPause_) {
			UpdateCollision();

			player_->Update(useCamera_);
			boss_->SetPlayerPos(player_->GetWorldPosition());
			boss_->Update(useCamera_);
			exprotion_->Update(*useCamera_);

			UpdateBuilding();
			UpdateLoad();

            #pragma region ObjectUpdate

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

			builM_->Update(*useCamera_);
			builN_->Update(*useCamera_);
			builO_->Update(*useCamera_);
			builP_->Update(*useCamera_);
			builQ_->Update(*useCamera_);
			builR_->Update(*useCamera_);
			builS_->Update(*useCamera_);
			builT_->Update(*useCamera_);
			builU_->Update(*useCamera_);
			builV_->Update(*useCamera_);
			builW_->Update(*useCamera_);
			builX_->Update(*useCamera_);

			loadA_->Update(*useCamera_);
			loadB_->Update(*useCamera_);
			loadC_->Update(*useCamera_);
			loadD_->Update(*useCamera_);
			loadE_->Update(*useCamera_);
			loadF_->Update(*useCamera_);
			loadEnd_->Update(*useCamera_);

#pragma endregion

			skydome_->Update(*useCamera_);

			ranking_->Update();

			if (boss_->IsDie()) {
				ranking_->IsEndGame();
				state_ = kEnd;
			}
		}

		break; 
	case GameScene::kEnd:

		if (!isPause_) {

			player_->Update(useCamera_);
			exprotion_->Update(*useCamera_);

			UpdateBuilding();
			UpdateLoad();

            #pragma region ObjectUpdate

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

			builM_->Update(*useCamera_);
			builN_->Update(*useCamera_);
			builO_->Update(*useCamera_);
			builP_->Update(*useCamera_);
			builQ_->Update(*useCamera_);
			builR_->Update(*useCamera_);
			builS_->Update(*useCamera_);
			builT_->Update(*useCamera_);
			builU_->Update(*useCamera_);
			builV_->Update(*useCamera_);
			builW_->Update(*useCamera_);
			builX_->Update(*useCamera_);

			loadA_->Update(*useCamera_);
			loadB_->Update(*useCamera_);
			loadC_->Update(*useCamera_);
			loadD_->Update(*useCamera_);
			loadE_->Update(*useCamera_);
			loadF_->Update(*useCamera_);
			loadEnd_->Update(*useCamera_);

#pragma endregion

			skydome_->Update(*useCamera_);
			exprotion_->Update(*useCamera_);

			endParticleScale_ += deltaTime_ * 1.0f;
			endParticleVelocity_ += deltaTime_ * 1.0f;
			endParticleRange_.minScale = { endParticleScale_,endParticleScale_,0.0f };
			endParticleRange_.maxScale = { endParticleScale_,endParticleScale_,0.0f };
			endParticleRange_.minVelocity = { -endParticleVelocity_,-endParticleVelocity_,-endParticleVelocity_ };
			endParticleRange_.maxVelocity = { endParticleVelocity_,endParticleVelocity_,endParticleVelocity_ };
			endParticle_->SetEmitterRange(endParticleRange_);

			endParticle_->SetEmitterPosition(boss_->GetBodyWorldPos());
			endParticle_->Update(*useCamera_);

			endTimer_ += deltaTime_;
			if (endTimer_ >= 3.0f) {
				endTimer_ = 0.0f;
				fade_->SetIsFade(true);
			}
		}

		break;
	}
	UpdateFade();
}

void GameScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	skydome_->Draw();

	DrawObject();

	boss_->Draw();
	player_->Draw();

	exprotion_->Draw();

	if (state_ == kStart) {
		keepOut1_->Draw();
		keepOut2_->Draw();
	} else {
		ranking_->Draw();
	}

	if (state_ == kEnd) {
		endParticle_->Draw();
	}

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

	//DrawFPS_ImGui();

	/*debugCamera_->DrawImgui();
	camera_->DrawImgui();*/

	boss_->DrawImGui();
	player_->DrawImGui();
	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void GameScene::UpdateCollision() {
	// プレイヤーがダメージを受ける側
	if (player_->GetState() == 0) {
		if (IsCollideSphere(
			player_->GetWorldPosition(), 0.75f,
			boss_->GetBossBulletPos(), 0.5f)) {
			if (boss_->GetBulletState() == DAMAGE) {
				player_->Damage();
			} else if (boss_->GetBulletState() == HEAL) {
				player_->Heal();
			}
			boss_->BulletHit();
		}

		if (IsCollideSphere(
			player_->GetWorldPosition(), 0.75f,
			boss_->GetArmPosL(), 1.0f)) {
			player_->Damage();
		}

		if (IsCollideSphere(
			player_->GetWorldPosition(), 0.75f,
			boss_->GetArmPosR(), 1.0f)) {
			player_->Damage();
		}
	}

	// 敵がダメージを受ける側
	exprotion_->SetEmitterPosition({ 0.0f,0.0f,-100.0f });
	boss_->SetBodyColor({ 1.0f,1.0f,1.0f,1.0f });

	std::vector<PlayerBullet*> bulletPtrs = player_->GetAllBullets();

	for (PlayerBullet* bullet : bulletPtrs) {
		if (!bullet->GetIsAlive()) continue;

		Vector3 bulletPos = bullet->GetWorldPosition();

		if (bullet->GetIsAlive()) {
			if (IsCollideSphere(bulletPos, 1.0f, boss_->GetBodyWorldPos(), 2.5f)) {
				exprotion_->SetEmitterPosition(bullet->GetWorldPosition());
				bullet->SetIsAlive(false);
				boss_->SetBodyColor({ 0.0f,0.0f,0.0f,1.0f });
				boss_->Damage(3.0f);
			}
		}
	}

	if (player_->BeamIsAlive()) {
		if (IsCollideSphere(player_->BeamWorldPosition(), 1.5f, boss_->GetBodyWorldPos(), 2.5f)) {
			player_->BeamHit();
			boss_->Damage(10.0f);
		}
	}
}

void GameScene::UpdateFade() {
	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		IScene::sceneNo = TITLE;
	}

	fade_->Update();
}

void GameScene::UpdatePause() {
	if (isPause_ && !fade_->GetIsFade()) {
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

	} else if(!isPause_ && !fade_->GetIsFade()) {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = true;
		}
		pause_ = kBack;
	}

	pauseBG_->Update();
	pauseUI_->Update();
}

void GameScene::InitParticle() {
	exprotion_->Initialize(&ctx_->dxCommon, "resources/image/particle/circle.png", 1);
	exprotion_->UseEmitter(true);

	exprotionEmitter_.isMove = true;
	exprotionEmitter_.count = 50;
	exprotionEmitter_.spawnTime = 0.01f;
	exprotionEmitter_.radius = 0.1f;

	exprotionRange_.minScale = { 1.0f,1.0f,0.0f };
	exprotionRange_.maxScale = { 2.5f,2.5f,0.0f };
	exprotionRange_.minVelocity = { -0.25f,-0.25f,-0.25f };
	exprotionRange_.maxVelocity = { 0.25f,0.25f,0.25f };
	exprotionRange_.minColor = { 0.5f,0.1f,0.0f };
	exprotionRange_.maxColor = { 1.0f,0.3f,0.0f };
	exprotionRange_.minLifeTime = 0.15f;
	exprotionRange_.maxLifeTime = 0.3f;

	exprotion_->SetEmitterValue(exprotionEmitter_);
	exprotion_->SetEmitterRange(exprotionRange_);

	endParticle_->Initialize(&ctx_->dxCommon, "resources/image/particle/circle.png", 100);
	endParticle_->UseEmitter(true);

	endParticleEmitter_.isMove = true;
	endParticleEmitter_.count = 100;
	endParticleEmitter_.spawnTime = 0.01f;
	endParticleEmitter_.radius = 1.f;

	endParticleScale_ = 1.0f;
	endParticleVelocity_ = 0.1f;

	endParticleRange_.minScale = { endParticleScale_,endParticleScale_,0.0f };
	endParticleRange_.maxScale = { endParticleScale_,endParticleScale_,0.0f };
	endParticleRange_.minVelocity = { -endParticleVelocity_,-endParticleVelocity_,-endParticleVelocity_ };
	endParticleRange_.maxVelocity = { endParticleVelocity_,endParticleVelocity_,endParticleVelocity_ };
	endParticleRange_.minColor = { 0.9f,0.0f,0.0f };
	endParticleRange_.maxColor = { 1.0f,0.5f,0.0f };
	endParticleRange_.minLifeTime = 0.1f;
	endParticleRange_.maxLifeTime = 1.0f;

	endParticle_->SetEmitterValue(endParticleEmitter_);
	endParticle_->SetEmitterRange(endParticleRange_);

	endTimer_ = 0.0f;
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

	builM_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-a.obj");
	builM_->SetSRT({ 28.0f,20.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 40.0f,-20.0f,485.0f });
	builN_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-b.obj");
	builN_->SetSRT({ 22.0f,25.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 40.0f,-20.0f,428.0f });
	builO_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-c.obj");
	builO_->SetSRT({ 22.0f,40.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 44.0f,-20.0f,380.0f });
	builP_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-d.obj");
	builP_->SetSRT({ 25.0f,23.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 42.0f,-20.0f,349.0f });
	builQ_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-e.obj");
	builQ_->SetSRT({ 20.0f,35.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 40.0f,-20.0f,318.0f });
	builR_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-f.obj");
	builR_->SetSRT({ 25.0f,30.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 40.0f,-20.0f,288.0f });
	builS_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-g.obj");
	builS_->SetSRT({ 22.0f,30.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 44.0f,-20.0f,235.0f });
	builT_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-h.obj");
	builT_->SetSRT({ 30.0f,35.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 42.0f,-20.0f,212.0f });
	builU_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-i.obj");
	builU_->SetSRT({ 45.0f,40.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 40.0f,-20.0f,171.0f });
	builV_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-j.obj");
	builV_->SetSRT({ 35.0f,25.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 40.0f,-20.0f,125.0f });
	builW_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-k.obj");
	builW_->SetSRT({ 33.0f,40.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 42.0f,-20.0f,70.0f });
	builX_->Initialize(&ctx_->dxCommon, "resources/object3d/building/building-l.obj");
	builX_->SetSRT({ 32.0f,35.0f ,20.0f }, { 0.0f,1.6f,0.0f }, { 42.0f,-20.0f,25.0f });
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

	Vector3 translateM = builM_->GetTranslate();
	translateM.z -= builSpeed_ * deltaTime_;
	if (translateM.z <= backLine_) {
		translateM.z = 500.0f;
	}
	builM_->SetTranslate(translateM);

	Vector3 translateN = builN_->GetTranslate();
	translateN.z -= builSpeed_ * deltaTime_;
	if (translateN.z <= backLine_) {
		translateN.z = 500.0f;
	}
	builN_->SetTranslate(translateN);

	Vector3 translateO = builO_->GetTranslate();
	translateO.z -= builSpeed_ * deltaTime_;
	if (translateO.z <= backLine_) {
		translateO.z = 500.0f;
	}
	builO_->SetTranslate(translateO);

	Vector3 translateP = builP_->GetTranslate();
	translateP.z -= builSpeed_ * deltaTime_;
	if (translateP.z <= backLine_) {
		translateP.z = 500.0f;
	}
	builP_->SetTranslate(translateP);

	Vector3 translateQ = builQ_->GetTranslate();
	translateQ.z -= builSpeed_ * deltaTime_;
	if (translateQ.z <= backLine_) {
		translateQ.z = 500.0f;
	}
	builQ_->SetTranslate(translateQ);

	Vector3 translateR = builR_->GetTranslate();
	translateR.z -= builSpeed_ * deltaTime_;
	if (translateR.z <= backLine_) {
		translateR.z = 500.0f;
	}
	builR_->SetTranslate(translateR);

	Vector3 translateS = builS_->GetTranslate();
	translateS.z -= builSpeed_ * deltaTime_;
	if (translateS.z <= backLine_) {
		translateS.z = 500.0f;
	}
	builS_->SetTranslate(translateS);

	Vector3 translateT = builT_->GetTranslate();
	translateT.z -= builSpeed_ * deltaTime_;
	if (translateT.z <= backLine_) {
		translateT.z = 500.0f;
	}
	builT_->SetTranslate(translateT);

	Vector3 translateU = builU_->GetTranslate();
	translateU.z -= builSpeed_ * deltaTime_;
	if (translateU.z <= backLine_) {
		translateU.z = 500.0f;
	}
	builU_->SetTranslate(translateU);

	Vector3 translateV = builV_->GetTranslate();
	translateV.z -= builSpeed_ * deltaTime_;
	if (translateV.z <= backLine_) {
		translateV.z = 500.0f;
	}
	builV_->SetTranslate(translateV);

	Vector3 translateW = builW_->GetTranslate();
	translateW.z -= builSpeed_ * deltaTime_;
	if (translateW.z <= backLine_) {
		translateW.z = 500.0f;
	}
	builW_->SetTranslate(translateW);

	Vector3 translateX = builX_->GetTranslate();
	translateX.z -= builSpeed_ * deltaTime_;
	if (translateX.z <= backLine_) {
		translateX.z = 500.0f;
	}
	builX_->SetTranslate(translateX);
};

void GameScene::InitLoad() {
	loadA_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-crossing.obj");
	loadB_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-driveway-double.obj");
	loadC_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-crossing.obj");
	loadD_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-driveway-double.obj");
	loadE_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-crossing.obj");
	loadF_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-driveway-double.obj");
	loadA_->SetSRT({ 101.0f,30.0f,70.0f }, { 0.0f,1.57f,0.0f }, { 0.0f,-20.0f,100.0f });
	loadB_->SetSRT({ 101.0f,30.0f,70.0f }, { 0.0f,1.57f,0.0f }, { 0.0f,-20.0f,200.0f });
	loadC_->SetSRT({ 101.0f,30.0f,70.0f }, { 0.0f,1.57f,0.0f }, { 0.0f,-20.0f,300.0f });
	loadD_->SetSRT({ 101.0f,30.0f,70.0f }, { 0.0f,1.57f,0.0f }, { 0.0f,-20.0f,400.0f });
	loadE_->SetSRT({ 101.0f,30.0f,70.0f }, { 0.0f,1.57f,0.0f }, { 0.0f,-20.0f,500.0f });
	loadF_->SetSRT({ 101.0f,30.0f,70.0f }, { 0.0f,1.57f,0.0f }, { 0.0f,-20.0f,600.0f });

	loadEnd_->Initialize(&ctx_->dxCommon, "resources/object3d/load/road-end-barrier.obj");
	loadEnd_->SetSRT({ 100.0f,30.0f,60.0f }, { 0.0f,-1.57f,0.0f }, { 0.0f,-20.0f,440.0f });
}

void GameScene::UpdateLoad() {
	Vector3 translateA = loadA_->GetTranslate();
	translateA.z -= builSpeed_ * deltaTime_;
	if (translateA.z <= backLine_) {
		translateA.z = 600.0f;
	}
	loadA_->SetTranslate(translateA);

	Vector3 translateB = loadB_->GetTranslate();
	translateB.z -= builSpeed_ * deltaTime_;
	if (translateB.z <= backLine_) {
		translateB.z = 600.0f;
	}
	loadB_->SetTranslate(translateB);

	Vector3 translateC = loadC_->GetTranslate();
	translateC.z -= builSpeed_ * deltaTime_;
	if (translateC.z <= backLine_) {
		translateC.z = 600.0f;
	}
	loadC_->SetTranslate(translateC);

	Vector3 translateD = loadD_->GetTranslate();
	translateD.z -= builSpeed_ * deltaTime_;
	if (translateD.z <= backLine_) {
		translateD.z = 600.0f;
	}
	loadD_->SetTranslate(translateD);

	Vector3 translateE = loadE_->GetTranslate();
	translateE.z -= builSpeed_ * deltaTime_;
	if (translateE.z <= backLine_) {
		translateE.z = 600.0f;
	}
	loadE_->SetTranslate(translateE);

	Vector3 translateF = loadF_->GetTranslate();
	translateF.z -= builSpeed_ * deltaTime_;
	if (translateF.z <= backLine_) {
		translateF.z = 600.0f;
	}
	loadF_->SetTranslate(translateF);
}

void GameScene::DrawObject() {
	loadA_->Draw();
	loadB_->Draw();
	loadC_->Draw();
	loadD_->Draw();
	loadE_->Draw();
	loadF_->Draw();
	loadEnd_->Draw();

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

	builM_->Draw();
	builN_->Draw();
	builO_->Draw();
	builP_->Draw();
	builQ_->Draw();
	builR_->Draw();
	builS_->Draw();
	builT_->Draw();
	builU_->Draw();
	builV_->Draw();
	builW_->Draw();
	builX_->Draw();
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