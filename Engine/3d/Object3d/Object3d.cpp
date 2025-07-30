#include "Object3d.h"
#include "WinApp.h"

#include<cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

//"resources/uvChecker.png"

void Object3d::Initialize(DirectXCommon* dxCommon,  const std::string& modelPath , const std::string& TexrurePath) {
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	CreatePSO();

	modelData_ = LoadObject3dFile(modelPath);

	textureName_ = TexrurePath;

	TextureManager::GetInstance()->Initialize(dxCommon_);
	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	// 読み込んだテクスチャの番号を取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	transform_ = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	direction_ = {1.0f,-1.0f,1.0f};

	CreateVertexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();
	CreateCameraResource();
	CreatePointLightResource();
	CreateSpotLightResource();
}

void Object3d::Update(Camera& useCamera) {

	cameraData_->worldPosition = useCamera.GetTranslate(); // カメラの位置を渡す

	directionalLightData_->direction = direction_;

	// 行列の内容を更新して三角形を動かす
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));

	// 追加処理：法線変換行列を計算
	Matrix4x4 worldInverseMatrix = InverseMatrix(worldMatrix);

	// 書き込み
	//transformationData_->WVP = MultiplyMatrix(modelData_.rootNode.localMatrix, MultiplyMatrix(worldMatrix,worldViewProjectionMatrix));
	transformationData_->WVP = MultiplyMatrix(modelData_.rootNode.localMatrix, worldViewProjectionMatrix);
	transformationData_->World = MultiplyMatrix(modelData_.rootNode.localMatrix, worldMatrix);
	transformationData_->WorldInverseTranspose = worldInverseMatrix;

	Matrix4x4 scale = MakeIdentityMatrix();
	scale.m[0][0] = uvScale_.x;
	scale.m[1][1] = uvScale_.y;

	Matrix4x4 rot = MakeIdentityMatrix();
	rot.m[0][0] = cos(uvRotate_);
	rot.m[0][1] = -sin(uvRotate_);
	rot.m[1][0] = sin(uvRotate_);
	rot.m[1][1] = cos(uvRotate_);

	Matrix4x4 trans = MakeIdentityMatrix();
	trans.m[3][0] = uvTranslate_.x;
	trans.m[3][1] = uvTranslate_.y;

	// 最終変換行列
	materialData_->uvTransform = scale * rot * trans;
}

void Object3d::Draw() {

	OutputDebugStringA("Object3d::Draw() 開始\n");

	commandList_ = dxCommon_->GetCommandList();
	if (!commandList_) {
		OutputDebugStringA("commandList_ is null!\n");
		return;
	}

	OutputDebugStringA("RootSignature 設定\n");
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	OutputDebugStringA("PSO 設定\n");
	commandList_->SetPipelineState(drawMode_ ? graphicsPipelineStateSolid_.Get() : graphicsPipelineStateWireframe_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(nullptr); // インデックス使っていないので設定しない
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// RootParams 設定（そのままでOK）
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());

	// 頂点描画（DrawIndexedではなくDrawInstanced）
	commandList_->DrawInstanced(
		static_cast<UINT>(modelData_.vertices.size()), // 頂点数ぶん描画
		1, 0, 0);
}

