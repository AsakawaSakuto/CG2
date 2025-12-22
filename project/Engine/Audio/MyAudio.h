#pragma once
#include "AudioManager.h"

namespace MyAudio {
    inline void PlayBGM(BGM_List bgm, float volume = 1.0f) {
        AudioManager::GetInstance()->PlayBGM(bgm, volume);
    }

    inline void StopBGM(BGM_List bgm) {
        AudioManager::GetInstance()->StopBGM(bgm);
    }

    // SE制御
    inline void PlaySE(SE_List se, float volume = 1.0f) {
        AudioManager::GetInstance()->PlaySE(se, volume);
    }
}