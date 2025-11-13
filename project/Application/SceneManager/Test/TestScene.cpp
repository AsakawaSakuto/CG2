#include "TestScene.h"
#include "../Quaternion/QuaternionFunction.h"

TestScene::~TestScene() {
    CleanupResources();
}

void TestScene::CleanupResources() {

}

void TestScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void TestScene::Initialize() {
    CleanupResources();
	testParticle_->Initialize(&ctx_->dxCommon);
	camera_.SetPosition({ 0.0f, 0.0f, -10.0f });
	debugCamera_.SetInput(&ctx_->input);
	debugCamera_.SetPosition({ 0.0f, 0.0f, -10.0f });

	cube_->Initialize(&ctx_->dxCommon, "cube.obj");
	cubeTransform_.scale = { 1.0f,1.0f,1.0f };

	testTimer_.Start(1.0f, true);
}

void TestScene::Update() {

	testParticle_->Update();

	//cubeTransform_.translate = Easing::LerpVector3(
	//	{ 0.0f,0.0f,0.0f }, { 10.0f,5.0f,3.0f }, testTimer_.GetProgress(),
	//	Easing::Type::EaseOutInBack);

	cubeTransform_.translate = Easing::LerpVector3_GAB(
		{ 0.0f,0.0f,0.0f }, { 10.0f,5.0f,3.0f }, testTimer_.GetProgress(), 
		Easing::Type::Linear, Easing::Type::EaseOutBounce);

	cube_->SetTransform(cubeTransform_);
	cube_->Update();

	testTimer_.Update();

    camera_ = debugCamera_;
	camera_.Update();
	debugCamera_.Update();
}

void TestScene::Draw() {
	cube_->Draw(camera_);
	testParticle_->Draw(camera_);
}

void TestScene::DrawImGui() {

    DrawSceneName();

    testParticle_->DrawImGui("TestParticle");

	cube_->DrawImGui("TestCube");

    Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
    Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

    Quaternion identity = IdentityQuaternion();
    Quaternion conj = Conjugate(q1);
    Quaternion inv = Inverse(q1);
    Quaternion normal = Normalize(q1);
    Quaternion mul1 = Multiply(q1, q2);
    Quaternion mul2 = Multiply(q2, q1);
    float norm = Norm(q1);

    ImGui::Begin("Quaternion Test");

    ImGui::Text("Identity");
    ImGui::Text("%.2f  %.2f  %.2f  %.2f",
        identity.x, identity.y, identity.z, identity.w);

    ImGui::Separator();

    ImGui::Text("Conjugate(q1)");
    ImGui::Text("%.2f  %.2f  %.2f  %.2f",
        conj.x, conj.y, conj.z, conj.w);

    ImGui::Separator();

    ImGui::Text("Inverse(q1)");
    ImGui::Text("%.2f  %.2f  %.2f  %.2f",
        inv.x, inv.y, inv.z, inv.w);

    ImGui::Separator();

    ImGui::Text("Normalize(q1)");
    ImGui::Text("%.2f  %.2f  %.2f  %.2f",
        normal.x, normal.y, normal.z, normal.w);

    ImGui::Separator();

    ImGui::Text("Multiply(q1, q2)");
    ImGui::Text("%.2f  %.2f  %.2f  %.2f",
        mul1.x, mul1.y, mul1.z, mul1.w);

    ImGui::Separator();

    ImGui::Text("Multiply(q2, q1)");
    ImGui::Text("%.2f  %.2f  %.2f  %.2f",
        mul2.x, mul2.y, mul2.z, mul2.w);

    ImGui::Separator();

    ImGui::Text("Norm(q1) = %.2f", norm);

    ImGui::End();
}
