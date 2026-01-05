#include "AudioManager.h"
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

    for (auto& bgmVolume : bgmVolumeArray_) {
        bgmVolume = 1.0f;
    }
    for (auto& seVolume : seVolumeArray_) {
        seVolume = 1.0f;
	}

    isFading_ = false;

    // SE.Load temp
	// LoadSE(SE_Variation::, "resources/sound/SE/");
	LoadSE(SE_List::StartUp, "resources/sound/SE/StartUp.mp3");

	// BGM.Load temp
	LoadBGM(BGM_List::Title, "resources/sound/BGM/titleBGM.mp3");
    LoadBGM(BGM_List::Game1, "resources/sound/BGM/Game/Bgm1.mp3");
    LoadBGM(BGM_List::Game2, "resources/sound/BGM/Game/Bgm2.mp3");
    LoadBGM(BGM_List::Game3, "resources/sound/BGM/Game/Bgm3.mp3");
    LoadBGM(BGM_List::Game4, "resources/sound/BGM/Game/Bgm4.mp3");
    LoadBGM(BGM_List::Game5, "resources/sound/BGM/Game/Bgm5.mp3");
}

void AudioManager::Finalize() {
	StopAllBGM();
    StopAllSE();
    
    for (auto& bgm : bgmArray_) {
        bgm.reset();
    }
    for (auto& se : seArray_) {
        se.reset();
    }
}

void AudioManager::LoadBGM(BGM_List bgm, const std::string& filePath) {
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

void AudioManager::LoadSE(SE_List se, const std::string& filePath) {
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

void AudioManager::PlayBGM(BGM_List bgm, float volume, bool loop) {
    size_t index = static_cast<size_t>(bgm);
    assert(index < bgmArray_.size() && "Invalid BGM variation");
    assert(bgmArray_[index] && "BGM not loaded");

	bgmVolumeArray_[index] = volume;
    bgmArray_[index]->PlayAudio(bgmVolumeArray_[index] * BGM_MasterVolume, loop);
}

void AudioManager::PlaySE(SE_List se, float volume, bool loop) {
    size_t index = static_cast<size_t>(se);
    assert(index < seArray_.size() && "Invalid SE variation");
    assert(seArray_[index] && "SE not loaded");

	seVolumeArray_[index] = volume;
    seArray_[index]->PlayAudio(seVolumeArray_[index] * SE_MasterVolume, loop);
}

void AudioManager::StopBGM(BGM_List bgm) {
	bgmArray_[static_cast<size_t>(bgm)]->StopAll();
}

void AudioManager::StopAllSE() {
    for (size_t i = 0; i < seArray_.size(); ++i) {
        if (seArray_[i]) {
            seArray_[i]->StopAll();
        }
    }
}

void AudioManager::StopAllBGM() {
    for (size_t i = 0; i < bgmArray_.size(); ++i) {
        if (bgmArray_[i]) {
            bgmArray_[i]->StopAll();
        }
    }
}

void AudioManager::Update() {

    // 全AudioXの更新（終了したインスタンスのクリーンアップ）
    for (auto& bgm : bgmArray_) {
        if (bgm) {
			bgm->SetVolume(bgmVolumeArray_[&bgm - &bgmArray_[0]] * BGM_MasterVolume);
            bgm->Update();
        }
    }
    
    for (auto& se : seArray_) {
        if (se) {
            se->Update();
        }
    }
}

// 個別音量設定（BGM）
void AudioManager::SetBGMVolume(BGM_List bgm, float volume) {
    size_t index = static_cast<size_t>(bgm);
    assert(index < bgmVolumeArray_.size() && "Invalid BGM variation");
    bgmVolumeArray_[index] = volume;
    if (bgmArray_[index]) {
        bgmArray_[index]->SetVolume(bgmVolumeArray_[index] * BGM_MasterVolume);
    }
}

// 個別音量設定（SE）
void AudioManager::SetSEVolume(SE_List se, float volume) {
    size_t index = static_cast<size_t>(se);
    assert(index < seVolumeArray_.size() && "Invalid SE variation");
    seVolumeArray_[index] = volume;
    if (seArray_[index]) {
        seArray_[index]->SetVolume(seVolumeArray_[index] * SE_MasterVolume);
    }
}

bool AudioManager::IsLoaded(BGM_List bgm) const {
    size_t index = static_cast<size_t>(bgm);
    return index < bgmArray_.size() && bgmArray_[index] != nullptr;
}

bool AudioManager::IsLoaded(SE_List se) const {
    size_t index = static_cast<size_t>(se);
    return index < seArray_.size() && seArray_[index] != nullptr;
}
