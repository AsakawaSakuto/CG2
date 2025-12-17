#pragma once

#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

#include "Math/Type/Vector3.h"
#include "Math/Type/Vector2.h"
#include <string>
#include <functional>

/// <summary>
/// ImGuiの汎用的な拡張機能
/// </summary>
namespace ImGuiHelper {

    /// <summary>
    /// Vector3の編集用ウィジェット（Unityスタイル）
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="v">編集するVector3</param>
    /// <param name="resetValue">リセット時の値</param>
    /// <param name="speed">ドラッグ速度</param>
    /// <returns>値が変更されたかどうか</returns>
    bool DrawVector3Control(const char* label, Vector3& v, const Vector3& resetValue = {0.0f, 0.0f, 0.0f}, float speed = 0.1f);

    /// <summary>
    /// 折りたたみ可能なヘッダー付きセクション
    /// </summary>
    bool BeginPropertySection(const char* label, bool defaultOpen = true);
    void EndPropertySection();

    /// <summary>
    /// 読み取り専用のテキストフィールド
    /// </summary>
    void ReadOnlyText(const char* label, const char* text);
    void ReadOnlyFloat(const char* label, float value);
    void ReadOnlyInt(const char* label, int value);

    /// <summary>
    /// ボタン付きの確認ダイアログ
    /// </summary>
    bool ConfirmButton(const char* label, const char* confirmText = "Are you sure?");

    /// <summary>
    /// カラーコードのついたログテキスト
    /// </summary>
    enum class LogLevel {
        Info,
        Warning,
        Error,
        Success
    };
    void ColoredText(const char* text, LogLevel level = LogLevel::Info);

    /// <summary>
    /// Transform編集用の統合ウィジェット
    /// </summary>
    void DrawTransformControls(Vector3& position, Vector3& rotation, Vector3& scale);

    /// <summary>
    /// セパレーター（見やすいスタイル付き）
    /// </summary>
    void StyledSeparator();

    /// <summary>
    /// ツールチップ付きヘルプマーカー
    /// </summary>
    void HelpMarker(const char* desc);

    /// <summary>
    /// 検索ボックス
    /// </summary>
    bool SearchBar(char* buffer, size_t bufferSize, const char* hint = "Search...");

    /// <summary>
    /// プログレスバー（Unityスタイル）
    /// </summary>
    void ProgressBar(float fraction, const char* overlay = nullptr);

} // namespace ImGuiHelper