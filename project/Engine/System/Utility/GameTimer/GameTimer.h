#pragma once
#include <functional>
#include <vector>
#include <string>

#include "Engine/System/Utility/Easing/Easing.h"
#include "DeltaTime.h"

/// @brief ゲーム用タイマークラス
/// シーン遷移、ゲーム内演出のタイミング制御に使用（イージングは分離）
class GameTimer {
public:

    /// @brief デフォルトコンストラクタ
    GameTimer() = default;

    /// @brief コンストラクタ
    /// @param duration タイマーの継続時間（秒）
    /// @param loop ループするかどうか
    GameTimer(float duration, bool loop = false);

    /// @brief タイマーの更新
    /// @param deltaTime 前フレームからの経過時間（秒）
    void Update();

    /// @brief タイマーを開始
    /// @param duration タイマーの継続時間（秒）
    /// @param loop ループするかどうか
    void Start(float duration, bool loop = false, bool countdown = false);

    /// @brief タイマーを停止
    void Stop();

    /// @brief タイマーをリセット（停止状態にして時間を0に戻す）
    void Reset();

    /// @brief タイマーを一時停止
    void Pause();

    /// @brief タイマーを再開
    void Resume();

    /// @brief タイマーが動作中かどうか
    /// @return アクティブな場合true
    bool IsActive() const;

    /// @brief タイマーが完了したかどうか
    /// @return 完了した場合true
    bool IsFinished() const;

    /// @brief タイマーの進行状況を取得（0.0～1.0）
    /// @return 進行状況（0.0=開始、1.0=完了）
    float GetProgress() const;

    /// @brief タイマーの逆進行状況を取得（1.0～0.0）
    /// @return 逆進行状況（1.0=開始、0.0=完了）
    float GetReverseProgress() const;

    /// @brief --- EasingUtilを使用したイージング進行状況を取得 ---
    /// @param easingType イージングタイプ
    /// @return イージング適用済み進行状況
    float GetEasedProgress(Easing::Type easingType = Easing::Type::Linear) const;

    /// @brief 残り時間を取得
    /// @return 残り時間（秒）
    float GetRemainingTime() const;

    /// @brief 経過時間を取得
    /// @return 経過時間（秒）
    float GetElapsedTime() const;

    /// @brief タイマーの継続時間を取得
    /// @return 継続時間（秒）
    float GetDuration() const;

    /// @brief タイマーがループ設定かどうか
    /// @return ループ設定の場合true
    bool IsLoop() const;

    /// @brief このフレームでタイマーがループしたかどうか
    /// @return ループした場合true
    bool HasLooped() const;

    /// @brief 継続時間を変更（実行中でも変更可能）
    /// @param duration 新しい継続時間（秒）
    void SetDuration(float duration);

    /// @brief ループ設定を変更
    /// @param loop ループするかどうか
    void SetLoop(bool loop);

    // --- フレームカウンター機能 ---

    /// @brief フレーム単位でタイマーを開始
    /// @param frameCount フレーム数
    /// @param loop ループするかどうか
    /// @param targetFPS 目標FPS（デフォルト60）
    void StartFrames(int frameCount, bool loop = false, float targetFPS = 60.0f);

    /// @brief 現在のフレーム数を取得
    /// @return フレーム数
    int GetCurrentFrame() const;

    /// @brief 総フレーム数を取得
    /// @return 総フレーム数
    int GetTotalFrames() const;

    // --- タイムスケール機能 ---

    /// @brief タイムスケールを設定
    /// @param scale タイムスケール（1.0=通常、0.5=半分速度、2.0=倍速）
    void SetTimeScale(float scale);

    /// @brief タイムスケールを取得
    /// @return タイムスケール
    float GetTimeScale() const;

private:
    float currentTime_ = 0.0f;          // 現在の経過時間
    float duration_ = 0.0f;             // タイマーの継続時間
    bool isActive_ = false;             // タイマーがアクティブかどうか
    bool loop_ = false;                 // ループするかどうか
    bool finished_ = false;             // タイマーが完了したかどうか
    bool loopedThisFrame_ = false;      // このフレームでループしたかどうか
    bool isCountdown_ = false;          // カウントダウンモードかどうか

    // タイムスケール機能
    float timeScale_ = 1.0f;            // タイムスケール

    // フレーム機能
    int totalFrames_ = 0;               // 総フレーム数（フレーム単位使用時）
    bool useFrameMode_ = false;         // フレーム単位モードかどうか
    float targetFPS_ = 60.0f;           // 目標FPS（フレーム単位モード用）
};