#pragma once
#include "Engine/2d/Sprite/Sprite.h"
#include "Engine/System/Utility/Math/Vector2.h"
#include "Engine/System/Utility/Math/Vector4.h"
#include <memory>

/// <summary>
/// ゲージクラス（HPや経験値などのバー表示用）
/// white2x2.pngを使用してゲージを描画
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
	/// <param name="dxCommon">DirectXCommonへのポインタ</param>
	/// <param name="position">ゲージの位置（左上基準）</param>
	/// <param name="size">ゲージの最大サイズ（幅、高さ）</param>
	/// <param name="maxValue">ゲージの最大値</param>
	void Initialize(DirectXCommon* dxCommon, const Vector2& position, const Vector2& size, float maxValue = 100.0f);

	/// <summary>
	/// ゲージの更新
	/// </summary>
	void Update();

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
	/// 現在値を設定（0 ～ maxValue の範囲）
	/// </summary>
	void SetCurrentValue(float value);

	/// <summary>
	/// 最大値を設定
	/// </summary>
	void SetMaxValue(float maxValue);

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
	/// 枠を表示するかどうか
	/// </summary>
	void SetShowBorder(bool show);

	/// <summary>
	/// 枠の色を設定
	/// </summary>
	void SetBorderColor(const Vector4& color);

	/// <summary>
	/// 枠の太さを設定
	/// </summary>
	void SetBorderThickness(float thickness);

	// === Getter ===

	/// <summary>
	/// 現在値を取得
	/// </summary>
	float GetCurrentValue() const { return currentValue_; }

	/// <summary>
	/// 最大値を取得
	/// </summary>
	float GetMaxValue() const { return maxValue_; }

	/// <summary>
	/// ゲージの割合を取得（0.0f ～ 1.0f）
	/// </summary>
	float GetRatio() const;

private:
	/// <summary>
	/// スプライトの位置とスケールを更新
	/// </summary>
	void UpdateSprites();

private:
	DirectXCommon* dxCommon_ = nullptr;

	// ゲージパーツ
	std::unique_ptr<Sprite> background_;  // 背景
	std::unique_ptr<Sprite> gauge_;       // ゲージ本体
	std::unique_ptr<Sprite> borderTop_;   // 枠（上）
	std::unique_ptr<Sprite> borderBottom_;// 枠（下）
	std::unique_ptr<Sprite> borderLeft_;  // 枠（左）
	std::unique_ptr<Sprite> borderRight_; // 枠（右）

	// ゲージのパラメータ
	Vector2 position_ = { 0.0f, 0.0f };  // 位置（左上基準）
	Vector2 size_ = { 200.0f, 20.0f };   // サイズ（幅、高さ）
	float currentValue_ = 100.0f;        // 現在値
	float maxValue_ = 100.0f;            // 最大値

	// 色設定
	Vector4 backgroundColor_ = { 0.2f, 0.2f, 0.2f, 1.0f }; // 背景色（暗いグレー）
	Vector4 gaugeColor_ = { 0.0f, 1.0f, 0.0f, 1.0f };      // ゲージ色（緑）
	Vector4 borderColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };     // 枠色（白）

	// 枠の設定
	bool showBorder_ = false;     // 枠を表示するか
	float borderThickness_ = 2.0f; // 枠の太さ
};
