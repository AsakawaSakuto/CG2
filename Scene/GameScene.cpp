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

	audio->Initialize("resources/sound/fanfare.wav");

	debugCamera->SetInput(input.get());
	
	sphereData->Initialize(dxCommon.get());
	sphere->Initialize(sphereData.get(), "resources/engineResources/uvChecker.png");

	spriteData->Initialize(dxCommon.get());
	sprite->Initialize(spriteData.get(), "resources/engineResources/uvChecker.png");
	sprite->SetPosition({ 128.0f,128.0f });

	skydome->Initialize(dxCommon.get(), "resources/object3d/skydome.obj", "resources/image/skydome.png");

	plane->Initialize(dxCommon.get(), "resources/object3d/plane.obj", "resources/engineResources/uvChecker.png");
	plane->SetPosition({ 3.0f,0.0f,0.0f });

	teapot->Initialize(dxCommon.get(), "resources/object3d/teapot.obj", "resources/engineResources/uvChecker.png");
	teapot->SetPosition({ -3.0f,0.0f,0.0f });

	bunny->Initialize(dxCommon.get(), "resources/object3d/bunny.obj", "resources/engineResources/uvChecker.png");
	bunny->SetPosition({ 6.0f,0.0f,0.0f });

	suzanne->Initialize(dxCommon.get(), "resources/object3d/suzanne.obj", "resources/engineResources/uvChecker.png");
	suzanne->SetPosition({ -6.0f,0.0f,0.0f });

	multiMesh->Initialize(dxCommon.get(), "resources/object3d/multiMesh.obj", "resources/engineResources/uvChecker.png");
	multiMesh->SetPosition({ 4.0f,4.0f,0.0f });

	multiMaterial->Initialize(dxCommon.get(), "resources/object3d/multiMaterial.obj", "resources/engineResources/uvChecker.png");
	multiMaterial->SetPosition({ -4.0f,4.0f,0.0f });

	particles->Initialize(dxCommon.get(), "resources/image/circle.png", 512 * 2, 64, 65);

	EmitterRange range = {};
	range.minScale = { 1.0f,1.0f,1.0f };
	range.maxScale = { 3.0f,3.0f,3.0f };
	range.minTranslate = { 1.0f,1.0f,1.0f };
	range.maxTranslate = { 1.0f,1.0f,1.0f };
	range.minVelocity = { -0.1f,0.1f,0.0f };
	range.maxVelocity = { 0.1f,1.0f,0.0f };
	range.minColor = { 0.0f,0.0f,0.0f };
	range.maxColor = { 1.0f,1.0f,1.0f };
	range.minLifeTime = 0.1f;
	range.maxLifeTime = 0.5f;

	particles->SetEmitterRange(range);
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

	if (input->TriggerKey(DIK_Z)) {
		audio->PlayAudio();
	}

	plane->Update(*useCamera);
	teapot->Update(*useCamera);
	bunny->Update(*useCamera);
	suzanne->Update(*useCamera);
	multiMesh->Update(*useCamera);
	multiMaterial->Update(*useCamera);

	particles->Update(*useCamera);

	sprite->Update();

	sphere->Update(*useCamera);
}

void GameScene::Draw() {

	dxCommon->PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	plane->Draw();
	teapot->Draw();
	bunny->Draw();
	suzanne->Draw();
	multiMesh->Draw();
	multiMaterial->Draw();

	particles->Draw();

	sprite->Draw();

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

	DrawFPS_ImGui();

	ImGui::Begin("ImGuiChecBox");

	ImGui::Checkbox("Sprite", &drawSprite);
	ImGui::Checkbox("Sphere", &drawSphere);
	ImGui::Checkbox("Plane", &drawPlane);
	ImGui::Checkbox("Teapot", &drawTeapot);
	ImGui::Checkbox("Bunny", &drawBunny);
	ImGui::Checkbox("Suzanne", &drawSuzanne);
	ImGui::Checkbox("MultiMesh", &drawMultiMesh);
	ImGui::Checkbox("MultiMaterial", &drawMultiMaterial);
	ImGui::Checkbox("Particle", &drawParticle);

	ImGui::End();

	debugCamera->DrawImgui();

	if (drawSprite) {
		sprite->DrawImGui("Sprite");
	}

	if (drawSphere) {
		sphere->DrawImGui("sphere");
	}

	if (drawPlane) {
		plane->DrawImGui("plane");
	}

	if (drawTeapot) {
		teapot->DrawImGui("teapot");
	}

	if (drawBunny) {
		bunny->DrawImGui("bunny");
	}

	if (drawSuzanne) {
		suzanne->DrawImGui("suzanne");
	}

	if (drawMultiMesh){
		multiMesh->DrawImGui("MultiMesh");
	}

	if (drawMultiMaterial) {
		multiMaterial->DrawImGui("MultiMaterial");
	}

	if (drawParticle) {
		particles->DrawImGui("particle");
	}

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