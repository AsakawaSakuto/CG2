#pragma once
#include "Utility/Collision/Type/AABB.h"
#include "Utility/Collision/Type/Sphere.h"
#include "Math/Type/Transform.h"
#include "Camera/Camera.h"
#include "3d/Model/Model.h"
#include "3d/Line/MyDebugLine.h"
#include "Utility/Random/Random.h"
#include "Utility/GameTimer/GameTimer.h"
#include "Utility/Easing/Easing.h"

enum class JarType {
    Exp,
    Money,
};

class BaseJar {
protected:

    std::unique_ptr<Model> model_;
    std::unique_ptr<Model> bottom_;

	bool jarIsActive_ = false;
	Transform bottomTransform_;
    GameTimer timer_;

    Transform transform_;
    AABB aabbCollision_;
    bool isAlive_ = true;
	bool isSpawn_ = false;

    int dropExpMin_;
    int dropExpMax_;
    int dropMoneyMin_;
    int dropMoneyMax_;

	JarType jarType_;

    Vector3 textMin = { 0.8f,0.8f,0.8f };
	Vector3 textMax = { 1.2f,1.2f,1.2f };
public:
    virtual void Initialize(Vector3 pos) = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera camera) = 0;
    virtual int Break() = 0;

    virtual const Vector3& GetPosition() { return transform_.translate; }
    virtual const AABB& GetAABBCollision() { return aabbCollision_; }
	virtual const JarType GetJarType() const { return jarType_; }
    virtual bool IsAlive() const { return isAlive_; }

	virtual void SetJarActive(bool flag) { jarIsActive_ = flag; }

    virtual ~BaseJar() {}
};