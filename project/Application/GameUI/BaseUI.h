#pragma once

class BaseUI {
public:
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawImGui() = 0;
};