#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <array>
#include "Data/Audio.h"

enum class SE_List {
    KAWAII,
    OU,
    DON,
    KIRAKIRA,

    Count
};

enum class BGM_List {
    TEST,
    TITLE,
    GAME,
    RESULT,

    Count
};

class AudioManager {
public:

    // シングルトンインスタンス取得
    static AudioManager* GetInstance();

    // 初期化・終了処理
    void Initialize();
    void Finalize();

    // 音源の事前読み込み
    void LoadBGM(BGM_List bgm, const std::string& filePath);
    void LoadSE(SE_List se, const std::string& filePath);

	// BGM制御
    void PlayBGM(BGM_List bgm, float volume = 1.0f, bool loop = true);
    void StopBGM(BGM_List bgm);

	// SE制御
    void PlaySE(SE_List se, float volume = 1.0f, bool loop = false);

	void StopAllBGM();
    void StopAllSE();

    // 毎フレーム更新
    void Update();

    // 読み込み済み確認
    bool IsLoaded(BGM_List bgm) const;
    bool IsLoaded(SE_List se) const;

    // デストラクタ
    ~AudioManager() = default;

private:

private:
    AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // BGM・SE管理
    std::array<std::unique_ptr<AudioX>, static_cast<size_t>(BGM_List::Count)> bgmArray_;
    std::array<std::unique_ptr<AudioX>, static_cast<size_t>(SE_List::Count)> seArray_;

	// 個別の音量管理
    std::array<float, static_cast<size_t>(BGM_List::Count)> bgmVolumeArray_;
    std::array<float, static_cast<size_t>(SE_List::Count)> seVolumeArray_;

    // 音量管理
    float BGM_MasterVolume = 1.0f;
    float SE_MasterVolume = 1.0f;

    // フェード制御
    bool isFading_ = false;
    float fadeTimer_ = 0.0f;
    float fadeDuration_ = 0.0f;
    float fadeStartVolume_ = 0.0f;
    float fadeTargetVolume_ = 0.0f;
    bool fadeOut_ = false;
    BGM_List nextBGM_ = BGM_List::Count;
};