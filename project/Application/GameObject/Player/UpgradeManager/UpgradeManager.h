#pragma once
#include "EngineSystem.h"
#include "UpgradeType.h"

class UpgradeManager {
public:

	void Initialize();

	void Update();

	void Draw();

	void DrawImGui();

	/// <summary>
	/// Playerがレベルアップしたときに呼び出す
	/// 強化の抽選を開始する
	/// まだ装備してい武器と、装備してる武器の強化を選択肢に含める
	/// 選択肢に出てくる武器はランダムに決定するが3つの選択し内で被ることはない
	/// </summary>
	void Upgrade() {
		upgradeSelect_ = UpgradeSelect::Select1;
		isUpgrade_ = true;
	};

	bool IsUpgradeSelect() const { return isUpgrade_; }
private:

	// アップグレード選択中かどうか
	bool isUpgrade_ = false;

	UpgradeSelect upgradeSelect_ = UpgradeSelect::Select1;

	std::unique_ptr<Sprite> upgradeSelectText_;
	std::unique_ptr<Sprite> upgradeBG1_;
	std::unique_ptr<Sprite> upgradeBG2_;
	std::unique_ptr<Sprite> upgradeBG3_;
	std::unique_ptr<Sprite> upgradeIcon1_;
	std::unique_ptr<Sprite> upgradeIcon2_;
	std::unique_ptr<Sprite> upgradeIcon3_;
	std::unique_ptr<Sprite> bg_;
};