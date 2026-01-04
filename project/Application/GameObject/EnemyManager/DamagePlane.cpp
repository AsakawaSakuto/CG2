#include "DamagePlane.h"

// 0-9   "resources/image/number/0-9.png"  
// 0 = { 0.0f,0.0f }, { 0.1f,1.0f } : 1 = { 0.1f,0.0f }, { 0.1f,1.0f } : 9 = { 0.9f,0.0f }, { 0.1f,1.0f }
// 10-19 "resources/image/number/10-19.png"
// 20-29 "resources/image/number/20-29.png"
// 30-39 "resources/image/number/30-39.png"
// 40-49 "resources/image/number/40-49.png"
// 50-59 "resources/image/number/50-59.png"
// 60-69 "resources/image/number/60-69.png"
// 70-79 "resources/image/number/70-79.png"
// 80-89 "resources/image/number/80-89.png"
// 90-99 "resources/image/number/90-99.png"

void DamagePlane::Initialize(Vector3 pos, int damage) {
	damage_ = std::clamp(damage, 0, 99);

	model_ = std::make_unique<Model>();
	model_->Initialize("enemy/DamagePlane.obj");
	model_->SetBillboard(true);
	model_->UseLight(false);

	// ダメージに応じたテクスチャファイルとUV座標を決定
	std::string texturePath;
	Vector2 topLeft;
	Vector2 size = { 0.1f, 1.0f };

	if (damage_ < 10) {
		// 0-9の場合
		texturePath = "resources/image/number/0-9.png";
		topLeft = { damage_ * 0.1f, 0.0f };
	}
	else {
		// 10-99の場合
		int tensDigit = damage_ / 10;  // 10の位
		int onesDigit = damage_ % 10;  // 1の位

		// 10の位に応じたテクスチャファイルを選択
		texturePath = "resources/image/number/" + std::to_string(tensDigit) + "0-" + std::to_string(tensDigit) + "9.png";
		
		// 1の位に応じたUV座標を設定
		topLeft = { onesDigit * 0.1f, 0.0f };
	}

	model_->SetTextureWithRect(texturePath, topLeft, size);

	transform_.translate = pos;
	transform_.scale =  { 0.5f, 0.5f, 0.5f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	
	// ライフタイマーを開始
	lifeTimer_.Start(lifeTime_, false);
}

void DamagePlane::Update() {
	transform_.translate.y += 10.0f * 1.0f / 60.0f;
	
	// ライフタイマーを更新
	lifeTimer_.Update();
}

void DamagePlane::Draw(Camera camera) {
	model_->Draw(camera, transform_);
}