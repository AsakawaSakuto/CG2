#pragma once
#include"IScene.h"
#include"AppGigaBonk/SceneManager/Scene/TitleScene.h"
#include"AppGigaBonk/SceneManager/Scene/GameScene.h"
#include"AppGigaBonk/SceneManager/Scene/ResultScene.h"
#include<memory>

// シーン数
constexpr uint32_t sceneNum = 3;

class SceneManager {
private:
    std::unique_ptr<IScene> sceneArr_[sceneNum];
    int currentSceneNo_;
    int prevSceneNo_;
public:
    SceneManager();
    ~SceneManager();
    int Run();  // メインループ
    std::unique_ptr<AppContext> appContext_;
};