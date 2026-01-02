#include "TextRenderer.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include "Core/TextureManager/TextureManager.h"

#include <algorithm>
#include <filesystem>

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

void TextRenderer::Initialize(
	const std::string& hiraganaImage,
	const std::string& katakanaImage,
	const std::string& alphabetImage
) {
	// ServiceLocatorからDirectXCommonを取得
	dxCommon_ = ServiceLocator::GetDXCommon();
	
	if (!dxCommon_) {
		throw std::runtime_error("DirectXCommon is not registered in ServiceLocator.");
	}

	hiraganaImage_ = hiraganaImage;
	katakanaImage_ = katakanaImage;
	alphabetImage_ = alphabetImage;

	// テクスチャを事前ロード
	std::string hiraganaPath = "resources/image/" + hiraganaImage_;
	std::string katakanaPath = "resources/image/" + katakanaImage_;
	std::string alphabetPath = "resources/image/" + alphabetImage_;

	TextureManager::GetInstance()->LoadTexture(hiraganaPath);
	TextureManager::GetInstance()->LoadTexture(katakanaPath);
	TextureManager::GetInstance()->LoadTexture(alphabetPath);

	// テクスチャサイズから列数を計算
	//Vector2 hiraganaTexSize = TextureManager::GetInstance()->GetTextureSize(hiraganaPath);
	//Vector2 katakanaTexSize = TextureManager::GetInstance()->GetTextureSize(katakanaPath);
	//Vector2 alphabetTexSize = TextureManager::GetInstance()->GetTextureSize(alphabetPath);

	Vector2 hiraganaTexSize = { 128.0f * 5.0f,128.0f * 17.0f };
	Vector2 katakanaTexSize = { 128.0f * 5.0f,128.0f * 17.0f };
	Vector2 alphabetTexSize = { 64.0f * 62.0f,0.0f };

	hiraganaColumns_ = static_cast<int>(hiraganaTexSize.x / kHiraganaWidth);
	katakanaColumns_ = static_cast<int>(katakanaTexSize.x / kKatakanaWidth);
	alphabetColumns_ = static_cast<int>(alphabetTexSize.x / kAlphabetWidth);

	// 0除算防止
	if (hiraganaColumns_ <= 0) hiraganaColumns_ = 1;
	if (katakanaColumns_ <= 0) katakanaColumns_ = 1;
	if (alphabetColumns_ <= 0) alphabetColumns_ = 1;

	// 文字マッピングを初期化
	InitializeCharMapping();

	// JsonManagerの初期化
	if (!jsonManager_) {
		jsonManager_ = std::make_unique<JsonManager>();
		jsonManager_->SetBasePath("resources/Data/Json/TextRenderer/");
	}
}

