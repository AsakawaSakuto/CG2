#pragma once
#include"IScene.h"

#include"TestScene.h"
#include"SecondTestScene.h"

#include<memory>

class SceneManager {
private:
    std::unique_ptr<IScene> sceneArr_[2]; // 最大4シーン想定
    int currentSceneNo_;
    int prevSceneNo_;
public:
    SceneManager();
    ~SceneManager();
    int Run();  // メインループ
    std::unique_ptr<AppContext> appContext_;
};