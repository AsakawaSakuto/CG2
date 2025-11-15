#pragma once
#include "../Model/ModelDataStruct.h"

#include "DirectXCommon.h"
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

	void Initialize(DirectXCommon* dxCommon, const std::string& fileName, Vector2 position = { 0.0f,0.0f }, Vector2 scale = { 1.0f,1.0f });

	void Update();

	void Draw();

	void DrawImGui(const char* objectName);

	void SetTransform(const Transform2D& transform) { transform2D_ = transform; }

	void SetUVTransform(const Transform2D& uvTransform) { uvTransform_ = uvTransform; }

	void SetPosition(const Vector2& position) { transform2D_.translate = position; }

	void SetScale(const Vector2& scale) { transform2D_.scale = scale; }

	void SetRotate(float rotate) { transform2D_.rotate = rotate; }

	void SetColor(const Vector4& Color) { materialData_->color = Color; }

	Vector2& GetSize() { return size_; }

	void SetTexture(const std::string& textureName);

private:
	DirectXCommon* dxCommon_ = nullptr;
	HRESULT hr_;

	uint32_t textureIndex_ = 0;

	Transform2D transform2D_ = {};
	Transform2D uvTransform_ = {};

	Vector2 size_ = {};
	Vector2 anchorPoint = { 0.5f,0.5f };

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
	ModelVertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	ModelMaterial* materialData_ = nullptr;
	ModelTransformationMatrix* transformationData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;
	PointLight* pointLightData_ = nullptr;
	SpotLight* spotLightData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	//----------------------------------------------//

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void CreateDirectionalLightResource();
};