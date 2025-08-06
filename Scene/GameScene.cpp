#include "GameScene.h"

#include "Xinput.h"
#pragma comment(lib, "xinput.lib")

struct GamePadState {
	bool connected;
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;
	bool buttons[14]; // A,B,X,Y,LB,RB,Back,Start,LS,RS,DPAD(↑↓←→)
};

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

	audio_->Initialize("resources/sound/fanfare.wav");

	debugCamera_->SetInput(input_.get());

	skydome_->Initialize(dxCommon_.get(), "resources/object3d/skydome.obj");
	skydome_->SetTexture("resources/image/skydome.png");

	player_->Initialize(dxCommon_.get(), "resources/object3d/player/player.obj");

	particle_->Initialize(dxCommon_.get(), "resources/image/particle/circle.png", 1, 64, 65);
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

	if (input_->TriggerKey(DIK_Z)) {
		audio_->PlayAudio();
	}

	UpdateGamePad();

	player_->Update(*useCamera_);

	skydome_->Update(*useCamera_);

	particle_->Update(*useCamera_);
	particle_->SetEmitterPosition(player_->GetPosition());
}

void GameScene::Draw() {

	dxCommon_->PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	player_->Draw();

	skydome_->Draw();

	particle_->Draw();

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

	player_->DrawImGui("player");

	particle_->DrawImGui("particle");

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

void GameScene::UpdateGamePad() {

	GamePadState pad = {};
	XINPUT_STATE xinputState = {};

	DWORD dwResult = XInputGetState(0, &xinputState); // コントローラー0を取得
	if (dwResult == ERROR_SUCCESS) {
		pad.connected = true;

		// スティック（-32768 ~ +32767）
		pad.leftStickX = xinputState.Gamepad.sThumbLX / 32767.0f;
		pad.leftStickY = xinputState.Gamepad.sThumbLY / 32767.0f;
		pad.rightStickX = xinputState.Gamepad.sThumbRX / 32767.0f;
		pad.rightStickY = xinputState.Gamepad.sThumbRY / 32767.0f;

		// トリガー（0 ~ 255）
		pad.leftTrigger = xinputState.Gamepad.bLeftTrigger / 255.0f;
		pad.rightTrigger = xinputState.Gamepad.bRightTrigger / 255.0f;

		// ボタン状態
		WORD buttons = xinputState.Gamepad.wButtons;
		pad.buttons[0] = (buttons & XINPUT_GAMEPAD_A) != 0;
		pad.buttons[1] = (buttons & XINPUT_GAMEPAD_B) != 0;
		pad.buttons[2] = (buttons & XINPUT_GAMEPAD_X) != 0;
		pad.buttons[3] = (buttons & XINPUT_GAMEPAD_Y) != 0;
		pad.buttons[4] = (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		pad.buttons[5] = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		pad.buttons[6] = (buttons & XINPUT_GAMEPAD_BACK) != 0;
		pad.buttons[7] = (buttons & XINPUT_GAMEPAD_START) != 0;
		pad.buttons[8] = (buttons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
		pad.buttons[9] = (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
		pad.buttons[10] = (buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		pad.buttons[11] = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		pad.buttons[12] = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		pad.buttons[13] = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
	}
	else {
		pad.connected = false;
	}

	XINPUT_VIBRATION vibration = {};
	if (pad.connected) {
		if (pad.buttons[0]) {
			vibration.wLeftMotorSpeed = static_cast<WORD>(padMotorRange.x * 65535.0f);
			vibration.wRightMotorSpeed = static_cast<WORD>(padMotorRange.y * 65535.0f);
		}
		else if (pad.buttons[4]) {
			vibration.wLeftMotorSpeed = static_cast<WORD>(padMotorRange.x * 65535.0f);
		}
		else if (pad.buttons[5]) {
			vibration.wRightMotorSpeed = static_cast<WORD>(padMotorRange.y * 65535.0f);
		}
		else {
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
		}
	}

	// 反映
	XInputSetState(0, &vibration);

}