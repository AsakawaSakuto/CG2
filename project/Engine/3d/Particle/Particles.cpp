#include "Particles.h"
#include "../Model/Model.h"
#include <cassert>
#include <filesystem>  // ディレクトリ作成用に追加
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

// 修正: PSOManagerをインクルード（相対パス修正）
#include "Core/PSOManager/PSOManager.h"

void Particles::Initialize(DirectXCommon* dxCommon, const std::string& filePath, const uint32_t maxParticle) {
	// DX共通クラスからデバイス・コマンドリストを取得
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	// JsonManagerの初期化
	jsonManager_ = std::make_unique<JsonManager>();
	jsonManager_->SetBasePath("resources/Data/Json/Particle/");

	line3d_ = std::make_unique<Line3d>();
	line3d_->Initialize(dxCommon_);

	// エミッターのデフォルト値を設定
	emitter_ = {}; // ゼロ初期化
	emitter_.translate = { 0.0f, 0.0f, 0.0f };
	emitter_.radius = 1.0f;
	emitter_.shapeType = static_cast<uint32_t>(EmitterShapeType::POINT);
	emitter_.blendMode = kBlendModeAdd;
	emitter_.texturePath = "circle"; // デフォルトテクスチャ
	emitter_.startScale = { 1.0f, 1.0f };
	emitter_.endScale = { 1.0f, 1.0f };
	emitter_.lifeTime = 3.0f;
	emitter_.frequency = 0.5f;
	emitter_.count = 10;
	emitter_.planeNormal = { 0.0f, 1.0f, 0.0f }; // デフォルト法線（上向き）
	emitter_.size = { 1.0f, 1.0f, 1.0f };
	emitter_.startColor = { 1.0f, 1.0f, 1.0f };
	emitter_.endColor = { 1.0f, 1.0f, 1.0f };

	// JSONファイルの読み込みを試行（失敗してもデフォルト値で続行）
	try {
		LoadJson(filePath);
	}
	catch (const std::exception& e) {
		printf("[WARNING] Failed to load particle JSON '%s': %s\n", filePath.c_str(), e.what());
		printf("[INFO] Using default particle settings.\n");
	}

	DescriptorAllocator& alloc = dxCommon_->GetParticleAlloc();

	// 既に初期化済みの場合は、リソース作成をスキップ
	if (isInitialized_) {
		// Emitterの値だけリセット
		// バッファリセットフラグを立てる（次回Update時にGPUバッファをクリア）
		needsBufferReset_ = true;
		return;
	}

	idxSrvParticles_ = alloc.Allocate();
	idxUavParticles_ = alloc.Allocate();
	idxUavFreeListIndex_ = alloc.Allocate();
	idxUavFreeList_ = alloc.Allocate();

	// ブレンドモードを加算合成に初期化
	blendMode_ = kBlendModeAdd;

	// テクスチャ名を保持しておく
	textureName_ = "resources/image/particle/circle.png";

	// テクスチャマネージャー初期化とテクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	// リソースの作成
	CreateVertexResource();   // 頂点バッファ
	CreateIndexResource();    // インデックスバッファ
	CreateMaterialResource(); // マテリアル

	kMaxParticles_ = 512 * maxParticle;
	uint32_t num = kMaxParticles_ / 512;
	kDispatchCount = num;

	CreateEmitterResource();
	CreateParticleResource();
	CreatePerViewResource();
	CreatePerFrameResource();

	isInitialized_ = true;
}

void Particles::Update() {

	// バッファリセットが必要な場合、コンピュートシェーダーで即座にリセット
	if (needsBufferReset_) {
		// コマンドリスト上で直接初期化シェーダーを実行（GPU同期なし）
		D3D12_RESOURCE_BARRIER toUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			particleBufferResource_.Get(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		commandList_->ResourceBarrier(1, &toUAV);

		commandList_->SetComputeRootSignature(csRootSignature_.Get());
		commandList_->SetPipelineState(csInitializePipelineState_.Get());
		commandList_->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress());
		commandList_->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());
		commandList_->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());
		commandList_->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());
		commandList_->Dispatch(kDispatchCount, 1, 1);

		D3D12_RESOURCE_BARRIER toSRV = CD3DX12_RESOURCE_BARRIER::Transition(
			particleBufferResource_.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		);
		commandList_->ResourceBarrier(1, &toSRV);

		needsBufferReset_ = false;
	}

	// ---- カメラ関連の行列計算（PreView構造体に書き込む） ----
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_.GetScale(), camera_.GetRotate(), camera_.GetTranslate());
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = cameraMatrix;

	//Matrix4x4 billboardMatrix = MultiplyMatrix(backToFrontMatrix, cameraMatrix);

	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(camera_.GetFovY(), static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), camera_.GetNearClip(), camera_.GetFarClip());

	// PreViewリソースに書き込み
	PerView tempPreView;
	tempPreView.viewProjection = MultiplyMatrix(viewMatrix, projectionMatrix);
	tempPreView.billboardMatrix = billboardMatrix;

	void* mappedPtr = nullptr;
	perViewResource_->Map(0, nullptr, &mappedPtr);
	memcpy(mappedPtr, &tempPreView, sizeof(PerView));
	perViewResource_->Unmap(0, nullptr);

	UpdatePerFrame();

	UpdateEmitter();

	UpdateParticle();
}

