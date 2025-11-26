#pragma once
#include "Application/EngineSystem.h"
#include "Application/GameObject/BaseGameObject.h"

class Bullet : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    void SetPosition(const Vector3& position);
    void SetDirectionToEnemy(const Vector3& direction);
    
    void StopParticle() {
        if (particle_) {
            particle_->Stop();
        }
	}

    // 共有パーティクルを設定
    void SetSharedParticle(std::shared_ptr<Particles> particle) {
        particle_ = particle;
        if (particle_) {
            // パーティクルの位置を現在のBullet位置に設定してから再生
			particle_->Stop();
            particle_->SetEmitterPosition(transform_.translate);
            particle_->Play(true); // ワンショット再生
        }
    }
    
    // パーティクルを解放して返す（プール返却用）
    std::shared_ptr<Particles> ReleaseParticle() {
        return std::move(particle_);
    }

private:
    // shared_ptrに変更して共有可能にする
    std::shared_ptr<Particles> particle_;
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
	GameTimer lifeTimer_;
};