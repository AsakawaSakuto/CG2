#include"Particles.h"

#include<cassert>

#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

bool IsCollision(const AABB& aabb, const Vector3& point) {
	return (point.x >= aabb.min.x && point.x <= aabb.max.x) &&
		(point.y >= aabb.min.y && point.y <= aabb.max.y) &&
		(point.z >= aabb.min.z && point.z <= aabb.max.z);
}

void Particles::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	blendMode = kBlendModeAdd;

	CreatePSO();

	emitter.transform.translate = { 0.0f,0.0f,0.0f };
	emitter.transform.rotate = { 0.0f,0.0f,0.0f };
	emitter.transform.scale = { 1.0f,1.0f,1.0f };
	emitter.count = 5;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;

	accelerationField.acceleration = { 15.0f, 0.0f, 0.0f };
	accelerationField.area.min = { -3.0f, -3.0f, -3.0f };
	accelerationField.area.max = { 3.0f, 3.0f, 3.0f };

	transformationResource_.resize(kMaxNumInstance);
	transformationData_.resize(kMaxNumInstance);

	textureName_ = "resources/engineResources/uvChecker.png";

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	// 読み込んだテクスチャの番号を取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();
}

void Particles::Update(Camera& useCamera) {

	// パーティクルを発生させる
	emitter.frequencyTime += kDeltaTime;
	if (emitter.frequency <= emitter.frequencyTime) {
		particles.splice(particles.end(), Emit(emitter, rand));
		emitter.frequencyTime -= emitter.frequency;
	}

	// カメラ行列とビルボード行列の作成
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = MultiplyMatrix(backToFrontMatrix, cameraMatrix);

	// 回転成分だけ使うように、平行移動を打ち消す
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// GPUバッファマップ
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instanceData));
	numInstance = 0;

	// パーティクルの更新・インスタンス登録
	for (auto particleIterator = particles.begin(); particleIterator != particles.end(); ) {

		// 寿命が尽きたパーティクルを削除
		if (particleIterator->lifeTime <= particleIterator->currentTime) {
			particleIterator = particles.erase(particleIterator);
			continue;
		}

		// World行列構築
		Matrix4x4 scaleMatrix = MakeScaleMatrix(particleIterator->transform.scale);
		Matrix4x4 translateMatrix = MakeTranslateMatrix(particleIterator->transform.translate);

		Matrix4x4 rotationMatrix = useBillboard
			? billboardMatrix
			: MakeRotateXYZMatrix(particleIterator->transform.rotate);

		Matrix4x4 worldMatrix = MultiplyMatrix(MultiplyMatrix(scaleMatrix, rotationMatrix), translateMatrix);

		// WVP行列計算
		Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
			0.45f,
			static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_),
			0.1f,
			100.0f
		);
		Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));

		// パーティクルの更新処理
		if (isMove) {
			if (useField)
			{
				// Fieldの範囲内のParticleには加速度を適用する
				if (IsCollision(accelerationField.area, (*particleIterator).transform.translate)) {
					(*particleIterator).velocity.x += accelerationField.acceleration.x * kDeltaTime;
					(*particleIterator).velocity.y += accelerationField.acceleration.y * kDeltaTime;
					(*particleIterator).velocity.z += accelerationField.acceleration.z * kDeltaTime;
				}
			}

			particleIterator->transform.translate.x += particleIterator->velocity.x * kDeltaTime;
			particleIterator->transform.translate.y += particleIterator->velocity.y * kDeltaTime;
			particleIterator->transform.translate.z += particleIterator->velocity.z * kDeltaTime;
			particleIterator->currentTime += kDeltaTime;
		}

		float alpha = 0.0f;
		float halfLife = particleIterator->lifeTime * 0.5f;

		if (particleIterator->currentTime < halfLife) {
			alpha = particleIterator->currentTime / halfLife;  // 0 → 1
		} else {
			alpha = 1.0f - ((particleIterator->currentTime - halfLife) / halfLife);  // 1 → 0
		}

		// 描画データに登録
		if (numInstance < kMaxNumInstance) {
			instanceData[numInstance].WVP = worldViewProjectionMatrix;
			instanceData[numInstance].World = worldMatrix;
			instanceData[numInstance].color = particleIterator->color;
			instanceData[numInstance].color.w = alpha;
			++numInstance;
		}

		++particleIterator;
	}

	// GPUバッファアンマップ
	instancingResource_->Unmap(0, nullptr);
}

void Particles::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Triangleの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);  // VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);
	//
	commandList_->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// Spriteを常にuvCheckerにする
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	//
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 描画！ (DrawCall/ドローコール)
	if (numInstance > 0) {
		commandList_->DrawIndexedInstanced(6, numInstance, 0, 0, 0);
	}
	
}

void Particles::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	ImGui::Text("ChecBox");
	ImGui::Checkbox("isMove", &isMove);
	ImGui::Checkbox("useBillboard", &useBillboard);
	ImGui::Checkbox("useField", &useField);

	ImGui::ColorEdit4("ColorEdit", &materialData_->color.x);

	ImGui::DragFloat3("Emitter", &emitter.transform.translate.x, 0.01f);

	ImGui::End();

}

