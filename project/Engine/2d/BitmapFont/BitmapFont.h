#pragma once
#include "2d/Sprite/Sprite.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Math/Type/Vector2.h"

#include <memory>
#include <vector>
#include <string>
#include <array>

/// <summary>
/// ビットマップフォントクラス（数字0-9を表示）
/// </summary>
class BitmapFont {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~BitmapFont() = default;

	/// <summary>
	/// ビットマップフォントの初期化
	/// </summary>
	/// <param name="dxCommon">DirectXCommonのポインタ</param>
	/// <param name="numberImageFolder">数字画像フォルダのパス（デフォルト: "number/"）</param>
	void Initialize(DirectXCommon* dxCommon, const std::string& numberImageFolder = "number/");

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 整数値を表示
	/// </summary>
	/// <param name="value">表示する整数値</param>
	/// <param name="position">表示位置（左端）</param>
	/// <param name="digitSpacing">数字間の間隔</param>
	void SetNumber(int value, const Vector2& position, float digitSpacing = 40.0f);

	/// <summary>
	/// 浮動小数点数を表示
	/// </summary>
	/// <param name="value">表示する浮動小数点数</param>
	/// <param name="position">表示位置（左端）</param>
	/// <param name="decimalPlaces">小数点以下の桁数</param>
	/// <param name="digitSpacing">数字間の間隔</param>
	void SetFloat(float value, const Vector2& position, int decimalPlaces = 2, float digitSpacing = 40.0f);

	/// <summary>
	/// 時間を表示（MM:SS形式）
	/// </summary>
	/// <param name="seconds">表示する秒数</param>
	/// <param name="position">表示位置（左端）</param>
	/// <param name="digitSpacing">数字間の間隔</param>
	void SetTime(float seconds, const Vector2& position, float digitSpacing = 40.0f);

	/// <summary>
	/// スケールを設定
	/// </summary>
	/// <param name="scale">スケール値</param>
	void SetScale(const Vector2& scale) { scale_ = scale; }

	/// <summary>
	/// 色を設定
	/// </summary>
	/// <param name="color">色（RGBA）</param>
	void SetColor(const Vector4& color) { color_ = color; }

	/// <summary>
	/// 数字のサイズを取得
	/// </summary>
	/// <returns>数字1つのサイズ</returns>
	Vector2 GetDigitSize() const;

private:
	static const int kMaxDigits = 10; // 最大桁数

	DirectXCommon* dxCommon_ = nullptr;

	// 各桁用のスプライト（最大桁数分）
	struct DigitSprite {
		std::array<std::unique_ptr<Sprite>, 10> sprites; // 0-9のスプライト
	};
	std::array<DigitSprite, kMaxDigits> digitSprites_;

	// コロンスプライト（時間表示用）
	std::unique_ptr<Sprite> colonSprite_;

	// ドットスプライト（小数点用）
	std::unique_ptr<Sprite> dotSprite_;

	// 表示する桁のリスト
	struct Digit {
		int digitIndex;      // 桁のインデックス（0が一番左）
		int number;          // 0-9の数字
		Vector2 position;    // 表示位置
		bool isColon;        // コロンかどうか
		bool isDot;          // ドットかどうか
	};
	std::vector<Digit> digits_;

	Vector2 scale_ = { 1.0f, 1.0f };
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	std::string numberImageFolder_;

	/// <summary>
	/// 整数を桁ごとに分解
	/// </summary>
	std::vector<int> SplitDigits(int value) const;
};
