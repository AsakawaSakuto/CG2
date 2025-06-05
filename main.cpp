#include <Windows.h>  // 00_01
#include <string>     // 00_01

#include <cstdint>    // 00_03

#include <filesystem> // 00_04 EX ファイルやディレクトリに関する操作を行うライブラリ
#include <fstream>    // 00_04 EX ファイルに書いたり読んだりするライブラリ
#include <chrono>     // 00_04 EX 時間を扱うライブラリ

#include <d3d12.h>               // 00_05
#include <dxgi1_6.h>             // 00_05
#include <cassert>               // 00_05
#pragma comment(lib,"d3d12.lib") // 00_05
#pragma comment(lib,"dxgi.lib")  // 00_05

#include <dxgidebug.h>            // 01_03
#pragma comment(lib,"dxguid.lib") // 01_03
#include <dxcapi.h>                   // 02_00
#pragma comment(lib,"dxcompiler.lib") // 02_00

#include "externals/imgui/imgui.h"            // 02_03
#include "externals/imgui/imgui_impl_dx12.h"  // 02_03
#include "externals/imgui/imgui_impl_win32.h" // 02_03

#include <fstream> // 06_02
#include <sstream> // 06_02

#include <wrl.h> // 06_03

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"

#include"Camera.h"
#include"DebugCamera.h"
#include"Input.h"
#include"Audio.h"
#include"WinApp.h"
#include"ConvertString.h"
#include"DirectXCommon.h"
#include"D3DResourceLeakChecker.h"
#include"Sprite.h"
#include"SpriteData.h"
#include"Object3d.h"
#include"Object3dData.h"
#include"TextureManager.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexData.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "MaterialData.h"
#include "ModelData.h"

// ウィンドウプロシャージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	// メッセージに応じて固有の処理を行う
	switch (msg) {
		// ウィンドウが破壊された
	case WM_DESTROY:
		// OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#pragma region 球体メッシュを作成する関数 05_00

// 球体メッシュを作成する関数
void CreateSphereMesh(std::vector<VertexData>& vertices, int subdivision) {
	const float pi = 3.1415926535f;
	const float lonEvery = 2.0f * pi / subdivision;
	const float latEvery = pi / subdivision;

	vertices.clear();
	vertices.resize(subdivision * subdivision * 6);

	auto Normalize = [](const Vector3& v) -> Vector3 {
		float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len > 0.0f) return { v.x / len, v.y / len, v.z / len };
		else return { 0.0f, 1.0f, 0.0f }; // fallback（ゼロベクトル回避）
		};

	for (int latIndex = 0; latIndex < subdivision; ++latIndex) {
		float lat = -pi / 2.0f + latEvery * latIndex;
		for (int lonIndex = 0; lonIndex < subdivision; ++lonIndex) {
			float lon = lonEvery * lonIndex;
			uint32_t start = (latIndex * subdivision + lonIndex) * 6;

			// 頂点座標
			Vector3 a = { cosf(lat) * cosf(lon), sinf(lat), cosf(lat) * sinf(lon) };
			Vector3 b = { cosf(lat) * cosf(lon + lonEvery), sinf(lat), cosf(lat) * sinf(lon + lonEvery) };
			Vector3 c = { cosf(lat + latEvery) * cosf(lon), sinf(lat + latEvery), cosf(lat + latEvery) * sinf(lon) };
			Vector3 d = { cosf(lat + latEvery) * cosf(lon + lonEvery), sinf(lat + latEvery), cosf(lat + latEvery) * sinf(lon + lonEvery) };

			// UV座標
			float u0 = float(lonIndex) / subdivision;
			float v0 = 1.0f - float(latIndex) / subdivision;
			float u1 = float(lonIndex + 1) / subdivision;
			float v1 = 1.0f - float(latIndex + 1) / subdivision;

			// 法線
			Vector3 na = Normalize(a);
			Vector3 nb = Normalize(b);
			Vector3 nc = Normalize(c);
			Vector3 nd = Normalize(d);

			// 頂点データ代入（paddingも構造体に含まれている前提）
			vertices[start + 0] = { {a.x, a.y, a.z, 1.0f}, {u0, v0}, na };
			vertices[start + 1] = { {c.x, c.y, c.z, 1.0f}, {u0, v1}, nc };
			vertices[start + 2] = { {b.x, b.y, b.z, 1.0f}, {u1, v0}, nb };

			vertices[start + 3] = { {c.x, c.y, c.z, 1.0f}, {u0, v1}, nc };
			vertices[start + 4] = { {d.x, d.y, d.z, 1.0f}, {u1, v1}, nd };
			vertices[start + 5] = { {b.x, b.y, b.z, 1.0f}, {u1, v0}, nb };
		}
	}
}

// 球体メッシュをインデックス付きで作成する関数
void CreateIndexedSphereMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, int subdivision) {
	const float pi = 3.1415926535f;
	const float lonEvery = 2.0f * pi / subdivision;
	const float latEvery = pi / subdivision;

	vertices.clear();
	indices.clear();

	auto Normalize = [](const Vector3& v) -> Vector3 {
		float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len > 0.0f) return { v.x / len, v.y / len, v.z / len };
		else return { 0.0f, 1.0f, 0.0f };
		};

	for (int lat = 0; lat <= subdivision; ++lat) {
		float theta = -pi / 2.0f + latEvery * lat;
		float v = 1.0f - float(lat) / subdivision;

		for (int lon = 0; lon <= subdivision; ++lon) {
			float phi = lonEvery * lon;
			float u = float(lon) / subdivision;

			Vector3 pos = {
				cosf(theta) * cosf(phi),
				sinf(theta),
				cosf(theta) * sinf(phi)
			};
			Vector3 normal = Normalize(pos);
			vertices.push_back({ { pos.x, pos.y, pos.z, 1.0f }, { u, v }, normal });
		}
	}

	for (int lat = 0; lat < subdivision; ++lat) {
		for (int lon = 0; lon < subdivision; ++lon) {
			int i0 = lat * (subdivision + 1) + lon;
			int i1 = i0 + 1;
			int i2 = i0 + (subdivision + 1);
			int i3 = i2 + 1;

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i1);

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}
}


#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//AllocConsole();
	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);
#pragma region main関数の先頭でCOMを初期化 03_00

	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma endregion

	Logger::Initialize();

	WinApp* winApp = new WinApp();
	winApp->Initialize(L"CG2_Window");
	winApp->EnableResize(true);

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

	// ログのディレクトリを用意
	std::filesystem::create_directory("logs");

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
			}

			if (input->TriggerKey(DIK_SPACE)) {
				if (isDebugCamera) {
					isDebugCamera = false;
				} else {
					isDebugCamera = true;
				}
			}

			if (isDebugCamera)
			{
				if (debugCamera != nullptr)
				{
					debugCamera->Update();
					useCamera = debugCamera;
				}
			} else {
				if (camera != nullptr)
				{
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

			///
		    /// ↑更新処理ここまで
		    ///

			///
		    /// ↓描画処理ここから
		    ///

			dxCommon->PreDraw(); // ここより上に描画処理を書かない

			spriteData->SpriteDataSet();
			object3dData->Object3dDataSet();


			sprite->Draw();
			sprite2->Draw();
			sprite3->Draw();

			model->Draw();
			model2->Draw();

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

	///
	/// ↑描画処理ここまで
	///

	return 0;
}
