#include "Particles.h"
#include "Object3d.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

void Particles::Initialize(DirectXCommon* dxCommon, const std::string& TextureName) {
	// DX共通クラスからデバイス・コマンドリストを取得
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	// ブレンドモードを加算合成に初期化
	blendMode_ = kBlendModeAdd;

	// パイプラインステートオブジェクトの作成
	CreatePSO();

	// テクスチャ名を保持しておく
	textureName_ = TextureName;

	// テクスチャマネージャー初期化とテクスチャの読み込み
	TextureManager::GetInstance()->Initialize(dxCommon_);
	TextureManager::GetInstance()->LoadTexture(textureName_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	// リソースの作成
	CreateVertexResource();   // 頂点バッファ
	CreateIndexResource();    // インデックスバッファ
	CreateMaterialResource(); // マテリアル

	uint32_t num = kMaxParticles_ / 512;
	kDispatchCount = num;

	// Emitterのデフォルト値
	emitter_.count = 10;
	emitter_.frequency = 0.1f;
	emitter_.frequencyTime = 0.0f;
	emitter_.translate = { 0.0f, 0.0f, 0.0f };
	emitter_.radius = 1.0f;
	emitter_.emit = 0;
	emitter_.kMaxParticle = kMaxParticles_;

	CreateEmitterResource();
	CreateParticleResource();
	CreatePerViewResource();
	CreatePerFrameResource();
}

void Particles::Update(Camera& useCamera) {

	// ---- カメラ関連の行列計算（PreView構造体に書き込む） ----
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = MultiplyMatrix(backToFrontMatrix, cameraMatrix);

	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
		0.45f,
		static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_),
		0.1f, 100.0f);

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

void Particles::Draw() {

	// ルートシグネチャの設定（パイプラインステートだけでは設定されないため明示的に指定が必要）
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());

	// 現在のブレンドモードに応じて、使用するパイプラインステートオブジェクト（PSO）を切り替え
	switch (blendMode_) {
	case kBlendModeNone:
		useGraphicsPipelineState_ = graphicsPipelineStateNone_.Get();
		break;
	case kBlendModeNormal:
		useGraphicsPipelineState_ = graphicsPipelineStateNormal_.Get();
		break;
	case kBlendModeAdd:
		useGraphicsPipelineState_ = graphicsPipelineStateAdd_.Get();
		break;
	case kBlendModeSubtract:
		useGraphicsPipelineState_ = graphicsPipelineStateSubtract_.Get();
		break;
	case kBlendModeMultily:
		useGraphicsPipelineState_ = graphicsPipelineStateMultily_.Get();
		break;
	case kBlendModeScreen:
		useGraphicsPipelineState_ = graphicsPipelineStateScreen_.Get();
		break;
	}

	commandList_->SetPipelineState(useGraphicsPipelineState_.Get());

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
	commandList_->SetGraphicsRootDescriptorTable(2, dxCommon_->GetSrvGPUHandle(particleSrvIndex_)); // ←今まで1番だったやつ

	// テクスチャSRV
	commandList_->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	// DrawIndexedInstanced(インデックス数, インスタンス数, 開始インデックス, ベース頂点, 開始インスタンス)
	commandList_->DrawIndexedInstanced(6, kMaxParticles_, 0, 0, 0);
}