void Particles::Draw(Camera& useCamera) {
	camera_ = useCamera;

	// PSOManagerからRootSignatureを取得
	auto& psoManager = PSOManager::GetInstance();
	auto rootSignature = psoManager.GetRootSignature("Particle");

	// ルートシグネチャの設定（パイプラインステートだけでは設定されないため明示的に指定が必要）
	commandList_->SetGraphicsRootSignature(rootSignature.Get());

	// 現在のブレンドモードに応じて、PSOManagerから適切なPSOを取得
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	switch (blendMode_) {
	case kBlendModeNone:
		pso = psoManager.GetPSO(PSOType::Particle_None);
		break;
	case kBlendModeNormal:
		pso = psoManager.GetPSO(PSOType::Particle_Normal);
		break;
	case kBlendModeAdd:
		pso = psoManager.GetPSO(PSOType::Particle_Add);
		break;
	case kBlendModeSubtract:
		pso = psoManager.GetPSO(PSOType::Particle_Subtract);
		break;
	case kBlendModeMultily:
		pso = psoManager.GetPSO(PSOType::Particle_Multiply);
		break;
	case kBlendModeScreen:
		pso = psoManager.GetPSO(PSOType::Particle_Screen);
		break;
	default:
		pso = psoManager.GetPSO(PSOType::Particle_Add);
		break;
	}

	commandList_->SetPipelineState(pso.Get());

	// 三角形リストで描画することを指定（パーティクルは四角形だが、インデックスで三角形×2として構成）
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファ（VBV）とインデックスバッファ（IBV）の設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);

	// Material
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// カメラ・ビルボード行列（PreViewなど）
	commandList_->SetGraphicsRootConstantBufferView(1, perViewResource_->GetGPUVirtualAddress()); // ←ここ重要

	// パーティクルインスタンシングSRV（StructuredBufferなど）
	commandList_->SetGraphicsRootDescriptorTable(2, dxCommon_->GetSrvGPUHandle(idxSrvParticles_)); // ←今まで1番だったやつ

	// テクスチャSRV
	commandList_->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	// DrawIndexedInstanced(インデックス数, インスタンス数, 開始インデックス, ベース頂点, 開始インスタンス)
	commandList_->DrawIndexedInstanced(6, kMaxParticles_, 0, 0, 0);

	DrawEmitterShape(line3d_.get());
	
	if (emitter_.enableVisualization) {
		line3d_->Draw(camera_);
	}
}

