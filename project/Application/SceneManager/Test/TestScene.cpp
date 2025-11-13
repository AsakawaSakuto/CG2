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

    //cube_->Initialize(&ctx_->dxCommon, "cube.obj");
	cube_->Initialize(&ctx_->dxCommon, "Animation/AnimatedCube.gltf");
	cubeTransform_.scale = { 1.0f,1.0f,1.0f };

	testTimer_.Start(5.0f, true);
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

	//MT4_01_03();
	//MT4_01_04();
    //MT4_01_05();
}

void TestScene::MT4_01_03() {
    Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
    Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

    Quaternion identity = IdentityQuaternion();
    Quaternion conj = Conjugate(q1);
    Quaternion inv = Inverse(q1);
    Quaternion normal = Normalize(q1);
    Quaternion mul1 = Multiply(q1, q2);
    Quaternion mul2 = Multiply(q2, q1);
    float norm = Norm(q1);

    ImGui::Begin("MT4_01_03");

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

void TestScene::MT4_01_04() {
    Vector3 axis{ 1.0f, 0.4f, -0.2f };
    float angle = 0.45f;
    Vector3 pointY{ 2.1f, -0.9f, 1.3f };

    Vector3 axisNormalized = Normalize(axis);

    Quaternion rotation = MakeRotateAxisAngleQuaternion(axisNormalized, angle);
    Matrix4x4  rotateMatrix = MakeRotateMatrix(rotation);
    Vector3    rotateByQuat = RotateVector(pointY, rotation);
    Vector3    rotateByMat = TransformVtoM(pointY, rotateMatrix);

    ImGui::Begin("MT4_01_04");

    ImGui::Text("Axis & Angle");
    ImGui::DragFloat3("Axis (raw)", &axis.x, 0.01f);
    ImGui::DragFloat("Angle (rad)", &angle, 0.01f);

    ImGui::Separator();
    ImGui::Text("Axis (normalized)");
    ImGui::Text("(%.2f, %.2f, %.2f)",
        axisNormalized.x, axisNormalized.y, axisNormalized.z);

    ImGui::Separator();
    ImGui::Text("rotation (Quaternion)");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        rotation.x, rotation.y, rotation.z, rotation.w);

    ImGui::Separator();
    ImGui::Text("rotateMatrix");
    for (int r = 0; r < 4; ++r) {
        ImGui::Text("%.3f  %.3f  %.3f  %.3f",
            rotateMatrix.m[r][0], rotateMatrix.m[r][1],
            rotateMatrix.m[r][2], rotateMatrix.m[r][3]);
    }

    ImGui::Separator();
    ImGui::Text("Point (original)");
    ImGui::Text("(%.2f, %.2f, %.2f)",
        pointY.x, pointY.y, pointY.z);

    ImGui::Separator();
    ImGui::Text("rotateByQuaternion");
    ImGui::Text("(%.2f, %.2f, %.2f)",
        rotateByQuat.x, rotateByQuat.y, rotateByQuat.z);

    ImGui::Text("rotateByMatrix");
    ImGui::Text("(%.2f, %.2f, %.2f)",
        rotateByMat.x, rotateByMat.y, rotateByMat.z);

    ImGui::End();
}

void TestScene::MT4_01_05() {

    Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
    Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, 3.141592f);

    Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
    Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
    Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
    Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
    Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

    ImGui::Begin("Quaternion Slerp Test");

    ImGui::Text("rotation0");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        rotation0.x, rotation0.y, rotation0.z, rotation0.w);

    ImGui::Separator();
    ImGui::Text("rotation1");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        rotation1.x, rotation1.y, rotation1.z, rotation1.w);

    ImGui::Separator();
    ImGui::Text("interpolate0  (t = 0.0f)");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);

    ImGui::Text("interpolate1  (t = 0.3f)");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);

    ImGui::Text("interpolate2  (t = 0.5f)");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);

    ImGui::Text("interpolate3  (t = 0.7f)");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);

    ImGui::Text("interpolate4  (t = 1.0f)");
    ImGui::Text("x:%.2f  y:%.2f  z:%.2f  w:%.2f",
        interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);

    ImGui::End();
}