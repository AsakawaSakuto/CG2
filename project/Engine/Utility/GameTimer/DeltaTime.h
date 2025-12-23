#pragma once

#include <chrono>

class DeltaTime {
public:
    // シングルトンインスタンスを取得
    static DeltaTime& GetInstance() {
        static DeltaTime instance;
        return instance;
    }

    // フレーム時間を更新（毎フレーム呼び出す）
    void Update() {
        auto currentTime = std::chrono::high_resolution_clock::now();

        if (!isFirstFrame_) {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime_);
            deltaTime_ = static_cast<float>(elapsed.count()) / 1000000.0f; // 秒単位に変換

            // 異常値の制限（最大0.1秒、最小0.001秒）
            if (deltaTime_ > 0.1f) deltaTime_ = 0.1f;
            if (deltaTime_ < 0.001f) deltaTime_ = 0.001f;

            // FPS計算
            fps_ = 1.0f / deltaTime_;
        } else {
            isFirstFrame_ = false;
            deltaTime_ = 1.0f / 60.0f; // 初回フレームは60FPSと仮定
            fps_ = 60.0f;
        }

        lastFrameTime_ = currentTime;
    }

    // デルタタイムを取得（秒単位）
    float GetDeltaTime() const { return deltaTime_; }

    // FPSを取得
    float GetFPS() const { return fps_; }

private:
    DeltaTime() = default;
    ~DeltaTime() = default;
    DeltaTime(const DeltaTime&) = delete;
    DeltaTime& operator=(const DeltaTime&) = delete;

    std::chrono::high_resolution_clock::time_point lastFrameTime_;
    float deltaTime_ = 1.0f / 60.0f;
    float fps_ = 60.0f;
    bool isFirstFrame_ = true;
};

// グローバルアクセス用のインライン関数
inline float GetDeltaTime() {
    return DeltaTime::GetInstance().GetDeltaTime();
}

inline float GetFPS() {
    return DeltaTime::GetInstance().GetFPS();
}

// 互換性のための定数
//constexpr float deltaTime_ = 1.0f / 60.0f;