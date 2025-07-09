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
	
	ball->Initialize(dxCommon.get(), "resources/object3d/uvSphere.obj", "resources/engineResources/uvChecker.png");
	ball->SetTexture("resources/image/monsterBall.png");
	skydome->Initialize(dxCommon.get(), "resources/object3d/skydome.obj", "resources/image/skydome.png");
	plane->Initialize(dxCommon.get(), "resources/object3d/planeobj.obj", "resources/image/GroundTexture.png");
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
	CameraController();

	/*particles->SetEmitter(ball->GetPosition());*/

	ball->Update(*useCamera);
	plane->Update(*useCamera);
	skydome->Update(*useCamera);
	particles->Update(*useCamera);
}

void GameScene::Draw() {

	dxCommon->PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	//ball->Draw();
	//plane->Draw();
	skydome->Draw();
	particles->Draw();

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

	DrawFPS_ImGui();

	debugCamera->DrawImgui();

	//ball->DrawImGui("ball");
	//plane->DrawImGui("Plane");
	particles->DrawImGui("particle");

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

void GameScene::CameraController() {
	if (input->TriggerKey(DIK_SPACE)) {
		if (isDebugCamera) {
			isDebugCamera = false;
		}
		else {
			isDebugCamera = true;
		}
	}

	if (isDebugCamera) {
		if (debugCamera != nullptr) {
			debugCamera->Update();
			useCamera = debugCamera.get();
		}
	}
	else {
		if (camera != nullptr) {
			camera->Update();
			useCamera = camera.get();
		}
	}
}

void GameScene::DrawFPS_ImGui() {
	static float fpsHistory[100] = {};
	static int frameCount = 0;

	float currentFPS = ImGui::GetIO().Framerate;
	fpsHistory[frameCount % IM_ARRAYSIZE(fpsHistory)] = currentFPS;
	frameCount++;

	ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - 20.0f, 20.0f); // 右上にオフセット付き
	ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f); // 原点を右上にする
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.35f); // 背景を少し透過

	ImGui::Begin("Client FPS", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	// FPSグラフと数値の描画
	ImGui::PlotLines("##FPSGraph", fpsHistory, IM_ARRAYSIZE(fpsHistory), 0, nullptr, 0.0f, 175.0f, ImVec2(0, 80));
	ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
	ImGui::Separator();
	ImGui::Text("LoadTexture Count: %zu", TextureManager::GetInstance()->GetTextureCount());
	ImGui::Text("Path-Index Map Size: %zu", TextureManager::GetInstance()->GetPathToIndexMapSize());
	ImGui::Text("Max SRV Slots: %u", DirectXCommon::kMaxSRVCount_);

	ImGui::End();
}