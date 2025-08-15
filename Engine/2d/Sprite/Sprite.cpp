#include"Sprite.h"
#include"WinApp.h"

#include<cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

void Sprite::Initialize(DirectXCommon* dxCommon, const std::string& fileName, Vector2 size) {
	//
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();
	
	textureName_ = fileName;
	TextureManager::GetInstance()->LoadTexture(textureName_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	CreatePSO();

	size_ = size;
	transform_.scale = { 1.f,1.f,1.f };
	transform_.translate = { 0.f,0.f,0.f };

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();

	cameraResource_ = CreateBufferResource(device_.Get(), sizeof(CameraForGPU));
	assert(cameraResource_ != nullptr);
	HRESULT hr = cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	assert(SUCCEEDED(hr));

	pointLightResource_ = CreateBufferResource(device_.Get(), sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,5.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 20.0f;
	pointLightData_->decay = 2.0f;

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
}

void Sprite::Update() {
	transform_.translate.x = position_.x;
	transform_.translate.y = position_.y;

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentityMatrix();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kClientWidth_), static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrixSprite));
	transformationData_->WVP = worldViewProjectionMatrixSprite;

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

void Sprite::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);  // VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);
	//
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	// 描画！ (DrawCall/ドローコール)
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	ImGui::Text("Transform");
	ImGui::DragFloat2("translate", &position_.x, 1.f);
	ImGui::DragFloat2("scale", &transform_.scale.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("rotate", &transform_.rotate.z, 0.01f);

	if (ImGui::Button("tReset")) {
		position_ = { 128.0f ,128.0f };
		transform_.rotate = { 0.0f, 0.0f, 0.0f };
		transform_.scale = { 1.0f, 1.0f, 1.0f };
	}

	ImGui::Separator();

	ImGui::Text("MaterialEdit");
	ImGui::DragFloat2("uvTranslate", &uvTranslate_.x, 0.01f);
	ImGui::DragFloat2("uvScale", &uvScale_.x, 0.01f);
	ImGui::DragFloat("uvRotate", &uvRotate_, 0.01f);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	
	if (ImGui::Button("mReset")) {
		uvTranslate_ = { 0.0f,0.0f };
		uvScale_ = { 1.0f,1.0f };
		uvRotate_ = 0.0f;
		materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	}

	ImGui::End();

}

void Sprite::SetTexture(const std::string& textureName) {
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

void Sprite::CreateVertexResource() {
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(Object3dVertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(Object3dVertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Object3dVertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	float width = size_.x;
	float height = size_.y;

	float left = 0.0f - anchorPoint.x * size_.x;
	float right = 1.0f - anchorPoint.x * size_.x;
	float top = 0.0f - anchorPoint.y * size_.y;
	float bottom = 1.0f - anchorPoint.y * size_.y;

	vertexData_[0].position = { left,bottom + height,0.0f,1.0f };  // 左下
	vertexData_[1].position = { left,top,0.0f,1.0f };     // 左上
	vertexData_[2].position = { right + width,bottom + height,0.0f,1.0f }; // 右下
	vertexData_[3].position = { right + width,top,0.0f,1.0f };    // 右上
	
	vertexData_[0].texcoord = { 0.f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.f,1.f };
	vertexData_[3].texcoord = { 1.f,0.0f };

	for (uint32_t i = 0; i < 4; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}
}

void Sprite::CreateIndexResource() {
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

	// 裏面（三角形2枚、巻き方向を逆に）
	/*indexData_[6] = 2; indexData_[7] = 1; indexData_[8] = 0;
	indexData_[9] = 2; indexData_[10] = 3; indexData_[11] = 1;*/
}

void Sprite::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Object3dMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentityMatrix();

	materialResource_->Unmap(0, nullptr);
}

void Sprite::CreateTransformationResource() {
	//
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(Object3dTransformationMatrix));
	//
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
}

void Sprite::CreateDirectionalLightResource() {
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	//
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
}

void Sprite::CreatePSO() {

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

void Sprite::CreateRootSignature() {

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
}

void Sprite::InputLayoutSet() {

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

void Sprite::CompileShaders() {

	// Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void Sprite::BlendStateSet() {
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// CG3↓
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void Sprite::RasiterzerStateSet() {
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void Sprite::DepthStencilStateSet() {
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数LessEqual、つまり、深ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//
	depthStencilDesc_.StencilEnable = FALSE;
}