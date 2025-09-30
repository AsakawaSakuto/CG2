#pragma once
#include "SphereData.h"

#include "Object3dVertexData.h"
#include "Object3dMaterial.h"
#include "Object3dTransformationMatrix.h"

#include "Transform.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "Camera.h"
#include "CameraForGPU.h"

#include <cmath>
#include <numbers>

class Sphere
{
public:

	void Initialize(SphereData* sphereData, const std::string& fileName);

	void Update(Camera& useCamera);

	void Draw();

	void DrawImGui(const char* objectName);

private:
	SphereData* sphereData_ = nullptr;

	uint32_t textureIndex_ = 0;

	Vector2 uvScale_ = { 1.0f,1.0f };
	Vector2 uvTranslate_ = { 0.0f,0.0f };
	float uvRotate_ = 0.0f;

	Vector3 direction_ = {};

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

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	std::vector<Object3dVertexData> vertices_;
	std::vector<uint32_t> indices_;
	const int subdivision_ = 32; // 分割数（細かさ）

	Transform transform_;
	Vector3 position_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void CreateDirectionalLightResource();

	// 球体メッシュをインデックス付きで作成する関数
	void CreateIndexedSphereMesh(std::vector<Object3dVertexData>& vertices, std::vector<uint32_t>& indices, int subdivision);
};