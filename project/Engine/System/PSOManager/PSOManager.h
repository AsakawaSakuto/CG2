#pragma once
#define USE_LAZY_LOADING false
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "DirectXCommon.h"
#include "PSOType.h"

// PSO作成用のパラメータ構造体
struct PSOCreateParams {
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    D3D12_INPUT_LAYOUT_DESC inputLayout;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
    D3D12_BLEND_DESC blendState;
    D3D12_RASTERIZER_DESC rasterizerState;
    D3D12_DEPTH_STENCIL_DESC depthStencilState;
    DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
    // ハッシュ計算用の比較演算子
    bool operator==(const PSOCreateParams& other) const;
};

// PSO用のハッシュ関数
struct PSOParamsHash {
    std::size_t operator()(const PSOCreateParams& params) const;
};

class PSOManager {
public:
    static PSOManager& GetInstance();
    
    void Initialize(DirectXCommon* dxCommon);
    
    // 特定のタイプのPSOを取得、遅延初期化対応
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPSO(PSOType type);
    
    // カスタムパラメータでPSOを作成・取得、キャッシュ機能付き
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetOrCreatePSO(const PSOCreateParams& params);
    
    // 共通のRoot Signatureを取得
    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature(const std::string& signatureType);

    // シェーダーキャッシュからシェーダーを取得
    Microsoft::WRL::ComPtr<IDxcBlob> GetOrCompileShader(const std::wstring& filePath, const wchar_t* profile);

    // キャッシュ統計情報
    void PrintCacheStats() const;

private:
    PSOManager() = default;
    ~PSOManager() = default;
    PSOManager(const PSOManager&) = delete;
    PSOManager& operator=(const PSOManager&) = delete;
    
    DirectXCommon* dxCommon_ = nullptr;
    
    // PSO キャッシュ
    std::unordered_map<PSOType, Microsoft::WRL::ComPtr<ID3D12PipelineState>> predefindedPSOs_;
    std::unordered_map<PSOCreateParams, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PSOParamsHash> customPSOCache_;
    
    // Root Signature キャッシュ
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatures_;
    
    // シェーダーキャッシュ
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<IDxcBlob>> shaderCache_;
    
    // 統計情報
    mutable size_t shaderCacheHits_ = 0;
    mutable size_t shaderCacheMisses_ = 0;
    mutable size_t psoCacheHits_ = 0;
    mutable size_t psoCacheMisses_ = 0;
    
    // 特定のPSOタイプを遅延作成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePSOOnDemand(PSOType type);
    
    // 共通のRoot Signatureを作成
    void CreateRootSignatures();
    
    // PSO作成の共通処理
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePSOInternal(const PSOCreateParams& params);
    
    // 共通ブレンドステート作成
    D3D12_BLEND_DESC CreateBlendState(const std::string& blendType);
    
    // 共通ラスタライザーステート作成
    D3D12_RASTERIZER_DESC CreateRasterizerState(const std::string& rasterizerType);
    
    // 共通深度ステンシルステート作成
    D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(const std::string& depthStencilType);
    
    // 共通入力レイアウト作成
    D3D12_INPUT_LAYOUT_DESC CreateInputLayout(const std::string& layoutType);
};