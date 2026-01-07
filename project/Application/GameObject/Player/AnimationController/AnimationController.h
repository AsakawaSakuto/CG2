#pragma once
#include "EngineSystem.h"

enum class PlayerMotion {
	Idle = 0,
	CrouIdle,
	Walk,
	Crouching,
	Jump,
	Landing,
	Die,

	Count,
};

/// <summary>
/// プレイヤーの描画やモデル管理について
/// </summary>
class AnimationController {
public:

	AnimationController() = default;
	~AnimationController() = default;

	void Initialize(std::map<PlayerMotion, Animation> animationMap);
	void Update(float deltaTime, const Transform& transform);
	void Draw(Camera& camera);

	/// <summary>
	/// 次のモーションを予約する
	/// </summary>
	/// <param name="motion">何のモーションか</param>
	/// <param name="calculationTime">補完の時間</param>
	/// <param name="animationSpeedScale">アニメーション速度スケール（1.0で通常速度）</param>
	/// <param name="isLoop">ループ再生するか</param>
	void SetMotion(PlayerMotion motion, float calculationTime = 0.0f, bool isLoop = true);

	/// <summary>
	/// 現在のモーションを取得
	/// </summary>
	PlayerMotion GetCurrentMotion() const { return currentMotionType_; }

	/// <summary>
	/// モデルを取得
	/// </summary>
	SkiningModel* GetModel() const { return model_.get(); }

	/// <summary>
	/// アニメーションが終了したかを判定
	/// </summary>
	/// <returns>true: 終了した / false: 再生中</returns>
	bool IsAnimationFinished() const { return isAnimationFinished_; }

	/// <summary>
	/// 現在のアニメーション再生時間を取得
	/// </summary>
	float GetAnimationTime() const { return animationTime_; }

	/// <summary>
	/// 現在のアニメーションの総再生時間を取得
	/// </summary>
	float GetAnimationDuration() const { return currentAnimation_.duration; }

	/// <summary>
	/// アニメーションの再生進行度を取得（0.0〜1.0）
	/// </summary>
	float GetAnimationProgress() const {
		if (currentAnimation_.duration <= 0.0f) return 0.0f;
		float progress = animationTime_ / currentAnimation_.duration;
		if (progress < 0.0f) return 0.0f;
		if (progress > 1.0f) return 1.0f;
		return progress;
	}

	/// <summary>
	/// アニメーションを完全に停止（最終フレームで固定）
	/// </summary>
	void StopAnimation() {
		isLoop_ = false;
		isAnimationFinished_ = true;
		animationTime_ = currentAnimation_.duration;
	}

	void SetShadowY(float y) {
		shadowTransform_.translate.y = y + 0.1f;
	}

	/// <summary>
	/// 影のY座標を地面の高さに設定
	/// </summary>
	/// <param name="groundY">地面のY座標</param>
	void SetShadowGroundY(float groundY) {
		shadowTransform_.translate.y = groundY;
	}

	void SetColor(Vector3 color) {
		model_->SetColor3(color);
	}
private:

	void SetAnimation(const Animation& animation, float beginTime = 0.0f);

	std::unique_ptr<SkiningModel> model_;
	unique_ptr<SkiningModel> shadowModel_;
	Transform transform_;
	Transform shadowTransform_;

	Animation currentAnimation_;
	std::vector<Animation> animationQueue_;
	std::map<PlayerMotion, Animation> animations_;

	PlayerMotion currentMotionType_ = PlayerMotion::Idle;

	float animationTime_ = 0.0f;
	float calculationTime_ = 0.0f;

	bool isLoop_ = true;               // ループ再生するか
	bool isAnimationFinished_ = false; // アニメーションが終了したか
};