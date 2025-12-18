#pragma once
#pragma region Include

#include <memory>
#include <random>
#include <numbers>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <wrl/client.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "DirectXTex.h"
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector4.h"
#include "Math/Type/Matrix4x4.h"
#include "Utility/Transform/Transform.h"
#include "Utility/BlendMode/BlendMode.h"
#include "Utility/GameTimer/DeltaTime.h"
#include "Utility/FileFormat/Json/JsonManager.h"
#include "Math/MatrixFunction/MatrixFunction.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/HeapManager/DescriptorAllocator.h"
#include "Core/TextureManager/TextureManager.h"
#include "Camera/Camera.h"
#include "3d/Particle/Struct/ParticleDataStruct.h"
#include "3d/Particle/Struct/EmitterState.h"
#include "3d/Line/Line.h"

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
	/// <param name="filePath">JSONファイルパス</param>
	/// <param name="maxParticle">整数値＊512粒のパーティクルを扱えるようになる</param>
	void Initialize(const std::string& filePath = "temp", const uint32_t maxParticle = 1);

	/// <summary>
	/// デストラクタ - 確保したSRV/UAVインデックスを解放
	/// </summary>
	~Particles();

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
	/// ImGuiでパラメータを操作 「BinaryFileの生成＋保存＋読み込みをできる」
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
	/// 一回の生成で出すパーティクルの数を設定する
	/// </summary>
	/// <param name="count">出すパーティクルの数</param>
	void SetSpawnCount(uint32_t count) { emitter_.count = count; }

	/// <summary>
	/// パーティクルの生成開始
	/// </summary>
	/// <param name="isLoop">trueならループ生成、falseなら一回だけ生成</param>
	void Play(bool isLoop = true) {
		if (isLoop) {
			emitter_.frequencyTime = emitter_.frequency;
			emitter_.useEmitter = true;
			isPlaying_ = true;
		}
		else {
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
	/// パーティクルをリセット（プール再利用時用の軽量リセット） 
	/// </summary>
	void Reset() {
		Stop();
		SetEmitterPosition({ 0.0f, 0.0f, 0.0f });
		SetOffSet({ 0.0f, 0.0f, 0.0f });
		// 次回の描画でGPUバッファをリセット
		needsBufferReset_ = true;
	}

	/// <summary>
	/// JsonFileからEmitterの値を読み込む
	/// </summary>
	/// <param name="filePath">Resources->Json->Particle の中にあるJsonFileのPathを入れる（拡張子不要）</param>
	void LoadJson(const std::string& filePath);

	/// <summary>
	/// エミッター形状の可視化（内部のLine3dを使用してワイヤーフレーム表示）
	/// </summary>
	/// <param name="color">描画色（デフォルト：黄色）</param>
	void DrawEmitterShape(Line3d* line3d, const Vector4& color = { 1.0f,1.0f,0.0f,1.0f });

	void SetEmitterState(EmitterState emitter) { emitter_ = emitter; }
private:

	void ExecuteInitialization();

	void ResetAllParticles();

	// JsonManagerでの保存・読み込み用のヘルパー関数
	void SaveToJson(const std::string& filePath);
	void LoadFromJson(const std::string& filePath);
	void CreateNewJsonFile(const std::string& filePath);

	Camera camera_;

	std::unique_ptr<JsonManager> jsonManager_;
	std::string loadToSaveName_ = "temp";

	// パーティクルの再生状態
	bool isPlaying_ = false;

	// Json読み込み済みフラグ
	bool isJsonLoaded_ = false;

	// 初期化済みフラグ
	bool isInitialized_ = false;

	// バッファリセットが必要かのフラグ
	bool needsBufferReset_ = false;

	// ParticleのSRV番号
	uint32_t idxSrvParticles_;
	uint32_t idxUavParticles_;
	uint32_t idxUavFreeListIndex_;
	uint32_t idxUavFreeList_;

	// 描画可能な最大パーティクル数
	uint32_t kMaxParticles_;

	// Dispatchを実行する回数
	uint32_t kDispatchCount;

	// GPU側に送るインスタンス情報
	ParticleDataCS* particleDataCS_ = nullptr;

	// 使用するテクスチャの名前
	std::string textureName_;

	// 現在のブレンドモード
	BlendMode blendMode_;

	float totalTime_ = 0.0f;

	bool isMove_ = false;

	float emitterSpeed_ = 0.0f;

	Vector3 offset_ = {};

	// エミッター形状可視化用のLine3d（内部で保持）
	std::unique_ptr<Line3d> line3d_;

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

	EmitterState emitter_ = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> emitterResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> emitterRangeResource_;
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
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;           // 項点
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
};