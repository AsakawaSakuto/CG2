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
#include "ParticleDataCS.h"
#include "ParticleForGPU.h"
#include "Emitter.h"
#include "BlendMode.h"
#include "PreView.h"
#include "PreFrame.h"
#include "EmitterSpfere.h"

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

	// ブレンドモードを変更
	void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

	// エミッタの位置（Translate）をセット
	void SetEmitter(const Vector3& translete) { emitter_.translate = translete; }
private:
	
	unique_ptr<Object3d> emitterModel_ = make_unique<Object3d>(); // エミッターの可視化用3Dオブジェクト

	std::list<ParticleDataCS> particles_; // パーティクルの本体情報

	uint32_t particleSrvIndex_ = 64;
	uint32_t numInstance_ = 0;               // 現在描画するインスタンスの数
	const uint32_t kMaxParticles_ = 16;      // 描画可能な最大パーティクル数
	ParticleDataCS* particleDataCS_ = nullptr; // GPU側に送るインスタンス情報

	std::string textureName_; // 使用するテクスチャの名前
	BlendMode blendMode_;     // 現在のブレンドモード
	
	bool isMove_ = true;         // パーティクルが動くかどうかのフラグ
	bool useBillboard_ = true;   // ビルボードを使用するかどうかのフラグ
	bool isDrawEmitter_ = false; // エミッターを可視化するかのフラグ

	const float kDeltaTime_ = 1.0f / 60.0f; // 1フレームあたりの固定デルタタイム
	float totalTime_ = 0.0f;

	// エミッターの設定に従って複数のパーティクルを生成し、リストとして返す関数
	std::list<ParticleDataCS> Emit(const Emitter& emitter, std::mt19937& rand);

	// 1つのパーティクルを生成し、初期化された ParticleData を返す
	ParticleDataCS MakeNewParticle(std::mt19937& rand, const Emitter& emitter);

	/*-----------GPUパーティクルに使用してる変数-----------*/

	Microsoft::WRL::ComPtr<ID3D12Resource> particleBufferResource_;       // GPUに渡すStructuredBuffer用
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBufferUploadResource_; // 初期化用UploadHeap

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> computeAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeQueue_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> computeList_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> csRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> csPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> csInitializePipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> csUpdatePipelineState_;

	// パーティクル配列
	std::vector<ParticleDataCS> particlesCS_;
	void CreateParticleResource();
	void UpdateGPUParticle();

	Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_;
	PerView* perViewData_ = nullptr;
	void CreatePerViewResource();

	Microsoft::WRL::ComPtr<ID3D12Resource> perFrameResource_;
	void CreatePerFrameResource();

	Microsoft::WRL::ComPtr<ID3D12Resource> emitterResource_;
	EmitterSphere emitter_ = {};
	void CreateEmitterResource();

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

	// StructuredBuffer用 SRV のGPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;           // 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;            // インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;         // マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_; // ライト
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;   // 行列用配列

	// リソースデータ
	ParticleVertexData* vertexData_ = nullptr;         // 頂点
	uint32_t* indexData_ = nullptr;                    // インデックス
	ParticleMaterial* materialData_ = nullptr;         // マテリアル
	DirectionalLight* directionalLightData_ = nullptr; // ライト

	// リソース作成系の内部関数
	void CreateVertexResource();           // 頂点
	void CreateIndexResource();            // インデックス
	void CreateMaterialResource();         // マテリアル
	void CreateDirectionalLightResource(); // ライト
	void CreateTransformationResource();   // 行列用配列

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> computeRootSignature_;

	// PSO作成に使う設定構造体
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_;

	// 作成済みPSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> useGraphicsPipelineState_;
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