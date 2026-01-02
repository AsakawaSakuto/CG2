#include "InputManager.h"
#include <algorithm>

void InputManager::Initialize() {
	SetupDefaultBindings();
}

void InputManager::SetInputDevices(Input* input, GamePad* gamePad) {
	input_ = input;
	gamePad_ = gamePad;
}

void InputManager::BindAction(Action action, const InputBinding& binding) {
	bindings_[action].clear();
	bindings_[action].push_back(binding);
}

void InputManager::AddAlternativeBinding(Action action, const InputBinding& binding) {
	bindings_[action].push_back(binding);
}

void InputManager::ClearBindings(Action action) {
	bindings_[action].clear();
}

bool InputManager::TriggerAction(Action action) const {
	if (!input_ && !gamePad_) return false;

	auto it = bindings_.find(action);
	if (it == bindings_.end()) return false;

	// いずれかのバインドでTriggerされていればtrue
	for (const auto& binding : it->second) {
		if (CheckBindingTrigger(binding)) {
			return true;
		}
	}
	return false;
}

bool InputManager::PushAction(Action action) const {
	if (!input_ && !gamePad_) return false;

	auto it = bindings_.find(action);
	if (it == bindings_.end()) return false;

	// いずれかのバインドでPushされていればtrue
	for (const auto& binding : it->second) {
		if (CheckBindingPush(binding)) {
			return true;
		}
	}
	return false;
}

bool InputManager::ReleaseAction(Action action) const {
	if (!input_ && !gamePad_) return false;

	auto it = bindings_.find(action);
	if (it == bindings_.end()) return false;

	// いずれかのバインドでReleaseされていればtrue
	for (const auto& binding : it->second) {
		if (CheckBindingRelease(binding)) {
			return true;
		}
	}
	return false;
}

float InputManager::GetActionValue(Action action) const {
	if (!input_ && !gamePad_) return 0.0f;

	auto it = bindings_.find(action);
	if (it == bindings_.end()) return 0.0f;

	// 最大値を返す
	float maxValue = 0.0f;
	for (const auto& binding : it->second) {
		float value = GetBindingValue(binding);
		maxValue = (std::max)(maxValue, value);
	}
	return maxValue;
}

InputManager::Vector2D InputManager::GetActionVector2D(Action action) const {
	if (!input_ && !gamePad_) return { 0.0f, 0.0f };

	auto it = bindings_.find(action);
	if (it == bindings_.end()) return { 0.0f, 0.0f };

	// 複数のバインドの合成（最大絶対値を優先）
	Vector2D result = { 0.0f, 0.0f };
	for (const auto& binding : it->second) {
		Vector2D vec = GetBindingVector2D(binding);
		if (fabsf(vec.x) > fabsf(result.x)) result.x = vec.x;
		if (fabsf(vec.y) > fabsf(result.y)) result.y = vec.y;
	}
	return result;
}

void InputManager::ResetToDefault() {
	bindings_.clear();
	SetupDefaultBindings();
}

const char* InputManager::GetActionName(Action action) {
	switch (action) {
	case Action::CONFIRM: return "決定";
	case Action::MOVE_UP: return "上移動";
	case Action::MOVE_DOWN: return "下移動";
	case Action::MOVE_LEFT: return "左移動";
	case Action::MOVE_RIGHT: return "右移動";
	case Action::MOVE_STICK: return "移動スティック";
	case Action::CAMERA_LOOK: return "カメラ操作";
	default: return "Unknown";
	}
}

const std::vector<InputBinding>& InputManager::GetBindings(Action action) const {
	static std::vector<InputBinding> empty;
	auto it = bindings_.find(action);
	if (it == bindings_.end()) return empty;
	return it->second;
}

