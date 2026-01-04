#pragma once
#include "EngineSystem.h"

class UpgradeManager
{
public:

	void Initialize();

	void Update();

	void Draw();

	void DrawImGui();

	void Upgrade();
private:
	std::unique_ptr<Sprite> upgradeSelect_;
	std::unique_ptr<Sprite> upgradeBG1_;
	std::unique_ptr<Sprite> upgradeBG2_;
	std::unique_ptr<Sprite> upgradeBG3_;
	std::unique_ptr<Sprite> upgradeIcon1_;
	std::unique_ptr<Sprite> upgradeIcon2_;
	std::unique_ptr<Sprite> upgradeIcon3_;
};