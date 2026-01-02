#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"
#include <unordered_set>

class Runa : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Runa() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

    void Bounce() {
        bounceCount_--;
        if (bounceCount_ > 0) {
			int r = MyRand::Int(0, 2);
            if (r == 0) { directionToEnemy_.x *= -1.0f; }
            if (r == 1) { directionToEnemy_.z *= -1.0f; }
            if (r == 2) { directionToEnemy_.x *= -1.0f; directionToEnemy_.z *= -1.0f; }
            
            // バウンス後は衝突履歴をクリア（新しい敵と衝突できるように）
            hitEnemies_.clear();
        } else {
			Dead();
        }
    }

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

    float speed_ = 10.0f;
    
    // 既に衝突した敵を追跡（ポインタをキーとして使用）
    std::unordered_set<const void*> hitEnemies_;
};