void InputManager::SetupDefaultBindings() {
	// 決定
	bindings_[Action::CONFIRM].push_back(InputBinding(InputSource::KEYBOARD, DIK_SPACE));
	bindings_[Action::CONFIRM].push_back(InputBinding(InputSource::KEYBOARD, DIK_Z));
	bindings_[Action::CONFIRM].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::A));

	// キャンセル
	bindings_[Action::CANCEL].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::B));

	// ポーズ
	bindings_[Action::PAUSE].push_back(InputBinding(InputSource::KEYBOARD, DIK_TAB));
	bindings_[Action::PAUSE].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::START));

	// インタラクト
	bindings_[Action::INTERACT].push_back(InputBinding(InputSource::KEYBOARD, DIK_E));
	bindings_[Action::INTERACT].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::X));

	// しゃがみ
	bindings_[Action::CROUCHING].push_back(InputBinding(InputSource::KEYBOARD, DIK_LSHIFT));
	bindings_[Action::CROUCHING].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::L));

	// セレクト
	bindings_[Action::CELECT_UP].push_back(InputBinding(InputSource::KEYBOARD, DIK_W));
	bindings_[Action::CELECT_UP].push_back(InputBinding(InputSource::KEYBOARD, DIK_UP));
	bindings_[Action::CELECT_UP].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::UP_STICK));
	bindings_[Action::CELECT_UP].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::UP_BOTTON));

	bindings_[Action::CELECT_DOWN].push_back(InputBinding(InputSource::KEYBOARD, DIK_S));
	bindings_[Action::CELECT_DOWN].push_back(InputBinding(InputSource::KEYBOARD, DIK_DOWN));
	bindings_[Action::CELECT_DOWN].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::DOWN_STICK));
	bindings_[Action::CELECT_DOWN].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::DOWN_BOTTON));

	bindings_[Action::CELECT_LEFT].push_back(InputBinding(InputSource::KEYBOARD, DIK_A));
	bindings_[Action::CELECT_LEFT].push_back(InputBinding(InputSource::KEYBOARD, DIK_LEFT));
	bindings_[Action::CELECT_LEFT].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::LEFT_STICK));
	bindings_[Action::CELECT_LEFT].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::LEFT_BOTTON));

	bindings_[Action::CELECT_RIGHT].push_back(InputBinding(InputSource::KEYBOARD, DIK_D));
	bindings_[Action::CELECT_RIGHT].push_back(InputBinding(InputSource::KEYBOARD, DIK_RIGHT));
	bindings_[Action::CELECT_RIGHT].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::RIGHT_STICK));
	bindings_[Action::CELECT_RIGHT].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::RIGHT_BOTTON));

	// 移動
	bindings_[Action::MOVE_UP].push_back(InputBinding(InputSource::KEYBOARD, DIK_W));
	bindings_[Action::MOVE_UP].push_back(InputBinding(InputSource::KEYBOARD, DIK_UP));
	bindings_[Action::MOVE_UP].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::UP_BOTTON));
	bindings_[Action::MOVE_UP].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::UP_STICK));

	bindings_[Action::MOVE_DOWN].push_back(InputBinding(InputSource::KEYBOARD, DIK_S));
	bindings_[Action::MOVE_DOWN].push_back(InputBinding(InputSource::KEYBOARD, DIK_DOWN));
	bindings_[Action::MOVE_DOWN].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::DOWN_BOTTON));
	bindings_[Action::MOVE_DOWN].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::DOWN_STICK));

	bindings_[Action::MOVE_LEFT].push_back(InputBinding(InputSource::KEYBOARD, DIK_A));
	bindings_[Action::MOVE_LEFT].push_back(InputBinding(InputSource::KEYBOARD, DIK_LEFT));
	bindings_[Action::MOVE_LEFT].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::LEFT_BOTTON));
	bindings_[Action::MOVE_LEFT].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::LEFT_STICK));

	bindings_[Action::MOVE_RIGHT].push_back(InputBinding(InputSource::KEYBOARD, DIK_D));
	bindings_[Action::MOVE_RIGHT].push_back(InputBinding(InputSource::KEYBOARD, DIK_RIGHT));
	bindings_[Action::MOVE_RIGHT].push_back(InputBinding(InputSource::GAMEPAD_BUTTON, GamePad::RIGHT_BOTTON));
	bindings_[Action::MOVE_RIGHT].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::RIGHT_STICK));

	// スティック入力による移動（左スティック全体）
	bindings_[Action::MOVE_STICK].push_back(InputBinding(InputSource::GAMEPAD_STICK_LEFT, GamePad::ALL_STICK));

	// カメラ操作（右スティック）
	bindings_[Action::CAMERA_LOOK].push_back(InputBinding(InputSource::GAMEPAD_STICK_RIGHT, GamePad::ALL_STICK));
}

bool InputManager::CheckBindingTrigger(const InputBinding& binding) const {
	switch (binding.source) {
	case InputSource::KEYBOARD:
		return input_ && input_->TriggerKey(binding.key);

	case InputSource::GAMEPAD_BUTTON:
		return gamePad_ && gamePad_->TriggerButton(binding.key);

	case InputSource::GAMEPAD_STICK_LEFT:
		return gamePad_ && gamePad_->TriggerLeftStick(binding.stickDirection);

	case InputSource::GAMEPAD_STICK_RIGHT:
		return gamePad_ && gamePad_->TriggerRightStick(binding.stickDirection);

	case InputSource::GAMEPAD_TRIGGER_LEFT:
	case InputSource::GAMEPAD_TRIGGER_RIGHT:
		// トリガーはアナログ値なので、しきい値超えをトリガーとして扱う
		return GetBindingValue(binding) > 0.5f;

	default:
		return false;
	}
}

