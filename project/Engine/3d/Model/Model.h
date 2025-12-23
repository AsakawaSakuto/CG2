#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>
#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")

#include "DirectXTex.h"

#include <cmath>
#include <numbers>
#include <unordered_map>
#include <memory>

#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/HeapManager/DescriptorAllocator.h"
#include "Core/TextureManager/TextureManager.h"

#include "Data/ModelDataStruct.h"
#include "Data/Animation/AnimationStruct.h"
#include "Data/Animation/Function/AnimationFunction.h"

#include "Utility/Transform/Transform.h"
#include "Utility/Light/DirectionalLight.h"
#include "Utility/Light/PointLight.h"
#include "Utility/Light/SpotLight.h"

#include "Camera/Camera.h"
#include "Camera/CameraForGPU.h"

#include "Math/MatrixFunction/MatrixFunction.h"

class DirectXCommon;

/// <summary>
/// 3Dモデルクラス
/// </summary>
class Model {
public:
	
	/// <summary>
	/// 静的リソースのクリーンアップ（アプリケーション終了時に呼び出す）
    /// </summary>
	static void Finalize();

	/// <summary>
	/// Modelの初期化
	/// </summary>
	/// <param name="dxCommon">dxCommonを渡す</param>
	/// <param name="ModelPath">"resources/model/" 以降のPathを渡す</param>
	void Initialize(const std::string& ModelPath);

	// デストラクタ
	~Model();

	/// <summary>
	/// Modelの描画
	/// </summary>
	/// <param name="useCamera">シーンで使用してるCameraを渡す</param>
	/// <param name="transform">Transformを渡してSRTの更新をする(任意)</param>
	void Draw(Camera& useCamera, const Transform& transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });

	/// <summary>
	/// MaterialやLightのImGui描画
	/// </summary>
	/// <param name="objectName">適切な名前を入力</param>
	void DrawImGui(const char* objectName);

	/// <summary>
	/// Transformを設定
	/// </summary>
	/// <param name="transform">更新したいTransformを渡す</param>
	void SetTransform(const Transform& transform) { transform_ = transform; }

	/// <summary>
	/// Translateを設定
	/// </summary>
	/// <param name="position">更新したいTranslateを渡す</param>
	void SetTranslate(Vector3 position) { transform_.translate = position; }

	/// <summary>
	/// Rotateを設定
	/// </summary>
	/// <param name="rotate">更新したいRotateを渡す</param>
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }

	/// <summary>
	/// Scaleを設定
	/// </summary>
	/// <param name="scale">更新したいScaleを渡す</param>
	void SetScale(Vector3 scale) { transform_.scale = scale; }

    /// <summary>
    /// 半透明描画を有効/無効
    /// </summary>
    /// <param name="enable">ture/false</param>
    void SetTransparent(bool enable) { useTransparent_ = enable; }

	/// <summary>
	/// Colorを設定 Alpha含む
	/// </summary>
	/// <param name="color">更新したいColorを渡す</param>
	void SetColor4(Vector4 color) { materialData_->color = color; }

	/// <summary>
	/// Colorを設定
	/// </summary>
	/// <param name="color">更新したいColorを渡す</param>
	void SetColor3(Vector3 color) { materialData_->color.x = color.x; materialData_->color.y = color.y; materialData_->color.z = color.z; }

	/// <summary>
	/// 使用してるTextureを変更
	/// </summary>
	/// <param name="textureName">Pathカット無し、全部入力してね</param>
	void SetTexture(const std::string& textureName);

	/// <summary>
	/// 描画モードを変更 trueで通常 falseでワイヤーフレーム
	/// </summary>
	/// <param name="drawMode">true/false</param>
	void SetDrawMode(bool drawMode) { useWireFrame = drawMode; }

	/// <summary>
	/// UVのTransformを設定
	/// </summary>
	/// <param name="uvT">更新したいUvTransformを渡す</param>
	void SetUvTransform(const Transform2D& uvTransform) { uvTransform_ = uvTransform; }

	/// <summary>
	/// UVのTranslateを設定
	/// </summary>
	/// <param name="uvT">更新したいUvTranslateを渡す</param>
	void SetUvTranslate(Vector2 uvTranslate) { uvTransform_.translate = uvTranslate; }

	/// <summary>
	/// UVのRotateを設定
	/// </summary>
	/// <param name="uvR">更新したいUvRotateを渡す</param>
	void SetUvRotate(float uvRotate) { uvTransform_.rotate = uvRotate; }

	/// <summary>
	/// UVのScaleを設定
	/// </summary>
	/// <param name="uvS">更新したいUvScaleを渡す</param>
	void SetUvScale(Vector2 uvScale) { uvTransform_.scale = uvScale; }

	// フラスタムカリング関連
	//void SetBoundingRadius(float radius) { boundingRadius_ = radius; }

	/// <summary>
	/// trueでカメラ外の描画を行わない、falseで描画する
	/// </summary>
	/// <param name="enable">true/false</param>
	void SetDrawFrustumCulling(bool enable) { useDrawFrustumCulling_ = enable; }

	/// <summary>
	/// trueでカメラ外の更新を行わない、falseで更新する
	/// </summary>
	/// <param name="enable">true/false</param>
	void SetUpdateFrustumCulling(bool enable) { useUpdateFrustumCulling_ = enable; }

	/// <summary>
	/// カメラ外か内かを取得
	/// </summary>
	/// <returns>true カメラ外 / false カメラ内</returns>
	bool GetIsInFrustum() const { return isInFrustum_; }

	void UseLight(bool use) { materialData_->enableLighting = use; }
