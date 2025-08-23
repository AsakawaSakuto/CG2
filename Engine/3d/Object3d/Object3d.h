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

#include"DirectXCommon.h"

#include"Object3dModelData.h"
#include"Object3dVertexData.h"
#include"Object3dMaterial.h"
#include"Object3dTransformationMatrix.h"

#include"Transform.h"
#include"DirectionalLight.h"
#include"PointLight.h"
#include"SpotLight.h"

#include"TextureManager.h"
#include"Camera.h"
#include"CameraForGPU.h"
#include"MatrixFunction.h"

class Object3d {
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon, const std::string& ModelPath);

	// 更新
	void Update(Camera& useCamera);

	// 描画
	void Draw();

	// ImGuiの描画
	void DrawImGui(const char* objectName);

	// 座標を設定
	void SetTranslate(Vector3 position) { transform_.translate = position; }

    // 角度を設定
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }

	// サイズを設定
	void SetScale(Vector3 scale) { transform_.scale = scale; }

	// 色を設定
	void SetColor(Vector4 color) { materialData_->color = color; }

	// 座標を取得（参照）
	Vector3& GetTranslate() { return transform_.translate; }

	Vector3& GetRotate() { return transform_.rotate; }
	
	Vector3& GetScale() { return transform_.scale; }

	// 色を取得（参照）
	Vector4& GetColor() { return materialData_->color; }

	Vector3 GetWorldPosition();

	// 使用するテクスチャを変更
	void SetTexture(const std::string& textureName);

	// 描画モードを変更 trueで通常 falseでワイヤーフレーム
	void SetDrawMode(bool drawMode) { drawMode_ = drawMode; }

	Matrix4x4& GetWorldMatrix() { return worldMatrix; }

	void SetUvTranslate(Vector2 uvT) { uvTranslate_ = uvT; }

	void SetUvScale(Vector2 uvS) { uvScale_ = uvS; }

	void SetUvRotate(float uvR) { uvRotate_ = uvR; }

	void SetUseLight(bool use) { materialData_->enableLighting = use; }

	void SetSRT(Vector3 s, Vector3 r, Vector3 t) { transform_.scale = s; transform_.rotate = r; transform_.translate = t; }
private:
	Matrix4x4 worldMatrix;

	// ワールド変換行列情報
	Transform transform_;

	// 頂点・インデックス・マテリアル情報など
	Object3dModelData modelData_;

	// 使用中のテクスチャ名と、そのインデックス
	std::string textureName_;
	uint32_t textureIndex_ = 0;

	// 描画モード切り替え（ワイヤーフレームなど）
	bool drawMode_ = true;

	Vector2 uvScale_ = { 1.0f,1.0f };
	Vector2 uvTranslate_ = { 0.0f,0.0f };
	float uvRotate_ = 0.0f;

	//-----------------------------------------------------------//

	// DirectX共通機能へのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;                       // GPUデバイス
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;     // GPUコマンドリスト

	// 各種GPUリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;             // 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;           // マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;     // 行列リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;   // ライトリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;

	// 各種リソースのCPU側ポインタ
	Object3dVertexData* vertexData_ = nullptr;                   // 頂点データ
	Object3dMaterial* materialData_ = nullptr;                   // マテリアルデータ
	Object3dTransformationMatrix* transformationData_ = nullptr; // 行列データ
	DirectionalLight* directionalLightData_ = nullptr;           // ライトデータ
	PointLight* pointLightData_ = nullptr;
	SpotLight* spotLightData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;

	Vector3 direction_ = {};

	// 頂点バッファ・インデックスバッファのビュー設定
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	// 各種リソースの作成関数
	void CreateVertexResource();             // 頂点バッファ生成
	void CreateMaterialResource();           // マテリアルバッファ生成
	void CreateTransformationResource();     // 行列バッファ生成
	void CreateDirectionalLightResource();   // ライトバッファ生成
	void CreateCameraResource();
	void CreatePointLightResource();
	void CreateSpotLightResource();

	HRESULT hr_;  // エラー確認用変数

	// RootSignature（シェーダとの接続点）の作成
	void CreateRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	// Graphics Pipeline State Object（描画設定）の作成
	void CreatePSO();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSolid_;      // 通常描画用
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateWireframe_;  // ワイヤーフレーム描画用

	// 頂点レイアウトの設定
	void InputLayoutSet();
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};  // 頂点要素のレイアウト（位置・UV・法線など）
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_ = {};        // 頂点レイアウト全体

	// シェーダの読み込みとコンパイル
	void CompileShaders();
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	// ブレンドステート（透明処理など）の設定
	void BlendStateSet();
	D3D12_BLEND_DESC blendDesc_{};

	// ラスタライザステート（描画方法：塗りつぶし or ワイヤーフレームなど）
	void RasiterzerStateSet();
	D3D12_RASTERIZER_DESC rasterizerDescSolid_{};
	D3D12_RASTERIZER_DESC rasterizerDescWireFrame_{};

	// 深度ステンシル（奥行き）テストの設定
	void DepthStencilStateSet();
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};