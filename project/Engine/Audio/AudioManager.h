#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <array>
#include "Audio.h"

enum class SE_Variation {
    KAWAII,
    OU,
    DON,
    KIRAKIRA,

    Count
};

enum class BGM_Variation {
    TEST,
    TITLE,
    GAME,
    RESULT,

    Count
};

enum class FadeState {
    None,
    FadingOut,
    FadingIn
};

class AudioManager {
public:

    // シングルトンインスタンス取得
    static AudioManager* GetInstance();

    // 初期化・終了処理
    void Initialize();
    void Finalize();

    // 音源の事前読み込み
    void LoadBGM(BGM_Variation bgm, const std::string& filePath);
    void LoadSE(SE_Variation se, const std::string& filePath);

    // 再生制御
    void PlayBGM(BGM_Variation bgm, float volume = 1.0f, bool loop = true);
    void PlaySE(SE_Variation se, float volume = 1.0f, bool loop = false);

    // BGM専用制御
    void StopBGM();
    void PauseBGM();
    void ResumeBGM();
    void SetBGMVolume(float volume);
    void FadeOutBGM(float duration);
    void FadeInBGM(BGM_Variation bgm, float duration, float targetVolume = 1.0f);

    // SE専用制御
    void StopAllSE();
    void SetSEVolume(float volume);

    // 毎フレーム更新
    void Update();

    // 読み込み済み確認
    bool IsLoaded(BGM_Variation bgm) const;
    bool IsLoaded(SE_Variation se) const;

    // デストラクタ
    ~AudioManager() = default;

private:

private:
    AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // BGM・SE管理
    std::array<std::unique_ptr<AudioX>, static_cast<size_t>(BGM_Variation::Count)> bgmArray_;
    std::array<std::unique_ptr<AudioX>, static_cast<size_t>(SE_Variation::Count)> seArray_;
    
    // 現在のBGM管理
    BGM_Variation currentBGM_ = BGM_Variation::Count;
    AudioX* currentBGMPtr_ = nullptr;
    float bgmVolume_ = 1.0f;
    bool bgmPaused_ = false;
    
    // フェード制御
    bool isFading_ = false;
    float fadeTimer_ = 0.0f;
    float fadeDuration_ = 0.0f;
    float fadeStartVolume_ = 0.0f;
    float fadeTargetVolume_ = 0.0f;
    bool fadeOut_ = false;
    BGM_Variation nextBGM_ = BGM_Variation::Count;

    // SE音量
    float seVolume_ = 1.0f;
};

namespace MyAudio {
    // BGM制御
    inline void LoadBGM(BGM_Variation bgm, const std::string& filePath) {
        AudioManager::GetInstance()->LoadBGM(bgm, filePath);
    }

    inline void PlayBGM(BGM_Variation bgm, float volume = 1.0f) {
        AudioManager::GetInstance()->PlayBGM(bgm, volume);
    }

    inline void StopBGM() {
        AudioManager::GetInstance()->StopBGM();
    }

    inline void PauseBGM() {
        AudioManager::GetInstance()->PauseBGM();
    }

    inline void ResumeBGM() {
        AudioManager::GetInstance()->ResumeBGM();
    }

    inline void SetBGMVolume(float volume) {
        AudioManager::GetInstance()->SetBGMVolume(volume);
    }

    inline void FadeOutBGM(float duration) {
        AudioManager::GetInstance()->FadeOutBGM(duration);
    }

    inline void FadeInBGM(BGM_Variation bgm, float duration, float targetVolume = 1.0f) {
        AudioManager::GetInstance()->FadeInBGM(bgm, duration, targetVolume);
    }

    // SE制御
    inline void LoadSE(SE_Variation se, const std::string& filePath) {
        AudioManager::GetInstance()->LoadSE(se, filePath);
    }

    inline void PlaySE(SE_Variation se, float volume = 1.0f) {
        AudioManager::GetInstance()->PlaySE(se, volume);
    }

    inline void StopAllSE() {
        AudioManager::GetInstance()->StopAllSE();
    }

    inline void SetSEVolume(float volume) {
        AudioManager::GetInstance()->SetSEVolume(volume);
    }
}