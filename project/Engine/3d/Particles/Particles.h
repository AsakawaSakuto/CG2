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

#include "ParticleVertexData.h"
#include "ParticleMaterial.h"
#include "Transform.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "Camera.h"
#include "ParticleDataCS.h"
#include "BlendMode.h"
#include "PreView.h"
#include "PreFrame.h"
#include "EmitterState.h"

#include "ParticleDescriptorAllocator.h"

#include <random>
#include <numbers>
#include <memory>

#include"Json/EmitterStateLoder.h"

#pragma endregion


/// <summary>
/// パーティクルクラス
/// </summary>
class Particles
{
public:

	/// <summary>
	/// パーティクルの初期化
	/// </summary>
	/// <param name="dxCommon">dxCommonを渡す</param>
	/// <param name="TextureName">使用するTextureのPathを入れる「デフォルトでCircle」</param>
	/// <param name="maxParticle">整数値＊512粒のパーティクルを扱えるようになる</param>
	void Initialize(DirectXCommon* dxCommon, const uint32_t maxParticle = 1, const std::string& TextureName = "circle");

	/// <summary>
	/// パーティクルの更新
	/// </summary>
	void Update();

	/// <summary>
	/// パーティクルの描画
	/// </summary>
	/// <param name="useCamera">シーンで使用してるCameraを渡す</param>
	void Draw(Camera& useCamera);

	/// <summary>
	/// ImGuiでパラメータを操作 「JsonFileの生成＋保存＋読み込みをできる」
	/// </summary>
	/// <param name="objectName">表示する名前を入力</param>
	void DrawImGui(const char* objectName);

	/// <summary>
	/// パーティクルに使用するTextureを変更する
	/// </summary>
	/// <param name="textureName">使用するTextureのPathを入れる</param>
	void SetTexture(const std::string& textureName);

	/// <summary>
	/// ブレンドモードを設定する
	/// </summary>
	/// <param name="blendMode">BlendMode構造体から入れる</param>
	void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

	/// <summary>
	/// Emitterの位置を設定する
	/// </summary>
	/// <param name="position">位置を入れる</param>
	void SetEmitterPosition(const Vector3& position) { emitter_.translate = position; }

	/// <summary>
	/// EmitterのOffSet値を設定する
	/// </summary>
	/// <param name="offset">設定するOffSet値</param>
	void SetOffSet(Vector3 offset) { offset_ = offset; }

	/// <summary>
	/// パーティクルの生成開始
	/// </summary>
	/// <param name="isLoop">trueならループ生成、falseなら一回だけ生成</param>
	void Play(bool isLoop = true) { 
		if (isLoop) {
			emitter_.frequencyTime = emitter_.frequency;
			emitter_.useEmitter = true;
			isPlaying_ = true;
		} else {
			emitter_.frequencyTime = emitter_.frequency;
			emitter_.useEmitter = false;
			isPlaying_ = false;
		}
	}

	/// <summary>
	/// パーティクルの生成停止
	/// </summary>
	void Stop() { 
		emitter_.useEmitter = false; 
		emitter_.emit = false;
		emitter_.frequencyTime = 0.0f;
		isPlaying_ = false;
	}

	/// <summary>
	/// パーティクルが生成中かどうかを判定する
	/// </summary>
	/// <returns>生成中の場合は true、そうでない場合は false を返す</returns>
	bool IsPlaying() const { return isPlaying_; }

	/// <summary>
	/// JsonFileからEmitterの値を読み込む
	/// </summary>
	/// <param name="filePath">Resources->Data->Particle の中にあるJsonFileのPathを入れる「.jsonは不要」</param>
	/// <param name="Pathの処理">"resources/Data/Particle/" + (filePath + ".json")</param>
	void LoadJson(const std::string& filePath) {
		jsonFilePath_ = "resources/Data/Particle/" + (filePath + ".json");
		emitter_ = EmitterStateLoader::Load(jsonFilePath_);
		
		// JSONから読み込んだテクスチャパスを適用
		if (!emitter_.texturePath.empty()) {
			std::string newTextureName = "resources/image/particle/" + emitter_.texturePath + ".png";
			if (textureName_ != newTextureName) {
				textureName_ = newTextureName;
				// テクスチャファイル読み込み
				TextureManager::GetInstance()->LoadTexture(textureName_);
				// 読み込んだテクスチャの番号を取得
				textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
			}
		}
	};
private:

	void ExecuteInitialization();

	void ResetAllParticles();

	Camera camera_;

	std::string jsonFilePath_;
	std::string loadToSaveName_ = "temp";
	std::string generateName = "emitterData";

	// パーティクルの再生状態
	bool isPlaying_ = false;

	// ParticleのSRV番号
	uint32_t idxSrvParticles_;
	uint32_t idxUavParticles_;
	uint32_t idxUavFreeListIndex_;
	uint32_t idxUavFreeList_;

	// 描画可能な最大パーティクル数 // 1048576*2048 // 16384*32
	uint32_t kMaxParticles_;
	
	// Dispatchを実行する回数
	uint32_t kDispatchCount;
	
	// GPU側に送るインスタンス情報
	ParticleDataCS* particleDataCS_ = nullptr;

	// 使用するテクスチャの名前
	std::string textureName_;

	// 現在のブレンドモード
	BlendMode blendMode_;
	
	// 1フレームあたりの固定デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;

	float totalTime_ = 0.0f;

	bool isMove_ = false;

	float emitterSpeed_ = 0.0f;

	Vector3 offset_ = {};
	/*-----------GPUパーティクルに使用してる変数-----------*/

	Microsoft::WRL::ComPtr<ID3D12Resource> particleBufferResource_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> csRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> csInitializePipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> csEmitterPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> csUpdatePipelineState_;

	Microsoft::WRL::ComPtr<ID3D12Resource> freeListIndexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> freeListResource_;

	void CreateParticleResource();
	void UpdateParticle();

	Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_;
	PerView* perViewData_ = nullptr;
	void CreatePerViewResource();

	Microsoft::WRL::ComPtr<ID3D12Resource> perFrameResource_;
	uint32_t frameIndex = 0;
	void CreatePerFrameResource();
	void UpdatePerFrame();

	Microsoft::WRL::ComPtr<ID3D12Resource> emitterResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> emitterRangeResource_;
	EmitterState emitter_ = {};
	void CreateEmitterResource();
	void UpdateEmitter();

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

	// リソースデータ
	uint32_t* indexData_ = nullptr;                    // インデックス
	ParticleVertexData* vertexData_ = nullptr;         // 頂点
	ParticleMaterial* materialData_ = nullptr;         // マテリアル

	// リソース作成系の内部関数
	void CreateVertexResource();           // 項点
	void CreateIndexResource();            // インデックス
	void CreateMaterialResource();         // マテリアル

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

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
};