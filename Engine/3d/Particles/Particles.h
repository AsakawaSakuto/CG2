#pragma once
#pragma region Include
#include"DirectXCommon.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>
#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include "externals/DirectXTex/DirectXTex.h"

#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "Camera.h"

#pragma endregion

class Particles
{
public:

	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera& useCamera);

	void Draw();

	void DrawImGui(const char* objectName);

	void SetTexture(const std::string& textureName);

private:

	uint32_t num = 10;

	std::vector<Transform> transform_;

	std::string textureName_;

	DirectXCommon* dxCommon_ = nullptr;
	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	HRESULT hr_;
	// テクスチャインデックス
	uint32_t textureIndex_ = 0;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> transformationResource_;
	// リソースデータ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;
	std::vector<TransformationMatrix*> transformationData_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void CreateDirectionalLightResource();

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	D3D12_BLEND_DESC blendDesc_;
	D3D12_RASTERIZER_DESC rasterizerDesc_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;

	void CreatePSO();
	void CreateRootSignature();
	void InputLayoutSet();
	void CompileShaders();
	void BlendStateSet();
	void RasiterzerStateSet();
	void DepthStencilStateSet();
};