bool InputManager::CheckBindingPush(const InputBinding& binding) const {
	switch (binding.source) {
	case InputSource::KEYBOARD:
		return input_ && input_->PushKey(binding.key);

	case InputSource::GAMEPAD_BUTTON:
		return gamePad_ && gamePad_->PushButton(binding.key);

	case InputSource::GAMEPAD_STICK_LEFT:
		return gamePad_ && gamePad_->PushLeftStick(binding.stickDirection);

	case InputSource::GAMEPAD_STICK_RIGHT:
		return gamePad_ && gamePad_->PushRightStick(binding.stickDirection);

	case InputSource::GAMEPAD_TRIGGER_LEFT:
	case InputSource::GAMEPAD_TRIGGER_RIGHT:
		return GetBindingValue(binding) > 0.5f;

	default:
		return false;
	}
}

bool InputManager::CheckBindingRelease(const InputBinding& binding) const {
	switch (binding.source) {
	case InputSource::KEYBOARD:
		// Inputクラスにはリリース判定がないので、Push判定の反転で代用
		// 必要に応じてInput側にRelease判定を追加してください
		return false;

	case InputSource::GAMEPAD_BUTTON:
		return gamePad_ && gamePad_->ReleaseButton(binding.key);

	case InputSource::GAMEPAD_STICK_LEFT:
		return gamePad_ && gamePad_->ReleaseLeftStick();

	case InputSource::GAMEPAD_STICK_RIGHT:
		return gamePad_ && gamePad_->ReleaseRightStick();

	case InputSource::GAMEPAD_TRIGGER_LEFT:
	case InputSource::GAMEPAD_TRIGGER_RIGHT:
		return GetBindingValue(binding) < 0.5f;

	default:
		return false;
	}
}

float InputManager::GetBindingValue(const InputBinding& binding) const {
	switch (binding.source) {
	case InputSource::KEYBOARD:
		return (input_ && input_->PushKey(binding.key)) ? 1.0f : 0.0f;

	case InputSource::GAMEPAD_BUTTON:
		return (gamePad_ && gamePad_->PushButton(binding.key)) ? 1.0f : 0.0f;

	case InputSource::GAMEPAD_STICK_LEFT:
		if (!gamePad_) return 0.0f;
		switch (binding.stickDirection) {
		case GamePad::UP_STICK:
			return (std::max)(0.0f, gamePad_->LeftStickY());
		case GamePad::DOWN_STICK:
			return (std::max)(0.0f, -gamePad_->LeftStickY());
		case GamePad::LEFT_STICK:
			return (std::max)(0.0f, -gamePad_->LeftStickX());
		case GamePad::RIGHT_STICK:
			return (std::max)(0.0f, gamePad_->LeftStickX());
		default:
			return sqrtf(gamePad_->LeftStickX() * gamePad_->LeftStickX() +
				gamePad_->LeftStickY() * gamePad_->LeftStickY());
		}

	case InputSource::GAMEPAD_STICK_RIGHT:
		if (!gamePad_) return 0.0f;
		switch (binding.stickDirection) {
		case GamePad::UP_STICK:
			return (std::max)(0.0f, gamePad_->RightStickY());
		case GamePad::DOWN_STICK:
			return (std::max)(0.0f, -gamePad_->RightStickY());
		case GamePad::LEFT_STICK:
			return (std::max)(0.0f, -gamePad_->RightStickX());
		case GamePad::RIGHT_STICK:
			return (std::max)(0.0f, gamePad_->RightStickX());
		default:
			return sqrtf(gamePad_->RightStickX() * gamePad_->RightStickX() +
				gamePad_->RightStickY() * gamePad_->RightStickY());
		}

	case InputSource::GAMEPAD_TRIGGER_LEFT:
		return gamePad_ ? gamePad_->LeftTrigger() : 0.0f;

	case InputSource::GAMEPAD_TRIGGER_RIGHT:
		return gamePad_ ? gamePad_->RightTrigger() : 0.0f;

	default:
		return 0.0f;
	}
}

InputManager::Vector2D InputManager::GetBindingVector2D(const InputBinding& binding) const {
	Vector2D result = { 0.0f, 0.0f };

	switch (binding.source) {
	case InputSource::KEYBOARD:
		// キーボードは方向入力として扱う（-1, 0, 1）
		if (input_ && input_->PushKey(binding.key)) {
			// アクションの種類に応じて方向を決定
			// ここではアクションに基づいて適切な方向を返す必要がある
			// 簡易的に上下左右を想定
			result = { 0.0f, 0.0f };
		}
		break;

	case InputSource::GAMEPAD_STICK_LEFT:
		if (gamePad_) {
			result.x = gamePad_->LeftStickX();
			result.y = gamePad_->LeftStickY();
		}
		break;

	case InputSource::GAMEPAD_STICK_RIGHT:
		if (gamePad_) {
			result.x = gamePad_->RightStickX();
			result.y = gamePad_->RightStickY();
		}
		break;

	default:
		break;
	}

	return result;
}