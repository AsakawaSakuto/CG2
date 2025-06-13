#include "Input.h"
#include <cassert> 

void Input::Initialize(WinApp* winApp) {

	this->winApp_ = winApp;

	result_ = DirectInput8Create(winApp_->GetInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result_));

	result_ = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result_));

	result_ = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result_));

	result_ = keyboard_->SetCooperativeLevel(winApp_->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result_));
}

void Input::Update() {

	memcpy(keyPre_, key_, sizeof(key_));

	// キーボードの取得開始
	keyboard_->Acquire();
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウスボタンの前フレーム値保存
	preMouseL_ = isMouseL_;
	preMouseR_ = isMouseR_;

	// WinAppから現在のボタン状態を取得
	isMouseL_ = winApp_->IsLButtonDown();
	isMouseR_ = winApp_->IsRButtonDown();

	// ホイール入力取得
	wheelDelta_ = static_cast<float>(winApp_->GetWheelDelta()) / WHEEL_DELTA;
	winApp_->ResetWheelDelta();
}

bool Input::PushKey(BYTE keyNumber) {
	if (key_[keyNumber])
	{
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	if (key_[keyNumber] && !keyPre_[keyNumber])
	{
		return true;
	}
	return false;
}