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

    // // 前フレームのキーボード状態をバックアップ
    memcpy(keyPre_, key_, sizeof(key_));

    // ===== キーボード（失敗時はゼロクリアで安全側） =====
    if (keyboard_) {
        HRESULT hr = keyboard_->Acquire();
        if (SUCCEEDED(hr)) {
            hr = keyboard_->GetDeviceState(sizeof(key_), key_);
        }
        if (FAILED(hr)) {
            // // フォーカス喪失/入力ロスト等。落とさず0埋めで次フレーム再取得。
            ZeroMemory(key_, sizeof(key_));
        }
    } else {
        ZeroMemory(key_, sizeof(key_));
    }

    // ===== マウス座標（グローバル→クライアント。ウィンドウ外でも更新される） =====
    previousMousePos_ = currentMousePos_;
    POINT pt{};
    if (::GetCursorPos(&pt)) {
        ::ScreenToClient(winApp_->GetHWND(), &pt);
        currentMousePos_.x = static_cast<float>(pt.x);
        currentMousePos_.y = static_cast<float>(pt.y);
    }

    // // 画面内に留めたい場合は任意でクランプ
    // currentMousePos_.x = std::clamp(currentMousePos_.x, 0.0f, (float)WinApp::kClientWidth_  - 1.0f);
    // currentMousePos_.y = std::clamp(currentMousePos_.y, 0.0f, (float)WinApp::kClientHeight_ - 1.0f);

    // ===== マウスボタン（WinApp依存をやめてグローバル取得） =====
    preMouseL_ = isMouseL_;
    preMouseR_ = isMouseR_;
    isMouseL_ = (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    isMouseR_ = (::GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

    // ===== ホイール（これはメッセージ由来のまま） =====
    wheelDelta_ = static_cast<float>(winApp_->GetWheelDelta()) / WHEEL_DELTA;
    winApp_->ResetWheelDelta();
}

bool Input::PushKey(int keyNumber) {
	// // ここで範囲外を弾く（0..255のみ）
	if (keyNumber < 0 || keyNumber > 255) { return false; }
	return key_[keyNumber] != 0;
}

bool Input::TriggerKey(int keyNumber) {
	if (keyNumber < 0 || keyNumber > 255) { return false; }
	return (key_[keyNumber] && !keyPre_[keyNumber]);
}

void Input::SetMousePosition(LONG x, LONG y) {
	previousMousePos_ = currentMousePos_;
	currentMousePos_ = { static_cast<float>(x), static_cast<float>(y) };
}

Vector2 Input::GetMouseDelta() const {
	return {
		currentMousePos_.x - previousMousePos_.x,
		currentMousePos_.y - previousMousePos_.y
	};
}