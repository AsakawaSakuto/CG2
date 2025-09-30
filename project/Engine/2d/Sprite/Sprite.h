#pragma once
#include "DirectXCommon.h"
#include "Object3dVertexData.h"
#include "Object3dMaterial.h"
#include "Object3dTransformationMatrix.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "CameraForGPU.h"
#include "SpotLight.h"
#include "PointLight.h"

#include <cmath>
#include <numbers>

class Sprite
{
public:

	void Initialize(DirectXCommon* dxCommon, const std::string& fileName, Vector2 size);

	void Update();

	void Draw();

	void DrawImGui(const char* objectName);

	const Vector2& GetPosition()const { return position_; }

	void SetPosition(const Vector2& position) { position_ = position; }

	void SetScale(const Vector2& s) { transform_.scale = { s.x,s.y,1.0f }; }

	void SetSize(const Vector2& size) { size_ = size; }

	void SetColor(const Vector4& Color) { materialData_->color = Color; }

	Vector2& GetPosition() { return position_; }

	Vector2& GetSize() { return size_; }

	Vector4& GetColor() { return materialData_->color; }

	void SetTexture(const std::string& textureName);

	Vector2 GetUvTranslate_() { return uvTranslate_; }

	void SetUvTranslate(Vector2 uvT) { uvTranslate_ = uvT; }
private:
	DirectXCommon* dxCommon_ = nullptr;
	HRESULT hr_;

	uint32_t textureIndex_ = 0;

	Vector2 uvScale_ = { 1.0f,1.0f };
	Vector2 uvTranslate_ = { 0.0f,0.0f };
	float uvRotate_ = 0.0f;

	std::string textureName_;

	//----------------------------------------------//

	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// コマンドリスト(まとまった命令群)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	// リソースデータ
	Object3dVertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Object3dMaterial* materialData_ = nullptr;
	Object3dTransformationMatrix* transformationData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;
	PointLight* pointLightData_ = nullptr;
	SpotLight* spotLightData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	Transform transform_ = {};
	Vector2 position_ = {};
	Vector2 size_ = {};
	Vector2 anchorPoint = { 0.5f,0.5f };

	//----------------------------------------------//

	void CreateRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	void CreatePSO();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	void InputLayoutSet();
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_ = {};

	void CompileShaders();
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	void BlendStateSet();
	D3D12_BLEND_DESC blendDesc_{};

	void RasiterzerStateSet();
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	void DepthStencilStateSet();
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void CreateDirectionalLightResource();
};