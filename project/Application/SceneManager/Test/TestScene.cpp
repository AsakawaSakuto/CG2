#include "TestScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"

#define WHITE {1.0f,1.0f,1.0f,1.0f}
#define RED   {1.0f,0.0f,0.0f,1.0f}

TestScene::~TestScene() {
    CleanupResources();
}

void TestScene::CleanupResources() {

}

void TestScene::Initialize() {
    nowSceneName_ = SCENE::TEST;
    
    CleanupResources();
	camera_.SetPosition({ 0.0f, 2.5f, -20.0f });
	debugCamera_.SetInput(MyInput::GetInput());
	debugCamera_.SetPosition({ 0.0f, 2.5f, -20.0f });

    testParticle_->Initialize("temp", 1000);

	testSprite_->Initialize("icon/fireBall.png", { 64.0f,64.0f });

	cube_->Initialize("cube.obj");
    cubeTransform_.translate = { -1.5f,2.5f,0.0f };

	spinCube_->Initialize("Animation/cube/AnimatedCube.gltf");
    spinCubeTransform_.translate = { 1.5f,4.5f,0.0f };
	spinCubeTransform_.scale = { 0.5f,0.5f,0.5f };

	simpleSkin_->Initialize("Animation/SimpleSkin/SimpleSkin.gltf");
	simpleSkinTransform_.rotate = { 0.0f,3.12f,0.0f };
	simpleSkinTransform_.translate = { -3.0f,0.0f,0.0f };

    walk_->Initialize("Animation/human/Walk.gltf");
    walkTransform_.rotate = { 0.0f,3.12f,0.0f };
    walkTransform_.translate = { 0.0f,0.0f,0.0f };

    sneakWalk_->Initialize("player/Animation/idle.gltf");
    sneakWalkTransform_.rotate = { 0.0f, 3.12f,0.0f };
    sneakWalkTransform_.translate = { 3.0f,0.0f,0.0f };

	walkAnimation_ = LoadAnimationFile("Animation/human/Walk.gltf");
	sneakWalkAnimation_ = LoadAnimationFile("Animation/human/sneakWalk.gltf");

	testPlayer_->Initialize("test/player.gltf");
	testPlayerTransform_.translate = { 2.0f,2.0f,0.0f };

	testTimer_.Start(2.0f, true);

	// 楕円球体の初期化
	testOvalSphere_.center = { 0.0f, 0.0f, 0.0f };
	testOvalSphere_.radius = { 1.5f, 1.0f, 1.5f };
	testOvalSphere_.rotate = { 0.0f, 0.0f, 0.0f };
	testOvalSphere_.UpdateOrientation();

	bitmapFont_.Initialize();

    testGauge_->Initialize();

    MyAudio::PlayBGM(BGM_List::TEST, 0.0f);
}

