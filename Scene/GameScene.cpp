#include "GameScene.h"

void GameScene::Initialize() {

	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);
	OutputDebugStringA((std::string("CWD: ") + path + "\n").c_str());

	// main関数の先頭でCOMを初期化 03_00
	CoInitializeEx(0, COINIT_MULTITHREADED);

	winApp_->Initialize(L"CG2_Window");
	winApp_->EnableResize(true);

	// ログのディレクトリを用意
	Logger::Initialize();
	std::filesystem::create_directory("logs");
	//AllocConsole();
	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);

	dxCommon_->Initialize(winApp_.get());

	TextureManager::GetInstance()->Initialize(dxCommon_.get());

	input_->Initialize(winApp_.get());

	debugCamera_->SetInput(input_.get());

	player_->Initialize(dxCommon_.get());
	skyBox_->Initialize(dxCommon_.get());
}

void GameScene::Update() {
	while (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	if (msg_.message == WM_QUIT) {
		endRequst_ = true;
	}

	input_->Update();
	CameraController();

	player_->Update(useCamera_);
	skyBox_->Update(useCamera_);
}

void GameScene::Draw() {

	dxCommon_->PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	player_->Draw();
	skyBox_->Draw();

	//ground_->Draw();

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

	debugCamera_->DrawImgui();

	player_->DrawImGui();

	//skyBox_->DrawImGui();

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	dxCommon_->PostDraw(); // ここより下に描画処理を書かない
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

	audio_->Reset();
	audio2_->Reset();

	// 解放処理(リソースチェックの前) 01_03
	dxCommon_->CloseFence();
	winApp_->Finalize();
	TextureManager::GetInstance()->Finalize();

	///
	/// ↓開放処理ここから
	///

	///
	/// ↑描画処理ここまで
	///
}

void GameScene::CameraController() {
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugCamera_) {
			isDebugCamera_ = false;
		}
		else {
			isDebugCamera_ = true;
		}
	}

	if (isDebugCamera_) {
		if (debugCamera_ != nullptr) {
			debugCamera_->Update();
			useCamera_ = debugCamera_.get();
		}
	}
	else {
		if (camera_ != nullptr) {
			camera_->Update();
			useCamera_ = camera_.get();
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
	ImGui::SetNextWindowBgAlpha(0.8f); // 背景を少し透過

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