#include "AudioManager.h"
#include "MasterVolume.h"
#include "Utility/GameTimer/DeltaTime.h"
#include <cassert>

AudioManager* AudioManager::GetInstance() {
    static AudioManager instance;
    return &instance;
}

void AudioManager::Initialize() {
    // 配列の初期化
    for (auto& bgm : bgmArray_) {
        bgm.reset();
    }
    for (auto& se : seArray_) {
        se.reset();
    }

    currentBGM_ = BGM_Variation::Count;
    currentBGMPtr_ = nullptr;
    bgmVolume_ = BGM_Volume;
    seVolume_ = SE_Volume;
    bgmPaused_ = false;
    isFading_ = false;

    // SE.Load temp
	// LoadSE(SE_Variation::, "resources/sound/SE/");
	LoadSE(SE_Variation::KAWAII, "resources/sound/SE/kawaii.mp3");
	LoadSE(SE_Variation::OU, "resources/sound/SE/ou.mp3");
	LoadSE(SE_Variation::DON, "resources/sound/SE/don.mp3");
	LoadSE(SE_Variation::KIRAKIRA, "resources/sound/SE/kirakira.mp3");
}

void AudioManager::Finalize() {
    StopBGM();
    StopAllSE();
    
    for (auto& bgm : bgmArray_) {
        bgm.reset();
    }
    for (auto& se : seArray_) {
        se.reset();
    }
}

void AudioManager::LoadBGM(BGM_Variation bgm, const std::string& filePath) {
    size_t index = static_cast<size_t>(bgm);
    assert(index < bgmArray_.size() && "Invalid BGM variation");

    // 既に読み込み済みならスキップ
    if (bgmArray_[index]) {
        return;
    }

    auto audio = std::make_unique<AudioX>();
    audio->Initialize(filePath);
    bgmArray_[index] = std::move(audio);
}

void AudioManager::LoadSE(SE_Variation se, const std::string& filePath) {
    size_t index = static_cast<size_t>(se);
    assert(index < seArray_.size() && "Invalid SE variation");

    // 既に読み込み済みならスキップ
    if (seArray_[index]) {
        return;
    }

    auto audio = std::make_unique<AudioX>();
    audio->Initialize(filePath);
    seArray_[index] = std::move(audio);
}

void AudioManager::PlayBGM(BGM_Variation bgm, float volume, bool loop) {
    size_t index = static_cast<size_t>(bgm);
    assert(index < bgmArray_.size() && "Invalid BGM variation");
    assert(bgmArray_[index] && "BGM not loaded");

    // 現在のBGMと異なる場合は停止
    if (currentBGMPtr_ && currentBGM_ != bgm) {
        currentBGMPtr_->StopAll();
    }

    currentBGM_ = bgm;
    currentBGMPtr_ = bgmArray_[index].get();
    bgmPaused_ = false;
    currentBGMPtr_->PlayAudio(volume * bgmVolume_ * BGM_Volume, loop);
}

void AudioManager::PlaySE(SE_Variation se, float volume, bool loop) {
    size_t index = static_cast<size_t>(se);
    assert(index < seArray_.size() && "Invalid SE variation");
    assert(seArray_[index] && "SE not loaded");

    seArray_[index]->PlayAudio(volume * seVolume_ * SE_Volume, loop);
}

void AudioManager::StopBGM() {
    if (currentBGMPtr_) {
        currentBGMPtr_->StopAll();
        currentBGMPtr_ = nullptr;
        currentBGM_ = BGM_Variation::Count;
        bgmPaused_ = false;
    }
}

void AudioManager::PauseBGM() {
    if (currentBGMPtr_ && !bgmPaused_) {
        currentBGMPtr_->StopAll();
        bgmPaused_ = true;
    }
}

void AudioManager::ResumeBGM() {
    if (currentBGMPtr_ && bgmPaused_) {
        currentBGMPtr_->PlayAudio(bgmVolume_ * BGM_Volume, true);
        bgmPaused_ = false;
    }
}

void AudioManager::SetBGMVolume(float volume) {
    bgmVolume_ = volume;
    if (currentBGMPtr_) {
        currentBGMPtr_->SetVolume(bgmVolume_ * BGM_Volume);
    }
}

void AudioManager::SetSEVolume(float volume) {
    seVolume_ = volume;
}

void AudioManager::StopAllSE() {
    for (size_t i = 0; i < seArray_.size(); ++i) {
        if (seArray_[i]) {
            seArray_[i]->StopAll();
        }
    }
}

void AudioManager::FadeOutBGM(float duration) {
    if (!currentBGMPtr_) return;

    isFading_ = true;
    fadeOut_ = true;
    fadeTimer_ = 0.0f;
    fadeDuration_ = duration;
    fadeStartVolume_ = bgmVolume_;
    fadeTargetVolume_ = 0.0f;
    nextBGM_ = BGM_Variation::Count;
}

void AudioManager::FadeInBGM(BGM_Variation bgm, float duration, float targetVolume) {
    PlayBGM(bgm, 0.0f, true);
    
    isFading_ = true;
    fadeOut_ = false;
    fadeTimer_ = 0.0f;
    fadeDuration_ = duration;
    fadeStartVolume_ = 0.0f;
    fadeTargetVolume_ = targetVolume;
    nextBGM_ = BGM_Variation::Count;
}

void AudioManager::Update() {
    // フェード処理
    if (isFading_) {
        fadeTimer_ += GetDeltaTime();
        float t = fadeTimer_ / fadeDuration_;

        if (t >= 1.0f) {
            t = 1.0f;
            isFading_ = false;
            
            if (fadeOut_) {
                StopBGM();
                
                // フェードアウト後に次のBGMがあればフェードイン
                if (nextBGM_ != BGM_Variation::Count) {
                    FadeInBGM(nextBGM_, fadeDuration_, fadeTargetVolume_);
                }
            }
        }

        float currentVolume = fadeStartVolume_ + (fadeTargetVolume_ - fadeStartVolume_) * t;
        SetBGMVolume(currentVolume);
    }

    // 全AudioXの更新（終了したインスタンスのクリーンアップ）
    for (auto& bgm : bgmArray_) {
        if (bgm) {
            bgm->Update();
        }
    }
    
    for (auto& se : seArray_) {
        if (se) {
            se->Update();
        }
    }
}

bool AudioManager::IsLoaded(BGM_Variation bgm) const {
    size_t index = static_cast<size_t>(bgm);
    return index < bgmArray_.size() && bgmArray_[index] != nullptr;
}

bool AudioManager::IsLoaded(SE_Variation se) const {
    size_t index = static_cast<size_t>(se);
    return index < seArray_.size() && seArray_[index] != nullptr;
}
