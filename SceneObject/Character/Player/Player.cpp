#include"Player.h"

void Player::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/object3d/player/player.obj");

    menu_->Initialize(dxCommon_, "resources/image/UI/menu.png", { 64.0f,64.0f });
    menu_->SetPosition({ 48.0f,48.0f });

    lifeUI_->Initialize(dxCommon_, "resources/image/UI/life3.png", { 1280.0f,720.0f });
    lifeUI_->SetPosition({ 640.0f,360.0f });

    gaugeUI_->Initialize(dxCommon_, "resources/image/UI/gauge.png", { 1280.0f,720.0f });
    gaugeUI_->SetPosition({ 640.0f,360.0f });

    gaugePosX_ = 30.0f;
    gaugeScaleX_ = 1.0f;

    gauge_->Initialize(dxCommon_, "resources/image/0.png", { 1.0f,1.0f });
    gauge_->SetPosition({ gaugePosX_,550.0f });
    gauge_->SetScale({ gaugeScaleX_,22.0f });
    gauge_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

    reticle3D_->Initialize(dxCommon_, "resources/object3d/cube.obj");

    reticle2D_->Initialize(dxCommon_, "resources/image/UI/crosshair.png", {64.0f,64.0f});
    reticle2D_->SetPosition({ 640.0f,360.0f });

    engineFire_->Initialize(dxCommon_, "resources/image/particle/circle.png", 2);
    engineFire_->UseEmitter(true);

    heal_->Initialize(dxCommon_, "resources/image/particle/closs.png", 1);
    damage_->Initialize(dxCommon_, "resources/image/particle/star2.png", 1);

    beamCharge_->Initialize(dxCommon_, "resources/image/particle/box.png", 10);

    dieFire_->Initialize(dxCommon_, "resources/image/particle/circle.png", 100);
    dieFire_->UseEmitter(false);

    dieSmork_->Initialize(dxCommon_, "resources/image/particle/smork.png", 2);
    dieSmork_->UseEmitter(false);

    bullets_.clear();
    for (int i = 0; i < 32; ++i) {
        auto bullet = std::make_unique<PlayerBullet>();
        bullet->Initialize(dxCommon_);
        bullet->SetIsAlive(false);
        bullets_.push_back(std::move(bullet));
    }

    beam_->Initialize(dxCommon_);

    bulletSpawnTimer_ = 0.0f;
    beamChargeTimer_ = 0.0f;
    beamChargeRadius_ = 0.0f;
    dashCoolTimer_ = 0.0f;
    isCanDash = true;

    state_ = NORMAL;
    life_ = 3;
    moveRotate_ = { 0.0f,0.0f,0.0f };
    isBeamShot_ = false;
    isCanDash = true;
    isHeal_ = false;
    isDamage_ = false;
    isInvincible_ = false;
    invincibleTimer_ = 0.0f;

    healSE_->Initialize("resources/sound/SE/heal.mp3");
    damageSE_->Initialize("resources/sound/SE/pHit.wav");
    dashSE_->Initialize("resources/sound/SE/dash.mp3");
    bShotSE_->Initialize("resources/sound/SE/pShot.mp3");
    beamChargeSE_->Initialize("resources/sound/SE/beamCharge.mp3");
    beamShotSE_->Initialize("resources/sound/SE/beamShot.mp3");

    isScreenOut_ = false;
    isScreenIn_ = false;
}

