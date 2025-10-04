#pragma once
#include"Application/AppContext.h"

enum SCENE {
    TEST,
    TITLE,
    GAME,
    RESULT
};

class IScene {
protected:
    static int sceneNo;

    // AppContext
    AppContext* ctx_ = nullptr;

    // InputSystem
    GamePad* gamePad_ = nullptr;
    Input* input_ = nullptr;

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