void TextRenderer::InitializeCharMapping() {
	// ひらがなマッピング（画像の実際の配置順に合わせる）
	// 行順: あいうえお かきくけこ さしすせそ...
	const char32_t hiraganaChars[] = {
		// あ行 (0-4)
		U'あ', U'い', U'う', U'え', U'お',
		// か行 (5-9)
		U'か', U'き', U'く', U'け', U'こ',
		// さ行 (10-14)
		U'さ', U'し', U'す', U'せ', U'そ',
		// た行 (15-19)
		U'た', U'ち', U'つ', U'て', U'と',
		// な行 (20-24)
		U'な', U'に', U'ぬ', U'ね', U'の',
		// は行 (25-29)
		U'は', U'ひ', U'ふ', U'へ', U'ほ',
		// ま行 (30-34)
		U'ま', U'み', U'む', U'め', U'も',
		// や行 (35-37)
		U'や', U'ゆ', U'よ',
		// ら行 (38-42)
		U'ら', U'り', U'る', U'れ', U'ろ',
		// わ行 (43-45)
		U'わ', U'を', U'ん',
		// が行 (46-50)
		U'が', U'ぎ', U'ぐ', U'げ', U'ご',
		// ざ行 (51-55)
		U'ざ', U'じ', U'ず', U'ぜ', U'ぞ',
		// だ行 (56-60)
		U'だ', U'ぢ', U'づ', U'で', U'ど',
		// ば行 (61-65)
		U'ば', U'び', U'ぶ', U'べ', U'ぼ',
		// ぱ行 (66-70)
		U'ぱ', U'ぴ', U'ぷ', U'ぺ', U'ぽ',
		// 小文字 (71-75)
		U'ぁ', U'ぃ', U'ぅ', U'ぇ', U'ぉ',
		// 小文字や行とっ (76-78)
		U'ゃ', U'ゅ', U'ょ', U'っ'
	};
	
	for (size_t i = 0; i < sizeof(hiraganaChars) / sizeof(hiraganaChars[0]); ++i) {
		hiraganaMap_[hiraganaChars[i]] = static_cast<int>(i);
	}

	// カタカナマッピング（ひらがなと同じ配置）
	const char32_t katakanaChars[] = {
		// ア行 (0-4)
		U'ア', U'イ', U'ウ', U'エ', U'オ',
		// カ行 (5-9)
		U'カ', U'キ', U'ク', U'ケ', U'コ',
		// サ行 (10-14)
		U'サ', U'シ', U'ス', U'セ', U'ソ',
		// タ行 (15-19)
		U'タ', U'チ', U'ツ', U'テ', U'ト',
		// ナ行 (20-24)
		U'ナ', U'ニ', U'ヌ', U'ネ', U'ノ',
		// ハ行 (25-29)
		U'ハ', U'ヒ', U'フ', U'ヘ', U'ホ',
		// マ行 (30-34)
		U'マ', U'ミ', U'ム', U'メ', U'モ',
		// ヤ行 (35-37)
		U'ヤ', U'ユ', U'ヨ',
		// ラ行 (38-42)
		U'ラ', U'リ', U'ル', U'レ', U'ロ',
		// ワ行 (43-45)
		U'ワ', U'ヲ', U'ン',
		// ガ行 (46-50)
		U'ガ', U'ギ', U'グ', U'ゲ', U'ゴ',
		// ザ行 (51-55)
		U'ザ', U'ジ', U'ズ', U'ゼ', U'ゾ',
		// ダ行 (56-60)
		U'ダ', U'ヂ', U'ヅ', U'デ', U'ド',
		// バ行 (61-65)
		U'バ', U'ビ', U'ブ', U'ベ', U'ボ',
		// パ行 (66-70)
		U'パ', U'ピ', U'プ', U'ペ', U'ポ',
		// 小文字 (71-75)
		U'ァ', U'ィ', U'ゥ', U'ェ', U'ォ',
		// 小文字ヤ行とッ (76-78)
		U'ャ', U'ュ', U'ョ', U'ッ',
		// 長音 (79)
		U'ー'
	};
	
	for (size_t i = 0; i < sizeof(katakanaChars) / sizeof(katakanaChars[0]); ++i) {
		katakanaMap_[katakanaChars[i]] = static_cast<int>(i);
	}

	// アルファベットマッピング（小文字から）
	// a-z: インデックス 0-25
	for (char32_t c = U'a'; c <= U'z'; ++c) {
		alphabetMap_[c] = static_cast<int>(c - U'a');
	}
	// A-Z: インデックス 26-51
	for (char32_t c = U'A'; c <= U'Z'; ++c) {
		alphabetMap_[c] = static_cast<int>(c - U'A' + 26);
	}
	// 数字 0-9: インデックス 52-61
	for (char32_t c = U'0'; c <= U'9'; ++c) {
		alphabetMap_[c] = static_cast<int>(c - U'0' + 52);
	}
}

