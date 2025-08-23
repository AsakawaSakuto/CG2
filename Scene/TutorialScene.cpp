#include"TutorialScene.h"

void TutorialScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void TutorialScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	player_->Initialize(&ctx_->dxCommon);
	player_->UseGamePad(true);

	skyBox_->Initialize(&ctx_->dxCommon);

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon);

	pauseBG_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause1.png", { 1280.0f,720.0f });
	pauseBG_->SetPosition({ 640.0f,360.0f });

	pauseUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/pause2.png", { 1280.0f,720.0f });
	pauseUI_->SetPosition({ 640.0f,360.0f });

	testUI_->Initialize(&ctx_->dxCommon, "resources/image/UI/tutoUI1.png", { 1280.0f,128.0f });
	testUI_->SetPosition({ 640.0f,-128.0f });

	enemy_->Initialize(&ctx_->dxCommon, "resources/object3d/Enemy/enemy.obj");
	enemy_->SetTranslate({ 0.0f,5.0f,50.0f });
	enemyBullet_->Initialize(&ctx_->dxCommon, "resources/object3d/Enemy/enemyBullet.obj");
	enemyBullet_->SetTranslate({ 0.0f,0.0f,-50.0f });

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

	lX_ = 0.0f;
	lY_ = 0.0f;
	rX_ = 0.0f;
	rY_ = 0.0f;
	timer_ = 0.0f;
	testUIPos_ = { 0.0f,0.0f };
	testUIClear_ = false;
	isUpDownMove_ = false;
	isAttack_ = false;
	isAlive_ = false;
	isPause_ = false;
	isAttackTimer_ = 0.0f;

	testState_ = Test1;
	pause_ = kBack;
}

void TutorialScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	UpdatePause();

	UpdateCollision();

	if (!isPause_) {
		UpdateEnemy();
		player_->Update(useCamera_);
		skyBox_->Update(useCamera_);
		enemy_->Update(*useCamera_);
		enemyBullet_->Update(*useCamera_);
		exprotion_->Update(*useCamera_);
		UpdateTutorialTest();
	}

	UpdateFade();
}

void TutorialScene::Draw() {

	if (!ctx_) {
		OutputDebugStringA("ctx_ is nullptr\n");
		return;
	}

	if (!ctx_->dxCommon.GetCommandList()) {
		OutputDebugStringA("commandList_ is nullptr\n");
		return;
	}

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	skyBox_->Draw();
	player_->Draw();

	enemy_->Draw();
	
	exprotion_->Draw();

	if (isAlive_) {
		enemyBullet_->Draw();
	}

	if (isPause_) {
		pauseBG_->Draw();
		pauseUI_->Draw();
	} else {
		testUI_->Draw();
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

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void TutorialScene::UpdateFade() {
	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		IScene::sceneNo = TITLE;
	}
	
	fade_->Update();
}

void TutorialScene::UpdatePause() {
	if (isPause_) {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = false;
		}

		switch (pause_)
		{
		case TutorialScene::kBack:
			pauseUI_->SetTexture("resources/image/UI/pause2.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_RIGHT)) {
				pause_ = kQuit;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				isPause_ = false;
			}
			break;
		case TutorialScene::kQuit:
			pauseUI_->SetTexture("resources/image/UI/pause3.png");

			if (gamePad_->TriggerButton(GamePad::DPAD_LEFT)) {
				pause_ = kBack;
			}

			if (gamePad_->TriggerButton(GamePad::A)) {
				fade_->SetIsFade(true);
			}
			break;
		}

	} else {
		if (gamePad_->TriggerButton(GamePad::START)) {
			isPause_ = true;
		}
		pause_ = kBack;
	}

	pauseBG_->Update();
	pauseUI_->Update();
}

