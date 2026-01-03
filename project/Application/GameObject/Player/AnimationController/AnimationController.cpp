#include "AnimationController.h"

void AnimationController::Initialize(std::map<PlayerMotion, Animation> animationMap) {
	model_ = std::make_unique<SkiningModel>();
	model_->Initialize("Player/Animation/idle.gltf");
	model_->SetTexture("resources/image/white16x16.png");
	//model_->UseLight(false);

	shadowModel_ = std::make_unique<SkiningModel>();
	shadowModel_->Initialize("Player/Animation/idle.gltf");
	shadowModel_->SetTexture("resources/image/white16x16.png");
	shadowModel_->UseLight(false);
	shadowModel_->SetColor3({ 0.1f,0.1f,0.1f });

	currentMotionType_ = PlayerMotion::Idle;
	animations_ = animationMap;

	transform_.translate = { 10.0f,100.0f,10.0f };
	shadowTransform_.scale.y = 0.01f;
}

void AnimationController::Update(float deltaTime, const Transform& transform) {

	if (MyInput::TriggerKey(DIK_O)) {
		model_->SetTexture("resources/image/uvChecker.png");
		OutputDebugStringA("[DEBUG] SetTexture called: uvChecker.png\n");
	}

	transform_ = transform;
	shadowTransform_.translate.x = transform.translate.x;
	shadowTransform_.translate.z = transform.translate.z;
	shadowTransform_.rotate = transform.rotate;
	animationTime_ += deltaTime;

	// 補間中の処理
	if (calculationTime_ > 0.0f) {
		// 補間アニメーション終了判定
		if (animationTime_ >= currentAnimation_.duration) {
			// 補間完了：次のアニメーションに切り替え
			calculationTime_ = 0.0f;
			
			if (!animationQueue_.empty()) {
				SetAnimation(animationQueue_.front());
				animationQueue_.erase(animationQueue_.begin());
			}
			
			isAnimationFinished_ = false;
		}
	}
	// 通常のアニメーション再生中
	else {
		// アニメーション終了判定
		if (animationTime_ >= currentAnimation_.duration) {
			if (isLoop_) {
				// ループ再生：アニメーションから飛び出た分を計算
				float diff = animationTime_ - currentAnimation_.duration;
				animationTime_ = diff;
				isAnimationFinished_ = false;
			} else {
				// ループしない：最後のフレームで停止
				animationTime_ = currentAnimation_.duration;
				isAnimationFinished_ = true;
			}
		} else {
			isAnimationFinished_ = false;
		}
	}
	
	// モデルの更新
	model_->Update(deltaTime);
}

void AnimationController::Draw(Camera& camera) {
	model_->Draw(camera, transform_);
	shadowModel_->Draw(camera, shadowTransform_);
}

void AnimationController::SetMotion(PlayerMotion motion, float calculationTime, bool isLoop) {
	// 指定されたモーションのアニメーションを取得
	const auto& it = animations_.find(motion);

	Animation nextAnimation;
	if (it == animations_.end()) {
		// 指定されたモーションが存在しない場合はIdleのアニメーションを入れる
		nextAnimation = animations_[PlayerMotion::Idle];
	} else {
		nextAnimation = it->second;
	}

	// アニメーション速度スケールとループ設定を更新
	isLoop_ = isLoop;
	isAnimationFinished_ = false;

	// 補完しなくていいなら即座に切り替えて終了
	if (calculationTime <= 0.0f) {
		// 補間中の場合はキューをクリア
		animationQueue_.clear();
		calculationTime_ = 0.0f;
		
		SetAnimation(nextAnimation);
		currentMotionType_ = motion;
		return;
	}

	// 既に補間中の場合は、現在の補間が終わるまで待つ
	if (calculationTime_ > 0.0f) {
		// 既存のキューをクリアして新しいアニメーションを予約
		animationQueue_.clear();
		animationQueue_.push_back(nextAnimation);
		currentMotionType_ = motion;
		return;
	}

	// 補完する場合、補完用アニメーションを作成する
	Animation calcAnimation;
	calcAnimation.duration = calculationTime;

	// 補完用アニメーションを作成する専用ラムダ
	auto setanimation = [&calcAnimation](Animation animation, float beginTime, float endTime) {
		for (const auto& [nodeName, nodeAnim] : animation.nodeAnimations) {
			NodeAnimation& anim = calcAnimation.nodeAnimations[nodeName];

			// scale
			KeyframeVector3 scale;
			scale.time = endTime;
			scale.value = CalculateValue(nodeAnim.scale.keyframes, beginTime);
			anim.scale.keyframes.push_back(scale);

			// rotate
			KeyframeQuaternion rotate;
			rotate.time = endTime;
			rotate.value = CalculateValue(nodeAnim.rotate.keyframes, beginTime);
			anim.rotate.keyframes.push_back(rotate);

			// translate
			KeyframeVector3 translate;
			translate.time = endTime;
			translate.value = CalculateValue(nodeAnim.translate.keyframes, beginTime);
			anim.translate.keyframes.push_back(translate);
		}
		};

	//=========================補完用アニメーション作成=========================

	// 現在のアニメーションの開始値を取得
	setanimation(currentAnimation_, animationTime_, 0.0f);

	// 次のアニメーションの開始値を取得
	setanimation(nextAnimation, 0.0f, calculationTime);

	// 完成した補完用アニメーションをセット
	SetAnimation(calcAnimation);
	calculationTime_ = calculationTime;

	// Queueに次のアニメーションを追加
	animationQueue_.clear();
	animationQueue_.push_back(nextAnimation);

	// モーションタイプを更新
	currentMotionType_ = motion;
}

void AnimationController::SetAnimation(const Animation& animation, float beginTime) {
	// アニメーションデータをコピー
	currentAnimation_ = animation;
	animationTime_ = beginTime;
	isAnimationFinished_ = false;
	
	// モデルに反映
	model_->SetAnimationData(currentAnimation_);
	model_->SetAnimationTime(animationTime_);
	shadowModel_->SetAnimationData(currentAnimation_);
	shadowModel_->SetAnimationTime(animationTime_);
}
