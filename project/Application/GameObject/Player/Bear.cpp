#include "Bear.h"

void Bear::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	// クマのモデル初期化
	modelBearHead_->Initialize(dxCommon_, "player/Head/Head.obj");
	modelBearHead_->SetTexture("resources/model/player/Head/Player.png");

	modelBearBody_->Initialize(dxCommon_, "player/Body/Body.obj");
	modelBearBody_->SetTexture("resources/model/player/Body/Player.png");

	modelBearArmR_->Initialize(dxCommon_, "player/Arm_R/Arm_R.obj");
	modelBearArmR_->SetTexture("resources/model/player/Arm_R/Player.png");

	modelBearArmL_->Initialize(dxCommon_, "player/Arm_L/Arm_L.obj");
	modelBearArmL_->SetTexture("resources/model/player/Arm_L/Player.png");

	modelBearLegR_->Initialize(dxCommon_, "player/Leg_R/Leg_R.obj");
	modelBearLegR_->SetTexture("resources/model/player/Leg_R/Player.png");

	modelBearLegL_->Initialize(dxCommon_, "player/Leg_L/Leg_L.obj");
	modelBearLegL_->SetTexture("resources/model/player/Leg_L/Player.png");

	// 根のTransform
	transform_.scale = {1.5f, 1.5f, 1.5f};
	transform_.rotate = {0.0f, std::numbers::pi_v<float>, 0.0f};
	transform_.translate = {640.0f, 360.0f, -1.0f};

	// オフセット
	offsetX_ = 0.2f;

	// ローカルTransform
	localTransform_[0] = {
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.3f, 0.0f},
	};

	localTransform_[1] = {
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, -0.4f},
	    {offsetX_, 0.2f, 0.4f },
	};

	localTransform_[2] = {
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.1f, 0.1f, 0.0f},
	};

	localTransform_[3] = {
	    {0.0f,  0.0f, 0.0f},
	    {0.0f,  0.0f, -1.2f},
	    {-0.1f, 0.1f, 0.5f},
	};

	localTransform_[4] = {
	    {0.0f, 0.0f,  0.0f},
	    {0.0f, 0.0f,  0.0f},
	    {0.1f, -0.2f, 0.0f},
	};

	localTransform_[5] = {
	    {0.0f,  0.0f,  0.0f},
	    {0.0f,  0.0f,  0.0f},
	    {-0.1f, -0.2f, 0.0f},
	};

	// Transformを親に合わせる
	modelBearHead_->SetTransform(transform_);
	modelBearBody_->SetTransform(transform_);
	modelBearArmR_->SetTransform(transform_);
	modelBearArmL_->SetTransform(transform_);
	modelBearLegR_->SetTransform(transform_);
	modelBearLegL_->SetTransform(transform_);
}

void Bear::Update() {
	localTransform_[1].translate = {offsetX_, 0.2f, 0.4f};

	// 親
	modelBearBody_->SetTranslate(transform_.translate + localTransform_[1].translate);
	modelBearBody_->SetRotate(transform_.rotate + localTransform_[1].rotate);
	modelBearBody_->SetScale(transform_.scale + localTransform_[1].scale);

	// 座標の更新
	Matrix4x4 bodyWorldMatrix = MakeAffineMatrix(modelBearBody_->GetScale(), modelBearBody_->GetRotate(), modelBearBody_->GetTranslate());

	// 頭
	Vector3 headWorld = TransformVtoM(localTransform_[0].translate, bodyWorldMatrix);
	modelBearHead_->SetTranslate(headWorld);
	modelBearHead_->SetRotate(modelBearBody_->GetRotate() + localTransform_[0].rotate);
	modelBearHead_->SetScale(modelBearBody_->GetScale() + localTransform_[0].scale);

	// 右腕
	Vector3 armRWorld = TransformVtoM(localTransform_[2].translate, bodyWorldMatrix);
	modelBearArmR_->SetTranslate(armRWorld);
	modelBearArmR_->SetRotate(modelBearBody_->GetRotate() + localTransform_[2].rotate);
	modelBearArmR_->SetScale(modelBearBody_->GetScale() + localTransform_[2].scale);

	// 左腕
	Vector3 armLWorld = TransformVtoM(localTransform_[3].translate, bodyWorldMatrix);
	modelBearArmL_->SetTranslate(armLWorld);
	modelBearArmL_->SetRotate(modelBearBody_->GetRotate() + localTransform_[3].rotate);
	modelBearArmL_->SetScale(modelBearBody_->GetScale() + localTransform_[3].scale);

	// 右脚
	Vector3 legRWorld = TransformVtoM(localTransform_[4].translate, bodyWorldMatrix);
	modelBearLegR_->SetTranslate(legRWorld);
	modelBearLegR_->SetRotate(modelBearBody_->GetRotate() + localTransform_[4].rotate);
	modelBearLegR_->SetScale(modelBearBody_->GetScale() + localTransform_[4].scale);

	// 左脚
	Vector3 legLWorld = TransformVtoM(localTransform_[5].translate, bodyWorldMatrix);
	modelBearLegL_->SetTranslate(legLWorld);
	modelBearLegL_->SetRotate(modelBearBody_->GetRotate() + localTransform_[5].rotate);
	modelBearLegL_->SetScale(modelBearBody_->GetScale() + localTransform_[5].scale);

	// クマのモデル更新
	modelBearHead_->Update();
	modelBearBody_->Update();
	modelBearArmR_->Update();
	modelBearArmL_->Update();
	modelBearLegR_->Update();
	modelBearLegL_->Update();
}

void Bear::Draw(Camera useCamera) {
	// クマのモデル更新
	modelBearHead_->Draw(useCamera);
	modelBearBody_->Draw(useCamera);
	modelBearArmR_->Draw(useCamera);
	modelBearArmL_->Draw(useCamera);
	modelBearLegR_->Draw(useCamera);
	modelBearLegL_->Draw(useCamera);
}

void Bear::ImGuiUpdate() {
	ImGui::Begin("Bear");

	const char* partNames[] = {"Head", "Body", "ArmR", "ArmL", "LegR", "LegL"};

	for (int i = 0; i < 6; ++i) {
		if (ImGui::CollapsingHeader(partNames[i], ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::PushID(i); // 適切なIDスコープを設定

			ImGui::Text("Position");
			ImGui::DragFloat3("LocalPos", &localTransform_[i].translate.x, 0.1f);

			ImGui::Text("Rotation");
			ImGui::DragFloat3("LocalRotate", &localTransform_[i].rotate.x, 0.1f);

			ImGui::Text("Scale");
			ImGui::DragFloat3("LocalScale", &localTransform_[i].scale.x, 0.1f);

			ImGui::Separator(); // 区切り線で視認性アップ

			ImGui::PopID();
		}
	}

	ImGui::End();
}
