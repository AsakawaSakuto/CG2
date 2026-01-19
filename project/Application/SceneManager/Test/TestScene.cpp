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
	

}

void TestScene::Update() {

}

void TestScene::Draw() {

}

void TestScene::DrawImGui() {
#ifdef USE_IMGUI

#endif
}

void TestScene::PostFrameCleanup() {

}