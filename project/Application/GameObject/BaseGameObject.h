#pragma once
#include "Utility/Collision/Type/AABB.h"
#include "Utility/Collision/Type/Sphere.h"
#include "Utility/Transform/Transform.h"
#include "Camera/Camera.h"

class BaseGameObject {
protected:

    bool isAlive_ = true;

	Transform transform_;
    AABB aabbCollision_;
	Sphere sphereCollision_;

	float deltaTime_ = 1.0f / 60.0f;
public:
    virtual void Initialize() = 0;
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