void Particles::DrawImGui(const char* objectName) {

#ifdef USE_IMGUI

	ImGui::Begin(objectName);

	// Play/Stop control buttons
	if (ImGui::Button("再生 (ループ)")) {
		Play(true);
	}

	ImGui::SameLine();

	if (ImGui::Button("再生 (1回)")) {
		Play(false);
	}

	ImGui::SameLine();

	if (ImGui::Button("停止")) {
		Stop();
	}

	// 現在の再生状態を表示
	ImGui::Text("状態: %s", isPlaying_ ? "再生中" : "停止中");

	ImGui::Separator();

	// BlendMode選択コンボボックスを追加
	const char* blendModeNames[] = {
		"None (なし)",
		"Normal (通常)",
		"Add (加算)",
		"Subtract (減算)",
		"Multily (乗算)",
		"Screen (スクリーン)"
	};
	int currentBlendMode = static_cast<int>(emitter_.blendMode);
	if (ImGui::Combo("ブレンドモード", &currentBlendMode, blendModeNames, IM_ARRAYSIZE(blendModeNames))) {
		emitter_.blendMode = static_cast<BlendMode>(currentBlendMode);
		blendMode_ = emitter_.blendMode; // 内部のblendMode_も更新
	}

	ImGui::Separator();

	// ファイル名入力
	static char fileNameBuffer[256] = "temp";
	strncpy_s(fileNameBuffer, loadToSaveName_.c_str(), sizeof(fileNameBuffer));
	if (ImGui::InputText("ファイル名", fileNameBuffer, sizeof(fileNameBuffer))) {
		loadToSaveName_ = fileNameBuffer;
	}

	if (ImGui::Button("読み込み")) {
		LoadFromJson(loadToSaveName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("保存")) {
		SaveToJson(loadToSaveName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("新規作成")) {
		CreateNewJsonFile(loadToSaveName_);
	}

	ImGui::Separator();

	// テクスチャ名入力
	static char textureNameBuffer[256] = "";
	strncpy_s(textureNameBuffer, emitter_.texturePath.c_str(), sizeof(textureNameBuffer));
	if (ImGui::InputText("テクスチャ名", textureNameBuffer, sizeof(textureNameBuffer))) {
		emitter_.texturePath = textureNameBuffer;
	}

	if (ImGui::Button("テクスチャ読み込み")) {
		// すでに同じテクスチャなら処理をスキップ
		std::string newTextureName = "resources/image/particle/" + emitter_.texturePath + ".png";
		if (textureName_ == newTextureName) {
			return;
		}
		textureName_ = newTextureName;
		// .objの参照しているテクスチャファイル読み込み
		TextureManager::GetInstance()->LoadTexture(textureName_);
		// 読み込んだテクスチャの番号を取得
		textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
	}

	ImGui::Separator();

	ImGui::DragFloat3("位置", &emitter_.translate.x, 0.01f);

	ImGui::DragFloat3("オフセット", &offset_.x, 0.01f);

	// Emitter Shape Selection
	const char* shapeNames[] = {
		"点 (POINT)",
		"線 (LINE)",
		"球 (SPHERE)",
		"箱 (BOX)",
		"平面 (PLANE)",
		"リング (RING)"
	};
	int currentShape = static_cast<int>(emitter_.shapeType);
	if (ImGui::Combo("エミッター形状", &currentShape, shapeNames, IM_ARRAYSIZE(shapeNames))) {
		emitter_.shapeType = static_cast<uint32_t>(currentShape);
	}

	// Spawn on edge flag
	ImGui::Checkbox("エッジ上に生成", reinterpret_cast<bool*>(&emitter_.spawnOnEdge));

	// Visualization flag
	ImGui::Checkbox("形状を可視化", reinterpret_cast<bool*>(&emitter_.enableVisualization));

	// Shape-specific parameters
	switch (static_cast<EmitterShapeType>(emitter_.shapeType))
	{
	case EmitterShapeType::POINT:
	{
		// Point emitter has no additional parameters
		ImGui::Text("点エミッター - 正確な位置で生成");
		break;
	}

	case EmitterShapeType::LINE:
	{
		ImGui::DragFloat3("線の開始位置", &emitter_.lineStart.x, 0.01f);
		ImGui::DragFloat3("線の方向", &emitter_.size.x, 0.01f);
		ImGui::DragFloat("線の長さ", &emitter_.lineLength, 0.01f, 0.0f, 100.0f);
		if (emitter_.spawnOnEdge) {
			ImGui::Text("エッジモード - 線の両端のみで生成");
		}
		else {
			ImGui::Text("線上の任意の位置で生成");
		}
		break;
	}

	case EmitterShapeType::SPHERE:
	{
		ImGui::DragFloat("半径", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
		if (emitter_.spawnOnEdge) {
			ImGui::Text("エッジモード - 球の表面のみで生成");
		}
		else {
			ImGui::Text("球の内部で生成");
		}
		break;
	}

	case EmitterShapeType::BOX:
	{
		ImGui::DragFloat3("箱のサイズ", &emitter_.size.x, 0.01f, 0.0f, 100.0f);
		if (emitter_.spawnOnEdge) {
			ImGui::Text("エッジモード - 箱の辺上のみで生成");
		}
		else {
			ImGui::Text("箱の内部で生成");
		}
		break;
	}

	case EmitterShapeType::PLANE:
	{
		ImGui::DragFloat3("平面のサイズ", &emitter_.size.x, 0.01f, 0.01f, 100.0f);
		ImGui::DragFloat3("平面法線", &emitter_.planeNormal.x, 0.01f);
		// Normalize normal vector
		float normalLength = sqrt(emitter_.planeNormal.x * emitter_.planeNormal.x +
			emitter_.planeNormal.y * emitter_.planeNormal.y +
			emitter_.planeNormal.z * emitter_.planeNormal.z);
		if (normalLength > 0.001f) {
			emitter_.planeNormal.x /= normalLength;
			emitter_.planeNormal.y /= normalLength;
			emitter_.planeNormal.z /= normalLength;
		}
		if (emitter_.spawnOnEdge) {
			ImGui::Text("エッジモード - 平面の境界線上のみで生成");
		}
		else {
			ImGui::Text("平面内の任意の位置で生成");
		}
		break;
	}

	case EmitterShapeType::RING:
	{
		ImGui::DragFloat("内側半径", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("外側半径", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat3("リング法線", &emitter_.planeNormal.x, 0.01f);
		// Ensure inner radius is not larger than outer radius
		if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
			emitter_.ringInnerRadius = emitter_.ringOuterRadius;
		}
		// Normalize normal vector
		float normalLength = sqrt(emitter_.planeNormal.x * emitter_.planeNormal.x +
			emitter_.planeNormal.y * emitter_.planeNormal.y +
			emitter_.planeNormal.z * emitter_.planeNormal.z);
		if (normalLength > 0.001f) {
			emitter_.planeNormal.x /= normalLength;
			emitter_.planeNormal.y /= normalLength;
			emitter_.planeNormal.z /= normalLength;
		}
		if (emitter_.spawnOnEdge) {
			ImGui::Text("エッジモード - リングの円周上のみで生成");
		}
		else {
			ImGui::Text("リング範囲内の任意の位置で生成");
		}
		break;
	}
	}

	ImGui::Separator();

	ImGui::Checkbox("エミッター使用", reinterpret_cast<bool*>(&emitter_.useEmitter));
	ImGui::Checkbox("放出", reinterpret_cast<bool*>(&emitter_.emit));
	ImGui::DragInt("放出数", reinterpret_cast<int*>(&emitter_.count), 1, 0, kMaxParticles_);

	ImGui::Separator();

	ImGui::DragFloat("放出頻度", &emitter_.frequency, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("頻度タイマー", &emitter_.frequencyTime, 0.01f, 0.0f, 10.0f);

	ImGui::Separator();

	ImGui::Checkbox("移動有効", reinterpret_cast<bool*>(&emitter_.isMove));
	ImGui::DragFloat3("通常速度", &emitter_.normalVelocity.x, 0.01f);
	ImGui::Checkbox("速度ランダム", reinterpret_cast<bool*>(&emitter_.velocityRandom));
	if (emitter_.velocityRandom) {
		ImGui::DragFloat3("最小速度", &emitter_.minVelocity.x, 0.01f);
		ImGui::DragFloat3("最大速度", &emitter_.maxVelocity.x, 0.01f);
	}

	ImGui::Separator();

	ImGui::Checkbox("スケールフェード", reinterpret_cast<bool*>(&emitter_.scaleFade));
	ImGui::DragFloat2("開始スケール", &emitter_.startScale.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat2("終了スケール", &emitter_.endScale.x, 0.01f, 0.0f, 10.0f);

	ImGui::Checkbox("スケールランダム", reinterpret_cast<bool*>(&emitter_.scaleRandom));
	if (emitter_.scaleRandom) {
		ImGui::DragFloat3("最小スケール", &emitter_.minScale.x, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat3("最大スケール", &emitter_.maxScale.x, 0.01f, 0.0f, 10.0f);
	}

	ImGui::Separator();

	ImGui::Checkbox("回転移動", reinterpret_cast<bool*>(&emitter_.rotateMove));
	ImGui::DragFloat("右回転速度", &emitter_.startRotateVelocity, 0.01f);
	ImGui::DragFloat("左回転速度", &emitter_.endRotateVelocity, 0.01f);

	ImGui::Checkbox("回転速度ランダム", reinterpret_cast<bool*>(&emitter_.rotateVelocityRandom));
	if (emitter_.rotateVelocityRandom) {
		ImGui::DragFloat("最小回転速度", &emitter_.minRotateVelocity, 0.01f);
		ImGui::DragFloat("最大回転速度", &emitter_.maxRotateVelocity, 0.01f);
	}

	ImGui::Separator();

	ImGui::Checkbox("アルファフェード有効", reinterpret_cast<bool*>(&emitter_.alphaFade));
	ImGui::Checkbox("カラーフェード有効", reinterpret_cast<bool*>(&emitter_.colorFade));

	ImGui::ColorEdit3("開始色", &emitter_.startColor.x);
	ImGui::ColorEdit3("終了色", &emitter_.endColor.x);

	ImGui::Checkbox("カラーランダム", reinterpret_cast<bool*>(&emitter_.colorRandom));
	if (emitter_.colorRandom) {
		ImGui::ColorEdit3("最小色", &emitter_.minColor.x);
		ImGui::ColorEdit3("最大色", &emitter_.maxColor.x);
	}

	ImGui::Separator();

	ImGui::DragFloat("寿命", &emitter_.lifeTime, 0.01f, 0.0f, 100.0f);
	ImGui::Checkbox("寿命ランダム", reinterpret_cast<bool*>(&emitter_.lifeTimeRandom));
	if (emitter_.lifeTimeRandom) {
		ImGui::DragFloat("最小寿命", &emitter_.minLifeTime, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("最大寿命", &emitter_.maxLifeTime, 0.01f, 0.0f, 100.0f);
	}

	ImGui::Separator();

	ImGui::Checkbox("重力使用", reinterpret_cast<bool*>(&emitter_.useGravity));
	ImGui::DragFloat("重力", &emitter_.gravityY, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("加速度", &emitter_.accelerationY, 0.01f, 0.0f, 100.0f);

	ImGui::Separator();

	// デバッグ情報表示
	ImGui::Text("デバッグ情報");
	ImGui::Text("最大パーティクル数: %u", kMaxParticles_);
	ImGui::Text("ディスパッチ数: %u", kDispatchCount);

	// パーティクル強制リセット
	if (ImGui::Button("全パーティクル強制リセット")) {
		// 初期化シェーダーを再実行してパーティクルをリセット
		ResetAllParticles();
	}

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::End();

#endif
}

void Particles::SetTexture(const std::string& textureName) {
	// すでに同じテクスチャなら処理をスキップ
	if (textureName_ == textureName) {
		return;
	}
	textureName_ = textureName;
	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	// 読み込んだテクスチャの番号を取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
}

void Particles::CreateParticleResource() {

	// DefaultHeap上にパーティクル配列を作成（UAV/SRV兼用）
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
		sizeof(ParticleDataCS) * kMaxParticles_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS // UAV必須
	);

	device_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON, // 初期はCOMMONでOK
		nullptr,
		IID_PPV_ARGS(&particleBufferResource_)
	);

	// UAV: Unordered Access View
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = kMaxParticles_;
	uavDesc.Buffer.StructureByteStride = sizeof(ParticleDataCS);

	device_->CreateUnorderedAccessView(
		particleBufferResource_.Get(),
		nullptr, // CounterResource: nullでOK
		&uavDesc,
		dxCommon_->GetSrvCPUHandle(idxUavParticles_) // UAVヒープ上の任意のスロット
	);

	// SRV: Shader Resource View
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = kMaxParticles_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleDataCS);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	device_->CreateShaderResourceView(
		particleBufferResource_.Get(),
		&srvDesc,
		dxCommon_->GetSrvCPUHandle(idxSrvParticles_) // SRVヒープ上の任意のスロット
	);

	//----------------------------------------------------------------//

	// freeListIndex u1
	D3D12_RESOURCE_DESC counterDesc2 = CD3DX12_RESOURCE_DESC::Buffer(sizeof(int32_t), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	device_->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &counterDesc2,
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListIndexResource_));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc2 = {};
	uavDesc2.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc2.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc2.Buffer.FirstElement = 0;
	uavDesc2.Buffer.NumElements = 1;
	uavDesc2.Buffer.StructureByteStride = sizeof(int32_t);

	device_->CreateUnorderedAccessView(
		freeListIndexResource_.Get(), nullptr, &uavDesc2,
		dxCommon_->GetSrvCPUHandle(idxUavFreeListIndex_));

	// freeList u2
	D3D12_RESOURCE_DESC counterDesc3 = CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint32_t) * kMaxParticles_, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	device_->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &counterDesc3,
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListResource_));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc3 = {};
	uavDesc3.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc3.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc3.Buffer.FirstElement = 0;
	uavDesc3.Buffer.NumElements = kMaxParticles_;
	uavDesc3.Buffer.StructureByteStride = sizeof(uint32_t);

	device_->CreateUnorderedAccessView(
		freeListResource_.Get(), nullptr, &uavDesc3,
		dxCommon_->GetSrvCPUHandle(idxUavFreeList_));

	//----------------------------------------------------------------//

	// 1. RootSignature作成
	D3D12_ROOT_PARAMETER csRootParams[5] = {};
	csRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV; // u0: Particleバッファ
	csRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[0].Descriptor.ShaderRegister = 0;

	csRootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV; // u1: ListIndex
	csRootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[1].Descriptor.ShaderRegister = 1;

	csRootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV; // u2: List
	csRootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[2].Descriptor.ShaderRegister = 2;

	csRootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // b5: Emitter
	csRootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[3].Descriptor.ShaderRegister = 5;

	csRootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // b6: PerFrame
	csRootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[4].Descriptor.ShaderRegister = 6;

	D3D12_ROOT_SIGNATURE_DESC csRootSigDesc = {};
	csRootSigDesc.pParameters = csRootParams;
	csRootSigDesc.NumParameters = _countof(csRootParams);
	csRootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<ID3DBlob> csSignature, csError;
	D3D12SerializeRootSignature(&csRootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &csSignature, &csError);

	device_->CreateRootSignature(0, csSignature->GetBufferPointer(), csSignature->GetBufferSize(), IID_PPV_ARGS(&csRootSignature_));

	// --- 初期化用CS（ParticlesInitialize.CS.hlsl） ---
	ComPtr<IDxcBlob> csInitBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/InitializeParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csInitDesc = {};
	csInitDesc.pRootSignature = csRootSignature_.Get();
	csInitDesc.CS = { csInitBlob->GetBufferPointer(), csInitBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csInitDesc, IID_PPV_ARGS(&csInitializePipelineState_));

	// --- エミッタ用CS（EmitterParticle.CS.hlsl） ---
	ComPtr<IDxcBlob> csEmitterBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/EmitterParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csEmitterDesc = {};
	csEmitterDesc.pRootSignature = csRootSignature_.Get();
	csEmitterDesc.CS = { csEmitterBlob->GetBufferPointer(), csEmitterBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csEmitterDesc, IID_PPV_ARGS(&csEmitterPipelineState_));

	// --- 動用途CS（EmitterParticle.CS.hlsl） ---
	ComPtr<IDxcBlob> csUpdateBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/UpdateParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csUpdateDesc = {};
	csUpdateDesc.pRootSignature = csRootSignature_.Get();
	csUpdateDesc.CS = { csUpdateBlob->GetBufferPointer(), csUpdateBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csUpdateDesc, IID_PPV_ARGS(&csUpdatePipelineState_));

	// 初期化実行
	ExecuteInitialization();
}

void Particles::ExecuteInitialization() {
	// 1. コマンドアロケータ/リスト作成（DIRECTでOK）
	ComPtr<ID3D12CommandAllocator> alloc;
	device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc));
	ComPtr<ID3D12GraphicsCommandList> list;
	device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc.Get(), csInitializePipelineState_.Get(), IID_PPV_ARGS(&list));

	// 2. 必要ならResourceBarrier（初期状態COMMON→UNORDERED_ACCESS）
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	list->ResourceBarrier(1, &barrier);

	// 3. RootSignature/PSO/UAVセット
	list->SetComputeRootSignature(csRootSignature_.Get());
	list->SetPipelineState(csInitializePipelineState_.Get());
	list->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress());
	list->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());
	list->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());
	list->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());

	// 4. Dispatch
	list->Dispatch(kDispatchCount, 1, 1);

	// 5. BarrierでSRVに戻す（必要なら）
	auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	list->ResourceBarrier(1, &barrier2);

	// 6. コマンド実行＆同期
	list->Close();
	ID3D12CommandList* lists[] = { list.Get() };
	dxCommon_->GetCommandQueue()->ExecuteCommandLists(_countof(lists), lists);
	// 必要に応じてWaitForGPU()でGPU待ち
	dxCommon_->WaitForGPU();
}

