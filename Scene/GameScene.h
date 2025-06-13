#pragma once
#pragma region Include
// System
#include "WinApp.h"
#include "Input.h"
#include "Camera.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "ConvertString.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "TextureManager.h"
#include <filesystem>
// Object
#include "Sprite.h"
#include "SpriteData.h"
#include "Object3d.h"
#include "Object3dData.h"
#include "Sphere.h"
#include "SphereData.h"
#include "Triangle.h"
#include "Particles.h"
// Math
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#pragma endregion

using Microsoft::WRL::ComPtr;

class GameScene
{
public:
	
	void Initialize();

	void Finalize();

	void Update();

	void Draw();
	
	bool IsEndRequst() { return endRequst_; }

private:
	bool endRequst_ = false;
	MSG msg{};

	WinApp* winApp = new WinApp();
	
	DirectXCommon* dxCommon = new DirectXCommon();
	
	D3ResourceLeakChecker d3ResourceLeakCheker;

	Input* input = new Input();

	/*---------------------------------------------*/

	SpriteData* spriteData = new SpriteData();

	Sprite* sprite = new Sprite();

	Sprite* sprite2 = new Sprite();

	Sprite* sprite3 = new Sprite();

	Object3dData* object3dData = new Object3dData();

	Object3d* model = new Object3d();

	Object3d* model2 = new Object3d();

	Object3d* skydome = new Object3d();

	Object3d* fenceModel = new Object3d();

	Audio* audio = new Audio();
	
	Audio* audio2 = new Audio();
	
	SphereData* sphereData = new SphereData();
	
	Sphere* sphere = new Sphere();
	
	Triangle* triangle = new Triangle();

	Particles* particles = new Particles();

	Camera* camera = new Camera();
	DebugCamera* debugCamera = new DebugCamera();
	Camera* useCamera = new Camera();
	bool isDebugCamera = true;
};