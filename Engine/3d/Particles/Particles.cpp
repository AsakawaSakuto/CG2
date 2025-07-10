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

	// 初回生成数を設定
	spawnCount_ = 1;

	// エミッターのトランスフォームを初期化（スケール・回転・座標）
	emitter_.transform.scale = { 1.0f, 1.0f, 1.0f };
	emitter_.transform.rotate = { 0.0f, 0.0f, 0.0f };
	emitter_.transform.translate = { 0.0f, 0.0f, 0.0f };

	// エミッターの生成設定
	emitter_.count = spawnCount_;        // 1回あたりの生成数
	emitter_.frequency = 0.1f;           // 生成間隔（秒）
	emitter_.frequencyTime = 0.0f;       // 経過時間の初期化

	// テクスチャ名を保持しておく
	textureName_ = TextureName;

	// テクスチャマネージャー初期化とテクスチャの読み込み
	TextureManager::GetInstance()->Initialize(dxCommon_);
	TextureManager::GetInstance()->LoadTexture(textureName_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	// リソースの作成
	CreateVertexResource();             // 頂点バッファ
	CreateIndexResource();              // インデックスバッファ
	CreateMaterialResource();           // マテリアル
	CreateTransformationResource();     // インスタンシング用行列バッファ
	CreateDirectionalLightResource();   // ライト情報

	// エミッターの可視化用オブジェクトの初期化
	emitterModel_->Initialize(dxCommon_, "resources/object3d/cube.obj", "resources/engineResources/white16x16.png");
	emitterModel_->SetDrawMode(false);   // 描画モードON/OFF設定
	emitterModel_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 可視化用の色（黒）
}


void Particles::Update(Camera& useCamera) {
	// 可視化用エミッター（箱）の位置とスケールを同期
	emitterModel_->SetScale(emitter_.transform.scale);
	emitterModel_->SetPosition(emitter_.transform.translate);
	emitterModel_->Update(useCamera); // 表示位置・行列をカメラに合わせて更新

	// GPUバッファをマップして、書き込み用ポインタを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instanceData_));
	numInstance_ = 0; // インスタンス数を初期化

	// エミッターの発生数を更新
	emitter_.count = spawnCount_;

	// パーティクルの発生処理（一定時間ごとにEmit）
	emitter_.frequencyTime += kDeltaTime_;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		// 新規パーティクル生成し、リスト末尾に追加
		particles_.splice(particles_.end(), Emit(emitter_, rand));
		emitter_.frequencyTime -= emitter_.frequency;
	}

	// カメラのビュー行列とプロジェクション行列の作成
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>); // 反転（Z+→Z-）用
	Matrix4x4 billboardMatrix = MultiplyMatrix(backToFrontMatrix, cameraMatrix); // ビルボード行列作成

	// ビルボード行列の平行移動成分を無効化（回転のみ利用）
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix); // ビュー行列
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);

	// パーティクルの更新＆インスタンスデータ登録
	for (auto particleIterator = particles_.begin(); particleIterator != particles_.end(); ) {
		// 寿命切れのパーティクルを削除
		if (particleIterator->lifeTime <= particleIterator->currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}

		// パーティクル個別のワールド行列（SRT順で構築）
		Matrix4x4 scaleMatrix = MakeScaleMatrix(particleIterator->transform.scale);
		Matrix4x4 rotationMatrix = useBillboard_ ? billboardMatrix : MakeRotateXYZMatrix(particleIterator->transform.rotate);
		Matrix4x4 translateMatrix = MakeTranslateMatrix(particleIterator->transform.translate);
		Matrix4x4 worldMatrix = MultiplyMatrix(MultiplyMatrix(scaleMatrix, rotationMatrix), translateMatrix);
		// WVP行列（ワールド→ビュー→プロジェクション）
		Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));

		// パーティクルの位置更新（速度に応じて移動）
		if (isMove_) {
			particleIterator->transform.translate.x += particleIterator->velocity.x * kDeltaTime_;
			particleIterator->transform.translate.y += particleIterator->velocity.y * kDeltaTime_;
			particleIterator->transform.translate.z += particleIterator->velocity.z * kDeltaTime_;
		}
		particleIterator->currentTime += kDeltaTime_;

		// アルファ（透明度）計算（フェードイン → フェードアウト）
		float alpha = 0.0f;
		float halfLife = particleIterator->lifeTime * 0.5f;
		if (particleIterator->currentTime < halfLife) {
			alpha = particleIterator->currentTime / halfLife;  // 0 → 1
		}
		else {
			alpha = 1.0f - ((particleIterator->currentTime - halfLife) / halfLife);  // 1 → 0
		}

		// インスタンスデータとして登録（最大数を超えないように）
		if (numInstance_ < kMaxNumInstance_) {
			instanceData_[numInstance_].color = particleIterator->color;
			instanceData_[numInstance_].color.w = alpha;
			instanceData_[numInstance_].WVP = worldViewProjectionMatrix;
			instanceData_[numInstance_].World = worldMatrix;
			++numInstance_;
		}
		++particleIterator;
	}

	// GPUバッファのアンマップ
	instancingResource_->Unmap(0, nullptr);
}

