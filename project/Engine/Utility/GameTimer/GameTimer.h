#pragma once
#include <functional>
#include <vector>
#include <string>

#include "Utility/Easing/Easing.h"
#include "DeltaTime.h"

class GameTimer {
public:

    // デフォルトコンストラクタ
    GameTimer() = default;

    /// @brief コンストラクタ
    /// @param duration タイマーの継続時間
    /// @param loop ループするかどうか
    GameTimer(float duration, bool loop = false);

    /// @brief タイマーの更新
    void Update();

    /// @brief タイマーを開始
    /// @param duration タイマーの継続時間
    /// @param loop ループするかどうか
    void Start(float duration, bool loop = false);

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

    /// @brief タイマーの進行状況を取得 0.0～1.0
    /// @return 進行状況 0.0 = 開始、1.0 = 完了
    float GetProgress() const;

    /// @brief タイマーの逆進行状況を取得 1.0～0.0 
    /// @return 逆進行状況 1.0=開始、0.0=完了
    float GetReverseProgress() const;

    /// @brief Easingを使用したイージング進行状況を取得
    /// @param easingType イージングタイプ
    /// @return イージング適用済み進行状況
    float GetEasedProgress(EaseType easingType = EaseType::Linear) const;

    /// @brief 残り時間を取得
    /// @return 残り時間
    float GetRemainingTime() const;

    /// @brief 経過時間を取得
    /// @return 経過時間
    float GetElapsedTime() const;

    /// @brief タイマーの継続時間を取得
    /// @return 継続時間
    float GetDuration() const;

    /// @brief 継続時間を変更
    /// @param duration 新しい継続時間
    void SetDuration(float duration);

    /// @brief ループ設定を変更
    /// @param loop ループするかどうか
    void SetLoop(bool loop);

    /// @brief フレーム単位でタイマーを開始
    /// @param frameCount フレーム数
    /// @param loop ループするかどうか
    /// @param targetFPS 目標FPS デフォルト60
    void StartFrames(int frameCount, bool loop = false, float targetFPS = 60.0f);

    /// @brief 現在のフレーム数を取得
    /// @return フレーム数
    int GetCurrentFrame() const;

    /// @brief 総フレーム数を取得
    /// @return 総フレーム数
    int GetTotalFrames() const;

    /// @brief タイムスケールを設定
    /// @param scale タイムスケール 1.0 = 通常、0.5 = 半分速度、2.0 = 倍速
    void SetTimeScale(float scale);

private:
    float currentTime_ = 0.0f;    
    float duration_ = 0.0f;       
    bool isActive_ = false;       
    bool loop_ = false;           
    bool finished_ = false;       
    bool loopedThisFrame_ = false;
    float timeScale_ = 1.0f;
    int totalFrames_ = 0;      
    bool useFrameMode_ = false;
    float targetFPS_ = 60.0f;  
};