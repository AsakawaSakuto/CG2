#pragma once
#include "2d/Sprite/Sprite.h"
#include "Math/Type/Vector2.h"
#include "Math/Type/Vector4.h"
#include "Utility/Easing/Easing.h"
#include "Utility/FileFormat/Json/JsonManager.h"
#include <memory>
#include <vector>

// 前方宣言
class DirectXCommon;

/// <summary>
/// ゲージの伸びる方向
/// </summary>
enum class GaugeDirection {
	Right,  // 左から右に伸びる
	Left,   // 右から左に伸びる
	Up,     // 下から上に伸びる
	Down    // 上から下に伸びる
};

/// <summary>
/// ゲージクラス（HPや経験値などのバー表示用）
/// white1x1.pngを使用してゲージを描画
/// </summary>
class Gauge {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Gauge() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Gauge() = default;

	/// <summary>
	/// ゲージの初期化
	/// </summary>
	/// <param name="Path">JSONパス</param>
	void Initialize(const std::string& Path = "temp");

	/// <summary>
	/// ゲージの更新
	/// </summary>
	void Update(float currenValue, float maxValue);
	void Update(int currenValue, int maxValue);

	/// <summary>
	/// ゲージの描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGuiでのデバッグ表示
	/// </summary>
	/// <param name="name">ImGuiウィンドウ名</param>
	void DrawImGui(const char* name);

	// === Setter ===

	/// <summary>
	/// ゲージの位置を設定
	/// </summary>
	void SetPosition(const Vector2& position);

	/// <summary>
	/// ゲージのサイズを設定
	/// </summary>
	void SetSize(const Vector2& size);

	/// <summary>
	/// 背景色を設定
	/// </summary>
	void SetBackgroundColor(const Vector4& color);

	/// <summary>
	/// ゲージの色を設定
	/// </summary>
	void SetGaugeColor(const Vector4& color);

	/// <summary>
	/// ゲージのグラデーション開始色を設定
	/// </summary>
	void SetGaugeStartColor(const Vector4& color);

	/// <summary>
	/// ゲージのグラデーション終了色を設定
	/// </summary>
	void SetGaugeEndColor(const Vector4& color);

	/// <summary>
	/// グラデーションを有効/無効にする
	/// </summary>
	void SetGradientEnabled(bool enabled);

	/// <summary>
	/// グラデーション分割数を設定
	/// </summary>
	void SetGradientSegments(int segments);

	/// <summary>
	/// ゲージの伸びる方向を設定
	/// </summary>
	void SetDirection(GaugeDirection direction);

	// === Getter ===

	/// <summary>
	/// ゲージの割合を取得（0.0f ～ 1.0f）
	/// </summary>
	float GetRatio() const;

	/// <summary>
	/// ゲージの伸びる方向を取得
	/// </summary>
	GaugeDirection GetDirection() const { return direction_; }

	/// <summary>
	/// グラデーションが有効かどうかを取得
	/// </summary>
	bool IsGradientEnabled() const { return useGradient_; }

private:
	DirectXCommon* dxCommon_ = nullptr;

	// ゲージパーツ
	std::unique_ptr<Sprite> background_;  // 背景
	std::unique_ptr<Sprite> gauge_;       // ゲージ本体（単色用）
	std::vector<std::unique_ptr<Sprite>> gradientSegments_; // グラデーション用のセグメント

	// ゲージのパラメータ
	Vector2 position_ = { 0.0f, 0.0f };  // 位置（左上基準）
	Vector2 size_ = { 0.0f, 0.0f };      // サイズ（幅、高さ）
	float currentValue_ = 100.0f;        // 現在値
	float maxValue_ = 100.0f;            // 最大値

	// 色設定
	Vector4 backgroundColor_ = { 0.2f, 0.2f, 0.2f, 1.0f }; // 背景色
	Vector4 gaugeColor_ = { 0.0f, 1.0f, 0.0f, 1.0f };      // ゲージ色（単色用）
	
	// グラデーション設定
	bool useGradient_ = false;                              // グラデーションを使用するか
	Vector4 gaugeStartColor_ = { 0.0f, 1.0f, 0.0f, 1.0f };  // グラデーション開始色
	Vector4 gaugeEndColor_ = { 1.0f, 0.0f, 0.0f, 1.0f };    // グラデーション終了色
	int numGradientSegments_ = 100;                         // グラデーション分割数
	int maxGradientSegments_ = 100;                         // グラデーション分割数の上限

	// 背景を描画するかどうか
	bool drawBackground_ = true; 

	// ゲージの伸びる方向
	GaugeDirection direction_ = GaugeDirection::Right;

	// 内部ヘルパー関数
	void InitializeGradientSegments();

	/// <summary>
	/// JsonFileに設定を保存
	/// </summary>
	/// <param name="filePath">保存先のファイルパス（拡張子不要）</param>
	void SaveToJson(const std::string& filePath);

	/// <summary>
	/// JsonFileから設定を読み込み
	/// </summary>
	/// <param name="filePath">読み込み元のファイルパス（拡張子不要）</param>
	void LoadFromJson(const std::string& filePath);

	std::unique_ptr<JsonManager> jsonManager_;
	std::string loadToSaveName_ = "filePath";
};
