#include "BitmapFont.h"
#include <algorithm>
#include <cmath>

void BitmapFont::Initialize(DirectXCommon* dxCommon, const std::string& numberImageFolder) {
	dxCommon_ = dxCommon;
	numberImageFolder_ = numberImageFolder;

	// 各桁用の0-9の数字スプライトを初期化
	for (int digitIndex = 0; digitIndex < kMaxDigits; ++digitIndex) {
		for (int i = 0; i < 10; ++i) {
			digitSprites_[digitIndex].sprites[i] = std::make_unique<Sprite>();
			std::string imagePath = numberImageFolder_ + std::to_string(i) + ".png";
			digitSprites_[digitIndex].sprites[i]->Initialize(dxCommon_, imagePath);
		}
	}

	// コロンスプライトを初期化
	colonSprite_ = std::make_unique<Sprite>();
	colonSprite_->Initialize(dxCommon_, numberImageFolder_ + "colon.png");

	// ドットスプライトを初期化（オプション、画像がある場合）
	// dotSprite_ = std::make_unique<Sprite>();
	// dotSprite_->Initialize(dxCommon_, numberImageFolder_ + "dot.png");
}

void BitmapFont::Update() {
	// digits_の情報は保持するが、スプライトの設定は描画時に行う
	// このメソッドでは何もしない（または共通設定のみ行う）
}

void BitmapFont::Draw() {
	// 各桁のスプライトを描画（描画時に位置を設定）
	for (const auto& digit : digits_) {
		if (digit.isColon && colonSprite_) {
			colonSprite_->SetPosition(digit.position);
			colonSprite_->SetScale(scale_);
			colonSprite_->SetColor(color_);
			colonSprite_->Update();
			colonSprite_->Draw();
		}
		else if (digit.isDot && dotSprite_) {
			dotSprite_->SetPosition(digit.position);
			dotSprite_->SetScale(scale_);
			dotSprite_->SetColor(color_);
			dotSprite_->Update();
			dotSprite_->Draw();
		}
		else if (digit.number >= 0 && digit.number <= 9 && digit.digitIndex < kMaxDigits) {
			auto& sprite = digitSprites_[digit.digitIndex].sprites[digit.number];
			sprite->SetPosition(digit.position);
			sprite->SetScale(scale_);
			sprite->SetColor(color_);
			sprite->Update();
			sprite->Draw();
		}
	}
}

void BitmapFont::SetNumber(int value, const Vector2& position, float digitSpacing) {
	digits_.clear();

	// 負の数の場合は0として扱う
	if (value < 0) {
		value = 0;
	}

	// 数字を桁ごとに分解
	std::vector<int> digitList = SplitDigits(value);

	// 桁数が最大桁数を超える場合は切り詰める
	if (digitList.size() > kMaxDigits) {
		digitList.resize(kMaxDigits);
	}

	// 桁ごとに表示位置を設定
	Vector2 currentPos = position;
	for (int i = 0; i < static_cast<int>(digitList.size()); ++i) {
		Digit d;
		d.digitIndex = i;
		d.number = digitList[i];
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);

		currentPos.x += digitSpacing * scale_.x;
	}
}

void BitmapFont::SetFloat(float value, const Vector2& position, int decimalPlaces, float digitSpacing) {
	digits_.clear();

	// 負の数の場合は0として扱う
	if (value < 0.0f) {
		value = 0.0f;
	}

	// 整数部分と小数部分を分離
	int integerPart = static_cast<int>(value);
	float fractionalPart = value - integerPart;

	// 整数部分の桁を取得
	std::vector<int> integerDigits = SplitDigits(integerPart);

	// 小数部分の桁を取得
	std::vector<int> fractionalDigits;
	for (int i = 0; i < decimalPlaces; ++i) {
		fractionalPart *= 10.0f;
		int digit = static_cast<int>(fractionalPart) % 10;
		fractionalDigits.push_back(digit);
	}

	// 桁インデックス
	int digitIndex = 0;

	// 整数部分を配置
	Vector2 currentPos = position;
	for (int digit : integerDigits) {
		if (digitIndex >= kMaxDigits) break;
		
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = digit;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);

		currentPos.x += digitSpacing * scale_.x;
	}

	// 小数点を配置（ドットスプライトがある場合）
	if (dotSprite_ && decimalPlaces > 0) {
		Digit d;
		d.digitIndex = 0;
		d.number = 0;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = true;
		digits_.push_back(d);

		currentPos.x += (digitSpacing * 0.5f) * scale_.x; // ドットは小さいので間隔を狭める
	}

	// 小数部分を配置
	for (int digit : fractionalDigits) {
		if (digitIndex >= kMaxDigits) break;
		
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = digit;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);

		currentPos.x += digitSpacing * scale_.x;
	}
}

void BitmapFont::SetTime(float seconds, const Vector2& position, float digitSpacing) {
	digits_.clear();

	// 秒を分と秒に変換
	int totalSeconds = static_cast<int>(seconds);
	int minutes = totalSeconds / 60;
	int secs = totalSeconds % 60;

	// 桁を配置
	Vector2 currentPos = position;
	int digitIndex = 0;

	// 常に M:SS または MM:SS 形式で表示

	// 分の表示（先頭の0は表示しない）
	if (minutes >= 10) {
		// 2桁以上の場合、十の位も表示
		int minuteTens = minutes / 10;
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = minuteTens;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);
		currentPos.x += digitSpacing * scale_.x;
	}

	// 分の一の位（常に表示）
	{
		int minuteOnes = minutes % 10;
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = minuteOnes;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);
		currentPos.x += digitSpacing * scale_.x;
	}

	// コロン（常に表示）
	{
		Digit d;
		d.digitIndex = 0; // コロンは桁インデックス不要
		d.number = 0;
		d.position = currentPos;
		d.isColon = true;
		d.isDot = false;
		digits_.push_back(d);
		currentPos.x += digitSpacing * scale_.x; // コロンも通常の間隔
	}

	// 秒の十の位（常に表示、0秒台でも0を表示）
	{
		int secondTens = secs / 10;
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = secondTens;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);
		currentPos.x += digitSpacing * scale_.x;
	}

	// 秒の一の位（常に表示）
	{
		int secondOnes = secs % 10;
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = secondOnes;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		digits_.push_back(d);
	}
}

Vector2 BitmapFont::GetDigitSize() const {
	if (digitSprites_[0].sprites[0]) {
		return digitSprites_[0].sprites[0]->GetSize();
	}
	return { 0.0f, 0.0f };
}

std::vector<int> BitmapFont::SplitDigits(int value) const {
	std::vector<int> digits;

	// 0の場合は0を1つだけ返す
	if (value == 0) {
		digits.push_back(0);
		return digits;
	}

	// 桁ごとに分解
	while (value > 0) {
		digits.push_back(value % 10);
		value /= 10;
	}

	// 逆順にする（上位桁が先頭になるように）
	std::reverse(digits.begin(), digits.end());

	return digits;
}
