#pragma once
#include "AppContext.h"

class BaseUI {
protected:
    AppContext* ctx_;
public:
    virtual void Initialize(AppContext* ctx) = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawImGui() = 0;
    virtual ~BaseUI() {}
};