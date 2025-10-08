#pragma once
#include"AppGigaBonk/EngineSystem.h"

using std::unique_ptr;
using std::make_unique;

/// <summary>
/// ゲームで使う3Dオブジェクトの基底クラス
/// </summary>
class GameObject {
public:
    virtual void Initialize(DirectXCommon* dxCommon) = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera useCamera) = 0;
    virtual ~GameObject() {}
    Sphere GetCollitionSphere() { return CollitionSphere_; }
    AABB GetCollisionAABB() { return CollisionAABB_; }
    bool GetIsAlive() const { return isAlive_; }
    void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
protected:
    unique_ptr<Model> model_ = make_unique<Model>();
    DirectXCommon* dxCommon_ = nullptr;

    Transform transform_;
    Sphere CollitionSphere_;
    AABB CollisionAABB_;
    bool isAlive_;
};