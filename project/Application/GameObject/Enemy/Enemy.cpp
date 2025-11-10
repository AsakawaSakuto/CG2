#include "Enemy.h"

void Enemy::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	transform_.scale = { 1.0f,1.0f,1.0f };
}

void Enemy::Update() {

}

void Enemy::Draw(Camera camera) {
	
}

void Enemy::DrawImGui() {

}