void Particles::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	ImGui::Text("kMaxParticle:%d", kMaxParticles_);
	ImGui::Text("kDispatchCount:%d", kDispatchCount);
	ImGui::DragInt("EmitterCount", &emitter_.count, 1);
	ImGui::DragFloat("EmitterFrequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat3("EmitterT", &emitter_.translate.x, 0.1f);

	ImGui::ColorEdit4("ColorEdit", &materialData_->color.x);

	const char* directionLabels[] = { "None", "Normal", "Add","Subtract","Multily" ,"Screen" };
	int current = static_cast<int>(blendMode_);
	ImGui::Combo("BlendMode", &current, directionLabels, 6);
	blendMode_ = static_cast<BlendMode>(current);

	ImGui::End();
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
		dxCommon_->GetSrvCPUHandle(particleSrvIndex_) // UAVヒープ上の任意のスロット
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
		dxCommon_->GetSrvCPUHandle(particleSrvIndex_) // SRVヒープ上の任意のスロット
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
		dxCommon_->GetSrvCPUHandle(65));

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
		dxCommon_->GetSrvCPUHandle(66));

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

	// --- 動作用CS（EmitterParticle.CS.hlsl） ---
	ComPtr<IDxcBlob> csUpdateBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/UpdateParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csUpdateDesc = {};
	csUpdateDesc.pRootSignature = csRootSignature_.Get();
	csUpdateDesc.CS = { csUpdateBlob->GetBufferPointer(), csUpdateBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csUpdateDesc, IID_PPV_ARGS(&csUpdatePipelineState_));

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
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	commandList_->ResourceBarrier(1, &toSRV);
}

void Particles::CreateEmitterResource() {

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(EmitterSphere));

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
	emitter_.frequencyTime += kDeltaTime_;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		emitter_.frequencyTime -= emitter_.frequency;
		emitter_.emit = 1;
	} else {
		emitter_.emit = 0;
	}

	// Unmapは不要。UploadHeapの場合、毎フレームマップしっぱなしでOK
	EmitterSphere* mappedEmitter = nullptr;
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedEmitter));
	// ここで値をコピーまたは書き換え
	*mappedEmitter = emitter_; // 構造体ごとコピー
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
	totalTime_ += kDeltaTime_;
	// フレームごとに
	PerFrame* mapped = nullptr;
	perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	mapped->time = totalTime_;      // 例えば経過時間など
	mapped->deltaTime = kDeltaTime_;  // フレームごとの経過時間
	mapped->index = frameIndex;
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

void Particles::CreatePSO() {

	CreateRootSignature();
	InputLayoutSet();
	CompileShaders();
	BlendStateSet();
	RasiterzerStateSet();
	DepthStencilStateSet();

	// グラフィックスパイプラインステートの設定
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_;
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc_.BlendState = blendDescNone_;
	graphicsPipelineStateDesc_.RasterizerState = rasterizerDesc_;
	graphicsPipelineStateDesc_.NumRenderTargets = 1; // 書き込み先のRTVの情報
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 利用するプリミティブ（幾何）タイプ。三角形
	graphicsPipelineStateDesc_.SampleDesc.Count = 1; // どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDesc_.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc_.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateNone_));
	assert(SUCCEEDED(hr_));

	graphicsPipelineStateDesc_.BlendState = blendDescNormal_;
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateNormal_));
	assert(SUCCEEDED(hr_));

	graphicsPipelineStateDesc_.BlendState = blendDescAdd_;
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateAdd_));
	assert(SUCCEEDED(hr_));

	graphicsPipelineStateDesc_.BlendState = blendDescSubtract_;
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateSubtract_));
	assert(SUCCEEDED(hr_));

	graphicsPipelineStateDesc_.BlendState = blendDescMultily_;
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateMultily_));
	assert(SUCCEEDED(hr_));

	graphicsPipelineStateDesc_.BlendState = blendDescScreen_;
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateScreen_));
	assert(SUCCEEDED(hr_));
}

