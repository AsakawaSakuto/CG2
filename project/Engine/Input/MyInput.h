#pragma once
#include "InputManager.h"

namespace MyInput {
	// Trigger判定（押した瞬間）
	inline bool Trigger(Action action) {
		return InputManager::GetInstance()->TriggerAction(action);
	}

	// Push判定（押し続けている）
	inline bool Push(Action action) {
		return InputManager::GetInstance()->PushAction(action);
	}

	// Release判定（離した瞬間")
	inline bool Release(Action action) {
		return InputManager::GetInstance()->ReleaseAction(action);
	}

	// アナログ値取得（0.0〜1.0）
	inline float Value(Action action) {
		return InputManager::GetInstance()->GetActionValue(action);
	}

	// 2D入力取得（-1.0〜1.0）
	inline InputManager::Vector2D GetVector2D(Action action) {
		return InputManager::GetInstance()->GetActionVector2D(action);
	}

	// 入力デバイス取得
	inline Input* GetInput() {
		return InputManager::GetInstance()->GetInput();
	}

	// ゲームパッドを接続しているか取得
	inline bool UseGamePad() {
		return InputManager::GetInstance()->IsGamePadConnected();
	}

	// Trigger（Key）判定
	inline bool TriggerKey(uint32_t key) {
		return InputManager::GetInstance()->GetInput()->TriggerKey(key);
	}

	// Push（Key）判定
	inline bool PushKey(uint32_t key) {
		return InputManager::GetInstance()->GetInput()->PushKey(key);
	}
}