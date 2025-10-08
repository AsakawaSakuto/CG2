#pragma once
#include"AppGigaBonk/GameUI/GameUI.h"

class TempUI : public GameUI
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw() override;
	~TempUI() {}
private:

};