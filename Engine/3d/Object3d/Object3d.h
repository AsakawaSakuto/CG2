#pragma once
#include"Object3dData.h"
#include"ModelData.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include "Camera.h"
#include"MatrixFunction.h"

class Object3d
{
public:
	
	void Initialize(Object3dData* object3dData, const std::string& file , const std::string& modelName, const std::string& fileName);

	void Update(Camera& useCamera);

	void Draw();

	void SetPostion(Vector3 position) { transform_.translate = position; }

	void SetDrawMode(bool drawMode) { drawMode_ = drawMode; }

private:

	Transform transform_;

	Object3dData* object3dData_;

	ModelData modelData_;

	uint32_t textureIndex_ = 0;

	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// コマンドリスト(まとまった命令群)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	// リソースデータ
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSolid_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateWireframe_;

	bool drawMode_;

	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void CreateDirectionalLightResource();
};