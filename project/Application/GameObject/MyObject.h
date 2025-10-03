#pragma once

class MyObject {
protected:
    static int sceneNo;
public:
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual ~MyObject() {}
};