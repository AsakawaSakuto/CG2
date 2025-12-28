#pragma once
#include "Utility/Collision/Type/AABB.h"
#include "Utility/Collision/Type/Sphere.h"
#include "Math/Type/Transform.h"
#include "Camera/Camera.h"
#include "3d/Model/Model.h"
#include "3d/Line/MyDebugLine.h"
#include "Utility/Random/Random.h"

class BaseChest {
protected:

    std::unique_ptr<Model> model_;
    Transform transform_;
    AABB aabbCollision_;
    bool isAlive_ = true;
    bool isSpawn_ = false;

public:
    virtual void Initialize(Vector3 pos) = 0;
    virtual void Draw(Camera camera) = 0;
    virtual void Open() = 0;

    virtual const Vector3& GetPosition() { return transform_.translate; }
    virtual const AABB& GetAABBCollision() { return aabbCollision_; }
    virtual bool IsAlive() const { return isAlive_; }

    virtual ~BaseChest() {}
};