void TextRenderer::SetText(const std::string& text) {
	if (currentText_ == text) {
		return;
	}
	
	currentText_ = text;

	// UTF-8からUTF-32に変換
	std::vector<char32_t> codepoints = ConvertToUTF32(text);

	// 最大文字数を制限（パフォーマンス対策）
	const size_t kMaxCharacters = 256;

	size_t charIndex = 0;

	for (char32_t cp : codepoints) {
		if (charIndex >= kMaxCharacters) break;

		// 改行とスペースはスキップ（位置計算は後で行う）
		if (cp == U'\n' || cp == U' ' || cp == U'　') {
			continue;
		}

		CharType type = GetCharType(cp);
		if (type == CharType::Unknown) {
			continue;
		}

		int index = GetCharIndex(cp, type);
		if (index < 0) {
			continue;
		}

		// スプライトを再利用または新規作成
		if (charIndex >= characters_.size()) {
			CharInfo info;
			info.sprite = std::make_unique<Sprite>();
			characters_.push_back(std::move(info));
		}

		CharInfo& charInfo = characters_[charIndex];
		charInfo.type = type;
		charInfo.index = index;

		// スプライトを設定（位置は後で設定)
		SetupSprite(charInfo, cp);

		charIndex++;
	}

	// 使用されなくなったスプライトを削除
	if (charIndex < characters_.size()) {
		characters_.resize(charIndex);
	}

	// 文字位置を再計算
	UpdateCharacterPositions();
}

void TextRenderer::UpdateCharacterPositions() {
	// UTF-32に変換
	std::vector<char32_t> codepoints = ConvertToUTF32(currentText_);

	// 現在の描画位置
	Vector2 currentPos = position_;
	float lineStartX = position_.x;
	
	size_t charIndex = 0;

	for (char32_t cp : codepoints) {
		// 改行処理
		if (cp == U'\n') {
			currentPos.x = lineStartX;
			currentPos.y += (kHiraganaHeight * scale_.y + lineSpacing_);
			continue;
		}

		// スペース処理
		if (cp == U' ' || cp == U'　') {
			float spaceWidth = (cp == U'　') ? kHiraganaWidth : kAlphabetWidth;
			currentPos.x += (spaceWidth * scale_.x + charSpacing_);
			continue;
		}

		CharType type = GetCharType(cp);
		if (type == CharType::Unknown) {
			continue;
		}

		int index = GetCharIndex(cp, type);
		if (index < 0) {
			continue;
		}

		if (charIndex >= characters_.size()) {
			break;
		}

		// 文字位置を更新
		characters_[charIndex].position = currentPos;

		// 文字幅を取得して次の位置を計算
		float charWidth = 0.0f;
		switch (type) {
		case CharType::Hiragana:
			charWidth = kHiraganaWidth;
			break;
		case CharType::Katakana:
			charWidth = kKatakanaWidth;
			break;
		case CharType::Alphabet:
			charWidth = kAlphabetWidth;
			break;
		default:
			break;
		}

		currentPos.x += (charWidth * scale_.x + charSpacing_);
		charIndex++;
	}
}

void TextRenderer::SetPosition(const Vector2& position) {
	position_ = position;
	UpdateCharacterPositions();
}

void TextRenderer::SetScale(const Vector2& scale) {
	scale_ = scale;
	UpdateCharacterPositions();
}

void TextRenderer::SetCharSpacing(float spacing) {
	charSpacing_ = spacing;
	UpdateCharacterPositions();
}

void TextRenderer::SetLineSpacing(float lineSpacing) {
	lineSpacing_ = lineSpacing;
	UpdateCharacterPositions();
}