void Player::Update(Camera* camera) {

    if (isInvincible_) {
        invincibleTimer_ += deltaTime_;
        if (invincibleTimer_ >= invincibleTime_) {
            isInvincible_ = false;
            invincibleTimer_ = 0.0f;
        }
    }

	Move();

    Attack();

    for (auto& bullet : bullets_) {
        if (bullet->GetIsAlive()) {
            bullet->Update(camera);
        }
    }

    Action();

    UpdateReticle(camera);

    UpdateParticle();

    UpdateLife();

	model_->Update(*camera);
    beam_->Update(camera);

    engineFire_->Update(*camera);
    heal_->Update(*camera);
    damage_->Update(*camera);

    beamCharge_->SetEmitterPosition(model_->GetTranslate());
    beamCharge_->Update(*camera);

    reticle3D_->Update(*camera);
    reticle2D_->Update();

    gaugePosX_ = std::clamp(gaugePosX_, 30.0f, 83.0f);
    gaugeScaleX_ = std::clamp(gaugeScaleX_, 1.0f, 95.0f);
    gauge_->SetPosition({ gaugePosX_,550.0f });
    gauge_->SetScale({ gaugeScaleX_,22.0f });

    gauge_->Update();
    gaugeUI_->Update();

    if (gamePad_->IsConnected()) {
        menu_->SetTexture("resources/image/UI/menu.png");
    } else {
        menu_->SetTexture("resources/image/UI/keyboard_tab.png");
    }

    menu_->Update();

    healSE_->Update();
    dashSE_->Update();
    damageSE_->Update();
    bShotSE_->Update();
    beamChargeSE_->Update();
    beamShotSE_->Update();
}

void Player::DieUpdate(Camera* camera) {

    Vector3 mT = model_->GetTranslate();
    Vector3 mR = model_->GetRotate();
    mR.x += dieRySpeed_ * deltaTime_;

    if (mR.x >= 1.5f) {
        mR.x = 1.5f;
        dieFire_->UseEmitter(true);
        dieSmork_->UseEmitter(false);
    } else {
        dieSmork_->UseEmitter(true);
        mT.y -= dieTySpeed_ * deltaTime_;
        mT.z += dieTzSpeed_ * deltaTime_;
    }

    model_->SetRotate(mR);
    model_->SetTranslate(mT);

    model_->Update(*camera);

    UpdateParticle();
    dieFire_->SetEmitterPosition(model_->GetTranslate());
    dieFire_->Update(*camera);

    dieSmork_->SetEmitterPosition(model_->GetTranslate());
    dieSmork_->Update(*camera);

    beam_->Update(camera);

    engineFire_->Update(*camera);
    heal_->Update(*camera);
    damage_->Update(*camera);

    beamCharge_->SetEmitterPosition(model_->GetTranslate());
    beamCharge_->Update(*camera);

    reticle3D_->Update(*camera);
    reticle2D_->Update();

    gaugePosX_ = std::clamp(gaugePosX_, 30.0f, 83.0f);
    gaugeScaleX_ = std::clamp(gaugeScaleX_, 1.0f, 95.0f);
    gauge_->SetPosition({ gaugePosX_,550.0f });
    gauge_->SetScale({ gaugeScaleX_,22.0f });

    gauge_->Update();
    gaugeUI_->Update();
    menu_->Update();
}

void Player::Draw() {
	model_->Draw();
    
    if (IsDie()) {
        dieFire_->Draw();
        dieSmork_->Draw();
    } else {

        for (auto& bullet : bullets_) {
            if (bullet->GetIsAlive()) {
                bullet->Draw();
            }
        }
        beam_->Draw();

        engineFire_->Draw();
        heal_->Draw();
        damage_->Draw();
        beamCharge_->Draw();
    }

    lifeUI_->Draw();

    gauge_->Draw();
    gaugeUI_->Draw();
    menu_->Draw();

    reticle2D_->Draw();
    //reticle3D_->Draw();
}

