#pragma once
#include"IScene.h"
#include"Application/SceneManager/Test/TestScene.h"
#include<memory>

// シーン数
constexpr uint32_t sceneNum = 4;

class SceneManager {
private:
    std::unique_ptr<IScene> sceneArr_[sceneNum];
    SCENE currentSceneNo_;
    SCENE prevSceneNo_;

    // プライベートヘルパーメソッド
    void CleanupAllScenes();
    std::unique_ptr<IScene> CreateScene(SCENE sceneNo);

public:
    SceneManager();
    ~SceneManager();
    int Run();  // メインループ
    std::unique_ptr<AppContext> appContext_;
};