void TextRenderer::SetupSprite(CharInfo& charInfo, char32_t codepoint) {
	std::string imagePath;
	float charWidth = 0.0f;
	float charHeight = 0.0f;
	int columns = 0;
	int rows = 1;
	
	switch (charInfo.type) {
	case CharType::Hiragana:
		imagePath = hiraganaImage_;
		charWidth = kHiraganaWidth;
		charHeight = kHiraganaHeight;
		columns = hiraganaColumns_;
		rows = hiraganaRows_;
		break;
	case CharType::Katakana:
		imagePath = katakanaImage_;
		charWidth = kKatakanaWidth;
		charHeight = kKatakanaHeight;
		columns = katakanaColumns_;
		rows = katakanaRows_;
		break;
	case CharType::Alphabet:
		imagePath = alphabetImage_;
		charWidth = kAlphabetWidth;
		charHeight = kAlphabetHeight;
		columns = alphabetColumns_;
		break;
	default:
		return;
	}

	// スプライトが未初期化または別のテクスチャの場合は再初期化
	if (!charInfo.sprite) {
		charInfo.sprite = std::make_unique<Sprite>();
	}
	
	charInfo.sprite->Initialize(imagePath);
	charInfo.sprite->SetAnchorPoint(AnchorPoint::TopLeft);
	
	// インデックスから切り取り位置を計算
	int row = charInfo.index % rows;
	int col = charInfo.index % columns;
	
	Vector2 leftTop = { col * charWidth, row * charHeight };
	Vector2 size = { charWidth, charHeight };
	
	// テクスチャの切り取り範囲を設定
	charInfo.sprite->SetTextureRect(leftTop, size);
	
	// スプライトのサイズを切り取りサイズに合わせる
	charInfo.sprite->GetSize() = size;
	
	// 頂点データを切り取りサイズに合わせて更新
	UpdateSpriteVertices(charInfo.sprite.get(), size);
}

void TextRenderer::UpdateSpriteVertices(Sprite* sprite, const Vector2& size) {
	if (!sprite) return;
	
	// Spriteの内部サイズを更新
	sprite->GetSize() = size;
	
	// アンカーポイントを再設定して頂点を更新
	sprite->SetAnchorPoint(AnchorPoint::TopLeft);
}

void TextRenderer::Draw() {
	for (auto& charInfo : characters_) {
		if (charInfo.sprite) {
			charInfo.sprite->SetPosition(charInfo.position);
			charInfo.sprite->SetScale(scale_);
			charInfo.sprite->SetColor(color_);
			charInfo.sprite->Update();
			charInfo.sprite->Draw();
		}
	}
}

TextRenderer::CharType TextRenderer::GetCharType(char32_t codepoint) const {
	// ひらがな: U+3040 - U+309F
	if (codepoint >= 0x3040 && codepoint <= 0x309F) {
		return CharType::Hiragana;
	}
	// カタカナ: U+30A0 - U+30FF
	if (codepoint >= 0x30A0 && codepoint <= 0x30FF) {
		return CharType::Katakana;
	}
	// 半角カタカナ: U+FF65 - U+FF9F
	if (codepoint >= 0xFF65 && codepoint <= 0xFF9F) {
		return CharType::Katakana;
	}
	// ASCII英数字
	if ((codepoint >= U'a' && codepoint <= U'z') ||
		(codepoint >= U'A' && codepoint <= U'Z') ||
		(codepoint >= U'0' && codepoint <= U'9')) {
		return CharType::Alphabet;
	}
	
	return CharType::Unknown;
}

int TextRenderer::GetCharIndex(char32_t codepoint, CharType type) const {
	switch (type) {
	case CharType::Hiragana:
		{
			auto it = hiraganaMap_.find(codepoint);
			if (it != hiraganaMap_.end()) {
				return it->second;
			}
		}
		break;
	case CharType::Katakana:
		{
			auto it = katakanaMap_.find(codepoint);
			if (it != katakanaMap_.end()) {
				return it->second;
			}
		}
		break;
	case CharType::Alphabet:
		{
			auto it = alphabetMap_.find(codepoint);
			if (it != alphabetMap_.end()) {
				return it->second;
			}
		}
		break;
	default:
		break;
	}
	return -1;
}