void Particles::ResetAllParticles() {
	OutputDebugStringA("Force resetting all particles...\n");
	ExecuteInitialization();
	OutputDebugStringA("Particle reset completed.\n");
}

void Particles::UpdateParticle() {
	// 1. 必要な場合だけ「UAV」に遷移
	D3D12_RESOURCE_BARRIER toUAV = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, // ←前回の状態に応じて
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	commandList_->ResourceBarrier(1, &toUAV);

	// Emitterの更新、Particleの生成
	commandList_->SetComputeRootSignature(csRootSignature_.Get());
	commandList_->SetPipelineState(csEmitterPipelineState_.Get()); // CSをセット
	commandList_->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress()); // u0
	commandList_->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());  // u1
	commandList_->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());       // u2
	commandList_->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());         // b5
	commandList_->SetComputeRootConstantBufferView(4, perFrameResource_->GetGPUVirtualAddress());        // b6
	// Emitterの処理を実行
	commandList_->Dispatch(1, 1, 1);

	D3D12_RESOURCE_BARRIER bs[] = {
	CD3DX12_RESOURCE_BARRIER::UAV(particleBufferResource_.Get()),
	CD3DX12_RESOURCE_BARRIER::UAV(freeListIndexResource_.Get()),
	CD3DX12_RESOURCE_BARRIER::UAV(freeListResource_.Get()),
	};
	commandList_->ResourceBarrier(_countof(bs), bs);

	// Particleの更新
	commandList_->SetComputeRootSignature(csRootSignature_.Get());
	commandList_->SetPipelineState(csUpdatePipelineState_.Get()); // CSをセット
	commandList_->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress()); // u0
	commandList_->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());  // u1
	commandList_->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());       // u2
	commandList_->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());         // b5
	commandList_->SetComputeRootConstantBufferView(4, perFrameResource_->GetGPUVirtualAddress());        // b6
	// Particleの更新を「kDispatchCount」の数分実行
	commandList_->Dispatch(kDispatchCount, 1, 1);

	// 4. SRV状態へバリア
	D3D12_RESOURCE_BARRIER toSRV = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	commandList_->ResourceBarrier(1, &toSRV);
}