private:

	// モデルジオメトリ共有キャッシュ
	struct GeometryCache {
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;  // 共有頂点リソース
		D3D12_INDEX_BUFFER_VIEW indexBufferView;               // 共有VBV
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource; // 共有頂点リソース
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;             // 共有VBV
		ModelData modelData;                                   // 共有モデルデータ
		std::string textureName;                               // 使用テクスチャ名
		uint32_t textureIndex = 0;                             // テクスチャインデックス
		float boundingRadius = 1.0f;                           // バウンディング半径
		DirectXCommon* dxCommon = nullptr;                     // SRV解放用
	};
	static std::unordered_map<std::string, std::shared_ptr<GeometryCache>> s_geometryCache_;

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	/// <summary>
	/// 行列計算の更新
	/// </summary>
	void UpdateMatrix();

private:

	// モデルのパス
	std::string modelPath_;

	// モデルデータ
	ModelData modelData_;

	// ワールド変換行列
	Matrix4x4 worldMatrix;
	
	// ワールド変換行列情報
	Transform transform_;     // SRT情報
	Transform2D uvTransform_; // UV変換情報

	// テクスチャ関連
	std::string textureName_;   // 使用テクスチャファイルパス
	uint32_t textureIndex_ = 0; // 使用テクスチャインデックス

	// 描画切り替えフラグ
	bool useWireFrame = false;    // ワイヤーフレーム描画、有効/無効
	bool useTransparent_ = false; // 半透明描画、有効/無効

	// フラスタムカリング関連(カメラ外か否かの判定)
	Camera camera_; 				       // カメラ情報
	float boundingRadius_ = 0.5f;          // オブジェクトのバウンディング半径
	bool useDrawFrustumCulling_ = false;   // カメラ外の描画、有効/無効
	bool useUpdateFrustumCulling_ = false; // カメラ外の更新、有効/無効
	bool isInFrustum_ = false;             // フラスタム内か否か

	//-----------------------------------------------------------//

	// DirectX共通機能へのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;                       // GPUデバイス
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;     // GPUコマンドリスト

	// 各種GPUリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;             // 頂点リソース（共有可）
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;           // マテリアルリソース（インスタンス毎）
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;     // 行列リソース（インスタンス毎）
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;   // ライトリソース（インスタンス毎）
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;

	// 各種リソースのCPU側ポインタ
	ModelVertexData* vertexData_ = nullptr;                   // 頂点データ（初期化時のみ使用）
	uint32_t* indexData_ = nullptr;                        // インデックスデータ（初期化時のみ使用）
	ModelMaterial* materialData_ = nullptr;                   // マテリアルデータ
	ModelTransformationMatrix* transformationData_ = nullptr; // 行列データ
	DirectionalLight* directionalLightData_ = nullptr;           // ライトデータ
	PointLight* pointLightData_ = nullptr;
	SpotLight* spotLightData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;

	Vector3 direction_ = {};

	// 頂点バッファ・インデックスバッファのビュー設定
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	// 各種リソースの作成関数
	void CreateMaterialResource();         // マテリアルバッファ生成
	void CreateTransformationResource();   // 行列バッファ生成
	void CreateDirectionalLightResource(); // ライトバッファ生成
	void CreateCameraResource();
	void CreatePointLightResource();
	void CreateSpotLightResource();

	HRESULT hr_;  // エラー確認用変数
};