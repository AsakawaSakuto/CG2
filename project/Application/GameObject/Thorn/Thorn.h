#pragma once
#include "Application/GameObject/GameObject.h"

enum class ThornType
{
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

	// Setter
	void SetPosition(Vector3 position) { transform_.translate = position; }
	void SetThornType(ThornType type) { type_ = type; }
	void SetUpgradeCooldownBullet(int frames) { upgradeCooldownFramesBullet_ = frames; }
	void SetUpgradeCooldownWing(int frames) { upgradeCooldownFramesWing_ = frames; }

	// クールタイムの更新
	void TickCooldown();

private:
	// トゲのレベルアップ
	void UpgradeThorn();

private:
	// トゲのタイプ
	ThornType type_ = ThornType::MIN;

	// スコア加算量
	int scoreAmount_ = 100;

	// トゲのレベルアップクールタイム
	int upgradeCooldownFramesBullet_ = 0;

	// 羽のスコアアップクールタイム
	int upgradeCooldownFramesWing_ = 0;
};
