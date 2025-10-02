#pragma once
#include"Application/AppContext.h"

enum SCENE {
    TEMP,
    TITLE,
    GAME
};

class IScene {
protected:
    static int sceneNo;
public:
    virtual void SetAppContext(AppContext* ctx) = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual ~IScene() {}
    int GetSceneNo();
};