#pragma once
#include"AppContext.h"

enum SCENE {
    TEST = 0,
	SECONDTEST = 1,
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