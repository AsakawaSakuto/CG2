#pragma once
#include "AudioManager.h"

namespace MyAudio {
    inline void PlayBGM(BGM_List bgm, bool f = true) {
        AudioManager::GetInstance()->PlayBGM(bgm, f);
    }

    inline void StopBGM(BGM_List bgm) {
        AudioManager::GetInstance()->StopBGM(bgm);
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

    // 個別音量設定（BGM）
    inline void SetBGMVolume(BGM_List bgm, float volume) {
        AudioManager::GetInstance()->SetBGMVolume(bgm, volume);
    }

    // 個別音量設定（SE）
    inline void SetSEVolume(SE_List se, float volume) {
        AudioManager::GetInstance()->SetSEVolume(se, volume);
    }

    // SE制御
    inline void PlaySE(SE_List se, bool f = false) {
        AudioManager::GetInstance()->PlaySE(se, f);
    }

    inline void StopSE(SE_List se) {
        AudioManager::GetInstance()->StopSE(se);
    }
}