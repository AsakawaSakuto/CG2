#pragma once
#include "Application/EngineSystem.h"

using std::unique_ptr;
using std::make_unique;

/// <summary>
/// ゲームで使うUIの基底クラス
/// </summary>
class GameUI {
public:
    virtual void Initialize(DirectXCommon* dxCommon) = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual ~GameUI() {}
protected:
    DirectXCommon* dxCommon_ = nullptr;
};