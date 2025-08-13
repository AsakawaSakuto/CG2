#include"Player.h"

void Player::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/object3d/player/player.obj");

    reticle3D_->Initialize(dxCommon_, "resources/object3d/cube.obj");

    reticle2D_->Initialize(dxCommon_, "resources/image/reticle.png", {64.0f,64.0f});
    reticle2D_->SetPosition({ 640.0f,360.0f });

	gamePad_.Initialize();

    for (int i = 0; i < 32; ++i) {
        auto bullet = std::make_unique<PlayerBullet>();
        bullet->Initialize(dxCommon_); // 重い処理はここで全部
        bullet->SetIsAlive(false);
        bullets_.push_back(std::move(bullet));
    }

    state_ = NORMAL;
}

void Player::Update(Camera* camera) {

	gamePad_.Update();
	
	Move();

    Attack();

    Action();

    UpdateReticle(camera);

    for (auto& bullet : bullets_) {
        if (bullet->GetIsAlive()) {
            bullet->Update(camera);
        }
    }

	model_->Update(*camera);

    reticle3D_->Update(*camera);

    reticle2D_->Update();
}

void Player::Draw() {
	model_->Draw();
    
    for (auto& bullet : bullets_) {
        if (bullet->GetIsAlive()) {
            bullet->Draw();
        }
    }

    reticle3D_->Draw();

    reticle2D_->Draw();
}

void Player::DrawImGui() {
    ImGui::DragFloat("Speed", &speed_, 0.1f);
    ImGui::DragFloat("BullerSpeed", &bulletSpeed_, 1.0f);
    ImGui::DragFloat("BulledSpawn", &bulletSpawnTime_, 0.01f);
    ImGui::DragFloat("Distance", &kDistanceToReticle, 1.0f);
    model_->DrawImGui("player");
}

void Player::Move() {
    if (state_ == NORMAL) {
        Vector3 translate = model_->GetTranslate();

        // 左スティックの入力取得
        float lx = gamePad_.LeftStickX(); // -1.0 ~ +1.0
        float ly = gamePad_.LeftStickY(); // -1.0 ~ +1.0

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
        translate.x += move.x * speed_ * deltaTime;
        translate.y += move.y * speed_ * deltaTime;

        model_->SetTranslate(translate);
    }
}

void Player::UpdateReticle(Camera* camera) {

    Vector2 position = reticle2D_->GetPosition();

    // 左スティックの入力取得
    float lx = gamePad_.RightStickX(); // -1.0 ~ +1.0
    float ly = gamePad_.RightStickY(); // -1.0 ~ +1.0

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
    position.x += move.x * reticleSpeed_ * deltaTime;
    position.y -= move.y * reticleSpeed_ * deltaTime;

    reticle2D_->SetPosition(position);

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
    bulletVelocity_ = bulletVelocity_.Normalize() * bulletSpeed_;
}

void Player::Attack() {
    if (gamePad_.PushButton(GamePad::R) && state_== NORMAL) {
        bulletSpawnTimer_ += deltaTime;
        if (bulletSpawnTimer_ >= bulletSpawnTime_) {
            for (auto& bullet : bullets_) {
                if (!bullet->GetIsAlive()) {
                    bullet->Spawn(model_->GetTranslate(), bulletVelocity_); // 軽い処理
                    bulletSpawnTimer_ = 0.0f;
                    break;
                }
            }
        }
    }
}

void Player::Action() {
    if (state_ == NORMAL && isCanDash) {
        if (gamePad_.RightTrigger() >= 0.5f) {
            state_ = DASH;
            isCanDash = false;
            rotateSpeed_ = -6.28f * 2.0f;
            dashDirection_ = 1.0f;
        } else if (gamePad_.LeftTrigger() >= 0.5f) {
            state_ = DASH;
            isCanDash = false;
            rotateSpeed_ = 6.28f * 2.0f;
            dashDirection_ = -1.0f;
        }
    }

    if (state_ == DASH) {
        rotate_.z += rotateSpeed_ * deltaTime;
        model_->SetRotate(rotate_);

        Vector3 translate = model_->GetTranslate();
        translate.x += dashDirection_ * speed_ * deltaTime;
        model_->SetTranslate(translate);

        rotateTimer_ += deltaTime;
        if (rotateTimer_ >= rotateTime_) {
            rotateTimer_ = 0.0f;
            rotate_.z = 0.0f;
            model_->SetRotate(rotate_);
            state_ = NORMAL;
        }
    }

    if (state_ == NORMAL && !isCanDash) {
        dashCoolTimer_ += deltaTime;
        if (dashCoolTimer_ >= dashCoolTime) {
            dashCoolTimer_ = 0.0f;
            isCanDash = true;
        }
    }
}

Vector3 Player::GetWorldPosition() {
    Vector3 worldPos = {};
    worldPos.x = model_->GetWorldMatrix().m[3][0];
    worldPos.y = model_->GetWorldMatrix().m[3][1];
    worldPos.z = model_->GetWorldMatrix().m[3][2];
    return worldPos;
}