void Object3d::SetTexture(const std::string& textureName) {
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

void Object3d::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	ImGui::Text("Transform");
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);

	if (ImGui::Button("tReset")) {
		transform_.translate = { 0.0f, 0.0f, 0.0f };
		transform_.rotate = { 0.0f, 0.0f, 0.0f };
		transform_.scale = { 1.0f, 1.0f, 1.0f };
	}

	ImGui::Separator();

	ImGui::Text("MaterialEdit");
	ImGui::DragFloat2("uvTranslate", &uvTranslate_.x, 0.01f);
	ImGui::DragFloat2("uvScale", &uvScale_.x, 0.01f);
	ImGui::DragFloat("uvRotate", &uvRotate_, 0.01f);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	ImGui::Checkbox("DrawMode", &drawMode_);
	
	if (ImGui::Button("mReset")) {
		uvTranslate_ = { 0.0f,0.0f };
		uvScale_ = { 1.0f,1.0f };
		uvRotate_ = 0.0f;
		materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	}

	ImGui::Separator();

	ImGui::Text("LightEdit");

	uint32_t useLight = materialData_->enableLighting;
	bool temp = (useLight != 0);
	if (ImGui::Checkbox("UseLight", &temp)) {
		materialData_->enableLighting = temp ? 1 : 0;
	}

	if (materialData_->enableLighting != 0) {
		ImGui::Separator();

		uint32_t useLight1 = directionalLightData_->useLight;
		bool temp1 = (useLight1 != 0);
		if (ImGui::Checkbox("UseDirectionLight", &temp1)) {
			directionalLightData_->useLight = temp1 ? 1 : 0;
		}

		if (directionalLightData_->useLight != 0) {

			uint32_t useHalfLambert = directionalLightData_->useHalfLambert;
			bool temp = (useHalfLambert != 0);
			if (ImGui::Checkbox("UseHalfLambert", &temp)) {
				directionalLightData_->useHalfLambert = temp ? 1 : 0;
			}

			ImGui::DragFloat3("d.Direction", &direction_.x, 0.01f, -1.0f, 1.0f);
			direction_ = direction_.Normalize();
			ImGui::DragFloat("d.Intensity", &directionalLightData_->intensity, 0.01f, 0.0f, 5.0f);
			ImGui::DragFloat("d.Shininess", &materialData_->shininess, 0.01f, 0.0f, 100.0f);
			ImGui::ColorEdit4("d.Color", &directionalLightData_->color.x);

			if (ImGui::Button("dReset")) {
				direction_ = { 1.0f,-1.0f,1.0f };
				materialData_->shininess = 30.0f;
				directionalLightData_->intensity = 1.0f;
				directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
			}
		}

		ImGui::Separator();

		uint32_t useLight2 = pointLightData_->useLight;
		bool temp2 = (useLight2 != 0);
		if (ImGui::Checkbox("UsePointLight", &temp2)) {
			pointLightData_->useLight = temp2 ? 1 : 0;
		}

		if (pointLightData_->useLight != 0) {
			ImGui::DragFloat3("p.Pos", &pointLightData_->position.x, 0.01f);
			ImGui::DragFloat("p.Intensity", &pointLightData_->intensity, 0.01f, 0.0f, 5.0f);
			ImGui::DragFloat("p.Radius", &pointLightData_->radius, 0.01f);
			ImGui::DragFloat("p.Decay", &pointLightData_->decay, 0.01f);
			ImGui::ColorEdit4("p.Color", &pointLightData_->color.x);

			if (ImGui::Button("pReset")) {
				pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
				pointLightData_->position = { 0.0f,5.0f,0.0f };
				pointLightData_->intensity = 1.0f;
				pointLightData_->radius = 20.0f;
				pointLightData_->decay = 2.0f;
			}
		}

		ImGui::Separator();

		uint32_t useLight3 = spotLightData_->useLight;
		bool temp3 = (useLight3 != 0);
		if (ImGui::Checkbox("UseSpotLight", &temp3)) {
			spotLightData_->useLight = temp3 ? 1 : 0;
		}

		if (spotLightData_->useLight != 0) {
			ImGui::DragFloat3("s.Pos", &spotLightData_->position.x, 0.01f);
			ImGui::DragFloat("s.Intensity", &spotLightData_->intensity, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("s.Distance", &spotLightData_->distance, 0.01f);
			spotLightData_->direction = spotLightData_->direction.Normalize();
			ImGui::DragFloat3("s.Direction", &spotLightData_->direction.x, 0.01f);
			ImGui::DragFloat("s.Decay", &spotLightData_->decay, 0.01f);
			float angleDeg = 60.0f;            // cosAngle用の角度（UI用、一時変数）
			float falloffStartDeg = 30.0f;     // cosFalloffStart用の角度
			// 現在のcos値から度に変換してUIに表示（必要なら）
			angleDeg = std::acos(spotLightData_->cosAngle) * 180.0f / std::numbers::pi_v<float>;
			falloffStartDeg = std::acos(spotLightData_->cosFalloffStart) * 180.0f / std::numbers::pi_v<float>;
			// ImGuiスライダー（例：0〜90度まで）
			ImGui::SliderFloat("Spot Angle (deg)", &angleDeg, 1.0f, 90.0f);
			ImGui::SliderFloat("Falloff Start (deg)", &falloffStartDeg, 0.0f, angleDeg - 0.01f); // 必ず angle より小さく
			// 入力された角度からcos値に変換して反映
			spotLightData_->cosAngle = std::cos(angleDeg * std::numbers::pi_v<float> / 180.0f);
			spotLightData_->cosFalloffStart = std::cos(falloffStartDeg * std::numbers::pi_v<float> / 180.0f);
			ImGui::ColorEdit4("s.Color", &spotLightData_->color.x);

			if (ImGui::Button("sReset")) {
				spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
				spotLightData_->position = { 2.0f,1.25f,0.0f };
				spotLightData_->distance = 7.0f;
				spotLightData_->direction = { -1.0f,-1.0f,0.0f };
				spotLightData_->intensity = 4.0f;
				spotLightData_->decay = 2.0f;
				spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 6.0f);
				spotLightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f);
			}
		}
	}

	ImGui::End();
}

