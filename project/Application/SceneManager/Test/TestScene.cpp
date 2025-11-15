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

	normalCube_->Initialize(&ctx_->dxCommon, "cube.obj");
    normalCubeTransform_.translate = { -1.5f,2.5f,0.0f };

	animationCube_->Initialize(&ctx_->dxCommon, "Animation/cube/AnimatedCube.gltf");
    animationCubeTransform_.translate = { 1.5f,2.5f,0.0f };
	animationCubeTransform_.scale = { 0.5f,0.5f,0.5f };

	simpleSkin_->Initialize(&ctx_->dxCommon, "Animation/SimpleSkin/SimpleSkin.gltf");
	simpleSkinTransform_.rotate = { 0.0f,3.12f,0.0f };
	simpleSkinTransform_.translate = { -3.0f,0.0f,0.0f };

    walk_->Initialize(&ctx_->dxCommon, "Animation/human/Walk.gltf");
    walkTransform_.rotate = { 0.0f,3.12f,0.0f };
    walkTransform_.translate = { 0.0f,0.0f,0.0f };

    sneakWalk_->Initialize(&ctx_->dxCommon, "Animation/human/sneakWalk.gltf");
    sneakWalkTransform_.rotate = { 0.0f, 3.12f,0.0f };
    sneakWalkTransform_.translate = { 3.0f,0.0f,0.0f };

	walkAnimation_ = LoadAnimationFile("Animation/human/Walk.gltf");

	testTimer_.Start(5.0f, true);
}

void TestScene::Update() {

	testParticle_->Update();

    if (ctx_->input.TriggerKey(DIK_Z)) {
        animationCube_->PlayAnimation();
    }

    if (ctx_->input.TriggerKey(DIK_X)) {
        animationCube_->StopAnimation();
    }

    if (ctx_->input.TriggerKey(DIK_V)) {
        sneakWalk_->SetAnimationData(walkAnimation_);
    }

	normalCube_->Update();
	animationCube_->Update();
    simpleSkin_->Update();
    sneakWalk_->Update();
	walk_->Update();

	testTimer_.Update();

    camera_ = debugCamera_;
	camera_.Update();
	debugCamera_.Update();
}

void TestScene::Draw() {
    normalCube_->Draw(camera_, normalCubeTransform_);
    animationCube_->Draw(camera_, animationCubeTransform_);
    simpleSkin_->Draw(camera_, simpleSkinTransform_);
	walk_->Draw(camera_, walkTransform_);
    sneakWalk_->Draw(camera_, sneakWalkTransform_);

	testParticle_->Draw(camera_);
}

void TestScene::DrawImGui() {
#ifdef USE_IMGUI
    testParticle_->DrawImGui("TestParticle");

	walk_ ->DrawImGui("WalkModel");

	normalCubeTransform_.DrawImGui("NormalCubeTransform");
	animationCubeTransform_.DrawImGui("AnimationCubeTransform");
	//simpleSkinTransform_.DrawImGui("SimpleSkinTransform");
	//walkTransform_.DrawImGui("WalkTransform");
	//sneakWalkTransform_.DrawImGui("SneakWalkTransform");

	//MT4_01_03();
	//MT4_01_04();
    //MT4_01_05();
#endif
}

void TestScene::MT4_01_03() {
#ifdef USE_IMGUI
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
#endif
}

void TestScene::MT4_01_04() {
#ifdef USE_IMGUI
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
#endif
}

void TestScene::MT4_01_05() {
#ifdef USE_IMGUI
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
#endif
}