void Particles::CreateRootSignature() {
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	// Input Assembler の入力レイアウトを使用することを許可するフラグ
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// --- Descriptor Ranges ---

	// 2つのDescriptorRangeを用意（テクスチャSRV用とStructuredBuffer用）
	D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = {};

	// t0 : ピクセルシェーダで使う通常テクスチャ用のSRV（gTexture）
	descriptorRanges[0].BaseShaderRegister = 0; // t0
	descriptorRanges[0].NumDescriptors = 1; // 1つだけ
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// t1 : 頂点シェーダで使う構造化バッファ（StructuredBuffer）用のSRV（gParticles）
	descriptorRanges[1].BaseShaderRegister = 1; // t1
	descriptorRanges[1].NumDescriptors = 1; // 1つだけ
	descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// --- Static Sampler ---

	// サンプラーステートの定義（PixelShaderで使用）
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 線形フィルタリング
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0; // s0 に対応
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// --- Root Parameters ---

	// ルートパラメータを4つ設定（CBV × 2、SRV × 2）
	D3D12_ROOT_PARAMETER rootParameters[7] = {};

	// b0 : マテリアル（Pixel Shader）
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// b1 : カメラ＆ビルボード行列（PreView/Vertex Shader用 CBV！★追加！）
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 1;

	// t1 : パーティクル情報（構造化バッファ）（Vertex Shader用SRV）
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[1]; // t1
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

	// t0 : テクスチャ（Pixel Shader用SRV）
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].DescriptorTable.pDescriptorRanges = &descriptorRanges[0]; // t0
	rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

	// b2 : ディレクショナルライト（Pixel Shader用CBV）
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;

	// b5 : エミッター（Compute/Vertex/Pixel 用CBV）
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // 必要に応じて
	rootParameters[5].Descriptor.ShaderRegister = 5;

	// b6 : PerFrame（Compute/Vertex/Pixel 用CBV）
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // 必要に応じて
	rootParameters[6].Descriptor.ShaderRegister = 6;

	// ルートパラメータ配列をルートシグネチャ記述に登録
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_ROOT_PARAMETER csRootParameters[1] = {};

	// u0 : パーティクルUAV
	csRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	csRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // or COMPUTE
	csRootParameters[0].Descriptor.ShaderRegister = 0; // u0

	// --- シリアライズしてRootSignatureを生成 ---
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	// ルートシグネチャのシリアライズ
	hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);
	assert(SUCCEEDED(hr_));

	// 実際のルートシグネチャオブジェクトを作成
	hr_ = device_->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(hr_));
}

void Particles::InputLayoutSet() {

	// POSITION
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].InputSlot = 0;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs_[0].InstanceDataStepRate = 0;

	// TEXCOORD
	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].InputSlot = 0;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs_[1].InstanceDataStepRate = 0;

	// NORMAL
	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].InputSlot = 0;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs_[2].InstanceDataStepRate = 0;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void Particles::CompileShaders() {

	// Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void Particles::BlendStateSet() {
	// kBlendModeNone:
	blendDescNone_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescNone_.RenderTarget[0].BlendEnable = FALSE;
	blendDescNone_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescNone_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescNone_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// kBlendModeNormal:
	blendDescNormal_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescNormal_.RenderTarget[0].BlendEnable = TRUE;
	blendDescNormal_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescNormal_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescNormal_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescNormal_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescNormal_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescNormal_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// kBlendModeAdd:
	blendDescAdd_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescAdd_.RenderTarget[0].BlendEnable = TRUE;
	blendDescAdd_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAdd_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescAdd_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAdd_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAdd_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescAdd_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// kBlendModeSubtract:
	blendDescSubtract_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescSubtract_.RenderTarget[0].BlendEnable = TRUE;
	blendDescSubtract_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescSubtract_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescSubtract_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDescSubtract_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescSubtract_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescSubtract_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// kBlendModeMultily:
	blendDescMultily_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescMultily_.RenderTarget[0].BlendEnable = TRUE;
	blendDescMultily_.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDescMultily_.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	blendDescMultily_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescMultily_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescMultily_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescMultily_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// kBlendModeScreen:
	blendDescScreen_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescScreen_.RenderTarget[0].BlendEnable = TRUE;
	blendDescScreen_.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDescScreen_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescScreen_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescScreen_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescScreen_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescScreen_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Particles::RasiterzerStateSet() {
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void Particles::DepthStencilStateSet() {
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数LessEqual、つまり、深ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}