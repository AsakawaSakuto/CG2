#pragma once
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

#include <cmath>
#include <numbers>
#include <unordered_map>
#include <memory>

#include"DirectXCommon.h"

#include "../Model/ModelDataStruct.h"

#include"Transform.h"
#include"DirectionalLight.h"
#include"PointLight.h"
#include"SpotLight.h"

#include"TextureManager.h"
#include"Camera.h"
#include"CameraForGPU.h"
#include"MatrixFunction.h"

class Model {
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon, const std::string& ModelPath);

	// デストラクタ
	~Model();

	// 更新
	void Update();

	// 描画
	void Draw(Camera& useCamera, const Transform& transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });

	// ImGuiの描画
	void DrawImGui(const char* objectName);

	// 座標変換情報を設定
	void SetTransform(const Transform& transform) { transform_ = transform; }

	// 座標を設定
	void SetTranslate(Vector3 position) { transform_.translate = position; }

    // 角度を設定
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }

	// サイズを設定
	void SetScale(Vector3 scale) { transform_.scale = scale; }

    // 半透明描画を有効/無効
    void SetTransparent(bool enable) { useTransparent_ = enable; }

	// 色を設定 Vector4
	void SetColor(Vector4 color) { materialData_->color = color; }

	// 色を設定 Vector3
	void SetColor(Vector3 color) { materialData_->color.x = color.x; materialData_->color.y = color.y; materialData_->color.z = color.z; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 使用するテクスチャを変更
	void SetTexture(const std::string& textureName);

	// 描画モードを変更 trueで通常 falseでワイヤーフレーム
	void SetDrawMode(bool drawMode) { useWireFrame = drawMode; }

	// uv座標変換情報を設定
	void SetUvTransform(const UvTransform& uvT) { uvTransform_ = uvT; }

	// uv座標を設定
	void SetUvTranslate(Vector2 uvT) { uvTransform_.translate = uvT; }

	// uv角度を設定
	void SetUvRotate(float uvR) { uvTransform_.rotate = uvR; }

	// uvサイズを設定
	void SetUvScale(Vector2 uvS) { uvTransform_.scale = uvS; }

	// フラスタムカリング関連
	void SetBoundingRadius(float radius) { boundingRadius_ = radius; }
	void SetDrawFrustumCulling(bool enable) { useDrawFrustumCulling_ = enable; }
	void SetUpdateFrustumCulling(bool enable) { useUpdateFrustumCulling_ = enable; }

	void PlayAnimation() { if (!useAnimationTimer_) { useAnimationTimer_ = true; } }
	void StopAnimation() { if (useAnimationTimer_) { useAnimationTimer_ = false; } }

	//float GetBoundingRadius() const { return boundingRadius_; }
	//bool GetDrawFrustumCulling() const { return useDrawFrustumCulling_; }
	//bool GetUpdateFrustumCulling() const { return useUpdateFrustumCulling_; }

private:

	// モデルジオメトリ共有キャッシュ
	struct GeometryCache {
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource; // 共有頂点リソース
		D3D12_INDEX_BUFFER_VIEW indexBufferView;             // 共有VBV
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource; // 共有頂点リソース
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;             // 共有VBV
		ModelData modelData;                           // 共有モデルデータ
		Animation animationData;                               // 共有アニメーションデータ
		std::string textureName;                               // 使用テクスチャ名
		uint32_t textureIndex = 0;                             // テクスチャインデックス
		float boundingRadius = 1.0f;                           // バウンディング半径
	};
	static std::unordered_map<std::string, std::shared_ptr<GeometryCache>> s_geometryCache_;

	enum class AnimationType {
		NONE,
		NORMAL,
		BONE,
	};

private:
	AnimationType animationType_ = AnimationType::NONE;

	// モデルのパス
	std::string modelPath_;

	// モデルデータ
	ModelData modelData_;

	// ワールド変換行列
	Matrix4x4 worldMatrix;
	
	// ワールド変換行列情報
	Transform transform_;     // SRT情報
	UvTransform uvTransform_; // UV変換情報

	// テクスチャ関連
	std::string textureName_;   // 使用テクスチャファイルパス
	uint32_t textureIndex_ = 0; // 使用テクスチャインデックス

	// 描画切り替えフラグ
	bool useWireFrame = false;    // ワイヤーフレーム描画、有効/無効
	bool useTransparent_ = false; // 半透明描画、有効/無効

	// フラスタムカリング関連(カメラ外か否かの判定)
	Camera camera_; 				      // カメラ情報
	float boundingRadius_ = 0.5f;         // オブジェクトのバウンディング半径
	bool useDrawFrustumCulling_ = false;   // カメラ外の描画、有効/無効
	bool useUpdateFrustumCulling_ = false; // カメラ外の更新、有効/無効

	// アニメーション関連
	bool useAnimationTimer_ = false; // アニメーション使用フラグ
	float animationTime_ = 0.0f;     // アニメーション再生時間
	Animation animationData_;        // アニメーションデータ
	Skeleton skeleton_;              // スケルトンデータ
	SkinCluster skinCluster_;        // スキンクラスター
	uint32_t skinClusterSrvIndex_ = 0; // SkinCluster用のSRVインデックス（動的割り当て）

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