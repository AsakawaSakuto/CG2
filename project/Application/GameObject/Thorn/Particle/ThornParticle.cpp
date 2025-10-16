#include "Application/GameObject/Thorn/Particle/ThornParticle.h"

void ThornParticle::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	for (auto& model : pModels_) {
		model = std::make_unique<Model>();
		model->Initialize(dxCommon_, "Candy/Candy.obj");
	}
}

void ThornParticle::Update() {

	// 当たり判定用の球の中心を更新
	collisionSphere_.center = transform_.translate;

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void ThornParticle::Draw(Camera useCamera) {
	model_->Draw(useCamera);
}