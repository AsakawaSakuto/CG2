#pragma once
#include"IScene.h"
#include"Application/SceneManager/Test/TestScene.h"
#include"Application/SceneManager/Scene/TitleScene.h"
#include"Application/SceneManager/Scene/GameScene.h"
#include"Application/SceneManager/Scene/ResultScene.h"
#include<memory>

// シーン数
constexpr uint32_t sceneNum = 4;

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