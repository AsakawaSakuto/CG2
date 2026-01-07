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
    Load(SE_List::StartUp,      "StartUp.mp3", 1.0f);
    Load(SE_List::Confirm,      "Confirm.mp3", 1.0f);
    Load(SE_List::Select,       "Select.mp3", 1.0f);
	Load(SE_List::LockIn,       "LockIn.mp3", 1.0f);
    Load(SE_List::Jar,          "jar.mp3", 1.0f);
    Load(SE_List::Chest,        "chest.mp3", 1.0f);
	Load(SE_List::PlayerDamage, "damage.mp3", 1.0f);
	Load(SE_List::Jump,         "jump.mp3", 1.0f);
    Load(SE_List::Upgrade,      "upgrade.mp3", 1.0f);
	Load(SE_List::ExpGet,       "expGet.mp3", 1.0f);
	Load(SE_List::PlayerDie,    "playerDie.mp3", 1.0f);

    // Weapon SE
    Load(SE_List::FireBall,  "Weapon/fireball.mp3", 1.0f);
    Load(SE_List::Laser,     "Weapon/laser.mp3", 1.0f);
    Load(SE_List::Runa,      "Weapon/runa.mp3", 1.0f);
    Load(SE_List::Axe,       "Weapon/axe.mp3", 1.0f);
    Load(SE_List::Boomerang, "Weapon/boomerang.mp3", 1.0f);
    Load(SE_List::Dice,      "Weapon/dice.mp3", 1.0f);
    Load(SE_List::Toxic,     "Weapon/toxic.mp3", 1.0f);
	Load(SE_List::Gun,       "Weapon/gun.mp3", 1.0f);

	// Enemy Damage SE
    Load(SE_List::EnemyDamage1, "EnemyDamage/1.mp3", 1.0f);
    Load(SE_List::EnemyDamage2, "EnemyDamage/2.mp3", 1.0f);
    Load(SE_List::EnemyDamage3, "EnemyDamage/3.mp3", 1.0f);
    Load(SE_List::EnemyDamage4, "EnemyDamage/4.mp3", 1.0f);
    Load(SE_List::EnemyDamage5, "EnemyDamage/5.mp3", 1.0f);

	// BGM.Load temp
    Load(BGM_List::Title, "titleBGM.mp3");
    Load(BGM_List::Game1, "Game/Bgm1.mp3");
    Load(BGM_List::Game2, "Game/Bgm2.mp3");
    Load(BGM_List::Game3, "Game/Bgm3.mp3");
    Load(BGM_List::Game4, "Game/Bgm4.mp3");
    Load(BGM_List::Game5, "Game/Bgm5.mp3");
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

void AudioManager::Load(BGM_List bgm, const std::string& filePath, float volume) {
    size_t index = static_cast<size_t>(bgm);
    assert(index < bgmArray_.size() && "Invalid BGM variation");

    // 既に読み込み済みならスキップ
    if (bgmArray_[index]) {
        return;
    }

    auto audio = std::make_unique<AudioX>();
    audio->Initialize("resources/sound/BGM/" + filePath);
    bgmArray_[index] = std::move(audio);
	bgmVolumeArray_[index] = volume;
}

void AudioManager::Load(SE_List se, const std::string& filePath, float volume) {
    size_t index = static_cast<size_t>(se);
    assert(index < seArray_.size() && "Invalid SE variation");

    // 既に読み込み済みならスキップ
    if (seArray_[index]) {
        return;
    }

    auto audio = std::make_unique<AudioX>();
    audio->Initialize("resources/sound/SE/" + filePath);
    seArray_[index] = std::move(audio);
	seVolumeArray_[index] = volume;
}

void AudioManager::PlayBGM(BGM_List bgm, bool loop) {
    size_t index = static_cast<size_t>(bgm);
    assert(index < bgmArray_.size() && "Invalid BGM variation");
    assert(bgmArray_[index] && "BGM not loaded");

    bgmArray_[index]->PlayAudio(bgmVolumeArray_[index] * BGM_MasterVolume, loop);
}

void AudioManager::PlaySE(SE_List se, bool loop) {
    size_t index = static_cast<size_t>(se);
    assert(index < seArray_.size() && "Invalid SE variation");
    assert(seArray_[index] && "SE not loaded");

    seArray_[index]->PlayAudio(seVolumeArray_[index] * SE_MasterVolume, loop);
}

void AudioManager::StopBGM(BGM_List bgm) {
	bgmArray_[static_cast<size_t>(bgm)]->StopAll();
}

void AudioManager::StopSE(SE_List se) {
    seArray_[static_cast<size_t>(se)]->StopAll();
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
