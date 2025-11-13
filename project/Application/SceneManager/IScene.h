#pragma once
#include"Application/AppContext.h"

using std::unique_ptr;
using std::make_unique;

enum class SCENE {
    TEST = 0,
    TITLE,
    GAME,
    RESULT,
};

class IScene {
protected:
    SCENE nowSceneName_ = SCENE::TEST;

    // AppContext
    AppContext* ctx_ = nullptr;

	void DrawSceneName();

	void ChangeScene(SCENE nextSceneName) { nowSceneName_ = nextSceneName; }

    void Quit();
public:
    virtual void SetAppContext(AppContext* ctx) = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawImGui() = 0;
    virtual ~IScene() {}
    SCENE GetSceneNo();
};