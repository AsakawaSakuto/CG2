#pragma once
#include "2d/Sprite/Sprite.h"
#include "Math/Type/Vector2.h"
#include "Math/Type/Vector4.h"
#include "Utility/FileFormat/Json/JsonManager.h"

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// 前方宣言
class DirectXCommon;

/// <summary>
/// テキストレンダラークラス（ひらがな・カタカナ・アルファベットを表示）
/// </summary>
class TextRenderer {
public:
	/// <summary>
	/// テキストの配置方向
	/// </summary>
	enum class Alignment {
		Left,   // 左揃え
		Center, // 中央揃え
		Right   // 右揃え
	};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextRenderer() = default;

	/// <summary>
	/// テキストレンダラーの初期化
	/// </summary>
	/// <param name="hiraganaImage">ひらがな画像のパス（resources/image/からの相対パス）</param>
	/// <param name="katakanaImage">カタカナ画像のパス（resources/image/からの相対パス）</param>
	/// <param name="alphabetImage">アルファベット画像のパス（resources/image/からの相対パス）</param>
	void Initialize(
		const std::string& hiraganaImage = "font/hiragana.png",
		const std::string& katakanaImage = "font/katakana.png",
		const std::string& alphabetImage = "font/alphabet.png"
	);

	/// <summary>
	/// テキストを設定
	/// </summary>
	/// <param name="text">表示するテキスト（UTF-8）</param>
	void SetText(const std::string& text);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void DrawImGui(const char* name);

	/// <summary>
	/// 位置を設定
	/// </summary>
	/// <param name="position">表示位置</param>
	void SetPosition(const Vector2& position);

	/// <summary>
	/// スケールを設定
	/// </summary>
	/// <param name="scale">スケール値</param>
	void SetScale(const Vector2& scale);

	/// <summary>
	/// 色を設定
	/// </summary>
	/// <param name="color">色（RGBA）</param>
	void SetColor(const Vector4& color) { color_ = color; }

	/// <summary>
	/// 文字間隔を設定
	/// </summary>
	/// <param name="spacing">文字間隔（ピクセル）</param>
	void SetCharSpacing(float spacing);

	/// <summary>
	/// 行間を設定
	/// </summary>
	/// <param name="lineSpacing">行間（ピクセル）</param>
	void SetLineSpacing(float lineSpacing);

	/// <summary>
	/// 配置方向を設定
	/// </summary>
	/// <param name="alignment">配置方向</param>
	void SetAlignment(Alignment alignment) { alignment_ = alignment; UpdateCharacterPositions(); }

	/// <summary>
	/// 位置を取得
	/// </summary>
	const Vector2& GetPosition() const { return position_; }

	/// <summary>
	/// スケールを取得
	/// </summary>
	const Vector2& GetScale() const { return scale_; }

	/// <summary>
	/// テキストをクリア
	/// </summary>
	void Clear() { characters_.clear(); currentText_.clear(); }

private:
	/// <summary>
	/// 文字タイプ
	/// </summary>
	enum class CharType {
		Hiragana,
		Katakana,
		Alphabet,
		Unknown
	};

	/// <summary>
	/// 文字情報
	/// </summary>
	struct CharInfo {
		std::unique_ptr<Sprite> sprite;
		Vector2 position;
		CharType type;
		int index; // 文字のインデックス
	};

	/// <summary>
	/// 文字タイプを判定
	/// </summary>
	CharType GetCharType(char32_t codepoint) const;

	/// <summary>
	/// 文字のインデックスを取得
	/// </summary>
	int GetCharIndex(char32_t codepoint, CharType type) const;

	/// <summary>
	/// UTF-8文字列をUTF-32に変換
	/// </summary>
	std::vector<char32_t> ConvertToUTF32(const std::string& utf8) const;

	/// <summary>
	/// 文字マッピングを初期化
	/// </summary>
	void InitializeCharMapping();

	/// <summary>
	/// スプライトを生成して設定
	/// </summary>
	void SetupSprite(CharInfo& charInfo, char32_t codepoint);

	/// <summary>
	/// スプライトの頂点データを更新
	/// </summary>
	void UpdateSpriteVertices(Sprite* sprite, const Vector2& size);

	/// <summary>
	/// 文字の位置を再計算
	/// </summary>
	void UpdateCharacterPositions();

private:
	DirectXCommon* dxCommon_ = nullptr;

	// テクスチャパス
	std::string hiraganaImage_;
	std::string katakanaImage_;
	std::string alphabetImage_;

	// 文字サイズ（ピクセル）
	static constexpr float kHiraganaWidth = 128.0f;
	static constexpr float kHiraganaHeight = 150.0f;
	static constexpr float kKatakanaWidth = 128.0f;
	static constexpr float kKatakanaHeight = 150.0f;
	static constexpr float kAlphabetWidth = 64.0f;
	static constexpr float kAlphabetHeight = 128.0f;

	// 画像の列数（横方向の文字数）
	int hiraganaColumns_ = 17;
	int hiraganaRows_ = 5;
	int katakanaColumns_ = 17;
	int katakanaRows_ = 5;
	int alphabetColumns_ = 1;

	// 表示する文字のリスト
	std::vector<CharInfo> characters_;

	// 表示設定
	Vector2 position_ = { 0.0f, 0.0f };
	Vector2 scale_ = { 1.0f, 1.0f };
	float charSpacing_ = 0.0f;   // 文字間隔
	float lineSpacing_ = 0.0f;  // 行間
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	Alignment alignment_ = Alignment::Left;

	// 現在表示中のテキスト
	std::string currentText_;

	// ひらがなマッピング（Unicodeコードポイント -> インデックス）
	std::unordered_map<char32_t, int> hiraganaMap_;
	// カタカナマッピング
	std::unordered_map<char32_t, int> katakanaMap_;
	// アルファベットマッピング（小文字から）
	std::unordered_map<char32_t, int> alphabetMap_;

	// JSON管理
	std::unique_ptr<JsonManager> jsonManager_;
	std::string loadToSaveName_ = "filePath";
};