void Player::DrawImGui() {
    //ImGui::DragFloat("Speed", &moveSpeed_, 0.1f);
    //ImGui::DragFloat("BullerSpeed", &bulletSpeed_, 1.0f);
    //ImGui::DragFloat("BulledSpawn", &bulletSpawnTime_, 0.01f);
    //ImGui::DragFloat("Distance", &kDistanceToReticle, 1.0f);
    //ImGui::DragFloat3("player", &engineFireOffset_.x, 0.01f);
    //model_->DrawImGui("player");
    //engineFire_->DrawImGui("engineFire");
    //heal_->DrawImGui("h");
    //damage_->DrawImGui("d");
    //beamCharge_->DrawImGui("bc");
    //beam_->DrawImGui();
    //gauge_->DrawImGui("g");
    ImGui::Begin("foolSpeed");

    ImGui::Text("m X:%f Y%f", input_->GetMousePos().x, input_->GetMousePos().y);
    if (isScreenOut_) {
        ImGui::Text("true");
    } else {
        ImGui::Text("false");
    }

    ImGui::End();

    dieFire_->DrawImGui("direFire");
}

void Player::UpdateReticle(Camera* camera) {
    Vector2 position;
    if (gamePad_->IsConnected()) {
        position = reticle2D_->GetPosition();

        // 左スティックの入力取得
        float lx = gamePad_->RightStickX(); // -1.0 ~ +1.0
        float ly = gamePad_->RightStickY(); // -1.0 ~ +1.0

        // 入力ベクトル
        Vector2 move(lx, ly);

        // 斜めで速くなりすぎないように長さをクランプ
        float length = sqrt(move.x * move.x + move.y * move.y);
        if (length > 1.0f)
        {
            move.x /= length;
            move.y /= length;
        }

        // 位置を更新（倒し量＝速度）
        position.x += move.x * reticleSpeed_ * deltaTime_;
        position.y -= move.y * reticleSpeed_ * deltaTime_;

        position.x = std::clamp(position.x, 0.0f, 1280.0f);
        position.y = std::clamp(position.y, 0.0f, 720.0f);

        reticle2D_->SetPosition(position);

        isScreenOut_ = false;
    } else {
        position = input_->GetMousePos();
        reticle2D_->SetPosition(position);

        // 画面サイズ
        const float w = static_cast<float>(WinApp::kClientWidth_);
        const float h = static_cast<float>(WinApp::kClientHeight_);

        // マウス座標
        const Vector2 mp = input_->GetMousePos();

        // まず「画面内か」を判定（ANDで内側全部を満たす）
        const bool isInside =
            (mp.x >= 0.0f) && (mp.x < w) &&
            (mp.y >= 0.0f) && (mp.y < h);

        // 画面外なら true、中なら false を明示代入
        if (!isInside) {
            isScreenOut_ = true;
        } else {
            isScreenOut_ = false;
        }
    }

    //-------------------------------------------------//

    // ビュー行列、プロジェクション行列、ビューポート行列（もしくは変換行列）を用意
    Matrix4x4 viewMatrix = camera->GetViewMatrix();
    Matrix4x4 projectionMatrix = camera->GetProjectionMatrix();
    Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, WinApp::kClientWidth_, WinApp::kClientHeight_, 0.0f, 1.0f);

    // 各行列の掛け算で合成行列（スクリーン→ビュー→ワールドの変換）を作る
    Matrix4x4 matVPV = MultiplyMatrix(viewMatrix, MultiplyMatrix(projectionMatrix, viewportMatrix));

    // 合成行列の逆行列を計算
    Matrix4x4 matInverseVPV = InverseMatrix(matVPV);

    // スクリーン座標（Z=0:ニア, Z=1:ファー）
    Vector3 posNear = Vector3(position.x, position.y, 0.0f); // ニアクリップ面上のスクリーン座標
    Vector3 posFar = Vector3(position.x, position.y, 1.0f);   // ファークリップ面上のスクリーン座標

    // スクリーン座標 → ワールド座標への変換（合成行列の逆行列を使用）
    posNear = TransformVtoM(posNear, matInverseVPV); // ワールド空間上のニア点
    posFar = TransformVtoM(posFar, matInverseVPV);   // ワールド空間上のファー点

    // マウスレイの方向（Far - Near）を求める
    Vector3 mouseDirection = posFar - posNear;
    mouseDirection = mouseDirection.Normalize(); // 正規化

    // 任意の距離だけ進めた位置に3Dレティクルを配置
    Vector3 reticleWorldPosition = posNear + mouseDirection * kDistanceToReticle;

    reticle3D_->SetTranslate(reticleWorldPosition);

    Vector3 playerPos = GetWorldPosition();
    bulletVelocity_ = reticleWorldPosition - playerPos;
    bulletVelocity_ = bulletVelocity_.Normalize();

    beamVelocity_ = reticleWorldPosition - playerPos;
    beamVelocity_ = beamVelocity_.Normalize();
}

