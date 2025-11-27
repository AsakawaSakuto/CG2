#pragma once
#include"Application/AppContext.h"
#include"AABB.h"
#include"Sphere.h"
#include"Transform.h"
#include"Camera.h"
#include"Engine/3d/Line/Line.h"

class BaseGameObject {
protected:
    AppContext* ctx_;

    bool isAlive_ = true;

	Transform transform_;
    AABB aabbCollision_;
	Sphere sphereCollision_;

	std::unique_ptr<Line> debugLine_ = std::make_unique<Line>();
public:
    virtual void Initialize(AppContext* ctx) = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera camera) = 0;
    virtual void DrawImGui() {};

	virtual const Vector3& GetPosition() { return transform_.translate; }

	virtual const AABB& GetAABBCollision() { return aabbCollision_; }
	virtual const Sphere& GetSphereCollision() { return sphereCollision_; }

	virtual bool IsAlive() const { return isAlive_; }
    virtual void Dead() { isAlive_ = false; }

    virtual ~BaseGameObject() {}
};