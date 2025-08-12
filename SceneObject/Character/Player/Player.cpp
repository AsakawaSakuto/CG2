#include"Player.h"

void Player::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	model_->Initialize(dxCommon_, "resources/object3d/player/player.obj");

	gamePad_.Initialize();

    for (int i = 0; i < 32; ++i) {
        auto bullet = std::make_unique<PlayerBullet>();
        bullet->Initialize(dxCommon_); // 重い処理はここで全部
        bullet->SetIsAlive(false);
        bullets_.push_back(std::move(bullet));
    }
}

void Player::Update(Camera* camera) {

	gamePad_.Update();
	
    if (gamePad_.PushButton(GamePad::A)) {
        bulletSpawnTimer_ += deltaTime;
        if (bulletSpawnTimer_ >= bulletSpawnTime_) {
            for (auto& bullet : bullets_) {
                if (!bullet->GetIsAlive()) {
                    bullet->Spawn(model_->GetTranslate()); // 軽い処理
                    bulletSpawnTimer_ = 0.0f;
                    break;
                }
            }
        }
    }

	Move();

    for (auto& bullet : bullets_) {
        if (bullet->GetIsAlive()) {
            bullet->Update(camera);
        }
    }

	model_->Update(*camera);
}

void Player::Draw() {
	model_->Draw();
    
    for (auto& bullet : bullets_) {
        if (bullet->GetIsAlive()) {
            bullet->Draw();
        }
    }
}

void Player::DrawImGui() {
    model_->DrawImGui("player");
    ImGui::DragFloat("speed", &speed_, 0.1f);
}

void Player::Move() {

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