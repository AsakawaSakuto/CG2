#include "BitmapFont.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

void BitmapFont::Initialize(const std::string& jsonPath, const std::string& numberImageFolder) {
	// ServiceLocatorからDirectXCommonを取得
	dxCommon_ = ServiceLocator::GetDXCommon();
	
	if (!dxCommon_) {
		// エラー処理：DirectXCommonが登録されていない場合
		throw std::runtime_error("DirectXCommon is not registered in ServiceLocator. Call ServiceLocator::Provide(dxCommon) first.");
	}
	
	numberImageFolder_ = numberImageFolder;

	// JsonManagerの初期化（初回のみ）
	if (!jsonManager_) {
		jsonManager_ = std::make_unique<JsonManager>();
		jsonManager_->SetBasePath("resources/Data/Json/BitmapFont/");
	}

	// 各桁用の0-9の数字スプライトを初期化
	for (int digitIndex = 0; digitIndex < kMaxDigits; ++digitIndex) {
		for (int i = 0; i < 10; ++i) {
			digitSprites_[digitIndex].sprites[i] = std::make_unique<Sprite>();
			std::string imagePath = numberImageFolder_ + std::to_string(i) + ".png";
			digitSprites_[digitIndex].sprites[i]->Initialize(imagePath);
			digitSprites_[digitIndex].sprites[i]->SetAnchorPoint(AnchorPoint::Center);
		}
	}

	// コロンスプライトを初期化
	colonSprite_ = std::make_unique<Sprite>();
	colonSprite_->Initialize(numberImageFolder_ + "colon.png");
	colonSprite_->SetAnchorPoint(AnchorPoint::Center);

	// ドットスプライトを初期化（オプション、画像がある場合）
	dotSprite_ = std::make_unique<Sprite>();
	dotSprite_->Initialize(numberImageFolder_ + "dot.png");
	dotSprite_->SetAnchorPoint(AnchorPoint::Center);

	// パーセントスプライトを初期化
	percentSprite_ = std::make_unique<Sprite>();
	percentSprite_->Initialize(numberImageFolder_ + "parcent.png");
	percentSprite_->SetAnchorPoint(AnchorPoint::Center);

	LoadFromJson(jsonPath);
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
		else if (digit.isPercent && percentSprite_) {
			// パーセント記号は専用のオフセットとスケールを適用
			Vector2 percentPosition = digit.position;
			percentPosition.x += percentOffset_.x;
			percentPosition.y += percentOffset_.y;
			percentSprite_->SetPosition(percentPosition);
			percentSprite_->SetScale(Vector2{ scale_.x * percentScale_.x, scale_.y * percentScale_.y });
			percentSprite_->SetColor(color_);
			percentSprite_->Update();
			percentSprite_->Draw();
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

void BitmapFont::SetNumber(int value) {
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

	// 全体の幅を計算（右揃えの場合に必要）
	float totalWidth = digitList.size() * spaceWidth_ * scale_.x;
	if (showPercent_ && percentSprite_) {
		totalWidth += spaceWidth_ * scale_.x; // パーセント記号の分も追加
	}

	// 開始位置を決定
	Vector2 currentPos = position_;
	if (alignment_ == Alignment::Right) {
		// 右揃えの場合は左側にオフセット
		currentPos.x -= totalWidth;
	}

	// 桁ごとに表示位置を設定
	for (int i = 0; i < static_cast<int>(digitList.size()); ++i) {
		Digit d;
		d.digitIndex = i;
		d.number = digitList[i];
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		d.isPercent = false;
		digits_.push_back(d);

		currentPos.x += spaceWidth_ * scale_.x;
	}

	// パーセント記号を配置
	if (showPercent_ && percentSprite_) {
		Digit d;
		d.digitIndex = 0;
		d.number = 0;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		d.isPercent = true;
		digits_.push_back(d);
	}
}

void BitmapFont::SetFloat(float value, int decimalPlaces, bool showPercent) {
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

	// 全体の幅を計算
	float totalWidth = integerDigits.size() * spaceWidth_ * scale_.x;
	if (decimalPlaces > 0) {
		totalWidth += (spaceWidth_ * 0.5f) * scale_.x; // ドット分
		totalWidth += fractionalDigits.size() * spaceWidth_ * scale_.x;
	}
	if (showPercent && percentSprite_) {
		totalWidth += spaceWidth_ * scale_.x; // パーセント記号の分も追加
	}

	// 開始位置を決定
	Vector2 currentPos = position_;
	if (alignment_ == Alignment::Right) {
		// 右揃えの場合は左側にオフセット
		currentPos.x -= totalWidth;
	}

	// 桁インデックス
	int digitIndex = 0;

	// 整数部分を配置
	for (int digit : integerDigits) {
		if (digitIndex >= kMaxDigits) break;
		
		Digit d;
		d.digitIndex = digitIndex++;
		d.number = digit;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		d.isPercent = false;
		digits_.push_back(d);

		currentPos.x += spaceWidth_ * scale_.x;
	}

	// 小数点を配置（ドットスプライトがある場合）
	if (dotSprite_ && decimalPlaces > 0) {
		Digit d;
		d.digitIndex = 0;
		d.number = 0;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = true;
		d.isPercent = false;
		digits_.push_back(d);

		currentPos.x += (spaceWidth_ * 0.5f) * scale_.x; // ドットは小さいので間隔を狭める
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
		d.isPercent = false;
		digits_.push_back(d);

		currentPos.x += spaceWidth_ * scale_.x;
	}

	// パーセント記号を配置
	if (showPercent && percentSprite_) {
		Digit d;
		d.digitIndex = 0;
		d.number = 0;
		d.position = currentPos;
		d.isColon = false;
		d.isDot = false;
		d.isPercent = true;
		digits_.push_back(d);
	}
}

void BitmapFont::SetTime(float seconds) {
	digits_.clear();

	// 秒を分と秒に変換
	int totalSeconds = static_cast<int>(seconds);
	int minutes = totalSeconds / 60;
	int secs = totalSeconds % 60;

	// 全体の幅を計算
	int digitCount = 3; // 最低 M:SS (3桁 + コロン)
	if (minutes >= 10) {
		digitCount = 4; // MM:SS (4桁 + コロン)
	}
	float totalWidth = (digitCount + 1) * spaceWidth_ * scale_.x; // +1 はコロン分

	// 開始位置を決定
	Vector2 currentPos = position_;
	if (alignment_ == Alignment::Right) {
		// 右揃えの場合は左側にオフセット
		currentPos.x -= totalWidth;
	}

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
		d.isPercent = false;
		digits_.push_back(d);
		currentPos.x += spaceWidth_ * scale_.x;
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
		d.isPercent = false;
		digits_.push_back(d);
		currentPos.x += spaceWidth_ * scale_.x;
	}

	// コロン（常に表示）
	{
		Digit d;
		d.digitIndex = 0; // コロンは桁インデックス不要
		d.number = 0;
		d.position = currentPos;
		d.isColon = true;
		d.isDot = false;
		d.isPercent = false;
		digits_.push_back(d);
		currentPos.x += spaceWidth_ * scale_.x; // コロンも通常の間隔
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
		d.isPercent = false;
		digits_.push_back(d);
		currentPos.x += spaceWidth_ * scale_.x;
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
		d.isPercent = false;
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

void BitmapFont::DrawImGui(const char* name) {
#ifdef USE_IMGUI
	ImGui::Begin(name);

	// ファイル名入力
	static char fileNameBuffer[256] = "filePath";
	strncpy_s(fileNameBuffer, loadToSaveName_.c_str(), sizeof(fileNameBuffer));
	if (ImGui::InputText("ファイル名", fileNameBuffer, sizeof(fileNameBuffer))) {
		loadToSaveName_ = fileNameBuffer;
	}

	if (ImGui::Button("読み込み")) {
		LoadFromJson(loadToSaveName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("保存")) {
		SaveToJson(loadToSaveName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("新規作成")) {
		CreateNewJsonFile(loadToSaveName_);
	}

	ImGui::Separator();

	ImGui::DragFloat2("開始位置", &position_.x, 1.0f);
	ImGui::DragFloat2("スケール", &scale_.x, 0.01f);
	ImGui::DragFloat("表示幅", &spaceWidth_, 1.0f);
	ImGui::Checkbox("パーセント表示", &showPercent_);

	// 配置方向の選択
	const char* alignmentItems[] = { "左から右(右伸び)", "右から左(左伸び)" };
	int currentAlignment = static_cast<int>(alignment_);
	if (ImGui::Combo("配置方向", &currentAlignment, alignmentItems, IM_ARRAYSIZE(alignmentItems))) {
		alignment_ = static_cast<Alignment>(currentAlignment);
	}

	ImGui::DragFloat2("%位置", &percentOffset_.x, 1.0f);
	ImGui::DragFloat2("%スケール", &percentScale_.x, 0.01f);
	ImGui::ColorEdit4("色", &color_.x);

	ImGui::End();
#endif // USE_IMGUI
}

void BitmapFont::LoadJson(const std::string& filePath) {
	LoadFromJson(filePath);
}

void BitmapFont::SaveToJson(const std::string& filePath) {
	// ディレクトリが存在しない場合は作成
	std::filesystem::path fullPath = jsonManager_->GetBasePath() + filePath + ".json";
	std::filesystem::path directory = fullPath.parent_path();
	
	if (!directory.empty() && !std::filesystem::exists(directory)) {
		try {
			std::filesystem::create_directories(directory);
			printf("[INFO] Created directory: %s\n", directory.string().c_str());
		} catch (const std::filesystem::filesystem_error& e) {
			printf("[ERROR] Failed to create directory: %s\n", e.what());
			return;
		}
	}

	// BitmapFontの各フィールドをJsonManagerに登録
	jsonManager_->RegistOutput(position_, "position");
	jsonManager_->RegistOutput(scale_, "scale");
	jsonManager_->RegistOutput(spaceWidth_, "spaceWidth");
	jsonManager_->RegistOutput(color_, "color");
	jsonManager_->RegistOutput(showPercent_, "showPercent");
	jsonManager_->RegistOutput(static_cast<uint32_t>(alignment_), "alignment");
	jsonManager_->RegistOutput(percentOffset_, "percentOffset");
	jsonManager_->RegistOutput(percentScale_, "percentScale");

	// JSONファイルに書き込み
	jsonManager_->Write(filePath);
}

void BitmapFont::CreateNewJsonFile(const std::string& filePath) {
	// ディレクトリが存在しない場合は作成
	std::filesystem::path fullPath = jsonManager_->GetBasePath() + filePath + ".json";
	std::filesystem::path directory = fullPath.parent_path();
	
	if (!directory.empty() && !std::filesystem::exists(directory)) {
		try {
			std::filesystem::create_directories(directory);
			printf("[INFO] Created directory: %s\n", directory.string().c_str());
		} catch (const std::filesystem::filesystem_error& e) {
			printf("[ERROR] Failed to create directory: %s\n", e.what());
			return;
		}
	}

	// デフォルト値でJSONファイルを作成
	SaveToJson(filePath);
}

void BitmapFont::LoadFromJson(const std::string& filePath) {
	// JSONファイルから読み込み
	auto values = jsonManager_->Read(filePath);
	
	if (values.empty()) {
		printf("[WARNING] Failed to load JSON file: %s.json\n", filePath.c_str());
		return;
	}

	// 読み込んだ値を順番に取得して適用
	size_t index = 0;
	if (index < values.size()) {
		position_ = JsonManager::Reverse<Vector2>(values[index++]);
		scale_ = JsonManager::Reverse<Vector2>(values[index++]);
		spaceWidth_ = JsonManager::Reverse<float>(values[index++]);
		color_ = JsonManager::Reverse<Vector4>(values[index++]);
		showPercent_ = JsonManager::Reverse<bool>(values[index++]);
		alignment_ = static_cast<Alignment>(JsonManager::Reverse<uint32_t>(values[index++]));
		percentOffset_ = JsonManager::Reverse<Vector2>(values[index++]);
		percentScale_ = JsonManager::Reverse<Vector2>(values[index++]);
	}

	//loadToSaveName_ = filePath;
}