void Object3d::CreateVertexResource() {
	// 頂点リソースをつくる
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * modelData_.vertices.size());
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size()); 
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	// 頂点リソースにデータを書き込む、書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点データをリソースにコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size()); 
}

void Object3d::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentityMatrix();
	materialData_->shininess =30.0f;
}

void Object3d::CreateTransformationResource() {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	// 単位行列を書き込む
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
	transformationData_->WorldInverseTranspose;
}

void Object3d::CreateDirectionalLightResource() {
	// ライトのリソースを作る
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	// リソースをマップしてデータを書き込む
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
	directionalLightData_->useLight = true;
	directionalLightData_->useHalfLambert = true;
}

void Object3d::CreateCameraResource() {
	cameraResource_ = CreateBufferResource(device_.Get(), sizeof(CameraForGPU));
	assert(cameraResource_ != nullptr);
	HRESULT hr = cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	assert(SUCCEEDED(hr));
}

void Object3d::CreatePointLightResource() {
	pointLightResource_ = CreateBufferResource(device_.Get(), sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,5.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 20.0f;
	pointLightData_->decay = 2.0f;
	pointLightData_->useLight = false;
}

void Object3d::CreateSpotLightResource() {
	spotLightResource_ = CreateBufferResource(device_.Get(), sizeof(SpotLight));
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 2.0f,1.25f,0.0f };
	spotLightData_->distance = 7.0f;
	spotLightData_->direction = { -1.0f,-1.0f,0.0f };
	spotLightData_->intensity = 4.0f;
	spotLightData_->decay = 2.0f;
	spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 6.0f);
	spotLightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData_->useLight = false;
}

void Object3d::CreatePSO() {

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
	graphicsPipelineStateDesc_.BlendState = blendDesc_;
	graphicsPipelineStateDesc_.RasterizerState = rasterizerDescSolid_;
	graphicsPipelineStateDesc_.NumRenderTargets = 1; // 書き込み先のRTVの情報
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 利用するプリミティブ（幾何）タイプ。三角形
	graphicsPipelineStateDesc_.SampleDesc.Count = 1; // どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDesc_.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc_.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateSolid_));
	assert(SUCCEEDED(hr_));

	graphicsPipelineStateDesc_.RasterizerState = rasterizerDescWireFrame_;

	// 実際に生成
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineStateWireframe_));
	assert(SUCCEEDED(hr_));
}

void Object3d::CreateRootSignature() {
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DiscriptorRange 03_00

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// Samplerの設定

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0-1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // あったたのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// RootParameter作成。複数設定できるので配列。今回は単1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[7] = {};

	// RootParam[0] → b0: Material（PS）
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// RootParam[1] → b1: WVP（VS）
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 1;

	// RootParam[2] → t0: テクスチャ（PS）
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	// RootParam[3] → b2: DirectionalLight（PS）
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 2;

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 3; // Camera

	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].Descriptor.ShaderRegister = 4; // Point

	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].Descriptor.ShaderRegister = 5; // Point

	// レジスタ番号0をバインド
	descriptionRootSignature.pParameters = rootParameters;              // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

	// シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

	// バイナリを元に生成
	hr_ = dxCommon_->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));

	if (FAILED(hr_)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		assert(false); // またはエラーハンドリング
	}
}

void Object3d::InputLayoutSet() {

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

void Object3d::CompileShaders() {

	// Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void Object3d::BlendStateSet() {
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// CG3↓
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;
	// BlendMode
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	// α値の設定
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

}

void Object3d::RasiterzerStateSet() {
	
	// 裏面（時計回り）を表示しない
	rasterizerDescSolid_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす // WIREFRAME SOLID
	rasterizerDescSolid_.FillMode = D3D12_FILL_MODE_SOLID;

	// 裏面（時計回り）を表示しない
	rasterizerDescWireFrame_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす // WIREFRAME SOLID
	rasterizerDescWireFrame_.FillMode = D3D12_FILL_MODE_WIREFRAME;
}

void Object3d::DepthStencilStateSet() {
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数LessEqual、つまり、深ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}