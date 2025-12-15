#pragma once
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "Math/Type/Vector2.h"

class GamePad {
public:
    // 初期化（パッド番号はデフォルト0）
    void Initialize(DWORD padIndex = 0);

    // 毎フレーム更新
    void Update();

    // --- ボタン判定 ---
    //押下中
    bool PushButton(int button) const;
    // 押した瞬間（立ち上がり）
    bool TriggerButton(int button) const;
    // 離した瞬間（立ち下がり）
    bool ReleaseButton(int button) const;

    //  --- スティック瞬間判定 ---
    // 左スティックが倒された瞬間（中央→倒した状態）
    bool TriggerLeftStick() const;
    // 左スティックが中央に戻った瞬間（倒した状態→中央）
    bool ReleaseLeftStick() const;
    // 右スティックが倒された瞬間（中央→倒した状態")
    bool TriggerRightStick() const;
    // 右スティックが中央に戻った瞬間（倒した状態→中央）
    bool ReleaseRightStick() const;

    enum DownStick {
        ALL_STICK,
        UP_STICK,
        DOWN_STICK,
        LEFT_STICK,
        RIGHT_STICK,
    };

    // --- 方向別スティック判定 ---
    // 左スティックの方向別瞬間判定
    bool TriggerLeftStick(DownStick direction) const;
    // 右スティックの方向別瞬間判定
    bool TriggerRightStick(DownStick direction) const;
    // 左スティックの方向別押下中判定
    bool PushLeftStick(DownStick direction) const;
    // 右スティックの方向別押下中判定
    bool PushRightStick(DownStick direction) const;

    // --- 便利アクセサ ---
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
        L_STICK, R_STICK,
        UP_BOTTON, DOWN_BOTTON, LEFT_BOTTON, RIGHT_BOTTON,
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

    // アナログ（現在値）
    float leftStickX_ = 0.0f, leftStickY_ = 0.0f;
    float rightStickX_ = 0.0f, rightStickY_ = 0.0f;
    float leftTrigger_ = 0.0f, rightTrigger_ = 0.0f;

    // アナログ（前フレーム値）
    float leftStickXPrev_ = 0.0f, leftStickYPrev_ = 0.0f;
    float rightStickXPrev_ = 0.0f, rightStickYPrev_ = 0.0f;

    // ボタン（今/前）
    bool buttons_[BUTTON_COUNT] = {};
    bool buttonsPrev_[BUTTON_COUNT] = {};

    // デッドゾーン
    static constexpr float kLXDeadZone = float(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);   // 7849
    static constexpr float kLYDeadZone = float(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    static constexpr float kRXDeadZone = float(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);  // 8689
    static constexpr float kRYDeadZone = float(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    static constexpr float kTrigThresh = float(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);     // 30

    // スティック判定しきい値（デッドゾーンを超えた状態を「倒されている」と判定）
    static constexpr float kStickActiveThreshold = 0.1f;
    // 方向判定しきい値（この値以上で各方向に「倒されている」と判定）
    static constexpr float kStickDirectionThreshold = 0.5f;

    // 正規化ヘルパ
    static float NormalizeStick(short v, float deadzone);
    
    // スティック状態判定ヘルパ
    bool IsStickActive(float x, float y) const;
    // 方向別スティック状態判定ヘルパ
    bool IsStickDirection(float x, float y, DownStick direction) const;
};