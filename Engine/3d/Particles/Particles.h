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
#include "ParticleVertexData.h"
#include "Material.h"
#include "ParticleMaterial.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "Camera.h"
#include "ParticleData.h"
#include "ParticleForGPU.h"
#include "Emitter.h"
#include "BlendMode.h"

#include <random>
#include <numbers>
#include <memory>
#pragma endregion

class Object3d;

using std::unique_ptr;
using std::make_unique;

// パーティクルクラス
class Particles
{
public:

	// 初期化
	void Initialize(DirectXCommon* dxCommon, const std::string& TextureName);

	// 更新 パーティクルの動きや行列更新
	void Update(Camera& useCamera);

	// 描画処理
	void Draw();

	// ImGuiでのパラメータ表示
	void DrawImGui(const char* objectName);

	// テクスチャの設定（描画用）
	void SetTexture(const std::string& textureName);

	void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

	void SetEmitter(const Vector3& translete) { emitter_.transform.translate = translete; }
private:
	unique_ptr<Object3d> drawEmitter_ = make_unique<Object3d>();
	// パーティクルが動くかどうかのフラグ
	bool isMove_ = true;
	bool useBillboard_ = false;
	// 1フレームの固定デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;

	Matrix4x4 worldViewProjectionMatrix_ = {};
	Matrix4x4 worldMatrix_ = {};

	// パーティクルの数
	uint32_t numInstance_ = 0;
	uint32_t kMaxNumInstance_ = 100;
	
	ParticleForGPU* instanceData_ = nullptr;

	ParticleData MakeNewParticle(std::mt19937& rand, const Emitter& emitter);

	Matrix4x4 billboardMatrix = {};

	// パーティクル本体のデータ
	std::list<ParticleData> particles_;

	// 使用するテクスチャ名
	std::string textureName_;

	BlendMode blendMode_;

	std::list<ParticleData> Emit(const Emitter& emitter, std::mt19937& rand);

	Emitter emitter_;

	int spawnCount_;

	/*----------作成から描画までの様々な変数や関数----------*/

	// DirectX共通機能へのポインタ
	DirectXCommon* dxCommon_ = nullptr;

	// GPUデバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	// GPUコマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	// HRESULTの戻り値格納用
	HRESULT hr_;

	// 描画に使用するテクスチャのSRVインデックス
	uint32_t textureIndex_ = 0;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	// StructuredBuffer用（インスタンシング）バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;

	// StructuredBuffer用 SRV のGPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;           // 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;            // インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;         // マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_; // ライト
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;   // 行列用配列

	// リソースデータ
	ParticleVertexData* vertexData_ = nullptr;                 // 頂点
	uint32_t* indexData_ = nullptr;                    // インデックス
	ParticleMaterial* materialData_ = nullptr;                 // マテリアル
	DirectionalLight* directionalLightData_ = nullptr; // ライト

	// リソース作成系の内部関数
	void CreateVertexResource();           // 頂点
	void CreateIndexResource();            // インデックス
	void CreateMaterialResource();         // マテリアル
	void CreateDirectionalLightResource(); // ライト
	void CreateTransformationResource();   // 行列用配列

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	// PSO作成に使う設定構造体
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_;

	// 作成済みPSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNone_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNormal_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateAdd_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSubtract_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateMultily_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateScreen_;

	// 頂点入力レイアウト情報
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	// シェーダバイナリ（コンパイル済み）
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	// 各種ステート設定
	D3D12_BLEND_DESC blendDescNone_;
	D3D12_BLEND_DESC blendDescNormal_;
	D3D12_BLEND_DESC blendDescAdd_;
	D3D12_BLEND_DESC blendDescSubtract_;
	D3D12_BLEND_DESC blendDescMultily_;
	D3D12_BLEND_DESC blendDescScreen_;

	D3D12_RASTERIZER_DESC rasterizerDesc_;       // ラスタライザステート
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;  // 深度ステンシルステート

	// PSO・シグネチャ構築関連の関数
	void CreatePSO();
	void CreateRootSignature();
	void InputLayoutSet();
	void CompileShaders();
	void BlendStateSet();
	void RasiterzerStateSet();
	void DepthStencilStateSet();

	// 乱数生成器（パーティクル初期値用）
	std::mt19937 rand;

public:

	// コンストラクタで乱数エンジン初期化
	Particles() {
		std::random_device seedGenerator;
		rand = std::mt19937(seedGenerator());
	}
};