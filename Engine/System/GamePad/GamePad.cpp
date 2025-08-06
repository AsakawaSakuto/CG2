#include "GamePad.h"

void GamePad::Initialize() {

}

void GamePad::Update() {

	GamePadState pad = {};
	XINPUT_STATE xinputState = {};

	DWORD dwResult = XInputGetState(0, &xinputState); // コントローラー0を取得
	if (dwResult == ERROR_SUCCESS) {
		pad.connected = true;

		// スティック（-32768 ~ +32767）
		pad.leftStickX = xinputState.Gamepad.sThumbLX / 32767.0f;
		pad.leftStickY = xinputState.Gamepad.sThumbLY / 32767.0f;
		pad.rightStickX = xinputState.Gamepad.sThumbRX / 32767.0f;
		pad.rightStickY = xinputState.Gamepad.sThumbRY / 32767.0f;

		// トリガー（0 ~ 255）
		pad.leftTrigger = xinputState.Gamepad.bLeftTrigger / 255.0f;
		pad.rightTrigger = xinputState.Gamepad.bRightTrigger / 255.0f;

		// ボタン状態
		WORD buttons = xinputState.Gamepad.wButtons;
		pad.buttons[0] = (buttons & XINPUT_GAMEPAD_A) != 0;
		pad.buttons[1] = (buttons & XINPUT_GAMEPAD_B) != 0;
		pad.buttons[2] = (buttons & XINPUT_GAMEPAD_X) != 0;
		pad.buttons[3] = (buttons & XINPUT_GAMEPAD_Y) != 0;
		pad.buttons[4] = (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		pad.buttons[5] = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		pad.buttons[6] = (buttons & XINPUT_GAMEPAD_BACK) != 0;
		pad.buttons[7] = (buttons & XINPUT_GAMEPAD_START) != 0;
		pad.buttons[8] = (buttons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
		pad.buttons[9] = (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
		pad.buttons[10] = (buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		pad.buttons[11] = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		pad.buttons[12] = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		pad.buttons[13] = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
	}
	else {
		pad.connected = false;
	}

	XINPUT_VIBRATION vibration = {};
	if (pad.connected) {
		if (pad.buttons[0]) {
			vibration.wLeftMotorSpeed = static_cast<WORD>(padMotorRange.x * 65535.0f);
			vibration.wRightMotorSpeed = static_cast<WORD>(padMotorRange.y * 65535.0f);
		}
		else if (pad.buttons[4]) {
			vibration.wLeftMotorSpeed = static_cast<WORD>(padMotorRange.x * 65535.0f);
		}
		else if (pad.buttons[5]) {
			vibration.wRightMotorSpeed = static_cast<WORD>(padMotorRange.y * 65535.0f);
		}
		else {
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
		}
	}

	// 反映
	XInputSetState(0, &vibration);

}