ParticleData Particles::MakeNewParticle(std::mt19937& rand, const Vector3& translate) {

	std::uniform_real_distribution<float> randTranslateX(-1.0f, 1.0f);
	std::uniform_real_distribution<float> randTranslateY(-1.0f, 1.0f);
	std::uniform_real_distribution<float> randTranslateZ(-1.0f, 1.0f);
	std::uniform_real_distribution<float> randVelocity(-1.0f, 1.0f);
	std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> randTime(1.0f, 2.0f);
	ParticleData particle = {};

	particle.transform.scale = { 1.f,1.f,1.f };
	particle.transform.rotate = { 0.f,0.f,0.f };
	particle.transform.translate.x = translate.x + randTranslateX(rand);
	particle.transform.translate.y = translate.y + randTranslateY(rand);
	particle.transform.translate.z = translate.z + randTranslateZ(rand);

	particle.velocity = { randVelocity(rand), randVelocity(rand), randVelocity(rand) };
	particle.color = { randColor(rand),randColor(rand),randColor(rand),1.0f };
	particle.lifeTime = randTime(rand);
	particle.currentTime = 0;

	return particle;
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

std::list<ParticleData> Particles::Emit(const Emitter& emitter, std::mt19937& rand) {
	std::list<ParticleData> particles;
	for (uint32_t count = 0; count < emitter.count; count++) {
		particles.push_back(MakeNewParticle(rand, emitter.transform.translate));
	}
	return particles;
}

#pragma region 

void Particles::CreateVertexResource() {
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData_[1].position = { -0.5f,0.5f,0.0f,1.0f };
	vertexData_[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData_[3].position = { 0.5f,0.5f,0.0f,1.0f };

	vertexData_[0].texcoord = { 0.f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.f };
	vertexData_[3].texcoord = { 1.0f,0.0f };

	for (uint32_t i = 0; i < 4; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}
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
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentityMatrix();

	materialResource_->Unmap(0, nullptr);
}

void Particles::CreateTransformationResource() {
	for (uint32_t i = 0; i < kMaxNumInstance; i++) {
		transformationResource_[i] = CreateBufferResource(device_.Get(), sizeof(ParticleForGPU));
		transformationResource_[i]->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_[i]));
		transformationData_[i]->WVP = MakeIdentityMatrix();
		transformationData_[i]->World = MakeIdentityMatrix();
		transformationData_[i]->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	//instancingResource_->Unmap(0, nullptr);
	// --- Instancing用 StructuredBuffer をまとめて1つ作成 ---
	instancingResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleForGPU) * kMaxNumInstance);
	// SRV設定（StructuredBufferとして使う）
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = kMaxNumInstance;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// SRVハンドルの取得（Heap内の空いているスロット、ここでは例として3番）
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = GetCPUDescriptorHandle(dxCommon_->GetSRV().Get(), dxCommon_->GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon_->GetSRV().Get(), dxCommon_->GetDescriptorSizeSRV(), 3);

	// SRVをHeapに登録
	device_->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, handleCPU);
}

void Particles::CreateDirectionalLightResource() {
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	//
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
}

void Particles::CreatePSO() {

	CreateRootSignature();
	InputLayoutSet();
	CompileShaders();
	BlendStateSet(blendMode);
	RasiterzerStateSet();
	DepthStencilStateSet();

	// グラフィックスパイプラインステートの設定
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_;
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc_.BlendState = blendDesc_;
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
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr_));
}

void Particles::CreateRootSignature() {
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// --- Descriptor Ranges ---
	D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = {};

	// t0 : テクスチャ用 SRV (Pixel Shader)
	descriptorRanges[0].BaseShaderRegister = 0;
	descriptorRanges[0].NumDescriptors = 1;
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// t1 : StructuredBuffer用 SRV (Vertex Shader)
	descriptorRanges[1].BaseShaderRegister = 1;
	descriptorRanges[1].NumDescriptors = 1;
	descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// --- Static Sampler ---
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// --- Root Parameters ---
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	// b0 : Material（Pixel Shader）
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// t1 : StructuredBuffer用 SRV（Vertex Shader）
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;

	// t0 : テクスチャ（Pixel Shader）
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

	// b2 : DirectionalLight（Pixel Shader）
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 2;

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	// --- シリアライズして RootSignature を生成 ---
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr_));

	hr_ = device_->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
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

	// NORMAL ← ここが大事！
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

void Particles::BlendStateSet(BlendMode blendMode) {
	// 全色成分を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;

	switch (blendMode) {
	case kBlendModeNone:
		blendDesc_.RenderTarget[0].BlendEnable = FALSE;
		break;

	case kBlendModeNormal:
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case kBlendModeAdd:
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case kBlendModeSubtract:
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; // 減算
		break;

	case kBlendModeMultily:
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case kBlendModeScreen:
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;

	default:
		// 無効なモードは無効化
		blendDesc_.RenderTarget[0].BlendEnable = FALSE;
		break;
	}

	// アルファブレンド設定（基本固定でOK）
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
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

#pragma endregion