void Particles::Draw() {

	// ルートシグネチャの設定（パイプラインステートだけでは設定されないため明示的に指定が必要）
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());

	// 現在のブレンドモードに応じて、使用するパイプラインステートオブジェクト（PSO）を切り替え
	switch (blendMode_) {
	case kBlendModeNone:
		commandList_->SetPipelineState(graphicsPipelineStateNone_.Get());
		break;
	case kBlendModeNormal:
		commandList_->SetPipelineState(graphicsPipelineStateNormal_.Get());
		break;
	case kBlendModeAdd:
		commandList_->SetPipelineState(graphicsPipelineStateAdd_.Get());
		break;
	case kBlendModeSubtract:
		commandList_->SetPipelineState(graphicsPipelineStateSubtract_.Get());
		break;
	case kBlendModeMultily:
		commandList_->SetPipelineState(graphicsPipelineStateMultily_.Get());
		break;
	case kBlendModeScreen:
		commandList_->SetPipelineState(graphicsPipelineStateScreen_.Get());
		break;
	}

	// 三角形リストで描画することを指定（パーティクルは四角形だが、インデックスで三角形×2として構成）
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファ（VBV）とインデックスバッファ（IBV）の設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);

	// インスタンシング用データ（StructuredBuffer）をSRVとしてルートパラメータ1番に設定
	commandList_->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	// マテリアル用定数バッファ（b0）をルートパラメータ0番に設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// テクスチャのSRVをルートパラメータ2番に設定（ここではuvCheckerなどを使用）
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	// ライト情報（ディレクショナルライト）をルートパラメータ3番に設定
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// パーティクルのインスタンシング描画
	// DrawIndexedInstanced(インデックス数, インスタンス数, 開始インデックス, ベース頂点, 開始インスタンス)
	if (numInstance_ > 0) {
		commandList_->DrawIndexedInstanced(6, numInstance_, 0, 0, 0);
	}

	// エミッターのワイヤーフレーム（Cubeモデル）の描画（デバッグ・可視化用）
	if (isDrawEmitter_) {
		emitterModel_->Draw();
	}
}

void Particles::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	ImGui::Text("ChecBox");
	ImGui::Checkbox("isMove", &isMove_);
	ImGui::Checkbox("useBillboard", &useBillboard_); 
	ImGui::Checkbox("isDrawEmitter", &isDrawEmitter_);

	ImGui::ColorEdit4("ColorEdit", &materialData_->color.x);

	const char* directionLabels[] = { "None", "Normal", "Add","Subtract","Multily" ,"Screen" };
	int current = static_cast<int>(blendMode_);
	ImGui::Combo("BlendMode", &current, directionLabels, 6);
	blendMode_ = static_cast<BlendMode>(current);

	ImGui::DragFloat3("Emitter", &emitter_.transform.translate.x, 0.01f);
	ImGui::DragFloat3("EmitterRange", &emitter_.transform.scale.x, 0.01f);
	ImGui::DragInt("GenerateCount", &spawnCount_, 1.0f, 0, 1000);
	ImGui::DragFloat("GenerateInterval", &emitter_.frequency, 0.001f, 0.0f, 1.0f);

	ImGui::Text("Particle Count: %d", static_cast<int>(particles_.size()));

	ImGui::End();
}

