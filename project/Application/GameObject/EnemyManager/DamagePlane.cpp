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

/// <summary>
/// ダメージ値に応じた色を計算（1-10のまとまりごとに薄いオレンジから赤へ）
/// </summary>
/// <param name="damage">ダメージ値（0-99）</param>
/// <returns>RGB色（Vector3）</returns>
Vector3 CalculateDamageColor(int damage) {
	// 10の位を取得（0-9）
	int tensDigit = damage / 10;
	
	// 10のまとまりごとに色を変化させる
	// 0-9:   薄いオレンジ (1.0, 0.8, 0.6)
	// 10-19: オレンジ     (1.0, 0.7, 0.4)
	// 20-29: 濃いオレンジ (1.0, 0.6, 0.2)
	// 30-39: オレンジ赤   (1.0, 0.5, 0.1)
	// 40-49: 赤オレンジ   (1.0, 0.4, 0.0)
	// 50-59: 赤           (1.0, 0.3, 0.0)
	// 60-69: 濃い赤       (1.0, 0.2, 0.0)
	// 70-79: より濃い赤   (1.0, 0.1, 0.0)
	// 80-89: 深紅         (1.0, 0.05, 0.0)
	// 90-99: 真紅         (1.0, 0.0, 0.0)
	
	float greenValue = 0.0f;
	float blueValue = 0.0f;
	
	switch (tensDigit) {
	case 0:
		greenValue = 0.8f;
		blueValue = 0.6f;
		break;
	case 1:
		greenValue = 0.7f;
		blueValue = 0.4f;
		break;
	case 2:
		greenValue = 0.6f;
		blueValue = 0.2f;
		break;
	case 3:
		greenValue = 0.5f;
		blueValue = 0.1f;
		break;
	case 4:
		greenValue = 0.4f;
		blueValue = 0.0f;
		break;
	case 5:
		greenValue = 0.3f;
		blueValue = 0.0f;
		break;
	case 6:
		greenValue = 0.2f;
		blueValue = 0.0f;
		break;
	case 7:
		greenValue = 0.1f;
		blueValue = 0.0f;
		break;
	case 8:
		greenValue = 0.05f;
		blueValue = 0.0f;
		break;
	case 9:
	default:
		greenValue = 0.0f;
		blueValue = 0.0f;
		break;
	}
	
	return Vector3(1.0f, greenValue, blueValue);
}

void DamagePlane::Initialize(Vector3 pos, int damage) {
	damage_ = std::clamp(damage, 0, 99);

	model_ = std::make_unique<Model>();
	model_->Initialize("enemy/DamagePlane.obj");
	model_->SetBillboard(true);
	model_->UseLight(false);

	// ダメージに応じた色を設定
	Vector3 damageColor = CalculateDamageColor(damage_);
	model_->SetColor3(damageColor);

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