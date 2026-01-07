#pragma once
#include "AudioManager.h"

namespace MyAudio {
    inline void Play(BGM_List bgm, bool f = true) {
        AudioManager::GetInstance()->PlayBGM(bgm, f);
    }

    inline void Play(SE_List se, bool f = false) {
        AudioManager::GetInstance()->PlaySE(se, f);
    }

    inline void Stop(BGM_List bgm) {
        AudioManager::GetInstance()->StopBGM(bgm);
    }

    inline void Stop(SE_List se) {
        AudioManager::GetInstance()->StopSE(se);
    }

    inline void SetVolume(BGM_List bgm, float volume) {
        AudioManager::GetInstance()->SetBGMVolume(bgm, volume);
    }

    inline void SetVolume(SE_List se, float volume) {
        AudioManager::GetInstance()->SetSEVolume(se, volume);
    }

    inline void SetBgmMasterVolume(float volume) {
        AudioManager::GetInstance()->SetBgmMasterVolume(volume);
	}

    inline void SetSeMasterVolume(float volume) {
        AudioManager::GetInstance()->SetSeMasterVolume(volume);
    }

    inline float GetBgmMasterVolume() {
        return AudioManager::GetInstance()->GetBgmMasterVolume();
	}

    inline float GetSeMasterVolume() {
        return AudioManager::GetInstance()->GetSeMasterVolume();
	}
}