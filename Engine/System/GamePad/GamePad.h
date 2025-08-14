#pragma once
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "Vector2.h"

class GamePad {
public:
    // 初期化（パッド番号はデフォルト0）
    void Initialize(DWORD padIndex = 0);

    // 毎フレーム更新
    void Update();

    // // --- ボタン判定 ---
    // 押下中
    bool PushButton(int button) const;
    // 押した瞬間（立ち上がり）
    bool TriggerButton(int button) const;
    // 離した瞬間（立ち下がり）
    bool ReleaseButton(int button) const;

    // // --- 便利アクセサ ---
    bool IsConnected() const { return connected_; }
    float LeftStickX()  const { return leftStickX_; }
    float LeftStickY()  const { return leftStickY_; }
    float RightStickX() const { return rightStickX_; }
    float RightStickY() const { return rightStickY_; }
    float LeftTrigger() const { return leftTrigger_; }   // 0..1
    float RightTrigger()const { return rightTrigger_; }  // 0..1

    // 振動
    void SetVibration(float left01, float right01, float Time); // 0..1

    // ボタンのインデックス定義（使いやすいように公開）
    enum Button {
        A = 0, B, X, Y,
        L, R,
        BACK, START,
        LS, RS,
        DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT,
        BUTTON_COUNT
    };

private:
    // 内部状態
    DWORD padIndex_ = 0;
    bool connected_ = false;

    float deltaTime_ = 1.0f / 60.0f;
    float time_ = 0.0f;
    float timer_ = 0.0f;
    bool isVib_ = false;
    Vector2 vibValue_ = {};

    // アナログ
    float leftStickX_ = 0.0f, leftStickY_ = 0.0f;
    float rightStickX_ = 0.0f, rightStickY_ = 0.0f;
    float leftTrigger_ = 0.0f, rightTrigger_ = 0.0f;

    // ボタン（今/前）
    bool buttons_[BUTTON_COUNT] = {};
    bool buttonsPrev_[BUTTON_COUNT] = {};

    // デッドゾーン
    static constexpr float kLXDeadZone = float(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);   // 7849
    static constexpr float kLYDeadZone = float(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    static constexpr float kRXDeadZone = float(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);  // 8689
    static constexpr float kRYDeadZone = float(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    static constexpr float kTrigThresh = float(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);     // 30

    // 正規化ヘルパ
    static float NormalizeStick(short v, float deadzone);
};