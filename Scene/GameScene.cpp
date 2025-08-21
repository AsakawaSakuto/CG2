#include "GameScene.h"

void GameScene::SetAppContext(AppContext* ctx) {
	ctx_ = ctx;
}

void GameScene::Initialize() {

	debugCamera_->SetInput(&ctx_->input);

	gamePad_ = &ctx_->gamePad;

	fade_->Initialize(&ctx_->dxCommon);

	reticle2D_->Initialize(&ctx_->dxCommon, "resources/image/UI/crosshair.png",{64.0f, 64.0f});

	yuka_->Initialize(&ctx_->dxCommon, "resources/object3d/yuka.obj");

	player1_->Initialize(&ctx_->dxCommon, "resources/object3d/human.obj");
	player1_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	player1_->SetTranslate({ 0.0f,0.0f,-30.0f });

	player2_->Initialize(&ctx_->dxCommon, "resources/object3d/human.obj");
	player2_->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	player2_->SetTranslate({ 0.0f,0.0f,30.0f });

	InitParticle();
}

void GameScene::Update() {

	gamePad_->Update();
	ctx_->input.Update();
	CameraController();

	if (ctx_->input.TriggerKey(DIK_SPACE)) {
		fade_->SetIsFade(true);
	}

	if (fade_->GetFadeAlpha() >= 1.0f && fade_->GetIsFade()) {
		IScene::sceneNo = TITLE;
	}

	reticle2D_->SetPosition(ctx_->input.GetMousePos());
	reticle2D_->Update();

	// ビュー行列、プロジェクション行列、ビューポート行列（もしくは変換行列）を用意
	Matrix4x4 viewMatrix = useCamera_->GetViewMatrix();
	Matrix4x4 projectionMatrix = useCamera_->GetProjectionMatrix();
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, WinApp::kClientWidth_, WinApp::kClientHeight_, 0.0f, 1.0f);

	// 各行列の掛け算で合成行列（スクリーン→ビュー→ワールドの変換）を作る
	Matrix4x4 matVPV = MultiplyMatrix(viewMatrix, MultiplyMatrix(projectionMatrix, viewportMatrix));

	// 合成行列の逆行列を計算
	Matrix4x4 matInverseVPV = InverseMatrix(matVPV);

	// スクリーン座標（Z=0:ニア, Z=1:ファー）
	Vector3 posNear = Vector3(reticle2D_->GetPosition().x, reticle2D_->GetPosition().y, 0.0f); // ニアクリップ面上のスクリーン座標
	Vector3 posFar = Vector3(reticle2D_->GetPosition().x, reticle2D_->GetPosition().y, 1.0f);   // ファークリップ面上のスクリーン座標

	// スクリーン座標 → ワールド座標への変換（合成行列の逆行列を使用）
	posNear = TransformVtoM(posNear, matInverseVPV); // ワールド空間上のニア点
	posFar = TransformVtoM(posFar, matInverseVPV);   // ワールド空間上のファー点

	// マウスレイの方向（Far - Near）を求める
	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = mouseDirection.Normalize(); // 正規化

	// 任意の距離だけ進めた位置に3Dレティクルを配置
	Vector3 reticleWorldPosition = posNear + mouseDirection * kDistanceToReticle;

	Vector3 playerPos = player1_->GetWorldPosition();
	fireVelocity_ = reticleWorldPosition - playerPos;
	fireVelocity_ = fireVelocity_.Normalize();

	if (ctx_->input.TriggerMouseButtonL()) {
		if(!fireIsAlive_) {
			fireIsAlive_ = true;
			firePos_ = player1_->GetWorldPosition();
		}
	} 

	if (fireIsAlive_) {
		firePos_ += fireVelocity_ * fireSpeed_ * deltaTime_;
		fireLifeTimer_ += deltaTime_;
		if (fireLifeTimer_ >= fireLifeTime_) {
			fireLifeTimer_ = 0.0f;
			fireIsAlive_ = false;
		}
	}

	player1_->Update(*useCamera_);
	player2_->Update(*useCamera_);
	yuka_->Update(*useCamera_);

	fire_->SetEmitterPosition(firePos_);
	fire_->Update(*useCamera_);

	fade_->Update();
}

void GameScene::Draw() {

	ctx_->dxCommon.PreDraw(); // ここより上に描画処理を書かない

	///
	/// ↓描画処理ここから
	///

	yuka_->Draw();

	player1_->Draw();
	player2_->Draw();

	fire_->Draw();

	reticle2D_->Draw();
	fade_->Draw();

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

	// Imguiの内部コマンドを生成する
	ImGui::Render();

	///
	/// ↑ImGuiここまで
	///

	ctx_->dxCommon.PostDraw(); // ここより下に描画処理を書かない
}

void GameScene::InitParticle() {
	fire_->Initialize(&ctx_->dxCommon, "resources/image/particle/fire.png", 2);
	fire_->UseEmitter(true);

	fireEmitter_.count = 10;
	fireEmitter_.isMove = true;
	fireEmitter_.radius = 0.01f;
	fireEmitter_.spawnTime = 0.01f;

	fireRange_.minScale = { 0.1f,0.1f,0.0f };
	fireRange_.maxScale = { 1.0f,1.0f,0.0f };
	fireRange_.minVelocity = { -0.05f,-0.01f,-0.05f };
	fireRange_.maxVelocity = { 0.05f,0.2f,0.05f };
	fireRange_.minColor = { 0.2f,0.0f,0.0f };
	fireRange_.maxColor = { 1.0f,0.1f,0.0f };
	fireRange_.minLifeTime = 0.2f;
	fireRange_.maxLifeTime = 0.5f;

	fire_->SetEmitterValue(fireEmitter_);
	fire_->SetEmitterRange(fireRange_);
}

void GameScene::CameraController() {
	if (ctx_->input.TriggerKey(DIK_Z)) {
		if (isDebugCamera_) {
			isDebugCamera_ = false;
		} else {
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