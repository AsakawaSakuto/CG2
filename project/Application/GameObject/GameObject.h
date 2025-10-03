#pragma once
#include "Application/EngineSystem.h"

using std::unique_ptr;
using std::make_unique;

class GameObject {
public:
    virtual void Initialize(DirectXCommon* dxCommon) = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera useCamera) = 0;
    virtual ~GameObject() {}
	Sphere GetCollitionSphere() { return CollitionSphere_; }
protected:
    unique_ptr<Model> model_ = make_unique<Model>();
	DirectXCommon* dxCommon_ = nullptr;

	Transform transform_;
	Sphere CollitionSphere_;
};