void TestScene::Update() {

    if (MyInput::TriggerKey(DIK_1)) {
        MyAudio::PlaySE(SE_List::KAWAII, 0.1f);
    }
    if (MyInput::TriggerKey(DIK_2)) {
        MyAudio::PlaySE(SE_List::OU, 0.1f);
    }
    if (MyInput::TriggerKey(DIK_3)) {
        MyAudio::PlaySE(SE_List::KIRAKIRA, 0.1f);
    }
    if (MyInput::TriggerKey(DIK_4)) {
        MyAudio::PlaySE(SE_List::DON, 0.1f);
    }

    Vector3 start = { 0.0f, 0.0f, 0.0f };
    Vector3 end = { 1.0f, 1.0f, 1.0f };
    Line line = { walkTransform_.translate, sneakWalkTransform_.translate };
    MyDebugLine::AddShape(line);

	// 回転を適用（orientation行列を更新）
	//testOBB_.UpdateOrientation();
	testOvalSphere_.UpdateOrientation();

	// 当たり判定チェック
	bool sphereHit = false;
	bool ovalSphereHit = false;
	bool aabbHit = false;
	bool obbHit = false;

	// Sphere との衝突判定
	sphereHit = Collision::IsHit(testSphere_, testOvalSphere_) ||
	            Collision::IsHit(testSphere_, testAABB_) ||
	            Collision::IsHit(testSphere_, testOBB_);

	// OvalSphere との衝突判定
	ovalSphereHit = Collision::IsHit(testOvalSphere_, testSphere_) ||
	                Collision::IsHit(testOvalSphere_, testAABB_) ||
	                Collision::IsHit(testOvalSphere_, testOBB_);

	// AABB との衝突判定
	aabbHit = Collision::IsHit(testAABB_, testSphere_) ||
	          Collision::IsHit(testAABB_, testOvalSphere_) ||
	          Collision::IsHit(testAABB_, testOBB_);

	// OBB との衝突判定
	obbHit = Collision::IsHit(testOBB_, testSphere_) ||
	         Collision::IsHit(testOBB_, testOvalSphere_) ||
	         Collision::IsHit(testOBB_, testAABB_);

	// 衝突状態に応じて色を設定して描画
	Vector4 sphereColor = sphereHit ? Vector4 RED : Vector4 WHITE;
	Vector4 ovalSphereColor = ovalSphereHit ? Vector4 RED : Vector4 WHITE;
	Vector4 aabbColor = aabbHit ? Vector4 RED : Vector4 WHITE;
	Vector4 obbColor = obbHit ? Vector4 RED : Vector4 WHITE;

    //testAABB_.UpdateOrientation();
	//testOBB_.UpdateOrientation();

	MyDebugLine::AddShape(testSphere_, sphereColor);
	MyDebugLine::AddShape(testOvalSphere_, ovalSphereColor);
	MyDebugLine::AddShape(testAABB_, aabbColor);
	MyDebugLine::AddShape(testOBB_, obbColor);
    
    MyDebugLine::AddShape(testPlane_);

    MyDebugLine::AddGrid(20.0f, 20);

	testParticle_->Update();

	spinCube_->Update();
    simpleSkin_->Update();
    sneakWalk_->Update();
    walk_->Update(1.0f / 30.0f);
	testPlayer_->Update();

	testSprite_->Update();

	testTimer_.Update();

    camera_ = debugCamera_;
	camera_.Update();
	debugCamera_.Update();

    bitmapFont_.SetNumber(setValue_);

    testGauge_->Update(currentGaugeValue_, maxGaugeValue_);
}

void TestScene::Draw() {

    auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
    postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());

    cube_->Draw(camera_);
    spinCube_->Draw(camera_, spinCubeTransform_);
    simpleSkin_->Draw(camera_, simpleSkinTransform_);
	walk_->Draw(camera_, walkTransform_);
    sneakWalk_->Draw(camera_, sneakWalkTransform_);
	testPlayer_->Draw(camera_, testPlayerTransform_);

	testParticle_->Draw(camera_);

	testSprite_->Draw();

	bitmapFont_.Draw();

	testGauge_->Draw();

    MyDebugLine::Draw(camera_);
}

void TestScene::DrawImGui() {
#ifdef USE_IMGUI

    auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	postEffect->DrawImGui();

	testOBB_.DrawImGui("testOBB");
    testAABB_.DrawImGui("testAABB");
    testSphere_.DrawImGui("testSphere");
    testOvalSphere_.DrawImGui("testOvalSphere");
    testPlane_.DrawImGui("testPlane");
    
	cube_->DrawImGui("cubeModel");

	testParticle_->DrawImGui("testParticle");

    debugCamera_.DrawImgui();

	ImGui::Begin("Set Number");

    ImGui::DragInt("Value", &setValue_, 1, 0, 999999);

	ImGui::DragFloat("Test Float", &currentGaugeValue_, 0.1f, 0.0f, 100.0f);

	ImGui::DragFloat("Max Float", &maxGaugeValue_, 0.1f, 1.0f, 1000.0f);

	ImGui::End();

	bitmapFont_.DrawImGui("bitmapFont");

	testGauge_->DrawImGui("testGauge");

    //testSprite_->DrawImGui("testSprite");

	//MT4_01_01();
	//MT4_01_02();
	//MT4_01_03();
	//MT4_01_04();
    //MT4_01_05();

#endif
}

void TestScene::PostFrameCleanup() {

}