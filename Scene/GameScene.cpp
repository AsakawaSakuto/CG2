#include "GameScene.h"

void GameScene::Initialize() {
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

	dxCommon->Initialize(winApp);

	TextureManager::GetInstance()->Initialize(dxCommon);

	input->Initialize(winApp);

	spriteData->Initialize(dxCommon);

	sprite->Initialize(spriteData, "resources/image/uvChecker.png");

	sprite2->Initialize(spriteData, "resources/image/monsterBall.png");

	sprite3->Initialize(spriteData, "resources/image/white16x16.png");

	object3dData->Initialize(dxCommon);

	model->Initialize(object3dData, "resources/object3d/monkey.obj");

	model2->Initialize(object3dData, "resources/object3d/icoSphere.obj");

	audio->Initialize("resources/sound/fanfare.wav");

	audio2->Initialize("resources/sound/start.wav");

	sphereData->Initialize(dxCommon);

	sphere->Initialize(sphereData, "resources/image/uvChecker.png");

	fenceModel->Initialize(object3dData, "resources/object3d/fence.obj");
	fenceModel->SetTexture("resources/image/fence.png");
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

	if (drawTexture) {
		model->SetTexture("resources/engineResources/uvChecker.png");
	} else {
		model->SetTexture("resources/engineResources/white16x16.png");
	}

	if (drawTexture2) {
		model2->SetTexture("resources/engineResources/uvChecker.png");
	}
	else {
		model2->SetTexture("resources/engineResources/white16x16.png");
	}

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

	sprite->SetPosition({ 600.f, 0.f });
	sprite2->SetPosition({ 0.f, 0.f });
	sprite3->SetPosition({ 300.f, 0.f });

	sprite->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

	sprite->Update();
	sprite2->Update();
	sprite3->Update();

	fenceModel->Update(*useCamera);

	model->Update(*useCamera);
	model->SetDrawMode(drawMode);

	model2->Update(*useCamera);
	model2->SetPosition({ 2.f,0.f,0.f });
	model2->SetDrawMode(drawMode2);

	sphere->Update(*useCamera);
}

void GameScene::Draw() {

	///
	/// ↓描画処理ここから
	///

	dxCommon->PreDraw(); // ここより上に描画処理を書かない

	sprite->Draw();
	sprite2->Draw();
	sprite3->Draw();

	//model->Draw();
	//model2->Draw();

	fenceModel->Draw();

	//sphere->Draw();

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

		ImGui::DragFloat3("fencePos", &fenceModel->GetPosition().x, 0.1f);
		ImGui::ColorEdit4("ModelColor", &fenceModel->GetColor().x);

		//ImGui::Checkbox("DrawMode", &drawMode);
		//ImGui::Checkbox("DrawMode2", &drawMode2);
		//ImGui::Checkbox("DrawTexture", &drawTexture);
		//ImGui::Checkbox("DrawTexture2", &drawTexture2);


		ImGui::Text("Texture Count: %zu", TextureManager::GetInstance()->GetTextureCount());
		ImGui::Text("Path-Index Map Size: %zu", TextureManager::GetInstance()->GetPathToIndexMapSize());
		ImGui::Text("Max SRV Slots: %u", DirectXCommon::kMaxSRVCount_);
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
}