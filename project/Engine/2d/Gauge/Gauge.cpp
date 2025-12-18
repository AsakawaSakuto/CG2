#include "Gauge.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include <algorithm>

#ifdef USE_IMGUI
#include "imgui.h"
#endif

void Gauge::Initialize(const std::string& Path) {
	// ServiceLocatorからDirectXCommonを取得
	dxCommon_ = ServiceLocator::GetDXCommon();
	
	if (!dxCommon_) {
		// エラー処理：DirectXCommonが登録されていない場合
		throw std::runtime_error("DirectXCommon is not registered in ServiceLocator. Call ServiceLocator::Provide(dxCommon) first.");
	}

	// JsonManagerの初期化（初回のみ）
	if (!jsonManager_) {
		jsonManager_ = std::make_unique<JsonManager>();
		jsonManager_->SetBasePath("resources/Data/Json/Gauge/");
		LoadFromJson(Path);
	}

	// 背景スプライトの初期化
	background_ = std::make_unique<Sprite>();
	background_->Initialize("white1x1.png");
	background_->SetColor(backgroundColor_);
	background_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	// ゲージスプライトの初期化（単色用）
	gauge_ = std::make_unique<Sprite>();
	gauge_->Initialize("white1x1.png");
	gauge_->SetColor(gaugeColor_);
	gauge_->SetAnchorPoint({ 0.0f, 0.0f }); // 左上基準

	// グラデーションセグメントの初期化
	InitializeGradientSegments();
}

void Gauge::Update(float currenValue, float maxValue) {

	currentValue_ = currenValue;
	maxValue_ = maxValue;

	// 値の範囲チェック
	currentValue_ = std::clamp(currentValue_, 0.0f, maxValue_);

	// 背景の位置とサイズを設定（左上基準）
	background_->SetPosition(position_);
	background_->SetScale(size_);

	// ゲージの位置とサイズを設定（方向に応じて計算）
	float ratio = std::clamp(GetRatio(), 0.0f, 1.0f);

	if (useGradient_) {
		// グラデーション描画の更新
		for (size_t i = 0; i < gradientSegments_.size(); ++i) {
			float segmentRatio = static_cast<float>(i) / static_cast<float>(numGradientSegments_);
			float nextSegmentRatio = static_cast<float>(i + 1) / static_cast<float>(numGradientSegments_);
			
			// このセグメントが表示範囲内かチェック
			if (segmentRatio >= ratio) {
				break; // これ以降のセグメントは非表示
			}

			Vector2 segmentPosition = position_;
			Vector2 segmentSize = size_;
			
			switch (direction_) {
			case GaugeDirection::Right:
				segmentSize.x = size_.x / static_cast<float>(numGradientSegments_);
				segmentPosition.x = position_.x + (size_.x * 2.0f) * segmentRatio;
				// 最後のセグメントは部分的に表示
				if (nextSegmentRatio > ratio) {
					float partialRatio = (ratio - segmentRatio) / (nextSegmentRatio - segmentRatio);
					segmentSize.x *= partialRatio;
				}
				break;

			case GaugeDirection::Left:
				segmentSize.x = size_.x / static_cast<float>(numGradientSegments_);
				segmentPosition.x = position_.x + (size_.x * 2.0f) * (1.0f - nextSegmentRatio);
				if (nextSegmentRatio > ratio) {
					float partialRatio = (ratio - segmentRatio) / (nextSegmentRatio - segmentRatio);
					segmentSize.x *= partialRatio;
					segmentPosition.x = position_.x + (size_.x * 2.0f) * (1.0f - ratio);
				}
				break;

			case GaugeDirection::Down:
				segmentSize.y = size_.y / static_cast<float>(numGradientSegments_);
				segmentPosition.y = position_.y + (size_.y * 2.0f) * segmentRatio;
				if (nextSegmentRatio > ratio) {
					float partialRatio = (ratio - segmentRatio) / (nextSegmentRatio - segmentRatio);
					segmentSize.y *= partialRatio;
				}
				break;

			case GaugeDirection::Up:
				segmentSize.y = size_.y / static_cast<float>(numGradientSegments_);
				segmentPosition.y = position_.y + (size_.y * 2.0f) * (1.0f - nextSegmentRatio);
				if (nextSegmentRatio > ratio) {
					float partialRatio = (ratio - segmentRatio) / (nextSegmentRatio - segmentRatio);
					segmentSize.y *= partialRatio;
					segmentPosition.y = position_.y + (size_.y * 2.0f) * (1.0f - ratio);
				}
				break;
			}

			gradientSegments_[i]->SetPosition(segmentPosition);
			gradientSegments_[i]->SetScale(segmentSize);
			
			// セグメントの色を計算（グラデーション）
			float colorT = (numGradientSegments_ > 1) ? static_cast<float>(i) / static_cast<float>(numGradientSegments_ - 1) : 0.0f;
			Vector4 segmentColor = Easing::Lerp(gaugeStartColor_, gaugeEndColor_, colorT);
			gradientSegments_[i]->SetColor(segmentColor);
			
			gradientSegments_[i]->Update();
		}
	} else {
		// 単色描画の更新
		Vector2 gaugePosition = position_;
		Vector2 gaugeSize = size_;

		switch (direction_) {
		case GaugeDirection::Right:
			gaugeSize.x = size_.x * ratio;
			break;

		case GaugeDirection::Left:
			gaugeSize.x = size_.x * ratio;
			gaugePosition.x = position_.x + (size_.x * 2.0f) * (1.0f - ratio);
			break;

		case GaugeDirection::Down:
			gaugeSize.y = size_.y * ratio;
			break;

		case GaugeDirection::Up:
			gaugeSize.y = size_.y * ratio;
			gaugePosition.y = position_.y + (size_.y * 2.0f) * (1.0f - ratio);
			break;
		}

		gauge_->SetPosition(gaugePosition);
		gauge_->SetScale(gaugeSize);
		gauge_->SetColor(gaugeColor_);
		gauge_->Update();
	}

	// 色の設定
	background_->SetColor(backgroundColor_);
	// 背景の更新
	background_->Update();
}

