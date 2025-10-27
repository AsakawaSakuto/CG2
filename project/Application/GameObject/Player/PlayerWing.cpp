#include "PlayerWing.h"

void PlayerWing::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	// model_->Initialize(dxCommon_, "cube.obj");

	// 腕のモデル生成・初期化
	modelArmR01_ = std::make_unique<Model>();
	modelArmR02_ = std::make_unique<Model>();
	modelArmR03_ = std::make_unique<Model>();
	modelArmR04_ = std::make_unique<Model>();
	modelArmL01_ = std::make_unique<Model>();
	modelArmL02_ = std::make_unique<Model>();
	modelArmL03_ = std::make_unique<Model>();
	modelArmL04_ = std::make_unique<Model>();

	modelArmR01_->Initialize(dxCommon_, "Machine/ArmR_01.obj");
	modelArmR02_->Initialize(dxCommon_, "Machine/ArmR_02.obj");
	modelArmR03_->Initialize(dxCommon_, "Machine/ArmR_01.obj");
	modelArmR04_->Initialize(dxCommon_, "Machine/ArmR_02.obj");
	modelArmL01_->Initialize(dxCommon_, "Machine/ArmL_01.obj");
	modelArmL02_->Initialize(dxCommon_, "Machine/ArmL_02.obj");
	modelArmL03_->Initialize(dxCommon_, "Machine/ArmL_01.obj");
	modelArmL04_->Initialize(dxCommon_, "Machine/ArmL_02.obj");

	modelArmR01_->SetColor({0.7f, 0.0f, 0.0f, 1.0f});
	modelArmR02_->SetColor({0.7f, 0.0f, 0.0f, 1.0f});
	modelArmL01_->SetColor({0.7f, 0.0f, 0.0f, 1.0f});
	modelArmL02_->SetColor({0.7f, 0.0f, 0.0f, 1.0f});

	modelArmR03_->SetColor({0.6f, 0.6f, 0.4f, 1.0f});
	modelArmR04_->SetColor({0.6f, 0.6f, 0.4f, 1.0f});
	modelArmL03_->SetColor({0.6f, 0.6f, 0.4f, 1.0f});
	modelArmL04_->SetColor({0.6f, 0.6f, 0.4f, 1.0f});

	modelArmR01_->SetUpdateFrustumCulling(false);
	modelArmR02_->SetUpdateFrustumCulling(false);
	modelArmR03_->SetUpdateFrustumCulling(false);
	modelArmR04_->SetUpdateFrustumCulling(false);
	modelArmL01_->SetUpdateFrustumCulling(false);
	modelArmL02_->SetUpdateFrustumCulling(false);
	modelArmL03_->SetUpdateFrustumCulling(false);
	modelArmL04_->SetUpdateFrustumCulling(false);

	transform_.scale = {2.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, -10.0f, 0.0f};

	modelArmR01_->SetRotate({0.0f, 0.0f, 0.0f});
	modelArmR02_->SetRotate({0.0f, 0.0f, std::numbers::pi_v<float> * 0.5f});
	modelArmR03_->SetRotate({0.0f, 0.0f, 0.0f});
	modelArmR04_->SetRotate({0.0f, 0.0f, std::numbers::pi_v<float> * 0.5f});
	modelArmL01_->SetRotate({0.0f, 0.0f, 0.0f});
	modelArmL02_->SetRotate({0.0f, 0.0f, std::numbers::pi_v<float> * 1.5f});
	modelArmL03_->SetRotate({0.0f, 0.0f, 0.0f});
	modelArmL04_->SetRotate({0.0f, 0.0f, std::numbers::pi_v<float> * 1.5f});

	modelArmR01_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmR02_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmR03_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmR04_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmL01_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmL02_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmL03_->SetScale({1.5f, 1.5f, 1.5f});
	modelArmL04_->SetScale({1.5f, 1.5f, 1.5f});

	// 当たり判定更新(AABB)
	CollisionUpdate();

	isAlive_ = true;
}

