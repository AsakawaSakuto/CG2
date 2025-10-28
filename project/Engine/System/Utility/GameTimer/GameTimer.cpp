#include "GameTimer.h"
#include <algorithm>
#include <cmath>

#ifdef _DEBUG
#include "externals/ImGui/imgui.h"
#endif

GameTimer::GameTimer(float duration, bool loop)
    : duration_(duration), loop_(loop) {
}

void GameTimer::Update() {
    if (!isActive_) return;

    loopedThisFrame_ = false;

    float scaledDeltaTime = deltaTime_ * timeScale_;

    if (isCountdown_) {
        currentTime_ -= scaledDeltaTime;
        if (currentTime_ <= 0.0f) {
            finished_ = true;

            if (loop_) {
                currentTime_ = duration_;
                finished_ = false;
                loopedThisFrame_ = true;
            } else {
                currentTime_ = 0.0f;
                isActive_ = false;
            }
        }
    } else {
        currentTime_ += scaledDeltaTime;
        if (currentTime_ >= duration_) {
            finished_ = true;

            if (loop_) {
                currentTime_ = 0.0f;
                finished_ = false;
                loopedThisFrame_ = true;
            } else {
                isActive_ = false;
            }
        }
    }
}


void GameTimer::Start(float duration, bool loop, bool countdown) {
    duration_ = duration;
    loop_ = loop;
    isCountdown_ = countdown;
    isActive_ = true;
    finished_ = false;
    useFrameMode_ = false;
    loopedThisFrame_ = false;

    currentTime_ = countdown ? duration : 0.0f;
}

void GameTimer::Stop() {
    isActive_ = false;
}

void GameTimer::Reset() {
    currentTime_ = 0.0f;
    isActive_ = false;
    finished_ = false;
    loopedThisFrame_ = false;
}

void GameTimer::Pause() {
    isActive_ = false;
}

void GameTimer::Resume() {
    if (currentTime_ < duration_) {
        isActive_ = true;
        finished_ = false;
    }
}

bool GameTimer::IsActive() const {
    return isActive_;
}

bool GameTimer::IsFinished() const {
    return finished_;
}

float GameTimer::GetProgress() const {
    if (duration_ <= 0.0f) return 1.0f;

    float progress = currentTime_ / duration_;
    progress = std::clamp(progress, 0.0f, 1.0f);

    if (isCountdown_) {
        progress = 1.0f - progress;
    }
    return progress;
}

float GameTimer::GetReverseProgress() const {
    return 1.0f - GetProgress();
}

float GameTimer::GetEasedProgress(Easing::Type easingType) const {
    float progress = GetProgress();
    return Easing::Apply(progress, easingType);  // --- EasingUtilを使用 ---
}

float GameTimer::GetRemainingTime() const {
    return (std::max)(0.0f, duration_ - currentTime_);
}

float GameTimer::GetElapsedTime() const {
    return currentTime_;
}

float GameTimer::GetDuration() const {
    return duration_;
}

bool GameTimer::IsLoop() const {
    return loop_;
}

bool GameTimer::HasLooped() const {
    return loopedThisFrame_;
}

void GameTimer::SetDuration(float duration) {
    duration_ = duration;
    // 現在時間が新しい継続時間を超えている場合の処理
    if (currentTime_ >= duration_ && isActive_) {
        finished_ = true;
        if (!loop_) {
            isActive_ = false;
        }
    }
}

void GameTimer::SetLoop(bool loop) {
    loop_ = loop;
}

// --- 新機能：フレームカウンター ---

void GameTimer::StartFrames(int frameCount, bool loop, float targetFPS) {
    totalFrames_ = frameCount;
    targetFPS_ = targetFPS;
    duration_ = frameCount / targetFPS;
    loop_ = loop;
    currentTime_ = 0.0f;
    isActive_ = true;
    finished_ = false;
    useFrameMode_ = true;
    loopedThisFrame_ = false;
}

int GameTimer::GetCurrentFrame() const {
    if (!useFrameMode_) return 0;
    return static_cast<int>(currentTime_ * targetFPS_);
}

int GameTimer::GetTotalFrames() const {
    return totalFrames_;
}

// --- 新機能：タイムスケール ---

void GameTimer::SetTimeScale(float scale) {
    timeScale_ = (std::max)(0.0f, scale);  // 負の値は防ぐ
}

float GameTimer::GetTimeScale() const {
    return timeScale_;
}