void Player::Move() {
    if (state_ == NORMAL) {
        Vector3 translate = model_->GetTranslate();

        float lx = 0.0f;
        float ly = 0.0f;

        if (gamePad_->IsConnected()) {
            // 左スティックの入力取得
            lx = gamePad_->LeftStickX(); // -1.0 ~ +1.0
            ly = gamePad_->LeftStickY(); // -1.0 ~ +1.0
        } else {
            if (input_->PushKey(DIK_A)) {
                lx = -1.0f;
            }
            
            if (input_->PushKey(DIK_D)) {
                lx = 1.0f;
            }
            
            if (input_->PushKey(DIK_W)) {
                ly = 1.0f;
            }
            
            if (input_->PushKey(DIK_S)) {
                ly = -1.0f;
            }
        }

        // 入力ベクトル
        Vector2 move(lx, ly);

        // 斜めで速くなりすぎないように長さをクランプ
        float length = sqrt(move.x * move.x + move.y * move.y);
        if (length > 1.0f)
        {
            move.x /= length;
            move.y /= length;
        }

        // 位置を更新（倒し量＝速度）
        translate.x += move.x * moveSpeed_ * deltaTime_;
        translate.y += move.y * moveSpeed_ * deltaTime_;

        translate.x = std::clamp(translate.x, -9.5f, 9.5f);
        translate.y = std::clamp(translate.y, -1.0f, 9.0f);

        model_->SetTranslate(translate);

        Vector3 velocity = reticle3D_->GetWorldPosition() - GetWorldPosition();
        Vector3 rotate = model_->GetRotate();
        rotate.y = std::atan2(velocity.x, velocity.z);
        float horizontalLength = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
        rotate.x = std::atan2(-velocity.y, horizontalLength);
        model_->SetRotate(rotate);
    }
}