std::list<ParticleData> Particles::Emit(const Emitter& emitter, std::mt19937& rand) {
	std::list<ParticleData> particles;
	for (uint32_t count = 0; count < emitter.count; count++) {
		// エミッター設定に基づいて1つずつパーティクルを生成してリストに追加
		particles.push_back(MakeNewParticle(rand, emitter));
	}
	return particles;
}

ParticleData Particles::MakeNewParticle(std::mt19937& rand, const Emitter& emitter) {

	// ランダムな位置生成用：スケールの範囲内でX, Y, Z座標をばらつかせる
	std::uniform_real_distribution<float> randTranslateX(emitter.transform.translate.x - emitter.transform.scale.x, emitter.transform.translate.x + emitter.transform.scale.x);
	std::uniform_real_distribution<float> randTranslateY(emitter.transform.translate.y - emitter.transform.scale.y, emitter.transform.translate.y + emitter.transform.scale.y);
	std::uniform_real_distribution<float> randTranslateZ(emitter.transform.translate.z - emitter.transform.scale.z, emitter.transform.translate.z + emitter.transform.scale.z);

	
	std::uniform_real_distribution<float> randVelocity(-1.0f, 1.0f); // ランダムな速度（方向）
	std::uniform_real_distribution<float> randColor(0.0f, 1.0f);     // ランダムなカラー（RGBのみ）
	std::uniform_real_distribution<float> randTime(1.0f, 2.0f);      // 寿命（lifeTime）のランダム生成

	ParticleData particle = {}; // 新しいパーティクルデータ構造体を初期化

	particle.transform.scale = { 1.f,1.f,1.f };  // スケールは固定（1,1,1）
	particle.transform.rotate = { 0.f,0.f,0.f }; // 回転は初期値として0に設定
	                                             // ↓生成位置をエミッターの位置を中心にランダムでばらつかせる
	particle.transform.translate.x = randTranslateX(rand) + emitter.transform.translate.x;
	particle.transform.translate.y = randTranslateY(rand) + emitter.transform.translate.y;
	particle.transform.translate.z = randTranslateZ(rand) + emitter.transform.translate.z;

	particle.velocity = { randVelocity(rand), randVelocity(rand), randVelocity(rand) }; // 初速度をランダムに設定
	particle.color = { randColor(rand),randColor(rand),randColor(rand),1.0f }; 	        // 色の設定：RGBはランダム
	particle.lifeTime = randTime(rand); 	                                            // パーティクルの寿命（最大生存時間）
	particle.currentTime = 0;                                                           // 現在の時間は初期化（誕生時は0）

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

void Particles::CreateTransformationResource() {
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleForGPU) * kMaxNumInstance_);
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&instanceData_));
	instanceData_->WVP = MakeIdentityMatrix();
	instanceData_->World = MakeIdentityMatrix();
	instanceData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	//instancingResource_->Unmap(0, nullptr);
	// --- Instancing用 StructuredBuffer をまとめて1つ作成 ---
	instancingResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleForGPU) * kMaxNumInstance_);
	// SRV設定（StructuredBufferとして使う）
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = kMaxNumInstance_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// SRVハンドルの取得（Heap内の空いているスロット、ここでは例として3番）
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = GetCPUDescriptorHandle(dxCommon_->GetSRV().Get(), dxCommon_->GetDescriptorSizeSRV(), 32);
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon_->GetSRV().Get(), dxCommon_->GetDescriptorSizeSRV(), 32);

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
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	// b0 : マテリアル（Material）構造体（Pixel Shader）
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// t1 : パーティクル情報（構造化バッファ）用SRV（Vertex Shader）
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRanges[1]; // t1
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;

	// t0 : テクスチャ情報用SRV（Pixel Shader）
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[0]; // t0
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

	// b2 : ディレクショナルライト情報（DirectionalLight）（Pixel Shader）
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 2;

	// ルートパラメータ配列をルートシグネチャ記述に登録
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

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

#pragma endregion