void Particles::CreateEmitterResource() {

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(EmitterStateGPU)); // ✅ サイズ変更

	device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&emitterResource_)
	);
}

void Particles::UpdateEmitter() {
	// このemitterSphereをCBufferとしてGPUへ転送
	if (emitter_.useEmitter) {
		emitter_.frequencyTime += GetDeltaTime();  // kDeltaTime_からGetDeltaTime()に変更
	}

	if (emitter_.frequency <= emitter_.frequencyTime) {
		emitter_.frequencyTime = 0.0f;
		emitter_.emit = true;
	}
	else {
		emitter_.emit = false;
	}

	emitter_.kMaxParticle = kMaxParticles_;
	emitter_.translate = emitter_.translate + offset_;

	// BlendModeをuint32_tに変換してGPU構造体に格納
	emitter_.blendModeValue = static_cast<uint32_t>(emitter_.blendMode);

	// EmitterStateをマップしてGPU互換部分のみをコピー
	EmitterStateGPU* mappedEmitter = nullptr;
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedEmitter));

	// PODデータ（EmitterStateGPU）のみをコピー
	memcpy(mappedEmitter, static_cast<EmitterStateGPU*>(&emitter_), sizeof(EmitterStateGPU));

	// Unmapを追加（リソースリークを防ぐ）
	emitterResource_->Unmap(0, nullptr);
}

void Particles::CreatePerFrameResource() {
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(PerFrame));
	device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&perFrameResource_)
	);
}

void Particles::UpdatePerFrame() {
	frameIndex++;
	totalTime_ += GetDeltaTime();  // kDeltaTime_からGetDeltaTime()に変更
	// フレームごとに
	PerFrame* mapped = nullptr;
	perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	mapped->time = totalTime_;      // 例えば経過時間など
	mapped->deltaTime = GetDeltaTime();  // kDeltaTime_からGetDeltaTime()に変更
	mapped->index = frameIndex;
	mapped->pad1 = 0.0f;            // パディングを初期化
}

void Particles::CreatePerViewResource() {
	perViewResource_ = CreateBufferResource(device_.Get(), sizeof(PerView));

	// 書き込むためのアドレスを取得
	perViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));

	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	perViewData_->viewProjection = {};
	perViewData_->billboardMatrix = {};

	perViewResource_->Unmap(0, nullptr);
}

