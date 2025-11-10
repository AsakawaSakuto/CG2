#pragma once
#include"Application/AppContext.h"
#include"AABB.h"
#include"Sphere.h"
#include"Transform.h"
#include"Camera.h"

class BaseGameObject {
protected:
    AppContext* ctx_;

	Transform transform_;
    AABB aabbCollision_;
	Sphere sphereCollision_;
public:
    virtual void Initialize(AppContext* ctx) = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera camera) = 0;
    virtual void DrawImGui() = 0;
	virtual const AABB& GetAABBCollision() { return aabbCollision_; }
	virtual const Sphere& GetSphereCollision() { return sphereCollision_; }
    virtual ~BaseGameObject() {}
};