void TutorialScene::UpdateTutorialTest() {
	switch (testState_) {
	case Test1:
		lX_ = gamePad_->LeftStickX();
		lY_ = gamePad_->LeftStickY();

		if (testUIPos_.y >= 128.0f) {
			if (lX_ != 0.0f || lY_ != 0.0f) {
				testUIClear_ = true;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test2;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI2.png");
			}
		} else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test2:
		rX_ = gamePad_->RightStickX();
		rY_ = gamePad_->RightStickY();

		if (testUIPos_.y >= 128.0f) {
			if (rX_ != 0.0f || rY_ != 0.0f) {
				testUIClear_ = true;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test3;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI3.png");
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test3:
		if (testUIPos_.y >= 128.0f) {
			if (gamePad_->PushButton(GamePad::R)) {
				timer_ += deltaTime_;
				if (timer_ >= 0.25f) {
					testUIClear_ = true;
					timer_ = 0.0f;
				}
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test4;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI4.png");
				timer_ = 0.0f;
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test4:
		if (testUIPos_.y >= 128.0f) {
			if (gamePad_->PushButton(GamePad::L)) {
				timer_ += deltaTime_;
			}

			if (timer_ >= 1.5f && gamePad_->ReleaseButton(GamePad::L)) {
				testUIClear_ = true;
				timer_ = 0.0f;
			} else if (timer_ < 1.5f && gamePad_->ReleaseButton(GamePad::L)) {
				timer_ = 0.0f;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test5;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI5.png");
				timer_ = 0.0f;
			}
		}
		else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test5:
		if (testUIPos_.y >= 128.0f) {
			if (gamePad_->RightTrigger()>=1.0f|| gamePad_->LeftTrigger() >= 1.0f) {
				testUIClear_ = true;
			}
		}

		if (testUIClear_) {
			testUI_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y -= testUISpeed_ * deltaTime_;
			if (testUIPos_.y <= -128.0f) {
				testState_ = Test6;
				testUIClear_ = false;
				testUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				testUI_->SetTexture("resources/image/UI/tutoUI6.png");
			}
		} else {
			testUIPos_ = testUI_->GetPosition();
			testUIPos_.y += testUISpeed_ * deltaTime_;
		}
		break;
	case TutorialScene::Test6:
		testUIPos_ = testUI_->GetPosition();
		testUIPos_.y += testUISpeed_ * deltaTime_;
		if (testUIPos_.y >= 128.0f) {
			fade_->SetIsFade(true);
		}
		break;
	}
	testUIPos_.y = std::clamp(testUIPos_.y, -128.0f, 128.0f);
	testUI_->SetPosition(testUIPos_);
	testUI_->Update();
}

void TutorialScene::UpdateEnemy() {

	// move
	Vector3 pos = enemy_->GetTranslate();

	if (isUpDownMove_) {
		pos.y += upDownSpeed_ * deltaTime_;
		if (pos.y >= 7.0f) {
			isUpDownMove_ = false;
		}
	} else {
		pos.y -= upDownSpeed_ * deltaTime_;
		if (pos.y <= 3.0f) {
			isUpDownMove_ = true;
		}
	}

	enemy_->SetTranslate(pos);

	// rotate
	Vector3 rotate = enemy_->GetRotate();

	if (isAttack_) {
		isAttackTimer_ += deltaTime_;
		rotate.z += zRotateSpeed_ * deltaTime_ * 4.0f;
		if (isAttackTimer_ >= 1.5f) {
			isAttackTimer_ = 0.0f;
			isAttack_ = false;
			isAlive_ = true;
			enemyBullet_->SetTranslate(enemy_->GetTranslate());
			bulletVelocity_ = player_->GetWorldPosition() - enemy_->GetWorldPosition();
			bulletVelocity_ = bulletVelocity_.Normalize();
		}
	} else {
		isAttackTimer_ += deltaTime_;
		rotate.z += zRotateSpeed_ * deltaTime_;
		if (isAttackTimer_ >= 5.0f) {
			isAttackTimer_ = 0.0f;
			isAttack_ = true;
		}
	}

	enemy_->SetRotate(rotate);

	// bulletMove
	Vector3 bPos = enemyBullet_->GetTranslate();
	bPos += bulletVelocity_ * bulletSpeed_ * deltaTime_;
	if (bPos.z <= -50.0f) {
		isAlive_ = false;
	}
	enemyBullet_->SetTranslate(bPos);

	// bulletR
	Vector3 bRotate = {};
	rotate.y = std::atan2(bulletVelocity_.x, bulletVelocity_.z);
	float horizontalLength = std::sqrt(bulletVelocity_.x * bulletVelocity_.x + bulletVelocity_.z * bulletVelocity_.z);
	rotate.x = std::atan2(-bulletVelocity_.y, horizontalLength);
	enemyBullet_->SetRotate(bRotate);
}

void TutorialScene::UpdateCollision() {
	if (player_->GetState() == 0 && isAlive_) {
		if (IsCollideSphere(
			player_->GetWorldPosition(), 0.75f,
			enemyBullet_->GetWorldPosition(), 0.5f)) {
			isAlive_ = false;
			player_->Damage();
		}
	}

	exprotion_->SetEmitterPosition({ 0.0f,0.0f,-100.0f });
	enemy_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	const Vector3 enemyPos = enemy_->GetWorldPosition();
	const float enemyRadius = 1.5f;

	std::vector<PlayerBullet*> bulletPtrs = player_->GetAllBullets();

	for (PlayerBullet* bullet : bulletPtrs) {
		if (!bullet->GetIsAlive()) continue;

		Vector3 bulletPos = bullet->GetWorldPosition();
		float bulletRadius = 0.5f;

		if (bullet->GetIsAlive()) {
			if (IsCollideSphere(bulletPos, bulletRadius, enemyPos, enemyRadius)) {
				exprotion_->SetEmitterPosition(bullet->GetWorldPosition());
				bullet->SetIsAlive(false);
				enemy_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			}
		}
	}

	if (player_->BeamIsAlive()) {
		if (IsCollideSphere(player_->BeamWorldPosition(), 1.5f, enemyPos, enemyRadius)) {
			player_->BeamHit();
			enemy_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		}
	}
}

void TutorialScene::CameraController() {
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