#pragma once
#include "Application/GameObject/GameObject.h"
#include "Application/GameObject/Thorn/Particle/ThornParticle.h"

enum class ThornType {
	MIN = 0,
	MIDDLE = 1,
	MAX = 2,
};

class Thorn : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Thorn() {}

	bool GetIsAlive() const { return isAlive_; }

	Transform& GetTransform() { return transform_; }

	// 生成時の位置設定
	void Spawn(Vector3 position);

	// Getter
	ThornType GetThornType() const { return type_; }
	int GetScoreAmount() const { return scoreAmount_; }
	bool CanUpgradeBullet() const { return upgradeCooldownFramesBullet_ <= 0; }
	bool CanUpgradeWing() const { return upgradeCooldownFramesWing_ <= 0; }
	Vector3 GetPosition() const { return transform_.translate; }
	Model* GetModel() const { return model_.get(); }

	// Setter
	void SetPosition(Vector3 position) { transform_.translate = position; }
	void SetThornType(ThornType type);
	void SetUpgradeCooldownBullet(int frames) { upgradeCooldownFramesBullet_ = frames; }
	void SetUpgradeCooldownWing(int frames) { upgradeCooldownFramesWing_ = frames; }
	void SetIsRotate(bool isRotate) { isRotate_ = isRotate; }
	void SetIsShaking(bool isShaking) { isShaking_ = isShaking; }

	void PlayParticle(uint32_t num) { particle_->Play(transform_.translate, num); }
	void UpdateParticle() { particle_->Update(); }
	void DrawParticle(Camera useCamera) { particle_->Draw(useCamera); }

	Vector3 GetColor(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return particle_->GetColor(index);
	}

	Vector3 GetTranslate(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return particle_->GetTranslate(index);
	}

	bool GetLifeTimerFinish(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return  particle_->GetLifeTimerFinish(index);
	}

	bool GetLifeTimerActive(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return  particle_->GetLifeTimerActive(index);
	}

	void ParticleReset(int index) {
		particle_->ParticleReset(index);
	}

	// クールタイムの更新
	void TickCooldown();

private:
	struct ScaleTween {
		Vector3 startScale;
		Vector3 targetScale;
		float duration;
		float elapsed;
		bool active = false;
	};

private:
	// トゲのレベルアップ
	void UpgradeThorn();

	// モデルの回転
	void UpdateRotate();

private:
	// トゲのタイプ
	ThornType type_ = ThornType::MIN;

	// スコア加算量
	int scoreAmount_ = 100;

	// トゲのレベルアップクールタイム
	int upgradeCooldownFramesBullet_ = 0;

	// 羽のスコアアップクールタイム
	int upgradeCooldownFramesWing_ = 0;

	unique_ptr<ThornParticle> particle_ = make_unique<ThornParticle>();

	// スケールを変更時にイージングをかけるための変数
	ScaleTween scaleTween_;

	// 回転のフラグ
	bool isRotate_;

	// 回転速度
	float rotateSpeed_ = 100.0f;

	// 左右に揺れる処理関連の変数
	float shakeElapsed_ = 0.0f;
	float shakeDuration_ = 1.5f;  // 揺れ時間
	float shakeAmplitude_ = 0.3f; // 揺れの最大幅
	bool isShaking_ = false;
	Vector3 basePosition_; // 揺れ前の基準位置
};
