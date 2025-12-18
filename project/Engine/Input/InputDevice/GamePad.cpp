#include "GamePad.h"
#include <algorithm>
#include <cstring> // memset
#include <cmath>   // sqrt

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

// スティックがアクティブ（デッドゾーンを超えて倒されている）状態かを判定
bool GamePad::IsStickActive(float x, float y) const {
    float magnitude = sqrtf(x * x + y * y);
    return magnitude > kStickActiveThreshold;
}

// 方向別スティック状態判定
bool GamePad::IsStickDirection(float x, float y, DownStick direction) const {
    switch (direction) {
    case ALL_STICK:
        return IsStickActive(x, y);
    case UP_STICK:
        return y > kStickDirectionThreshold;
    case DOWN_STICK:
        return y < -kStickDirectionThreshold;
    case LEFT_STICK:
        return x < -kStickDirectionThreshold;
    case RIGHT_STICK:
        return x > kStickDirectionThreshold;
    default:
        return false;
    }
}

void GamePad::Initialize(DWORD padIndex) {
    padIndex_ = padIndex;
    connected_ = false;
    std::memset(buttons_, 0, sizeof(buttons_));
    std::memset(buttonsPrev_, 0, sizeof(buttonsPrev_));
    leftStickX_ = leftStickY_ = rightStickX_ = rightStickY_ = 0.0f;
    leftStickXPrev_ = leftStickYPrev_ = rightStickXPrev_ = rightStickYPrev_ = 0.0f;
    leftTrigger_ = rightTrigger_ = 0.0f;
}

void GamePad::Update() {
    // 前フレームを保存
    std::memcpy(buttonsPrev_, buttons_, sizeof(buttons_));
    
    // スティックの前フレーム値を保存
    leftStickXPrev_ = leftStickX_;
    leftStickYPrev_ = leftStickY_;
    rightStickXPrev_ = rightStickX_;
    rightStickYPrev_ = rightStickY_;

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
        buttons_[L_STICK] = (b & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
        buttons_[R_STICK] = (b & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
        buttons_[UP_BOTTON] = (b & XINPUT_GAMEPAD_DPAD_UP) != 0;
        buttons_[DOWN_BOTTON] = (b & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
        buttons_[LEFT_BOTTON] = (b & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
        buttons_[RIGHT_BOTTON] = (b & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

        if (isVib_) {
            XINPUT_VIBRATION vib{};
            vib.wRightMotorSpeed = static_cast<WORD>(vibValue_.x * 65535.0f);
            vib.wLeftMotorSpeed = static_cast<WORD>(vibValue_.y * 65535.0f);
            XInputSetState(padIndex_, &vib);
            timer_ += deltaTime_;
            if (timer_ >= time_) {
                timer_ = 0.0f;
                isVib_ = false;
            }
        } else {
            XINPUT_VIBRATION vib{};
            vib.wRightMotorSpeed = static_cast<WORD>(0.0f * 65535.0f);
            vib.wLeftMotorSpeed = static_cast<WORD>(0.0f * 65535.0f);
            XInputSetState(padIndex_, &vib);
        }
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

// 左スティックが倒された瞬間
bool GamePad::TriggerLeftStick() const {
    bool currentActive = IsStickActive(leftStickX_, leftStickY_);
    bool prevActive = IsStickActive(leftStickXPrev_, leftStickYPrev_);
    return currentActive && !prevActive;
}

// 左スティックが中央に戻った瞬間
bool GamePad::ReleaseLeftStick() const {
    bool currentActive = IsStickActive(leftStickX_, leftStickY_);
    bool prevActive = IsStickActive(leftStickXPrev_, leftStickYPrev_);
    return !currentActive && prevActive;
}

// 右スティックが倒された瞬間
bool GamePad::TriggerRightStick() const {
    bool currentActive = IsStickActive(rightStickX_, rightStickY_);
    bool prevActive = IsStickActive(rightStickXPrev_, rightStickYPrev_);
    return currentActive && !prevActive;
}

// 右スティックが中央に戻った瞬間
bool GamePad::ReleaseRightStick() const {
    bool currentActive = IsStickActive(rightStickX_, rightStickY_);
    bool prevActive = IsStickActive(rightStickXPrev_, rightStickYPrev_);
    return !currentActive && prevActive;
}

// 左スティックの方向別瞬間判定
bool GamePad::TriggerLeftStick(DownStick direction) const {
    bool currentDirection = IsStickDirection(leftStickX_, leftStickY_, direction);
    bool prevDirection = IsStickDirection(leftStickXPrev_, leftStickYPrev_, direction);
    return currentDirection && !prevDirection;
}

// 右スティックの方向別瞬間判定
bool GamePad::TriggerRightStick(DownStick direction) const {
    bool currentDirection = IsStickDirection(rightStickX_, rightStickY_, direction);
    bool prevDirection = IsStickDirection(rightStickXPrev_, rightStickYPrev_, direction);
    return currentDirection && !prevDirection;
}

// 左スティックの方向別押下中判定
bool GamePad::PushLeftStick(DownStick direction) const {
    return IsStickDirection(leftStickX_, leftStickY_, direction);
}

// 右スティックの方向別押下中判定
bool GamePad::PushRightStick(DownStick direction) const {
    return IsStickDirection(rightStickX_, rightStickY_, direction);
}

void GamePad::SetVibration(float left01, float right01, float Time) {
    if (!connected_) return;
    left01 = std::clamp(left01, 0.0f, 1.0f);
    right01 = std::clamp(right01, 0.0f, 1.0f);
    vibValue_.x = right01;
    vibValue_.y = left01;
    time_ = Time;
    isVib_ = true;
}