void Player::Attack() {
    if (gamePad_->IsConnected()) {
        // Rボタントリガーで弾を発射
        if (gamePad_->PushButton(GamePad::R) && state_ == NORMAL) {
            bulletSpawnTimer_ += deltaTime_;
            if (bulletSpawnTimer_ >= bulletSpawnTime_) {
                for (auto& bullet : bullets_) {
                    if (!bullet->GetIsAlive()) {
                        bullet->Spawn(model_->GetTranslate(), bulletVelocity_); // 軽い処理
                        bulletSpawnTimer_ = 0.0f;

                        bShotSE_->PlayAudio();
                        bShotSE_->SetVolume(0.5f);
                        break;
                    }
                }
            }
        }

        if (gamePad_->TriggerButton(GamePad::L)) {
            if (!isBeamShot_) {
                beamChargeSE_->PlayAudio();
                beamChargeSE_->SetVolume(0.5f);
            }
        }

        if (gamePad_->PushButton(GamePad::L)) {
            moveSpeed_ = minSpeed_;
            beamChargeTimer_ += deltaTime_;
            beamChargeRadius_ += deltaTime_;
            beamCharge_->UseEmitter(true);

            gaugePosX_ = Lerp(30.0f, 83.0f, beamChargeTimer_);
            gaugeScaleX_ = Lerp(1.0f, 95.0f, beamChargeTimer_);

            if (beamChargeTimer_ >= beamChargeTime_) {
                beamChargeTimer_ = beamChargeTime_;
                isBeamShot_ = true;
                gauge_->SetColor({ 0.0f,0.5f,0.8f,1.0f });
            }
        }

        if (isBeamShot_) {
            if (gamePad_->ReleaseButton(GamePad::L)) {
                if (!beam_->GetIsAlive()) {
                    beam_->Spawn(model_->GetTranslate(), beamVelocity_);
                    beam_->SetIsAlive(true);
                    isBeamShot_ = false;
                    beamCharge_->UseEmitter(false);
                    beamChargeTimer_ = 0.0f;
                    beamChargeRadius_ = 0.0f;
                    moveSpeed_ = maxSpeed_;

                    beamShotSE_->PlayAudio();
                }
                gaugePosX_ = 30.0f;
                gaugeScaleX_ = 1.0f;
                gauge_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            }
            beamChargeRange_.minColor = { 0.0f,0.0f,0.2f };
            beamChargeRange_.maxColor = { 0.0f,0.5f,0.8f };
        } else {
            if (gamePad_->ReleaseButton(GamePad::L)) {
                beamCharge_->UseEmitter(false);
                beamChargeTimer_ = 0.0f;
                beamChargeRadius_ = 0.0f;
                moveSpeed_ = maxSpeed_;
                gaugePosX_ = 30.0f;
                gaugeScaleX_ = 1.0f;
                gauge_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

                beamChargeSE_->StopAll();
            }
            beamChargeRange_.minColor = { 0.0f,0.0f,0.0f };
            beamChargeRange_.maxColor = { 1.0f,1.0f,1.0f };
        }

        beamChargeRadius_ = std::clamp(beamChargeRadius_, 0.0f, 1.5f);
    } else {
        // Rボタントリガーで弾を発射
        if (input_->PushMouseButtonL() && state_ == NORMAL) {
            bulletSpawnTimer_ += deltaTime_;
            if (bulletSpawnTimer_ >= bulletSpawnTime_) {
                for (auto& bullet : bullets_) {
                    if (!bullet->GetIsAlive()) {
                        bullet->Spawn(model_->GetTranslate(), bulletVelocity_); // 軽い処理
                        bulletSpawnTimer_ = 0.0f;

                        bShotSE_->PlayAudio();
                        bShotSE_->SetVolume(0.5f);
                        break;
                    }
                }
            }
        }

        if (input_->TriggerMouseButtonR()) {
            if (!isBeamShot_) {
                beamChargeSE_->PlayAudio();
                beamChargeSE_->SetVolume(0.5f);
            }
        }

        if (input_->PushMouseButtonR()) {
            moveSpeed_ = minSpeed_;
            beamChargeTimer_ += deltaTime_;
            beamChargeRadius_ += deltaTime_;
            beamCharge_->UseEmitter(true);

            gaugePosX_ = Lerp(30.0f, 83.0f, beamChargeTimer_);
            gaugeScaleX_ = Lerp(1.0f, 95.0f, beamChargeTimer_);

            if (beamChargeTimer_ >= beamChargeTime_) {
                beamChargeTimer_ = beamChargeTime_;
                isBeamShot_ = true;
                gauge_->SetColor({ 0.0f,0.5f,0.8f,1.0f });
            }
        }

        if (isBeamShot_) {
            if (input_->ReleaseMouseButtonR()) {
                if (!beam_->GetIsAlive()) {
                    beam_->Spawn(model_->GetTranslate(), beamVelocity_);
                    beam_->SetIsAlive(true);
                    isBeamShot_ = false;
                    beamCharge_->UseEmitter(false);
                    beamChargeTimer_ = 0.0f;
                    beamChargeRadius_ = 0.0f;
                    moveSpeed_ = maxSpeed_;

                    beamShotSE_->PlayAudio();
                }
                gaugePosX_ = 30.0f;
                gaugeScaleX_ = 1.0f;
                gauge_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            }
            beamChargeRange_.minColor = { 0.0f,0.0f,0.2f };
            beamChargeRange_.maxColor = { 0.0f,0.5f,0.8f };
        } else {
            if (input_->ReleaseMouseButtonR()) {
                beamCharge_->UseEmitter(false);
                beamChargeTimer_ = 0.0f;
                beamChargeRadius_ = 0.0f;
                moveSpeed_ = maxSpeed_;
                gaugePosX_ = 30.0f;
                gaugeScaleX_ = 1.0f;
                gauge_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

                beamChargeSE_->StopAll();
            }
            beamChargeRange_.minColor = { 0.0f,0.0f,0.0f };
            beamChargeRange_.maxColor = { 1.0f,1.0f,1.0f };
        }

        beamChargeRadius_ = std::clamp(beamChargeRadius_, 0.0f, 1.5f);
    }
}