void Gauge::Update(int currenValue, int maxValue) {
	Update(static_cast<float>(currenValue), static_cast<float>(maxValue));
}

void Gauge::Draw() {
	// 背景を描画
	if (drawBackground_) {
		background_->Draw();
	}

	// ゲージを描画（現在値が0より大きい場合のみ）
	if (currentValue_ > 0.0f) {
		if (useGradient_) {
			// グラデーションセグメントを描画
			float ratio = std::clamp(GetRatio(), 0.0f, 1.0f);
			for (size_t i = 0; i < gradientSegments_.size(); ++i) {
				float segmentRatio = static_cast<float>(i) / static_cast<float>(numGradientSegments_);
				if (segmentRatio >= ratio) {
					break;
				}
				gradientSegments_[i]->Draw();
			}
		} else {
			// 単色ゲージを描画
			gauge_->Draw();
		}
	}
}

void Gauge::DrawImGui(const char* name) {
#ifdef USE_IMGUI
	ImGui::Begin(name);

	// JSON保存・読み込みセクション
	ImGui::Separator();
	ImGui::Text("JSON Settings");

	// ファイル名入力
	char fileNameBuffer[256];
	strncpy_s(fileNameBuffer, loadToSaveName_.c_str(), sizeof(fileNameBuffer) - 1);
	if (ImGui::InputText("File Name", fileNameBuffer, sizeof(fileNameBuffer))) {
		loadToSaveName_ = fileNameBuffer;
	}

	// 保存・読み込みボタン
	if (ImGui::Button("Save JSON", ImVec2(150, 0))) {
		if (!loadToSaveName_.empty()) {
			SaveToJson(loadToSaveName_);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Load JSON", ImVec2(150, 0))) {
		if (!loadToSaveName_.empty()) {
			LoadFromJson(loadToSaveName_);
		}
	}

	ImGui::Separator();

	ImGui::Text("Gauge: %.1f / %.1f (%.1f%%)", currentValue_, maxValue_, GetRatio() * 100.0f);
	
	ImGui::DragFloat2("Position", &position_.x, 1.0f);
	ImGui::DragFloat2("Size", &size_.x, 1.0f, 10.0f, 1000.0f);

	ImGui::Separator();
	
	// 方向の選択
	const char* directionNames[] = { "Right", "Left", "Up", "Down" };
	int currentDirection = static_cast<int>(direction_);
	if (ImGui::Combo("Direction", &currentDirection, directionNames, 4)) {
		direction_ = static_cast<GaugeDirection>(currentDirection);
	}

	ImGui::Separator();
	ImGui::Checkbox("Draw Background", &drawBackground_);
	ImGui::ColorEdit4("Background Color", &backgroundColor_.x);
	
	// グラデーション設定
	if (ImGui::Checkbox("Use Gradient", &useGradient_)) {
		if (useGradient_) {
			InitializeGradientSegments();
		}
	}
	
	if (useGradient_) {
		ImGui::ColorEdit4("Start Color", &gaugeStartColor_.x);
		ImGui::ColorEdit4("End Color", &gaugeEndColor_.x);
		
		ImGui::Text("GradientSegments : %d", maxGradientSegments_);
		ImGui::DragInt("Num Segments", &numGradientSegments_, 1, 2, maxGradientSegments_);
	} else {
		ImGui::ColorEdit4("Gauge Color", &gaugeColor_.x);
	}

	ImGui::End();
#endif
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

void Gauge::SetGaugeStartColor(const Vector4& color) {
	gaugeStartColor_ = color;
}

void Gauge::SetGaugeEndColor(const Vector4& color) {
	gaugeEndColor_ = color;
}

void Gauge::SetGradientEnabled(bool enabled) {
	useGradient_ = enabled;
	if (enabled && dxCommon_) {
		InitializeGradientSegments();
	}
}

void Gauge::SetGradientSegments(int segments) {
	numGradientSegments_ = std::clamp(segments, 2, maxGradientSegments_);
	if (useGradient_ && dxCommon_) {
		InitializeGradientSegments();
	}
}

void Gauge::SetDirection(GaugeDirection direction) {
	direction_ = direction;
}

float Gauge::GetRatio() const {
	if (maxValue_ <= 0.0f) {
		return 0.0f;
	}
	return currentValue_ / maxValue_;
}

void Gauge::InitializeGradientSegments() {
	numGradientSegments_ = std::clamp(numGradientSegments_, 2, (std::max)(int(size_.x), int(size_.y)));
	maxGradientSegments_ = numGradientSegments_;
	gradientSegments_.clear();
	gradientSegments_.resize(maxGradientSegments_);
	
	for (int i = 0; i < maxGradientSegments_; ++i) {
		gradientSegments_[i] = std::make_unique<Sprite>();
		gradientSegments_[i]->Initialize("white1x1.png");
		gradientSegments_[i]->SetAnchorPoint({ 0.0f, 0.0f });
	}
}

void Gauge::SaveToJson(const std::string& filePath) {
	// 各パラメータをJsonManagerに登録
	jsonManager_->RegistOutput(position_, "position");
	jsonManager_->RegistOutput(size_, "size");
	jsonManager_->RegistOutput(backgroundColor_, "backgroundColor");
	jsonManager_->RegistOutput(gaugeColor_, "gaugeColor");
	jsonManager_->RegistOutput(useGradient_, "useGradient");
	jsonManager_->RegistOutput(gaugeStartColor_, "gaugeStartColor");
	jsonManager_->RegistOutput(gaugeEndColor_, "gaugeEndColor");
	jsonManager_->RegistOutput(numGradientSegments_, "numGradientSegments");
	jsonManager_->RegistOutput(drawBackground_, "drawBackground");
	jsonManager_->RegistOutput(static_cast<uint32_t>(direction_), "direction");
	
	// JSONファイルに書き込み
	jsonManager_->Write(filePath);
}

void Gauge::LoadFromJson(const std::string& filePath) {
	// JSONファイルから読み込み
	auto values = jsonManager_->Read(filePath);
	
	if (values.empty()) {
		return;
	}

	// 読み込んだ値を順番に取得して適用
	size_t index = 0;
	if (index < values.size()) position_ = JsonManager::Reverse<Vector2>(values[index++]);
	if (index < values.size()) size_ = JsonManager::Reverse<Vector2>(values[index++]);
	if (index < values.size()) backgroundColor_ = JsonManager::Reverse<Vector4>(values[index++]);
	if (index < values.size()) gaugeColor_ = JsonManager::Reverse<Vector4>(values[index++]);
	if (index < values.size()) useGradient_ = JsonManager::Reverse<bool>(values[index++]);
	if (index < values.size()) gaugeStartColor_ = JsonManager::Reverse<Vector4>(values[index++]);
	if (index < values.size()) gaugeEndColor_ = JsonManager::Reverse<Vector4>(values[index++]);
	if (index < values.size()) numGradientSegments_ = JsonManager::Reverse<int>(values[index++]);
	if (index < values.size()) drawBackground_ = JsonManager::Reverse<bool>(values[index++]);
	if (index < values.size()) direction_ = static_cast<GaugeDirection>(JsonManager::Reverse<uint32_t>(values[index++]));
	
	// グラデーションが有効な場合、セグメントを再初期化
	if (useGradient_ && dxCommon_) {
		InitializeGradientSegments();
	}
	
	//loadToSaveName_ = filePath;
}