#pragma once
#include"DirectXCommon.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>
#include "externals/DirectXTex/DirectXTex.h"
#pragma comment(lib, "dxcompiler.lib")
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
#include <dxcapi.h>                  
#pragma comment(lib,"dxcompiler.lib")

class SpriteData
{
public:

	void Initialize(DirectXCommon* dxCommon);

	void SpriteDataSet();

	DirectXCommon* GetDxCommon()const { return dxCommon_; }

private:
	DirectXCommon* dxCommon_;
	HRESULT hr_;
	// コマンドリスト(まとまった命令群)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	void CreateRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	void CreatePSO();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	void InputLayoutSet();
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {}; // InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_ = {};

	void CompileShaders();
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	
	void BlendStateSet(); // BlendStateの設定
	D3D12_BLEND_DESC blendDesc_{};

	void RasiterzerStateSet();
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	void DepthStencilStateSet();
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};