void Player::Action() {
    // ZR ZL で左右にダッシュ
    if (state_ == NORMAL && isCanDash) {
        if (gamePad_->RightTrigger() >= 0.5f || input_->TriggerKey(DIK_SPACE) && input_->PushKey(DIK_D)) {
            state_ = DASH;
            isCanDash = false;
            dashRotateSpeed_ = -6.28f * 2.0f;
            dashDirection_ = 1.0f;

            dashSE_->PlayAudio();
        } else if (gamePad_->LeftTrigger() >= 0.5f || input_->TriggerKey(DIK_SPACE) && input_->PushKey(DIK_A)) {
            state_ = DASH;
            isCanDash = false;
            dashRotateSpeed_ = 6.28f * 2.0f;
            dashDirection_ = -1.0f;

            dashSE_->PlayAudio();
        }
    }

    // ダッシュ中は実機のZ軸を回転＋動かす
    if (state_ == DASH) {
        dashRotate_.x = moveRotate_.x;
        dashRotate_.y = moveRotate_.y;
        dashRotate_.z += dashRotateSpeed_ * deltaTime_;
        model_->SetRotate(dashRotate_);

        Vector3 translate = model_->GetTranslate();
        translate.x += dashDirection_ * moveSpeed_ * deltaTime_;
        translate.x = std::clamp(translate.x, -9.5f, 9.5f);
        model_->SetTranslate(translate);

        dashRotateTimer_ += deltaTime_;
        if (dashRotateTimer_ >= dashRotateTime_) {
            dashRotateTimer_ = 0.0f;
            dashRotate_.z = 0.0f;
            model_->SetRotate(dashRotate_);
            state_ = NORMAL;
        }
    }

    // ダッシュのクールタイムをカウント
    if (state_ == NORMAL && !isCanDash) {
        dashCoolTimer_ += deltaTime_;
        if (dashCoolTimer_ >= dashCoolTime_) {
            dashCoolTimer_ = 0.0f;
            isCanDash = true;
        }
    }

    if (state_ == NORMAL) {
        engineFireEmitter_.count = 2;
    } else if (state_ == DASH) {
        engineFireEmitter_.count = 10;
    }

    engineFire_->SetEmitterValue(engineFireEmitter_);
}

