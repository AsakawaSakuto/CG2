#include"Player.h"

void Player::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	model_->Initialize(dxCommon_, "resources/object3d/player/player.obj");

	gamePad_.Initialize();
}

void Player::Update(Camera* camera) {

	gamePad_.Update();
	
	Move();

	model_->Update(*camera);
}

void Player::Draw() {
	model_->Draw();
}

void Player::DrawImGui() {
    model_->DrawImGui("player");
    ImGui::DragFloat("speed", &speed_, 0.1f);
}

void Player::Move() {

	Vector3 translate = model_->GetPosition();

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

    model_->SetPosition(translate);
}