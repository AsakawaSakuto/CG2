#include "TitleObject.h"

void TitleObject::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	for (int i = 0; i < model_.size(); i++) {
		model_[i] = std::make_unique<Model>();
	}
	model_[0]->Initialize(dxCommon_, "Machine/Body.obj");
	model_[1]->Initialize(dxCommon_, "Machine/ArmL_01.obj");
	model_[2]->Initialize(dxCommon_, "Machine/ArmL_02.obj");
	model_[3]->Initialize(dxCommon_, "Machine/ArmR_01.obj");
	model_[4]->Initialize(dxCommon_, "Machine/ArmR_02.obj");
	model_[5]->Initialize(dxCommon_, "player/head/head.obj");
	model_[6]->Initialize(dxCommon_, "player/body/body.obj");
	model_[7]->Initialize(dxCommon_, "player/arm_L/arm_L.obj");
	model_[8]->Initialize(dxCommon_, "player/arm_R/arm_R.obj");
	model_[9]->Initialize(dxCommon_, "player/Leg_L/Leg_L.obj");
	model_[10]->Initialize(dxCommon_, "player/Leg_R/Leg_R.obj");

	InitTransform();

	for (int i = 0; i < model_.size(); i++) {
		model_[i]->SetTransform(transform_[i]);
	}

	ramuneParticle_->Initialize(dxCommon_,2);
	ramuneParticle_->LoadJson("ramuneTitle");
}

