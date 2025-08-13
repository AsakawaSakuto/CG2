#include "GamePad.h"
#include <algorithm>
#include <cstring> // memset

// // スティックの -32768..32767 を -1..1 に正規化（デッドゾーン反映）
float GamePad::NormalizeStick(short v, float deadzone) {
    float fv = static_cast<float>(v);
    float av = fabsf(fv);
    if (av < deadzone) return 0.0f;
    // デッドゾーン外を 0..1 に再マップ
    float sign = (fv >= 0.0f) ? 1.0f : -1.0f;
    float nv = (av - deadzone) / (32767.0f - deadzone);
    nv = std::clamp(nv, 0.0f, 1.0f);
    return sign * nv;
}

void GamePad::Initialize(DWORD padIndex) {
    padIndex_ = padIndex;
    connected_ = false;
    std::memset(buttons_, 0, sizeof(buttons_));
    std::memset(buttonsPrev_, 0, sizeof(buttonsPrev_));
    leftStickX_ = leftStickY_ = rightStickX_ = rightStickY_ = 0.0f;
    leftTrigger_ = rightTrigger_ = 0.0f;
}

void GamePad::Update() {
    // 前フレームを保存
    std::memcpy(buttonsPrev_, buttons_, sizeof(buttons_));

    XINPUT_STATE xi{};
    DWORD dw = XInputGetState(padIndex_, &xi);
    if (dw == ERROR_SUCCESS) {
        connected_ = true;

        // スティック（デッドゾーン込み正規化 -1..1）
        leftStickX_ = NormalizeStick(xi.Gamepad.sThumbLX, kLXDeadZone);
        leftStickY_ = NormalizeStick(xi.Gamepad.sThumbLY, kLYDeadZone);
        rightStickX_ = NormalizeStick(xi.Gamepad.sThumbRX, kRXDeadZone);
        rightStickY_ = NormalizeStick(xi.Gamepad.sThumbRY, kRYDeadZone);

        // トリガー（0..255 → 0..1、しきい値で打ち切り）
        leftTrigger_ = (xi.Gamepad.bLeftTrigger <= kTrigThresh) ? 0.0f : (xi.Gamepad.bLeftTrigger - kTrigThresh) / (255.0f - kTrigThresh);
        rightTrigger_ = (xi.Gamepad.bRightTrigger <= kTrigThresh) ? 0.0f : (xi.Gamepad.bRightTrigger - kTrigThresh) / (255.0f - kTrigThresh);

        // ボタンビット
        WORD b = xi.Gamepad.wButtons;
        buttons_[A] = (b & XINPUT_GAMEPAD_A) != 0;
        buttons_[B] = (b & XINPUT_GAMEPAD_B) != 0;
        buttons_[X] = (b & XINPUT_GAMEPAD_X) != 0;
        buttons_[Y] = (b & XINPUT_GAMEPAD_Y) != 0;
        buttons_[L] = (b & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
        buttons_[R] = (b & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
        buttons_[BACK] = (b & XINPUT_GAMEPAD_BACK) != 0;
        buttons_[START] = (b & XINPUT_GAMEPAD_START) != 0;
        buttons_[LS] = (b & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
        buttons_[RS] = (b & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
        buttons_[DPAD_UP] = (b & XINPUT_GAMEPAD_DPAD_UP) != 0;
        buttons_[DPAD_DOWN] = (b & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
        buttons_[DPAD_LEFT] = (b & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
        buttons_[DPAD_RIGHT] = (b & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
    }
    else {
        connected_ = false;
        // 接続が切れたら全リセット
        std::memset(buttons_, 0, sizeof(buttons_));
        leftStickX_ = leftStickY_ = rightStickX_ = rightStickY_ = 0.0f;
        leftTrigger_ = rightTrigger_ = 0.0f;
    }
}

bool GamePad::PushButton(int button) const {
    // 押下中
    if (button < 0 || button >= BUTTON_COUNT) return false;
    return buttons_[button];
}

bool GamePad::TriggerButton(int button) const {
    // 立ち上がり（今true && 前false）
    if (button < 0 || button >= BUTTON_COUNT) return false;
    return buttons_[button] && !buttonsPrev_[button];
}

bool GamePad::ReleaseButton(int button) const {
    // 立ち下がり（今false && 前true）
    if (button < 0 || button >= BUTTON_COUNT) return false;
    return !buttons_[button] && buttonsPrev_[button];
}

void GamePad::SetVibration(float left01, float right01) {
    if (!connected_) return;
    left01 = std::clamp(left01, 0.0f, 1.0f);
    right01 = std::clamp(right01, 0.0f, 1.0f);
    XINPUT_VIBRATION vib{};
    vib.wLeftMotorSpeed = static_cast<WORD>(left01 * 65535.0f);
    vib.wRightMotorSpeed = static_cast<WORD>(right01 * 65535.0f);
    XInputSetState(padIndex_, &vib);
}