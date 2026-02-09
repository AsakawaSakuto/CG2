#pragma once
#include "InputDevice/Input.h"
#include "InputDevice/GamePad.h"

#include <vector>
#include <unordered_map>
#include <string>

// アクションボタン
enum class Action {
	// 決定
	CONFIRM,
	CANCEL,
	PAUSE,
	INTERACT,
	CROUCHING,
	MAP_OPEN,

	UI_CHANGE,
	GO_SPAWN_POS,

	// セレクト
	SELECT_UP,
	SELECT_DOWN,
	SELECT_LEFT,
	SELECT_RIGHT,

	// 移動
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_STICK,  // スティック入力による移動

	// カメラ操作
	CAMERA_LOOK,

	COUNT
};

// 入力ソース種別
enum class InputSource {
	KEYBOARD,
	GAMEPAD_BUTTON,
	GAMEPAD_STICK_LEFT,
	GAMEPAD_STICK_RIGHT,
	GAMEPAD_TRIGGER_LEFT,
	GAMEPAD_TRIGGER_RIGHT,
};

// 入力バインド情報
struct InputBinding {
	InputSource source;
	int key;
	GamePad::DownStick stickDirection = GamePad::DownStick::ALL_STICK;  // スティックの場合の方向

	InputBinding() : source(InputSource::KEYBOARD), key(0) {}
	InputBinding(InputSource src, int k) : source(src), key(k) {}
	InputBinding(InputSource src, GamePad::DownStick dir) : source(src), key(0), stickDirection(dir) {}
};

// キーコンフィグ管理クラス（シングルトン）
class InputManager {
public:
	// シングルトンインスタンス取得
	static InputManager* GetInstance() {
		static InputManager instance;
		return &instance;
	}

	// コピー・ムーブ禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// 初期化
	void Initialize();

	// 入力デバイスの設定
	void SetInputDevices(Input* input, GamePad* gamePad);

	// アクションに対する入力バインド設定
	void BindAction(Action action, const InputBinding& binding);
	void AddAlternativeBinding(Action action, const InputBinding& binding);
	void ClearBindings(Action action);

	// 入力判定（Trigger: 押した瞬間）
	bool TriggerAction(Action action) const;

	// 入力判定（Push: 押し続けている）
	bool PushAction(Action action) const;

	// 入力判定（Release: 離した瞬間）
	bool ReleaseAction(Action action) const;

	// アナログ値取得（スティックやトリガー用）0.0〜1.0
	float GetActionValue(Action action) const;

	// 2D入力取得（スティック用）-1.0〜1.0
	struct Vector2D {
		float x = 0.0f;
		float y = 0.0f;
	};
	Vector2D GetActionVector2D(Action action) const;

	// デフォルトキーバインドに戻す
	void ResetToDefault();

	// アクション名取得（UI表示用）
	static const char* GetActionName(Action action);

	// バインド情報取得
	const std::vector<InputBinding>& GetBindings(Action action) const;

	// GamePad接続状態を確認
	bool IsGamePadConnected() const { return gamePad_ && gamePad_->IsConnected(); }

	Input* GetInput() const { return input_; }
	GamePad* GetGamePad() const { return gamePad_; }
private:
	InputManager() = default;
	~InputManager() = default;

	Input* input_ = nullptr;
	GamePad* gamePad_ = nullptr;

	// アクションごとのバインドリスト（複数バインド可能）
	std::unordered_map<Action, std::vector<InputBinding>> bindings_;

	// デフォルトバインド設定
	void SetupDefaultBindings();

	// 個別バインドの判定ヘルパー
	bool CheckBindingTrigger(const InputBinding& binding) const;
	bool CheckBindingPush(const InputBinding& binding) const;
	bool CheckBindingRelease(const InputBinding& binding) const;
	float GetBindingValue(const InputBinding& binding) const;
	Vector2D GetBindingVector2D(const InputBinding& binding) const;
};