void TitleObject::Update() {
	// ふわふわアニメーション用の時間更新
	floatTime_ += 1.0f / 60.0f; // 60FPS想定
	float floatOffset = sinf(floatTime_ * floatSpeed_) * floatAmplitude_;

	// モデルに座標情報を反映
	if (playerInTimer_.IsActive()) {
		transform_[0].translate.y = Easing::Lerp(-10.0f, 0.0f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[1].translate.y = Easing::Lerp(-10.0f, 0.0f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[2].translate.y = Easing::Lerp(-10.0f, -0.06f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[3].translate.y = Easing::Lerp(-10.0f, 0.0f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[4].translate.y = Easing::Lerp(-10.0f, -0.06f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[5].translate.y = Easing::Lerp(-10.0f, 1.1f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[6].translate.y = Easing::Lerp(-10.0f, 0.36f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[7].translate.y = Easing::Lerp(-10.0f, 0.71f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[8].translate.y = Easing::Lerp(-10.0f, 0.5f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[9].translate.y = Easing::Lerp(-10.0f, 0.1f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
		transform_[10].translate.y = Easing::Lerp(-10.0f, -0.03f , playerInTimer_.GetProgress(), Easing::Type::EaseOutBack);
	}

	if (playerOutTimer_.IsActive()) {
		transform_[0].translate.y = Easing::Lerp(0.0f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[1].translate.y = Easing::Lerp(0.0f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[2].translate.y = Easing::Lerp(-0.06f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[3].translate.y = Easing::Lerp(0.0f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[4].translate.y = Easing::Lerp(-0.06f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[5].translate.y = Easing::Lerp(1.1f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[6].translate.y = Easing::Lerp(0.36f, 10.0f,playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[7].translate.y = Easing::Lerp(0.71f , 10.0f,playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[8].translate.y = Easing::Lerp(0.5f , 10.0f,playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[9].translate.y = Easing::Lerp(0.1f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
		transform_[10].translate.y = Easing::Lerp(-0.03f , 10.0f, playerOutTimer_.GetProgress(), Easing::Type::EaseInBack);
	}

	if (playerInTimer_.IsFinished() && !playerOutTimer_.IsActive() && !playerOutTimer_.IsFinished()) {
		// アニメーションが完了した後の通常状態でもふわふわ
		transform_[0].translate.y = 0.0f + floatOffset;
		transform_[1].translate.y = 0.0f + floatOffset;
		transform_[2].translate.y = -0.06f + floatOffset;
		transform_[3].translate.y = 0.0f + floatOffset;
		transform_[4].translate.y = -0.06f + floatOffset;
		transform_[5].translate.y = 1.1f + floatOffset;
		transform_[6].translate.y = 0.36f + floatOffset;
		transform_[7].translate.y = 0.71f + floatOffset;
		transform_[8].translate.y = 0.5f + floatOffset;
		transform_[9].translate.y = 0.1f + floatOffset;
		transform_[10].translate.y = -0.03f + floatOffset;
	}

	if (playerStart_) {
		transform_[8].rotate.z = -1.57f;
	} else {
		floatTimeZ_ += 1.0f / 120.0f;
		float floatRz = 0.5f * sinf(2.0f * 3.14159265f * floatTimeZ_) + 1.05f;
		transform_[8].rotate.z = floatRz;
	}

	for (int i = 0; i < model_.size(); i++) {
		model_[i]->SetTransform(transform_[i]);
		//model_[i]->SetTranslate(transform_[i].translate);
		model_[i]->Update();
	}

	playerInTimer_.Update();
	playerOutTimer_.Update();

	ramuneParticle_->SetEmitterPosition(transform_[0].translate);
	ramuneParticle_->SetOffSet({ 0.0f,-1.5f,0.0f });
	ramuneParticle_->Update();
}

void TitleObject::Draw(Camera& useCamera) {
	for (int i = 0; i < model_.size(); i++) {
		model_[i]->Draw(useCamera);
	}

	ramuneParticle_->Draw(useCamera);
}

void TitleObject::DrawImGui() {
	ramuneParticle_->DrawImGui("Ramune");

	//model_[1]->DrawImGui("1");
	//model_[2]->DrawImGui("2");
	//model_[3]->DrawImGui("3");
	//model_[4]->DrawImGui("4");
	//model_[5]->DrawImGui("5");
	//model_[6]->DrawImGui("6");
	//model_[7]->DrawImGui("7");
	//model_[8]->DrawImGui("8");
	//model_[9]->DrawImGui("9");
	//model_[10]->DrawImGui("10");
}

void TitleObject::InitTransform() {
	transform_[0].scale = { 3.0f,3.0f,3.0f };
	transform_[0].rotate = { 0.0f,3.21f,0.0f };
	transform_[0].translate = { 3.0f, -10.0f, 0.0f };

	transform_[1].scale = { 3.0f,2.0f,3.0f };
	transform_[1].rotate = { 0.0f,0.0f,0.0f };
	transform_[1].translate = { 2.6f, -10.0f, 0.0f };

	transform_[2].scale = { 3.0f,3.0f,5.0f };
	transform_[2].rotate = { 0.0f,0.0f,-1.57f };
	transform_[2].translate = { 4.71f,-10.06f,0.0f };

	transform_[3].scale = { 3.0f,2.0f,3.0f };
	transform_[3].rotate = { 0.0f,0.0f,0.0f };
	transform_[3].translate = { 3.4f, -10.0f, 0.0f };

	transform_[4].scale = { 3.0f,3.0f,5.0f };
	transform_[4].rotate = { 0.0f,0.0f,1.57f };
	transform_[4].translate = { 1.3f, -10.06f, 0.0f };

	transform_[5].scale = { 2.5f,2.5f,2.5f };
	transform_[5].rotate = { 0.0f,3.37f,-0.42f };
	transform_[5].translate = { 4.01f, -8.9f, 1.0f };

	transform_[6].scale = { 2.5f,2.5f,2.5f };
	transform_[6].rotate = { 0.0f,3.2f,-0.49f };
	transform_[6].translate = { 3.62f, -9.77f, 1.09f };

	transform_[7].scale = { 2.5f,2.5f,2.5f };
	transform_[7].rotate = { 0.0f,0.0f,-1.35f };
	transform_[7].translate = { 3.65f, -9.52f, 1.0f };

	transform_[8].scale = { 2.5f,2.5f,2.5f };
	transform_[8].rotate = { 0.0f,0.0f,-1.42f };
	transform_[8].translate = { 3.52f, -9.55f, -0.79f };

	transform_[9].scale = { 2.5f,2.5f,2.5f };
	transform_[9].rotate = { 0.0f,0.0f,0.79f };
	transform_[9].translate = { 3.86f, -10.29f, 1.0f };

	transform_[10].scale = { 2.5f,2.5f,2.5f };
	transform_[10].rotate = { 0.0f,-0.09f,0.13f };
	transform_[10].translate = { 3.38f, -10.03f, 0.0f };
}