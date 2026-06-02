#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include <d3d12.h>
#include <wrl/client.h>

#include "3d/Ring/Ring.h"
#include "2d/Sprite/SpriteDataStruct.h"
#include "Math/Type/Vector4.h"
#include "Math/Type/Transform.h"
#include "Camera/Camera.h"

/// <summary>
/// Ringプリミティブを描画するレンダラー。
///
/// Sprite用RootSignature（AddressV = CLAMP）を流用しているため、
/// リングの中心部に白いアーティファクトが出るのを自動的に防ぐ。
/// </summary>
class RingRenderer {
public:
	RingRenderer()  = default;
	~RingRenderer();

	/// <summary>
	/// 初期化。テクスチャとリングパラメータを設定してGPUリソースを生成する
	/// </summary>
	/// <param name="texturePath">テクスチャのパス (例: "resources/image/xxx.png")</param>
	/// <param name="divideCount">分割数</param>
	/// <param name="outerRadius">外径</param>
	/// <param name="innerRadius">内径</param>
	void Initialize(const std::string& texturePath,
					uint32_t divideCount = 32,
					float outerRadius    = 1.0f,
					float innerRadius    = 0.2f);

	/// <summary>
	/// 描画。カメラからWVP行列を構築して描画コマンドを発行する
	/// </summary>
	void Draw(Camera& camera);

	/// <summary>
	/// ImGuiでパラメータをリアルタイム編集（デバッグ用）
	/// </summary>
	void DrawImGui(const char* label = "RingRenderer");

	// --- Transform ---
	Transform& GetTransform() { return transform_; }
	void SetTransform(const Transform& t) { transform_ = t; }

	// --- Material ---
	void SetColor(const Vector4& color);
	void SetUvScrollX(float offsetU);

private:
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateMaterialBuffer();
	void CreateTransformBuffer();

private:
	// Ring ジオメトリデータ
	Ring ring_;

	// GPUリソース
	Microsoft::WRL::ComPtr<ID3D12Device>              device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW  indexBufferView_  = {};

	// マップ済みポインタ
	SpriteVertexData*          vertexData_    = nullptr;
	uint32_t*                  indexData_     = nullptr;
	SpriteMaterial*            materialData_  = nullptr;
	SpriteTransformationMatrix* transformData_ = nullptr;

	// テクスチャ
	uint32_t    textureIndex_ = 0;
	std::string texturePath_;

	// ワールドトランスフォーム
	Transform transform_ = {};

	// UVスクロール用オフセット
	float uvOffsetU_ = 0.0f;
};