std::vector<char32_t> TextRenderer::ConvertToUTF32(const std::string& utf8) const {
	std::vector<char32_t> result;
	
	size_t i = 0;
	while (i < utf8.size()) {
		char32_t codepoint = 0;
		unsigned char c = static_cast<unsigned char>(utf8[i]);
		
		if ((c & 0x80) == 0) {
			// 1バイト文字 (ASCII)
			codepoint = c;
			i += 1;
		}
		else if ((c & 0xE0) == 0xC0) {
			// 2バイト文字
			if (i + 1 >= utf8.size()) break;
			codepoint = (c & 0x1F) << 6;
			codepoint |= (static_cast<unsigned char>(utf8[i + 1]) & 0x3F);
			i += 2;
		}
		else if ((c & 0xF0) == 0xE0) {
			// 3バイト文字
			if (i + 2 >= utf8.size()) break;
			codepoint = (c & 0x0F) << 12;
			codepoint |= (static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 6;
			codepoint |= (static_cast<unsigned char>(utf8[i + 2]) & 0x3F);
			i += 3;
		}
		else if ((c & 0xF8) == 0xF0) {
			// 4バイト文字
			if (i + 3 >= utf8.size()) break;
			codepoint = (c & 0x07) << 18;
			codepoint |= (static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 12;
			codepoint |= (static_cast<unsigned char>(utf8[i + 2]) & 0x3F) << 6;
			codepoint |= (static_cast<unsigned char>(utf8[i + 3]) & 0x3F);
			i += 4;
		}
		else {
			// 無効なバイト
			i += 1;
			continue;
		}
		
		result.push_back(codepoint);
	}
	
	return result;
}

void TextRenderer::DrawImGui(const char* name) {
#ifdef USE_IMGUI
	ImGui::Begin(name);

	if (ImGui::DragFloat2("位置", &position_.x, 1.0f)) {
		UpdateCharacterPositions();
	}
	if (ImGui::DragFloat2("スケール", &scale_.x, 0.01f, 0.01f, 10.0f)) {
		UpdateCharacterPositions();
	}
	if (ImGui::DragFloat("文字間隔", &charSpacing_, 1.0f, -100.0f, 100.0f)) {
		UpdateCharacterPositions();
	}
	if (ImGui::DragFloat("行間", &lineSpacing_, 1.0f, 0.0f, 200.0f)) {
		UpdateCharacterPositions();
	}
	ImGui::ColorEdit4("色", &color_.x);

	// 配置方向の選択
	const char* alignmentItems[] = { "左揃え", "中央揃え", "右揃え" };
	int currentAlignment = static_cast<int>(alignment_);
	if (ImGui::Combo("配置", &currentAlignment, alignmentItems, IM_ARRAYSIZE(alignmentItems))) {
		alignment_ = static_cast<Alignment>(currentAlignment);
		UpdateCharacterPositions();
	}

	ImGui::Separator();
	ImGui::Text("現在の文字数: %zu", characters_.size());
	
	// デバッグ情報: 各文字のインデックスを表示
	if (ImGui::TreeNode("文字インデックス（デバッグ）")) {
		std::vector<char32_t> codepoints = ConvertToUTF32(currentText_);
		for (size_t i = 0; i < codepoints.size() && i < 20; ++i) {
			char32_t cp = codepoints[i];
			CharType type = GetCharType(cp);
			int index = GetCharIndex(cp, type);
			
			// UTF-32コードポイントを文字列に変換（簡易表示）
			char utf8[5] = {0};
			if (cp < 0x80) {
				utf8[0] = static_cast<char>(cp);
			} else if (cp < 0x800) {
				utf8[0] = static_cast<char>(0xC0 | (cp >> 6));
				utf8[1] = static_cast<char>(0x80 | (cp & 0x3F));
			} else if (cp < 0x10000) {
				utf8[0] = static_cast<char>(0xE0 | (cp >> 12));
				utf8[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
				utf8[2] = static_cast<char>(0x80 | (cp & 0x3F));
			}
			
			const char* typeStr = "Unknown";
			if (type == CharType::Hiragana) typeStr = "Hiragana";
			else if (type == CharType::Katakana) typeStr = "Katakana";
			else if (type == CharType::Alphabet) typeStr = "Alphabet";
			
			ImGui::Text("%zu: '%s' (U+%04X) -> Index: %d (%s)", i, utf8, static_cast<unsigned int>(cp), index, typeStr);
		}
		ImGui::TreePop();
	}

	ImGui::End();
#endif // USE_IMGUI
}
