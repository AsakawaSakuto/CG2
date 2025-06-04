#include"SpriteData.h"

#include<cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

void SpriteData::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	commandList_ = dxCommon_->GetCommandList();
	CreatePSO();
}

void SpriteData::CreatePSO() {

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

void SpriteData::SpriteDataSet() {
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteData::CreateRootSignature() {

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
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

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

void SpriteData::InputLayoutSet() {

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

void SpriteData::CompileShaders() {

	// Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void SpriteData::BlendStateSet() {
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void SpriteData::RasiterzerStateSet() {
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void SpriteData::DepthStencilStateSet() {
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数LessEqual、つまり、深ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}