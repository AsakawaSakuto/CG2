#include "GameScene.h"

void GameScene::Initialize() {

	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);
	OutputDebugStringA((std::string("CWD: ") + path + "\n").c_str());

	// main関数の先頭でCOMを初期化 03_00
	CoInitializeEx(0, COINIT_MULTITHREADED);

	winApp->Initialize(L"CG2_Window");
	winApp->EnableResize(true);

	// ログのディレクトリを用意
	Logger::Initialize();
	std::filesystem::create_directory("logs");
	//AllocConsole();
	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);

	dxCommon->Initialize(winApp.get());

	TextureManager::GetInstance()->Initialize(dxCommon.get());

	input->Initialize(winApp.get());

	debugCamera->SetInput(input.get());
	
	//spriteData->Initialize(dxCommon.get());

	//sprite->Initialize(spriteData.get(), "resources/engineResources/uvChecker.png");
	//sprite->SetPosition({ 896.0f,128.0f });
	//
	//sprite2->Initialize(spriteData.get(), "resources/image/skydome.png");
	//sprite2->SetPosition({ 128.0f,128.0f });
	//
	//sprite3->Initialize(spriteData.get(), "resources/image/monsterBall.png");
	//sprite3->SetPosition({ 384.0f,128.0f });
	//
	//sprite4->Initialize(spriteData.get(), "resources/image/star.png");
	//sprite4->SetPosition({ 640.0f,128.0f });

	model->Initialize(dxCommon.get(), "resources/object3d/monkey.obj");
	model->SetPosition({ -2.f,0.f,0.f });

	model2->Initialize(dxCommon.get(), "resources/object3d/sphere.obj");
	model2->SetTexture("resources/image/monsterBall2.png");

	skydome->Initialize(dxCommon.get(), "resources/object3d/skydome.obj");
	//skydome->SetTexture("resources/image/Skydome.png");

	audio->Initialize("resources/sound/fanfare.wav");

	audio2->Initialize("resources/sound/start.wav");

	//sphereData->Initialize(dxCommon.get());

	//sphere->Initialize(sphereData.get(), "resources/engineResources/uvChecker.png");

	fenceModel->Initialize(dxCommon.get(), "resources/object3d/fence.obj");
	fenceModel->SetTexture("resources/image/fence.png");

	//triangle->Initialize(dxCommon.get());

	particles->Initialize(dxCommon.get(), "resources/image/circle.png");
}

void GameScene::Update() {
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {
		endRequst_ = true;
	}

	input->Update();

	// Zキーがトリガー（今回押されていて、前回押されていない）なら再生
	if (input->TriggerKey(DIK_Z)) {
		audio->PlayAudio();
		audio2->PlayAudio();
	}

	particles->SetEmitter(model->GetPosition());

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
			useCamera = debugCamera.get();
		}
	} else {
		if (camera != nullptr) {
			camera->Update();
			useCamera = camera.get();
		}
	}

	if (particlesTexture) {
		particles->SetTexture("resources/image/circle.png");
	} else {
		particles->SetTexture("resources/engineResources/uvChecker.png");
	}

	if (modelTexture) {
		model->SetTexture("resources/engineResources/white16x16.png");
	} else {
		model->SetTexture("resources/engineResources/uvChecker.png");
	}

	//sprite->Update();
	//sprite2->Update();
	//sprite3->Update();
	//sprite4->Update();

	model->Update(*useCamera);
	model2->Update(*useCamera);
	skydome->SetColor({ 1.0f,1.0f,1.0f,0.5f });
	skydome->Update(*useCamera);
	particles->Update(*useCamera);
	//fenceModel->Update(*useCamera);
	//sphere->Update(*useCamera);
	//triangle->Update();
}

void GameScene::Draw() {

	dxCommon->PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	//fenceModel->Draw();
	//sphere->Draw();
	//triangle->Draw();
	//sphere->Draw();

	skydome->Draw();
	model->Draw();
	model2->Draw();
	particles->Draw();

	//sprite->Draw();
	//sprite2->Draw();
	//sprite3->Draw();
	//sprite4->Draw();

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

	ImGui::Begin("GameScene Control");

	if (isDebugCamera) {

		ImGui::Text("Debug Camera");
		ImGui::Checkbox("CameraModeChange", &isDebugCamera);

		ImGui::Checkbox("particleTexture", &particlesTexture);
		ImGui::Checkbox("monkeyTexture", &modelTexture);

		ImGui::Text("LoadTexture Count: %zu", TextureManager::GetInstance()->GetTextureCount());
		ImGui::Text("Path-Index Map Size: %zu", TextureManager::GetInstance()->GetPathToIndexMapSize());
		ImGui::Text("Max SRV Slots: %u", DirectXCommon::kMaxSRVCount_);
	} else {

		ImGui::Text("Normal Camera");
		ImGui::Checkbox("CameraModeChange", &isDebugCamera);
		ImGui::DragFloat3("CameraTranslate", &useCamera->GetTranslate().x, 0.1f);
		ImGui::DragFloat3("CameraRotate", &useCamera->GetRotate().x, 0.1f);

	}

	ImGui::End();

	debugCamera->DrawImgui();
	model->DrawImGui("monkey");
	model2->DrawImGui("sphere");
	particles->DrawImGui("particles");
	skydome->DrawImGui("Skydome");

	//sprite->DrawImGui("Sprite");

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	dxCommon->PostDraw(); // ここより下に描画処理を書かない
}

void GameScene::Finalize() {
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

	///
	/// ↑描画処理ここまで
	///
}