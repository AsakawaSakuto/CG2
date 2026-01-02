#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"
#include <unordered_set>

class Laser : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Laser() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

    void DecrementPenetrationCount() {
        if (penetrationCount_ > 0) {
            penetrationCount_--;
        }
    }
	int GetPenetrationCount() const { return penetrationCount_; }

    // 衝突済みの敵かチェック
    bool HasHitEnemy(const void* enemyPtr) const {
        return hitEnemies_.find(enemyPtr) != hitEnemies_.end();
    }

    // 衝突した敵を記録
    void MarkEnemyAsHit(const void* enemyPtr) {
        hitEnemies_.insert(enemyPtr);
    }

private:
    unique_ptr<Particles> particle_ = make_unique<Particles>();
    unique_ptr<Particles> particle2_ = make_unique<Particles>();
    GameTimer lifeTimer_;

    float speed_ = 15.0f;
    
    // 既に衝突した敵を追跡（ポインタをキーとして使用）
    std::unordered_set<const void*> hitEnemies_;
};