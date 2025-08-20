#pragma once
#include"IScene.h"

#include"TitleScene.h"
#include"TutorialScene.h"
#include"GameScene.h"

#include<memory>

class SceneManager {
private:
    std::unique_ptr<IScene> sceneArr_[3]; // 最大4シーン想定
    int currentSceneNo_;
    int prevSceneNo_;
public:
    SceneManager();
    ~SceneManager();
    int Run();  // メインループ
    std::unique_ptr<AppContext> appContext_;
};