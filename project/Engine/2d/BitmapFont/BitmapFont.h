#pragma once
#include "2d/Sprite/Sprite.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Math/Type/Vector2.h"
#include "Utility/Binary/BinaryManager.h"

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
	/// 数字の配置方向
	/// </summary>
	enum class Alignment {
		Left,   // 左から右へ（右に伸びる）
		Right   // 右から左へ（左に伸びる）
	};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BitmapFont() = default;

	/// <summary>
	/// ビットマップフォントの初期化
	/// </summary>
	/// <param name="dxCommon">DirectXCommonのポインタ</param>
	/// <param name="numberImageFolder">数字画像フォルダのパス（デフォルト: "number/"）</param>
	void Initialize(DirectXCommon* dxCommon, const std::string& binaryPath = "temp", const std::string& numberImageFolder = "BitmapFont/");

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void DrawImGui(const char* name);

	/// <summary>
	/// BinaryFileからBitmapFontの設定を読み込む
	/// </summary>
	/// <param name="filePath">Resources->Binary->BitmapFont の中にあるBinaryFileのPathを入れる（拡張子不要）</param>
	void LoadBinary(const std::string& filePath);

	/// <summary>
	/// 整数値を表示
	/// </summary>
	/// <param name="value">表示する整数値</param>
	/// <param name="position">表示位置（左端）</param>
	/// <param name="digitSpacing">数字間の間隔</param>
	/// <param name="showPercent">パーセント記号を表示するかどうか</param>
	void SetNumber(int value);

	/// <summary>
	/// 浮動小数点数を表示
	/// </summary>
	/// <param name="value">表示する浮動小数点数</param>
	/// <param name="position">表示位置（左端）</param>
	/// <param name="decimalPlaces">小数点以下の桁数</param>
	/// <param name="digitSpacing">数字間の間隔</param>
	/// <param name="showPercent">パーセント記号を表示するかどうか</param>
	void SetFloat(float value, int decimalPlaces = 2, bool showPercent = false);

	/// <summary>
	/// 時間を表示（MM:SS形式）
	/// </summary>
	/// <param name="seconds">表示する秒数</param>
	/// <param name="position">表示位置（左端）</param>
	/// <param name="digitSpacing">数字間の間隔</param>
	void SetTime(float seconds);

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
	/// 配置方向を設定
	/// </summary>
	/// <param name="alignment">配置方向</param>
	void SetAlignment(Alignment alignment) { alignment_ = alignment; }

	/// <summary>
	/// 数字のサイズを取得
	/// </summary>
	/// <returns>数字1つのサイズ</returns>
	Vector2 GetDigitSize() const;

	/// <summary>
	/// パーセント記号の表示を設定
	/// </summary>
	/// <param name="show">表示するかどうか</param>
	void SetShowPercent(bool show) { showPercent_ = show; }

private:

	/// <summary>
	/// BinaryFileに設定を保存
	/// </summary>
	/// <param name="filePath">保存先のファイルパス（拡張子不要）</param>
	void SaveToBinary(const std::string& filePath);

	/// <summary>
	/// BinaryFileから設定を読み込み
	/// </summary>
	/// <param name="filePath">読み込み元のファイルパス（拡張子不要）</param>
	void LoadFromBinary(const std::string& filePath);

	/// <summary>
	/// 新規BinaryFileを作成
	/// </summary>
	/// <param name="filePath">作成するファイルパス（拡張子不要）</param>
	void CreateNewBinaryFile(const std::string& filePath);

	/// <summary>
	/// 整数を桁ごとに分解
	/// </summary>
	std::vector<int> SplitDigits(int value) const;

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

	// パーセントスプライト（パーセント記号用）
	std::unique_ptr<Sprite> percentSprite_;

	// 表示する桁のリスト
	struct Digit {
		int digitIndex;      // 桁のインデックス（0が一番左）
		int number;          // 0-9の数字
		Vector2 position;    // 表示位置
		bool isColon;        // コロンかどうか
		bool isDot;          // ドットかどうか
		bool isPercent;      // パーセント記号かどうか
	};
	std::vector<Digit> digits_;

	// 表示設定
	Vector2 position_ = { 0.0f, 0.0f };
	Vector2 scale_ = { 1.0f, 1.0f };
	float spaceWidth_ = 20.0f;
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool showPercent_ = false;
	Alignment alignment_ = Alignment::Left;

	// パーセント記号専用の設定
	Vector2 percentOffset_ = { 0.0f, 0.0f };  // パーセント記号の位置オフセット
	Vector2 percentScale_ = { 1.0f, 1.0f };   // パーセント記号専用のスケール

	std::string numberImageFolder_;

	std::unique_ptr<BinaryManager> binaryManager_;
	std::string loadToSaveName_ = "filePath";
};
