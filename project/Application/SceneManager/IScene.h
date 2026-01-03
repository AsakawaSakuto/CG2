#pragma once
#include <iostream>
#include "Core/WinApp/WinApp.h"
#include "GameObject/Player/playerStatus.h"
#include "GameObject/Player/WeaponManager/WeaponStatus.h"

using std::unique_ptr;
using std::make_unique;

enum class SCENE {
    TEST = 0,
    TITLE,
    GAME,
    RESULT,

    COUNT,
};

class IScene {
protected:
    SCENE nowSceneName_ = SCENE::TEST;

	void ChangeScene(SCENE nextSceneName) { nowSceneName_ = nextSceneName; }

    void Quit();

    // シーン間でデータを受け渡すための静的メンバ
    static PlayerName selectedPlayerName_;
    static WeaponName selectedWeaponName_;

public:

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawImGui() = 0;
    virtual void PostFrameCleanup() = 0;
    virtual ~IScene() {}
    SCENE GetSceneNo();
    void DrawSceneName();

    // シーン間データ受け渡し用の静的メソッド
    static void SetSelectedPlayerName(PlayerName name) { selectedPlayerName_ = name; }
    static void SetSelectedWeaponName(WeaponName name) { selectedWeaponName_ = name; }
    static PlayerName GetSelectedPlayerName() { return selectedPlayerName_; }
    static WeaponName GetSelectedWeaponName() { return selectedWeaponName_; }
};