void Player::UpdateParticle() {
    engineFireEmitter_.radius = 0.05f;
    engineFireEmitter_.spawnTime = 0.01f;
    engineFireEmitter_.isMove = true;

    engineFireRange_.minScale = { 0.1f,0.1f,0.1f };
    engineFireRange_.maxScale = { 1.1f,1.1f,1.1f };
    engineFireRange_.minVelocity = { 0.0f,0.0f,-0.3f };
    engineFireRange_.maxVelocity = { 0.0f,0.0f,0.0f };
    engineFireRange_.minColor = { 0.9f,0.0f,0.0f };
    engineFireRange_.maxColor = { 1.0f,0.5f,0.0f };
    engineFireRange_.minLifeTime = 0.1f;
    engineFireRange_.maxLifeTime = 0.2f;

    engineFire_->SetEmitterValue(engineFireEmitter_);
    engineFire_->SetEmitterRange(engineFireRange_);
    engineFire_->SetEmitterPosition(model_->GetTranslate());

    Vector3 playerRot = model_->GetRotate();
    const float scaleTY = 0.41f / 0.43f;
    const float scaleTX = 0.75f / 0.72f;
    Vector3 emitterOffset = { 0.0f, 0.0f,-1.5f };
    emitterOffset.y = playerRot.x * scaleTY;
    emitterOffset.x = playerRot.y * scaleTX;
    emitterOffset.x *= -1.0f;
    engineFireOffset_ = emitterOffset;
    engineFire_->SetOffSet(engineFireOffset_);
    
    //--------------------------------------------//

    healEmitter_.count = 2;
    healEmitter_.radius = 2.f;
    healEmitter_.spawnTime = 0.01f;
    healEmitter_.isMove = true;

    healRange_.minScale = { 0.1f,0.1f,0.1f };
    healRange_.maxScale = { 1.0f,1.0f,1.0f };
    healRange_.minVelocity = { 0.0f,0.05f,0.0f };
    healRange_.maxVelocity = { 0.0f,0.2f,0.0f };
    healRange_.minColor = { 0.0f,0.2f,0.0f };
    healRange_.maxColor = { 0.0f,1.0f,0.0f };
    healRange_.minLifeTime = 0.1f;
    healRange_.maxLifeTime = 0.3f;

    heal_->SetEmitterValue(healEmitter_);
    heal_->SetEmitterRange(healRange_);
    heal_->SetEmitterPosition(model_->GetTranslate());
    heal_->SetOffSet({ 0.0f,0.5f,0.0f });

    if (isHeal_) {
        heal_->UseEmitter(true);
        healTimer_ += deltaTime_;
        if (healTimer_ >= healTime_) {
            healTimer_ = 0.0f;
            isHeal_ = false;
        }
    } else {
        heal_->UseEmitter(false);
    }

    //--------------------------------------------//

    damageEmitter_.count = 50;
    damageEmitter_.radius = 0.01f;
    damageEmitter_.spawnTime = 0.01f;
    damageEmitter_.isMove = true;

    damageRange_.minScale = { 0.1f,0.1f,0.1f };
    damageRange_.maxScale = { 2.0f,2.0f,1.0f };
    damageRange_.minVelocity = { -0.2f,-0.2f,-0.2f };
    damageRange_.maxVelocity = { 0.2f,0.2f,0.2f };
    damageRange_.minColor = { 0.1f,0.0f,0.0f };
    damageRange_.maxColor = { 1.0f,0.1f,0.1f };
    damageRange_.minLifeTime = 0.1f;
    damageRange_.maxLifeTime = 0.3f;

    damage_->SetEmitterValue(damageEmitter_);
    damage_->SetEmitterRange(damageRange_);
    damage_->SetEmitterPosition(model_->GetTranslate());

    if (isDamage_) {
        damage_->UseEmitter(true);
        damageTimer_ += deltaTime_;
        if (damageTimer_ >= damageTime_) {
            damageTimer_ = 0.0f;
            isDamage_ = false;
        }
    } else {
        damage_->UseEmitter(false);
    }

    //--------------------------------------------//

    beamChargeEmitter_.count = 50;
    beamChargeEmitter_.radius = beamChargeRadius_;
    beamChargeEmitter_.spawnTime = 0.01f;
    beamChargeEmitter_.isMove = true;

    beamChargeRange_.minScale = { 0.1f,0.1f,0.1f };
    beamChargeRange_.maxScale = { 1.0f,1.0f,1.0f };
    beamChargeRange_.minVelocity = { -0.1f,-0.1f,-0.1f };
    beamChargeRange_.maxVelocity = { 0.1f,0.1f,0.1f };
    beamChargeRange_.minLifeTime = 0.1f;
    beamChargeRange_.maxLifeTime = 0.1f;

    beamCharge_->SetEmitterValue(beamChargeEmitter_);
    beamCharge_->SetEmitterRange(beamChargeRange_);
    beamCharge_->SetOffSet(beamChargeOffset_);
    beamCharge_->SetEmitterPosition(model_->GetTranslate());

    //--------------------------------------------//

    dieFireEmitter_.count = 250;
    dieFireEmitter_.radius = 1.0f;
    dieFireEmitter_.spawnTime = 0.01f;
    dieFireEmitter_.isMove = true;

    dieFireRange_.minScale = { 0.1f,0.1f,0.1f };
    dieFireRange_.maxScale = { 1.5f,1.5f,1.5f };
    dieFireRange_.minVelocity = { -0.5f,0.0f,-0.5f };
    dieFireRange_.maxVelocity = { 0.5f,2.0f,0.5f };
    dieFireRange_.minColor = { 0.9f,0.0f,0.0f };
    dieFireRange_.maxColor = { 1.0f,0.5f,0.0f };
    dieFireRange_.minLifeTime = 1.0f;
    dieFireRange_.maxLifeTime = 2.0f;

    dieFire_->SetEmitterValue(dieFireEmitter_);
    dieFire_->SetEmitterRange(dieFireRange_);

    dieSmorkEmitter_.count = 2;
    dieSmorkEmitter_.radius = 0.5f;
    dieSmorkEmitter_.spawnTime = 0.01f;
    dieSmorkEmitter_.isMove = true;

    dieSmorkRange_.minScale = { 0.1f,0.1f,0.1f };
    dieSmorkRange_.maxScale = { 1.0f,1.0f,1.0f };
    dieSmorkRange_.minVelocity = { -0.05f,0.0f,-0.05f };
    dieSmorkRange_.maxVelocity = { 0.05f,1.0f,0.05f };
    dieSmorkRange_.minColor = { 1.0f,1.0f,1.0f };
    dieSmorkRange_.maxColor = { 1.0f,1.0f,1.0f };
    dieSmorkRange_.minLifeTime = 0.1f;
    dieSmorkRange_.maxLifeTime = 0.75f;

    dieSmork_->SetEmitterValue(dieSmorkEmitter_);
    dieSmork_->SetEmitterRange(dieSmorkRange_);
}