void Particles::CreateVertexResource() {
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleVertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(ParticleVertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(ParticleVertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData_[1].position = { -0.5f,0.5f,0.0f,1.0f };
	vertexData_[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData_[3].position = { 0.5f,0.5f,0.0f,1.0f };

	vertexData_[0].texcoord = { 0.f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
}

void Particles::CreateIndexResource() {
	// IndexResource
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// 表面（三角形2枚）
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;
}

void Particles::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->uvTransform = MakeIdentityMatrix();

	materialResource_->Unmap(0, nullptr);
}

Particles::~Particles() {
	// 確保したSRV/UAVインデックスを返却
	if (dxCommon_) {
		DescriptorAllocator& alloc = dxCommon_->GetParticleAlloc();
		alloc.Free(idxSrvParticles_);
		alloc.Free(idxUavParticles_);
		alloc.Free(idxUavFreeListIndex_);
		alloc.Free(idxUavFreeList_);
	}
}

void Particles::LoadJson(const std::string& filePath) {
	LoadFromJson(filePath);
}

void Particles::SaveToJson(const std::string& filePath) {
	// ディレクトリが存在しない場合は作成
	std::filesystem::path fullPath = jsonManager_->GetBasePath() + filePath + ".json";
	std::filesystem::path directory = fullPath.parent_path();

	if (!directory.empty() && !std::filesystem::exists(directory)) {
		try {
			std::filesystem::create_directories(directory);
			printf("[INFO] Created directory: %s\n", directory.string().c_str());
		}
		catch (const std::filesystem::filesystem_error& e) {
			printf("[ERROR] Failed to create directory: %s\n", e.what());
			return;
		}
	}

	// EmitterStateの各フィールドをJsonManagerに登録
	jsonManager_->RegistOutput(emitter_.translate, "translate");
	jsonManager_->RegistOutput(emitter_.radius, "radius");
	jsonManager_->RegistOutput(emitter_.useEmitter, "useEmitter");
	jsonManager_->RegistOutput(emitter_.emit, "emit");
	jsonManager_->RegistOutput(emitter_.count, "count");
	jsonManager_->RegistOutput(emitter_.frequency, "frequency");
	jsonManager_->RegistOutput(emitter_.frequencyTime, "frequencyTime");
	jsonManager_->RegistOutput(Vector2{ emitter_.startScale.x, emitter_.startScale.y }, "startScale");
	jsonManager_->RegistOutput(Vector2{ emitter_.endScale.x, emitter_.endScale.y }, "endScale");
	jsonManager_->RegistOutput(emitter_.scaleFade, "scaleFade");
	jsonManager_->RegistOutput(emitter_.scaleRandom, "scaleRandom");
	jsonManager_->RegistOutput(emitter_.minScale, "minScale");
	jsonManager_->RegistOutput(emitter_.maxScale, "maxScale");
	jsonManager_->RegistOutput(emitter_.rotateMove, "rotateMove");
	jsonManager_->RegistOutput(emitter_.startRotateVelocity, "startRotateVelocity");
	jsonManager_->RegistOutput(emitter_.endRotateVelocity, "endRotateVelocity");
	jsonManager_->RegistOutput(emitter_.rotateVelocityRandom, "rotateVelocityRandom");
	jsonManager_->RegistOutput(emitter_.minRotateVelocity, "minRotateVelocity");
	jsonManager_->RegistOutput(emitter_.maxRotateVelocity, "maxRotateVelocity");
	jsonManager_->RegistOutput(emitter_.alphaFade, "alphaFade");
	jsonManager_->RegistOutput(emitter_.colorFade, "colorFade");
	jsonManager_->RegistOutput(emitter_.startColor, "startColor");
	jsonManager_->RegistOutput(emitter_.endColor, "endColor");
	jsonManager_->RegistOutput(emitter_.colorRandom, "colorRandom");
	jsonManager_->RegistOutput(emitter_.minColor, "minColor");
	jsonManager_->RegistOutput(emitter_.maxColor, "maxColor");
	jsonManager_->RegistOutput(emitter_.isMove, "isMove");
	jsonManager_->RegistOutput(emitter_.startVelocity, "startVelocity");
	jsonManager_->RegistOutput(emitter_.endVelocity, "endVelocity");
	jsonManager_->RegistOutput(emitter_.velocityRandom, "velocityRandom");
	jsonManager_->RegistOutput(emitter_.minVelocity, "minVelocity");
	jsonManager_->RegistOutput(emitter_.maxVelocity, "maxVelocity");
	jsonManager_->RegistOutput(emitter_.normalVelocity, "normalVelocity");
	jsonManager_->RegistOutput(emitter_.lifeTime, "lifeTime");
	jsonManager_->RegistOutput(emitter_.lifeTimeRandom, "lifeTimeRandom");
	jsonManager_->RegistOutput(emitter_.minLifeTime, "minLifeTime");
	jsonManager_->RegistOutput(emitter_.maxLifeTime, "maxLifeTime");
	jsonManager_->RegistOutput(emitter_.shapeType, "shapeType");
	jsonManager_->RegistOutput(emitter_.size, "size");
	jsonManager_->RegistOutput(emitter_.lineStart, "lineStart");
	jsonManager_->RegistOutput(emitter_.lineLength, "lineLength");
	jsonManager_->RegistOutput(emitter_.ringInnerRadius, "ringInnerRadius");
	jsonManager_->RegistOutput(emitter_.ringOuterRadius, "ringOuterRadius");
	jsonManager_->RegistOutput(emitter_.planeNormal, "planeNormal");
	jsonManager_->RegistOutput(emitter_.spawnOnEdge, "spawnOnEdge");
	jsonManager_->RegistOutput(emitter_.enableVisualization, "enableVisualization");
	jsonManager_->RegistOutput(emitter_.useGravity, "useGravity");
	jsonManager_->RegistOutput(emitter_.gravityY, "gravityY");
	jsonManager_->RegistOutput(emitter_.accelerationY, "accelerationY");
	jsonManager_->RegistOutput(static_cast<uint32_t>(emitter_.blendMode), "blendMode");
	jsonManager_->RegistOutput(emitter_.texturePath, "texturePath");

	// JSONファイルに書き込み
	jsonManager_->Write(filePath);
}

void Particles::CreateNewJsonFile(const std::string& filePath) {
	// ディレクトリが存在しない場合は作成
	std::filesystem::path fullPath = jsonManager_->GetBasePath() + filePath + ".json";
	std::filesystem::path directory = fullPath.parent_path();

	if (!directory.empty() && !std::filesystem::exists(directory)) {
		try {
			std::filesystem::create_directories(directory);
			printf("[INFO] Created directory: %s\n", directory.string().c_str());
		}
		catch (const std::filesystem::filesystem_error& e) {
			printf("[ERROR] Failed to create directory: %s\n", e.what());
			return;
		}
	}

	// デフォルト値でJSONファイルを作成
	SaveToJson(filePath);
}

void Particles::LoadFromJson(const std::string& filePath) {
	try {
		// JSONファイルから読み込み
		auto values = jsonManager_->Read(filePath);

		if (values.empty()) {
			printf("[WARNING] Failed to load JSON file or file is empty: %s\n", filePath.c_str());
			printf("[INFO] Using default emitter settings.\n");
			return; // 早期リターン
		}

		// 各フィールドを順番に取得して適用（try-catchで保護）
		size_t index = 0;

		try {
			if (index < values.size()) emitter_.translate = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.radius = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.useEmitter = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.emit = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.count = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.frequency = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.frequencyTime = JsonManager::Reverse<float>(values[index++]);

			if (index < values.size()) {
				Vector2 startScale = JsonManager::Reverse<Vector2>(values[index++]);
				emitter_.startScale.x = startScale.x;
				emitter_.startScale.y = startScale.y;
			}
			if (index < values.size()) {
				Vector2 endScale = JsonManager::Reverse<Vector2>(values[index++]);
				emitter_.endScale.x = endScale.x;
				emitter_.endScale.y = endScale.y;
			}

			if (index < values.size()) emitter_.scaleFade = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.scaleRandom = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.minScale = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.maxScale = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.rotateMove = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.startRotateVelocity = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.endRotateVelocity = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.rotateVelocityRandom = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.minRotateVelocity = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.maxRotateVelocity = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.alphaFade = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.colorFade = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.startColor = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.endColor = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.colorRandom = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.minColor = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.maxColor = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.isMove = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.startVelocity = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.endVelocity = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.velocityRandom = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.minVelocity = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.maxVelocity = JsonManager::Reverse<Vector3>(values[index++]); // ✅ Fixed
			if (index < values.size()) emitter_.normalVelocity = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.lifeTime = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.lifeTimeRandom = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.minLifeTime = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.maxLifeTime = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.shapeType = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.size = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.lineStart = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.lineLength = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.ringInnerRadius = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.ringOuterRadius = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.planeNormal = JsonManager::Reverse<Vector3>(values[index++]);
			if (index < values.size()) emitter_.spawnOnEdge = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.enableVisualization = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.useGravity = JsonManager::Reverse<uint32_t>(values[index++]);
			if (index < values.size()) emitter_.gravityY = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.accelerationY = JsonManager::Reverse<float>(values[index++]);
			if (index < values.size()) emitter_.blendMode = static_cast<BlendMode>(JsonManager::Reverse<uint32_t>(values[index++]));

			// texturePath は最後に取得（例外が発生しやすい）
			if (index < values.size()) {
				std::string tempPath = JsonManager::Reverse<std::string>(values[index++]);
				if (!tempPath.empty() && tempPath.length() < 256) { // サイズチェック
					emitter_.texturePath = tempPath;
				}
				else {
					printf("[WARNING] Invalid texture path in JSON, using default.\n");
					emitter_.texturePath = "circle";
				}
			}
		}
		catch (const std::length_error& e) {
			printf("[ERROR] Length error while parsing JSON: %s\n", e.what());
			printf("[INFO] Partially loaded settings. Some fields may use default values.\n");
			// 部分的にロードされた状態で続行
		}
		catch (const std::exception& e) {
			printf("[ERROR] Error while parsing JSON field at index %zu: %s\n", index, e.what());
			printf("[INFO] Using default values for remaining fields.\n");
		}

		// BlendModeを内部変数にも反映
		blendMode_ = emitter_.blendMode;

		// テクスチャパスが存在する場合、テクスチャを読み込む
		if (!emitter_.texturePath.empty()) {
			try {
				std::string newTextureName = "resources/image/particle/" + emitter_.texturePath + ".png";
				if (textureName_ != newTextureName) {
					textureName_ = newTextureName;
					TextureManager::GetInstance()->LoadTexture(textureName_);
					textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
				}
			}
			catch (const std::exception& e) {
				printf("[ERROR] Failed to load texture '%s': %s\n", emitter_.texturePath.c_str(), e.what());
				printf("[INFO] Using default texture.\n");
			}
		}

		loadToSaveName_ = filePath;
		isJsonLoaded_ = true;

	}
	catch (const std::exception& e) {
		printf("[ERROR] Critical error in LoadFromJson: %s\n", e.what());
		printf("[INFO] Using default emitter settings.\n");
		// デフォルト値はInitialize関数で既に設定済み
	}
}

void Particles::DrawEmitterShape(Line3d* line3d, const Vector4& color) {
	if (!line3d || !emitter_.enableVisualization) return;

	Vector3 emitterPos = emitter_.translate + offset_;

	switch (static_cast<EmitterShapeType>(emitter_.shapeType)) {
	case EmitterShapeType::POINT:
	{
		// 点を小さな球で表現
		line3d->AddPoint(emitterPos, color);
		break;
	}

	case EmitterShapeType::LINE:
	{
		// 線分を描画
		Vector3 start = {
			emitterPos.x + emitter_.lineStart.x,
			emitterPos.y + emitter_.lineStart.y,
			emitterPos.z + emitter_.lineStart.z
		};
		Vector3 direction = Normalize(emitter_.size);
		Vector3 end = {
			start.x + direction.x * emitter_.lineLength,
			start.y + direction.y * emitter_.lineLength,
			start.z + direction.z * emitter_.lineLength
		};
		line3d->AddLine(start, end, color);

		// エッジモードの場合、端点を強調表示
		if (emitter_.spawnOnEdge) {
			line3d->AddPoint(start, { 1.0f, 0.0f, 0.0f, 1.0f }); // 赤
			line3d->AddPoint(end, { 1.0f, 0.0f, 0.0f, 1.0f });   // 赤
		}
		break;
	}

	case EmitterShapeType::SPHERE:
	{
		// 球を描画
		Sphere sphere = { emitterPos, emitter_.radius };
		line3d->AddSphere(sphere, color);

		// エッジモード（表面のみ）の場合、色を変える
		if (emitter_.spawnOnEdge) {
			// 複数の円を描画して表面を強調
			line3d->AddCircle(emitterPos, emitter_.radius, { 1.0f, 0.0f, 0.0f }, color);
			line3d->AddCircle(emitterPos, emitter_.radius, { 0.0f, 1.0f, 0.0f }, color);
			line3d->AddCircle(emitterPos, emitter_.radius, { 0.0f, 0.0f, 1.0f }, color);
		}
		break;
	}

	case EmitterShapeType::BOX:
	{
		// 箱を描画
		line3d->AddBox(emitterPos, emitter_.size, color);

		// エッジモードの場合、辺を強調表示
		if (emitter_.spawnOnEdge) {
			// 12本の辺を太く表示するために、もう一度描画
			line3d->AddBox(emitterPos, emitter_.size, { 1.0f, 0.5f, 0.0f, 1.0f });
		}
		break;
	}

	case EmitterShapeType::PLANE:
	{
		// 平面の向きから接線ベクトルを計算
		Vector3 tangent;
		Vector3 bitangent;

		if (std::abs(emitter_.planeNormal.x) < 0.9f) {
			tangent = Normalize(Cross({ 1.0f, 0.0f, 0.0f }, emitter_.planeNormal));
		}
		else {
			tangent = Normalize(Cross({ 0.0f, 1.0f, 0.0f }, emitter_.planeNormal));
		}
		bitangent = Normalize(Cross(emitter_.planeNormal, tangent));

		// 平面の4隅の頂点を計算
		float halfWidth = emitter_.size.x * 0.5f;
		float halfHeight = emitter_.size.y * 0.5f;

		Vector3 corners[4] = {
			{ emitterPos.x + tangent.x * (-halfWidth) + bitangent.x * (-halfHeight),
			  emitterPos.y + tangent.y * (-halfWidth) + bitangent.y * (-halfHeight),
			  emitterPos.z + tangent.z * (-halfWidth) + bitangent.z * (-halfHeight) },
			{ emitterPos.x + tangent.x * halfWidth + bitangent.x * (-halfHeight),
			  emitterPos.y + tangent.y * halfWidth + bitangent.y * (-halfHeight),
			  emitterPos.z + tangent.z * halfWidth + bitangent.z * (-halfHeight) },
			{ emitterPos.x + tangent.x * halfWidth + bitangent.x * halfHeight,
			  emitterPos.y + tangent.y * halfWidth + bitangent.y * halfHeight,
			  emitterPos.z + tangent.z * halfWidth + bitangent.z * halfHeight },
			{ emitterPos.x + tangent.x * (-halfWidth) + bitangent.x * halfHeight,
			  emitterPos.y + tangent.y * (-halfWidth) + bitangent.y * halfHeight,
			  emitterPos.z + tangent.z * (-halfWidth) + bitangent.z * halfHeight }
		};

		// 境界線を描画
		line3d->AddLine(corners[0], corners[1], color);
		line3d->AddLine(corners[1], corners[2], color);
		line3d->AddLine(corners[2], corners[3], color);
		line3d->AddLine(corners[3], corners[0], color);

		// エッジモードでない場合、内部のグリッドを描画
		if (!emitter_.spawnOnEdge) {
			// 対角線を追加して平面を示す
			line3d->AddLine(corners[0], corners[2], Vector4{ color.x * 0.5f, color.y * 0.5f, color.z * 0.5f, color.w });
			line3d->AddLine(corners[1], corners[3], Vector4{ color.x * 0.5f, color.y * 0.5f, color.z * 0.5f, color.w });
		}

		// 法線ベクトルを表示
		Vector3 normalEnd = {
			emitterPos.x + emitter_.planeNormal.x * 2.0f,
			emitterPos.y + emitter_.planeNormal.y * 2.0f,
			emitterPos.z + emitter_.planeNormal.z * 2.0f
		};
		line3d->AddLine(emitterPos, normalEnd, { 0.0f, 1.0f, 1.0f, 1.0f }); // シアン色で法線表示
		break;
	}

	case EmitterShapeType::RING:
	{
		// 内側と外側の円を描画
		line3d->AddCircle(emitterPos, emitter_.ringInnerRadius, emitter_.planeNormal, color);
		line3d->AddCircle(emitterPos, emitter_.ringOuterRadius, emitter_.planeNormal, color);

		// エッジモードでない場合、リング範囲を示す線を追加
		if (!emitter_.spawnOnEdge) {
			// 接線ベクトルを計算
			Vector3 tangent;
			if (std::abs(emitter_.planeNormal.x) < 0.9f) {
				tangent = Normalize(Cross({ 1.0f, 0.0f, 0.0f }, emitter_.planeNormal));
			}
			else {
				tangent = Normalize(Cross({ 0.0f, 1.0f, 0.0f }, emitter_.planeNormal));
			}

			// 4方向に半径線を描画してリング範囲を示す
			for (int i = 0; i < 4; i++) {
				float angle = std::numbers::pi_v<float> *0.5f * i;
				Vector3 bitangent = Normalize(Cross(emitter_.planeNormal, tangent));
				Vector3 dir = {
					tangent.x * std::cos(angle) + bitangent.x * std::sin(angle),
					tangent.y * std::cos(angle) + bitangent.y * std::sin(angle),
					tangent.z * std::cos(angle) + bitangent.z * std::sin(angle)
				};

				Vector3 innerPoint = {
					emitterPos.x + dir.x * emitter_.ringInnerRadius,
					emitterPos.y + dir.y * emitter_.ringInnerRadius,
					emitterPos.z + dir.z * emitter_.ringInnerRadius
				};
				Vector3 outerPoint = {
					emitterPos.x + dir.x * emitter_.ringOuterRadius,
					emitterPos.y + dir.y * emitter_.ringOuterRadius,
					emitterPos.z + dir.z * emitter_.ringOuterRadius
				};

				line3d->AddLine(innerPoint, outerPoint, Vector4{ color.x * 0.5f, color.y * 0.5f, color.z * 0.5f, color.w });
			}
		}
		break;
	}
	}
}