#include "ExpItem.h"

void ExpItem::Initialize(AppContext* ctx) {
	ctx_ = ctx;

	model_->Initialize(&ctx_->dxCommon, "exp_item.obj");
}

void ExpItem::Update() {

	model_->Update();
}

void ExpItem::Draw(Camera camera) {
	model_->Draw(camera, transform_);
}