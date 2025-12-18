#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

enum class BulletType {
    None,
    Penetration,
    Bounce,
};

class Bullet : public BaseGameObject {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;
    
    // デストラクタを追加（unique_ptrの確実な解放を保証）
    ~Bullet() override = default;

    void SetPosition(const Vector3& position);
    void SetDirectionToEnemy(const Vector3& direction);
	void SetSpeed(float speed) { speed_ = speed; }

	BulletType GetBulletType() const { return bulletType_; }

    void LoadJson(const std::string& filePath, const std::string& filePath2 = "none") {
        particle_->LoadJson(filePath);
        particle2_->LoadJson(filePath2);
    }

    // 貫通カウント関連
    void SetPenetrationCount(int count) { penetrationCount_ = count; }
    void DecrementPenetrationCount() { 
        if (penetrationCount_ > 0) {
            penetrationCount_--;
        }
    }
    
    // 反射カウント関連
    void SetBounceCount(int count) { bounceCount_ = count; }
    void DecrementBounceCount() {
        if (bounceCount_ > 0) {
            bounceCount_--;

            Random rand;
			int n = rand.Int(0,2);

            if (n == 0) {
                directionToEnemy_.x *= -1.0f;
            }

            if (n == 1) {
                directionToEnemy_.z *= -1.0f;
            }

            if (n == 2) {
                directionToEnemy_.x *= -1.0f;
                directionToEnemy_.z *= -1.0f;
            }
        }
    }

private:
	unique_ptr<Particles> particle_ = make_unique<Particles>();
    unique_ptr<Particles> particle2_ = make_unique<Particles>();
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
	GameTimer lifeTimer_;
    int penetrationCount_ = 0; // 貫通カウント
    int bounceCount_ = 0;

	float speed_ = 25.0f;
	BulletType bulletType_ = BulletType::None;
};