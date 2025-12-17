#pragma once
#include"IScene.h"
#include"SceneManager/Test/TestScene.h"

#include"SceneManager/Scene/TiTleScene.h"
#include"SceneManager/Scene/GameScene.h"
#include"SceneManager/Scene/ResultScene.h"

#include"KeyConfig/InputDevice/Input.h"
#include"KeyConfig/InputDevice/GamePad.h"

#include<memory>

// シーン数
constexpr uint32_t sceneNum = 4;

class SceneManager {
private:
    std::unique_ptr<IScene> sceneArr_[sceneNum];
    SCENE currentSceneNo_;
    SCENE prevSceneNo_;

    std::unique_ptr<IScene> CreateScene(SCENE sceneNo);

    void Initialize();

    void Update();

    void Finalize();

    void Shortcut();
public:
    SceneManager();
    ~SceneManager();
    int Run();  // メインループ
    std::unique_ptr<AppContext> appContext_;
    std::unique_ptr<WinApp> winApp_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<GamePad> gamePad_;
};