void PlayerWing::Update() {
	if (!isAlive_)
		return;

	// 当たり判定更新(AABB)
	CollisionUpdate();

	// model_->SetTransform(transform_);
	// model_->Update();

	// 羽の中心点に応じて腕のモデルの位置を更新
	modelArmR01_->SetTranslate({transform_.translate.x + 25.0f * deltaTime_, transform_.translate.y, transform_.translate.z});
	modelArmR02_->SetTranslate({transform_.translate.x + 75.0f * deltaTime_, transform_.translate.y - 6.0f * deltaTime_, transform_.translate.z});
	modelArmR03_->SetTranslate({transform_.translate.x + 85.0f * deltaTime_, transform_.translate.y, transform_.translate.z});
	modelArmR04_->SetTranslate({transform_.translate.x + 130.0f * deltaTime_, transform_.translate.y - 6.0f * deltaTime_, transform_.translate.z});
	modelArmL01_->SetTranslate({transform_.translate.x - 25.0f * deltaTime_, transform_.translate.y, transform_.translate.z});
	modelArmL02_->SetTranslate({transform_.translate.x - 75.0f * deltaTime_, transform_.translate.y - 6.0f * deltaTime_, transform_.translate.z});
	modelArmL03_->SetTranslate({transform_.translate.x - 85.0f * deltaTime_, transform_.translate.y, transform_.translate.z});
	modelArmL04_->SetTranslate({transform_.translate.x - 130.0f * deltaTime_, transform_.translate.y - 6.0f * deltaTime_, transform_.translate.z});

	// 腕のモデル更新
	modelArmR01_->Update();
	modelArmR02_->Update();
	modelArmR03_->Update();
	modelArmR04_->Update();
	modelArmL01_->Update();
	modelArmL02_->Update();
	modelArmL03_->Update();
	modelArmL04_->Update();
}

void PlayerWing::Draw(Camera useCamera) {
	if (!isAlive_)
		return;

	// model_->Draw(useCamera);

	// 腕のモデル描画
	modelArmR01_->Draw(useCamera);
	modelArmR02_->Draw(useCamera);
	modelArmR03_->Draw(useCamera);
	modelArmR04_->Draw(useCamera);
	modelArmL01_->Draw(useCamera);
	modelArmL02_->Draw(useCamera);
	modelArmL03_->Draw(useCamera);
	modelArmL04_->Draw(useCamera);
}

void PlayerWing::WingImGui() {
	ImGui::Begin("Test");

	ImGui::DragFloat("TestX", &testX_, 0.1f);
	ImGui::DragFloat("TestY", &testY_, 0.1f);

	ImGui::DragFloat3("ArmR1position", &modelArmR01_->GetTranslate().x, 1.0f);
	ImGui::DragFloat3("ArmR2position", &modelArmR02_->GetTranslate().x, 1.0f);
	ImGui::DragFloat3("ArmR3position", &modelArmR03_->GetTranslate().x, 1.0f);
	ImGui::DragFloat3("ArmR4position", &modelArmR04_->GetTranslate().x, 1.0f);

	ImGui::DragFloat3("ArmL1position", &modelArmL01_->GetTranslate().x, 1.0f);
	ImGui::DragFloat3("ArmL2position", &modelArmL02_->GetTranslate().x, 1.0f);
	ImGui::DragFloat3("ArmL3position", &modelArmL03_->GetTranslate().x, 1.0f);
	ImGui::DragFloat3("ArmL4position", &modelArmL04_->GetTranslate().x, 1.0f);

	ImGui::DragFloat3("ArmR1Scale", &modelArmR01_->GetScale().x, 0.01f);
	ImGui::DragFloat3("ArmR2Scale", &modelArmR02_->GetScale().x, 0.01f);
	ImGui::DragFloat3("ArmR3Scale", &modelArmR03_->GetScale().x, 0.01f);
	ImGui::DragFloat3("ArmR4Scale", &modelArmR04_->GetScale().x, 0.01f);

	ImGui::DragFloat3("ArmL1Scale", &modelArmL01_->GetScale().x, 0.01f);
	ImGui::DragFloat3("ArmL2Scale", &modelArmL02_->GetScale().x, 0.01f);
	ImGui::DragFloat3("ArmL3Scale", &modelArmL03_->GetScale().x, 0.01f);
	ImGui::DragFloat3("ArmL4Scale", &modelArmL04_->GetScale().x, 0.01f);

	ImGui::End();
}

void PlayerWing::CollisionUpdate() {
	Vector3 t = transform_.translate;
	collisionAABB_.max = {t.x + 2.0f, t.y + 0.5f, t.z + 0.5f};
	collisionAABB_.min = {t.x - 2.0f, t.y - 0.5f, t.z - 0.5f};
}
