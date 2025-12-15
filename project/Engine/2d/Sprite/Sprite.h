#pragma once
#include "3d/Model/ModelDataStruct.h"

#include "Core/DirectXCommon/DirectXCommon.h"
#include "Utility/Transform/Transform.h"
#include "Core/TextureManager/TextureManager.h"
#include "Math/MatrixFunction/MatrixFunction.h"
#include "Camera/CameraForGPU.h"
#include "Utility/Light/DirectionalLight.h"
#include "Utility/Light/SpotLight.h"
#include "Utility/Light/PointLight.h"

#include <cmath>
#include <numbers>

/// <summary>
/// 2Dスプライトクラス
/// </summary>
class Sprite
{
public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Sprite();

	/// <summary>
	/// Spriteの初期化
	/// </summary>
	/// <param name="dxCommon">dxCommonを渡す</param>
	/// <param name="fileName">"resources/image/" 以降のPathを渡す</param>
	/// <param name="position">Vector2型でスクリーン座標を設定(任意)</param>
	/// <param name="scale">Vector2型でScaleを設定(任意)</param>
	void Initialize(DirectXCommon* dxCommon, const std::string& fileName, Vector2 position = { 0.0f,0.0f }, Vector2 scale = { 1.0f,1.0f });

	/// <summary>
	/// 行列計算などの更新
	/// </summary>
	void Update();

	/// <summary>
	/// Spriteの描画
	/// </summary>
	void Draw();

	/// <summary>
	/// TransformやMaterialのImGui描画
	/// </summary>
	/// <param name="objectName">適切な名前を入力</param>
	void DrawImGui(const char* objectName);

	/// <summary>
	/// Transform2Dを設定
	/// </summary>
	/// <param name="transform">更新したいTransform2Dを渡す</param>
	void SetTransform(const Transform2D& transform) { transform2D_ = transform; }

	/// <summary>
	/// Vector2型でTranslateを設定
	/// </summary>
	/// <param name="position">更新したいTranslateを渡す</param>
	void SetPosition(const Vector2& position) { transform2D_.translate = position; }

	/// <summary>
	/// Vector2型でScaleを設定
	/// </summary>
	/// <param name="scale">更新したいScaleを渡す</param>
	void SetScale(const Vector2& scale) { transform2D_.scale = scale; }

	/// <summary>
	/// floatでRotateを設定
	/// </summary>
	/// <param name="rotate">更新したいRotateを渡す</param>
	void SetRotate(float rotate) { transform2D_.rotate = rotate; }

	/// <summary>
	/// Colorを設定 Alpha含む
	/// </summary>
	/// <param name="Color">更新したいColorを渡す</param>
	void SetColor(const Vector4& Color) { materialData_->color = Color; }

	/// <summary>
	/// UV用のTransform2Dを設定
	/// </summary>
	/// <param name="transform">更新したいUV用のTransform2Dを渡す</param>
	void SetUVTransform(const Transform2D& uvTransform) { uvTransform_ = uvTransform; }

	/// <summary>
	/// Vector2型で画像サイズを取得
	/// </summary>
	/// <returns>Vector2型で画像サイズを取得</returns>
	Vector2& GetSize() { return size_; }

	/// <summary>
	/// 使用してるTextureを変更
	/// </summary>
	/// <param name="textureName">resources/image/" 以降のPathを渡す</param>
	void SetTexture(const std::string& textureName);

	/// <summary>
	/// アンカーポイントを設定
	/// </summary>
	/// <param name="anchor">アンカーポイント（0.0～1.0）</param>
	void SetAnchorPoint(const Vector2& anchor);

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