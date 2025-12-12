#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>

#include"Math/Type/Vector2.h"
#include"Core/WinApp/WinApp.h"

class Input
{
public:
	void Initialize(WinApp* winApp);
	void Update();
	bool PushKey(int keyNumber);
	bool TriggerKey(int keyNumber);
	float GetWheelDelta() const { return wheelDelta_; }

	// 左右押下状態（Push）
	bool PushMouseButtonL() const { return isMouseL_; }
	bool PushMouseButtonR() const { return isMouseR_; }

	// 左右トリガー状態（Trigger）
	bool TriggerMouseButtonL() const { return (isMouseL_ && !preMouseL_); }
	bool TriggerMouseButtonR() const { return (isMouseR_ && !preMouseR_); }

	bool ReleaseMouseButtonL() const;
	bool ReleaseMouseButtonR() const;

	void SetMousePosition(LONG x, LONG y);
	Vector2 GetMouseDelta() const;

	Vector2 GetMousePos() { return currentMousePos_; }
private:
	WinApp* winApp_ = nullptr;
	HRESULT result_;
	BYTE key_[256] = {};
	BYTE keyPre_[256] = {};
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;
	float wheelDelta_ = 0.0f;

	bool isMouseL_ = false;
	bool isMouseR_ = false;
	bool preMouseL_ = false;
	bool preMouseR_ = false;

	Vector2 currentMousePos_ = {};
	Vector2 previousMousePos_ = {};
};