void Player::UpdateLife() {
    if (life_ <= 0) {
        life_ = 0;
    }

    if (life_ >= 3) {
        life_ = 3;
    }

    if (life_ == 3) {
        lifeUI_->SetTexture("resources/image/UI/life3.png");
    } else if (life_ == 2) {
        lifeUI_->SetTexture("resources/image/UI/life2.png");
    } else if (life_ == 1) {
        lifeUI_->SetTexture("resources/image/UI/life1.png");
    } else if (life_ == 0) {
        lifeUI_->SetTexture("resources/image/UI/life0.png");
    }

    lifeUI_->Update();
}

void Player::Heal() {
    if (!isHeal_) {
        isHeal_ = true;
        life_++;
        healSE_->PlayAudio();
    }
}

void Player::Damage() {
    if (!isDamage_ && !isInvincible_) {
        isDamage_ = true;
        isInvincible_ = true;
        life_ --;
        damageSE_->PlayAudio();
    }
}

std::vector<PlayerBullet*> Player::GetAllBullets() {
    std::vector<PlayerBullet*> result;
    for (auto& bullet : bullets_) {
        result.push_back(bullet.get()); // unique_ptr → 生ポインタ
    }
    return result;
}

Vector3 Player::GetWorldPosition() {
    Vector3 worldPos = {};
    worldPos.x = model_->GetWorldMatrix().m[3][0];
    worldPos.y = model_->GetWorldMatrix().m[3][1];
    worldPos.z = model_->GetWorldMatrix().m[3][2];
    return worldPos;
}

void Player::CloseSound() {
    healSE_->Reset();
    dashSE_->Reset();
    damageSE_->Reset();
    bShotSE_->Reset();
    beamChargeSE_->Reset();
    beamShotSE_->Reset();
}