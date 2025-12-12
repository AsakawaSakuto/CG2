#include "Gauge.h"
#include <algorithm>

#ifdef USE_IMGUI
#include "imgui.h"
#endif

void Gauge::Initialize(DirectXCommon* dxCommon, const Vector2& position, const Vector2& size, float maxValue) {
	dxCommon_ = dxCommon;
	position_ = position;
	size_ = size;
	maxValue_ = maxValue;
	currentValue_ = maxValue;

	// 背景スプライトの初期化（white2x2.pngを使用）
	background_ = std::make_unique<Sprite>();
	background_->Initialize(dxCommon, "white2x2.png");
	background_->SetColor(backgroundColor_);
	background_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	// ゲージスプライトの初期化（white2x2.pngを使用）
	gauge_ = std::make_unique<Sprite>();
	gauge_->Initialize(dxCommon, "white2x2.png");
	gauge_->SetColor(gaugeColor_);
	gauge_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	// 枠スプライトの初期化（white2x2.pngを使用）
	borderTop_ = std::make_unique<Sprite>();
	borderTop_->Initialize(dxCommon, "white2x2.png");
	borderTop_->SetColor(borderColor_);
	borderTop_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	borderBottom_ = std::make_unique<Sprite>();
	borderBottom_->Initialize(dxCommon, "white2x2.png");
	borderBottom_->SetColor(borderColor_);
	borderBottom_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	borderLeft_ = std::make_unique<Sprite>();
	borderLeft_->Initialize(dxCommon, "white2x2.png");
	borderLeft_->SetColor(borderColor_);
	borderLeft_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	borderRight_ = std::make_unique<Sprite>();
	borderRight_->Initialize(dxCommon, "white2x2.png");
	borderRight_->SetColor(borderColor_);
	borderRight_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	UpdateSprites();
}

void Gauge::Update() {
	// 値の範囲チェック
	currentValue_ = std::clamp(currentValue_, 0.0f, maxValue_);

	UpdateSprites();

	// 各スプライトの更新
	background_->Update();
	gauge_->Update();

	if (showBorder_) {
		borderTop_->Update();
		borderBottom_->Update();
		borderLeft_->Update();
		borderRight_->Update();
	}
}

void Gauge::Draw() {
	// 背景を描画
	background_->Draw();

	// ゲージを描画（現在値が0より大きい場合のみ）
	if (currentValue_ > 0.0f) {
		gauge_->Draw();
	}

	// 枠を描画
	if (showBorder_) {
		borderTop_->Draw();
		borderBottom_->Draw();
		borderLeft_->Draw();
		borderRight_->Draw();
	}
}

void Gauge::DrawImGui(const char* name) {
#ifdef USE_IMGUI
	ImGui::Begin(name);

	ImGui::Text("Gauge: %.1f / %.1f (%.1f%%)", currentValue_, maxValue_, GetRatio() * 100.0f);
	
	if (ImGui::DragFloat("Current Value", &currentValue_, 1.0f, 0.0f, maxValue_)) {
		currentValue_ = std::clamp(currentValue_, 0.0f, maxValue_);
	}
	
	ImGui::DragFloat("Max Value", &maxValue_, 1.0f, 1.0f, 1000.0f);
	ImGui::DragFloat2("Position", &position_.x, 1.0f);
	ImGui::DragFloat2("Size", &size_.x, 1.0f, 10.0f, 1000.0f);

	ImGui::Separator();
	ImGui::ColorEdit4("Background Color", &backgroundColor_.x);
	ImGui::ColorEdit4("Gauge Color", &gaugeColor_.x);
	
	ImGui::Separator();
	ImGui::Checkbox("Show Border", &showBorder_);
	if (showBorder_) {
		ImGui::ColorEdit4("Border Color", &borderColor_.x);
		ImGui::DragFloat("Border Thickness", &borderThickness_, 0.1f, 0.0f, 10.0f);
	}

	ImGui::End();
#endif
}

void Gauge::SetCurrentValue(float value) {
	currentValue_ = std::clamp(value, 0.0f, maxValue_);
}

void Gauge::SetMaxValue(float maxValue) {
	maxValue_ = (std::max)(1.0f, maxValue); // 最大値は最低1
	currentValue_ = std::clamp(currentValue_, 0.0f, maxValue_);
}

void Gauge::SetPosition(const Vector2& position) {
	position_ = position;
}

void Gauge::SetSize(const Vector2& size) {
	size_ = size;
}

void Gauge::SetBackgroundColor(const Vector4& color) {
	backgroundColor_ = color;
	if (background_) {
		background_->SetColor(backgroundColor_);
	}
}

void Gauge::SetGaugeColor(const Vector4& color) {
	gaugeColor_ = color;
	if (gauge_) {
		gauge_->SetColor(gaugeColor_);
	}
}

void Gauge::SetShowBorder(bool show) {
	showBorder_ = show;
}

void Gauge::SetBorderColor(const Vector4& color) {
	borderColor_ = color;
	if (borderTop_) {
		borderTop_->SetColor(borderColor_);
		borderBottom_->SetColor(borderColor_);
		borderLeft_->SetColor(borderColor_);
		borderRight_->SetColor(borderColor_);
	}
}

void Gauge::SetBorderThickness(float thickness) {
	borderThickness_ = (std::max)(0.0f, thickness);
}

float Gauge::GetRatio() const {
	if (maxValue_ <= 0.0f) {
		return 0.0f;
	}
	return currentValue_ / maxValue_;
}

void Gauge::UpdateSprites() {
	// 枠がある場合の内側のサイズを計算
	float innerPaddingX = showBorder_ ? borderThickness_ : 0.0f;
	float innerPaddingY = showBorder_ ? borderThickness_ : 0.0f;
	
	Vector2 innerSize = { size_.x - innerPaddingX * 2.0f, size_.y - innerPaddingY * 2.0f };
	
	// 背景の位置とサイズを設定（左上基準）
	background_->SetPosition({ position_.x + innerPaddingX, position_.y + innerPaddingY });
	background_->SetScale(innerSize);

	// ゲージの位置とサイズを設定（左端から右に伸びる）
	float gaugeWidth = innerSize.x * GetRatio();
	gauge_->SetPosition({ position_.x + innerPaddingX, position_.y + innerPaddingY });
	gauge_->SetScale({ gaugeWidth, innerSize.y });

	// 枠の位置とサイズを設定
	if (showBorder_) {
		// 上の枠（左上基準）
		borderTop_->SetPosition({ position_.x, position_.y });
		borderTop_->SetScale({ size_.x, borderThickness_ });

		// 下の枠（左上基準）
		borderBottom_->SetPosition({ position_.x, position_.y + size_.y - borderThickness_ });
		borderBottom_->SetScale({ size_.x, borderThickness_ });

		// 左の枠（左上基準）
		borderLeft_->SetPosition({ position_.x, position_.y });
		borderLeft_->SetScale({ borderThickness_, size_.y });

		// 右の枠（左上基準）
		borderRight_->SetPosition({ position_.x + size_.x - borderThickness_, position_.y });
		borderRight_->SetScale({ borderThickness_, size_.y });
	}
}
