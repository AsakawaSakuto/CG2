#include"WinApp.h"
#include"Input.h"
#include"Camera.h"
#include"Audio.h"
#include"DebugCamera.h"
#include"ConvertString.h"
#include"DirectXCommon.h"
#include"D3DResourceLeakChecker.h"
#include"Sprite.h"
#include"SpriteData.h"
#include"Object3d.h"
#include"Object3dData.h"
#include"TextureManager.h"
#include"Sphere.h"

#include <filesystem>
#include "externals/DirectXTex/DirectXTex.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

using Microsoft::WRL::ComPtr;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // main関数の先頭でCOMを初期化 03_00
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* winApp = new WinApp();
	winApp->Initialize(L"CG2_Window");
	winApp->EnableResize(true);

	// ログのディレクトリを用意
	Logger::Initialize();
	std::filesystem::create_directory("logs");
	//AllocConsole();
	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);

	DirectXCommon* dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	D3ResourceLeakChecker d3ResourceLeakCheker;

	TextureManager::GetInstance()->Initialize(dxCommon);

	Input* input = new Input();
	input->Initialize(winApp);

	SpriteData* spriteData = new SpriteData();
	spriteData->Initialize(dxCommon);

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteData, "resources/image/uvChecker.png");

	Sprite* sprite2 = new Sprite();
	sprite2->Initialize(spriteData, "resources/image/monsterBall.png");

	Sprite* sprite3 = new Sprite();
	sprite3->Initialize(spriteData, "resources/image/star.png");

	Object3dData* object3dData = new Object3dData();
	object3dData->Initialize(dxCommon);

	Object3d* model = new Object3d();
	model->Initialize(object3dData, "resources/object3d", "plane.obj", "resources/image/monsterBall.png");

	Object3d* model2 = new Object3d();
	model2->Initialize(object3dData, "resources/object3d", "axis.obj", "resources/image/uvChecker.png");

	Audio* audio = new Audio();
	audio->Initialize("resources/sound/fanfare.wav");

	Audio* audio2 = new Audio();
	audio2->Initialize("resources/sound/start.wav");

	SphereData* sphereData = new SphereData();
	sphereData->Initialize(dxCommon);

	Sphere* sphere = new Sphere();
	sphere->Initialize(sphereData, "resources/image/uvChecker.png");

	Camera* camera = new Camera();
	DebugCamera* debugCamera = new DebugCamera();
	Camera* useCamera = new Camera();
	bool isDebugCamera = false;

	// メインループ 00_03
	MSG msg{};
	// ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMessage()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		} else { // ゲームの処理

			///
		    /// ↓更新処理ここから
		    ///

			input->Update();

			// Zキーがトリガー（今回押されていて、前回押されていない）なら再生
			if (input->TriggerKey(DIK_Z)) {
				audio->PlayAudio();
				audio2->PlayAudio();
			}

			if (input->TriggerKey(DIK_SPACE)) {
				if (isDebugCamera) {
					isDebugCamera = false;
				} else {
					isDebugCamera = true;
				}
			}

			if (isDebugCamera) {
				if (debugCamera != nullptr) {
					debugCamera->Update();
					useCamera = debugCamera;
				}
			} else {
				if (camera != nullptr) {
					camera->Update();
					useCamera = camera;
				}
			}

			sprite->SetPosition({ 640.f, 360.f });
			sprite2->SetPosition({ 0.f, 0.f });
			sprite3->SetPosition({ 300.f, 0.f });

			sprite->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

			sprite->Update();
			sprite2->Update();
			sprite3->Update();

			model->Update(*useCamera);
			model2->Update(*useCamera);

			sphere->Update(*useCamera);

			///
		    /// ↑更新処理ここまで
		    ///

			///
		    /// ↓描画処理ここから
		    ///

			dxCommon->PreDraw(); // ここより上に描画処理を書かない

			spriteData->SpriteDataSet();
			object3dData->Object3dDataSet();
			sphereData->SphereDataSet();

			sprite->Draw();
			sprite2->Draw();
			sprite3->Draw();

			model->Draw();
			model2->Draw();

			sphere->Draw();

			///
			/// ↑描画処理ここまで
			///

			///
			/// ↓ImGuiここから
			///

			// フレームの先頭でImguiにここからフレームが始まる旨を告げる
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			// 開発用UIの処理、実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
			/*ImGui::ShowDemoWindow();*/

			// --- ImGuiカメラコントローラ ---

			ImGui::Begin("Camera Control");

			if (isDebugCamera) {
				ImGui::Text("Debug Camera");
				ImGui::DragFloat3("CameraRotate", &useCamera->GetRotate().x, 0.1f);
				ImGui::DragFloat3("CameraTranslate", &useCamera->GetTranslate().x, 0.1f);
				ImGui::Checkbox("CameraModeChange", &isDebugCamera);
			} else {
				ImGui::Text("Normal Camera");
				ImGui::DragFloat3("CameraRotate", &useCamera->GetRotate().x, 0.1f);
				ImGui::DragFloat3("CameraTranslate", &useCamera->GetTranslate().x, 0.1f);
				ImGui::Checkbox("CameraModeChange", &isDebugCamera);
			}

			ImGui::End();

			// Imguiの内部コマンドを生成する
			ImGui::Render();

			///
			/// ↑ImGuiここまで
			///

			dxCommon->PostDraw(); // ここより下に描画処理を書かない

		}
	} // メインループ外

    //ゲーム終了時にはCOMの終了処理を行っておく
	CoUninitialize();

	// Imguiの終了処理
	// ImGuiの終了処理。詳細はさして重要ではないので解説は省略する。
	// こういうもんである。初期化と逆順に行う
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	audio->Reset();
	audio2->Reset();

    // 解放処理(リソースチェックの前) 01_03
	dxCommon->CloseFence();
	winApp->Finalize();
	TextureManager::GetInstance()->Finalize();

	///
	/// ↓開放処理ここから
	///

	delete camera;
	camera = nullptr;

	delete debugCamera;
	debugCamera = nullptr;

	useCamera = nullptr;

	delete input;
	input = nullptr;

	delete winApp;
	winApp = nullptr;

	delete sprite;
	sprite = nullptr;

	delete sprite2;
	sprite2 = nullptr;

	delete sprite3;
	sprite3 = nullptr;

	delete spriteData;
	spriteData = nullptr;

	delete model;
	model = nullptr;

	delete object3dData;
	object3dData = nullptr;

	delete audio;
	audio = nullptr;

	delete audio2;
	audio2 = nullptr;

	delete sphere;
	sphere = nullptr;

	delete sphereData;
	sphereData = nullptr;

	///
	/// ↑描画処理ここまで
	///

	return 0;
}
