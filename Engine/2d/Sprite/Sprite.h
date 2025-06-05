#pragma once
#include "SpriteData.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include"MatrixFunction.h"

class SpirteData;

class Sprite
{
public:

	void Initialize(SpriteData* spriteData, const std::string& fileName);

	void Update();

	void Draw();

	const Vector2& GetPosition()const { return position_; }

	void SetPosition(const Vector2& position) { position_ = position; }

	void SetSize(const Vector2& size) { size_ = size; }

	void SetColor(const Vector4& Color) { materialData_->color = Color; }

private:
	SpriteData* spriteData_;

	uint32_t textureIndex_ = 0;

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
	// リソースデータ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	Transform transform_;
	Vector2 position_;
	Vector2 size_;
	Vector2 anchorPoint = { 0.5f,0.5f };

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void CreateDirectionalLightResource();
};