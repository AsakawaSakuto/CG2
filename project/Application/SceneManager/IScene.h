#pragma once
#include"Application/AppContext.h"
#include"Application/GameUI/SceneFade/SceneFade.h"

using std::unique_ptr;
using std::make_unique;

enum SCENE {
    TEST,
    TITLE,
    GAME,
    RESULT,
};

class SceneFade;

class IScene {
protected:
    static int sceneNo;

    // AppContext
    AppContext* ctx_ = nullptr;

    // InputSystem
    GamePad* gamePad_ = nullptr;
    Input* input_ = nullptr;

    // Fade
	unique_ptr<SceneFade> sceneFade_;

	void DrawSceneName();

	void ChangeScene(int nextScene) { sceneNo = nextScene; }
public:
    virtual void SetAppContext(AppContext* ctx) = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual ~IScene() {}
    int GetSceneNo();
};