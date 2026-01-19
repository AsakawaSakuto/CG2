#include "PSOManager.h"
#include <cassert>
#include <functional>
#include <chrono>

PSOManager& PSOManager::GetInstance() {
    static PSOManager instance;
    return instance;
}

void PSOManager::Initialize(DirectXCommon* dxCommon) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    dxCommon_ = dxCommon;
    CreateRootSignatures();
    // 事前定義PSOの作成は削除、遅延初期化に移行
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    char buffer[256];
    sprintf_s(buffer, "PSOManager::Initialize completed in %lld ms\n", duration.count());
    OutputDebugStringA(buffer);
}

void PSOManager::Finalize() {
    // キャッシュ統計を出力
    PrintCacheStats();
    
    // シェーダーキャッシュをクリア
    shaderCache_.clear();
    
    // カスタムPSOキャッシュをクリア
    customPSOCache_.clear();
    
    // 事前定義済みPSOキャッシュをクリア
    predefinedPSOs_.clear();
    
    // RootSignatureキャッシュをクリア
    rootSignatures_.clear();
    
    // 参照をクリア
    dxCommon_ = nullptr;
    
    OutputDebugStringA("PSOManager::Finalize completed - All cached resources released\n");
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSOManager::GetPSO(PSOType type) {
    // キャッシュチェック
    auto it = predefinedPSOs_.find(type);
    if (it != predefinedPSOs_.end()) {
        psoCacheHits_++;
        return it->second;   // ここはそのままでOK
    }

    // 未作成 → 作成
    psoCacheMisses_++;

#ifdef _DEBUG
    auto startTime = std::chrono::high_resolution_clock::now();
    OutputDebugStringA("PSO Cache MISS - Creating PSO on demand...\n");
#endif

    auto pso = CreatePSOOnDemand(type);

    if (!pso) {
        char buf[256];
        sprintf_s(buf, "ERROR[GetPSO]: PSO creation FAILED for type=%d\n", (int)type);
        OutputDebugStringA(buf);
        // キャッシュしないでそのまま nullptr を返す
        return nullptr;
    }

    predefinedPSOs_[type] = pso;

#ifdef _DEBUG
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    char buffer[128];
    sprintf_s(buffer, "PSO created and cached in %lld ms\n", duration.count());
    OutputDebugStringA(buffer);
#endif

    return pso;
}

Microsoft::WRL::ComPtr<IDxcBlob> PSOManager::GetOrCompileShader(const std::wstring& filePath, const wchar_t* profile) {
    // キャッシュキーを生成
    std::wstring cacheKey = filePath + L"_" + profile;
    
    // キャッシュをチェック
    auto it = shaderCache_.find(cacheKey);
    if (it != shaderCache_.end()) {
        shaderCacheHits_++;
        // キャッシュヒット時のログは削除（頻繁すぎるため）
        return it->second;
    }
    
    // キャッシュミス：新規コンパイル
    shaderCacheMisses_++;
    
#ifdef _DEBUG
    auto startTime = std::chrono::high_resolution_clock::now();
    char buffer[512];
    sprintf_s(buffer, "Shader Cache MISS - Compiling %ws_%ws...\n", 
              filePath.c_str(), profile);
    OutputDebugStringA(buffer);
#endif
    
    auto shader = dxCommon_->CompileShader(filePath, profile);
    shaderCache_[cacheKey] = shader;
    
#ifdef _DEBUG
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    sprintf_s(buffer, "Shader Cache MISS for %ws_%ws - Compiled in %lld ms\n", 
              filePath.c_str(), profile, duration.count());
    OutputDebugStringA(buffer);
#endif
    
    return shader;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSOManager::CreatePSOOnDemand(PSOType type) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    PSOCreateParams params;
    params.rootSignature = GetRootSignature("Object3D");
    params.inputLayout = CreateInputLayout("Object3D");
    params.blendState = CreateBlendState("Normal");
    params.rasterizerState = CreateRasterizerState("Solid");
    params.depthStencilState = CreateDepthStencilState("Normal");

    // タイプ別の設定
    switch (type) {
        case PSOType::Model_Solid_Normal:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            break;
            
        case PSOType::Model_Wireframe_Normal:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Wireframe");
            break;
            
        case PSOType::Model_Solid_Add:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("Add");
            break;

        case PSOType::Model_Alpha_Normal: // 追加: 透明モデル用
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("Normal"); // アルファブレンド
            params.depthStencilState = CreateDepthStencilState("Particle"); // 深度書き込み無効
            params.rasterizerState = CreateRasterizerState("Solid_NoCull"); // 両面表示
            break;
            
        case PSOType::SkinningModel_Solid_Normal:
            params.rootSignature = GetRootSignature("Skinning"); // Skinning専用RootSignature
            params.inputLayout = CreateInputLayout("Skinning"); // Skinning専用InputLayout
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/SkinningObject3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            break;
            
        case PSOType::SkinningModel_Solid_Add:
            params.rootSignature = GetRootSignature("Skinning"); // Skinning専用RootSignature
            params.inputLayout = CreateInputLayout("Skinning"); // Skinning専用InputLayout
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/SkinningObject3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("Add");
            break;
            
        case PSOType::SkinningModel_Wireframe_Normal:
            params.rootSignature = GetRootSignature("Skinning"); // Skinning専用RootSignature
            params.inputLayout = CreateInputLayout("Skinning"); // Skinning専用InputLayout
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/SkinningObject3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Wireframe");
            break;
            
        case PSOType::SkinningModel_Alpha_Normal:
            params.rootSignature = GetRootSignature("Skinning"); // Skinning専用RootSignature
            params.inputLayout = CreateInputLayout("Skinning"); // Skinning専用InputLayout
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Model/SkinningObject3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Model/Object3d.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("Normal"); // アルファブレンド
            params.depthStencilState = CreateDepthStencilState("Particle"); // 深度書き込み無効
            params.rasterizerState = CreateRasterizerState("Solid_NoCull"); // 両面表示
            break;
            
        case PSOType::Sprite_Normal:
            params.rootSignature = GetRootSignature("Sprite"); // Sprite専用RootSignature（CLAMPサンプラー）
            params.inputLayout = CreateInputLayout("Sprite"); // Sprite専用InputLayout（Normalなし）
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Sprite/Sprite.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Sprite/Sprite.PS.hlsl", L"ps_6_0");
            params.depthStencilState = CreateDepthStencilState("Sprite");
            break;
            
        case PSOType::Particle_None:
            params.rootSignature = GetRootSignature("Particle");
            params.inputLayout = CreateInputLayout("Particle");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Particle");
            params.blendState = CreateBlendState("None");
            break;
            
        case PSOType::Particle_Normal:
            params.rootSignature = GetRootSignature("Particle");
            params.inputLayout = CreateInputLayout("Particle");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Particle");
            params.blendState = CreateBlendState("Normal");
            break;
            
        case PSOType::Particle_Add:
            params.rootSignature = GetRootSignature("Particle");
            params.inputLayout = CreateInputLayout("Particle");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Particle");
            params.blendState = CreateBlendState("Add");
            break;
            
        case PSOType::Particle_Subtract:
            params.rootSignature = GetRootSignature("Particle");
            params.inputLayout = CreateInputLayout("Particle");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Particle");
            params.blendState = CreateBlendState("Subtract");
            break;
            
        case PSOType::Particle_Multiply:
            params.rootSignature = GetRootSignature("Particle");
            params.inputLayout = CreateInputLayout("Particle");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Particle");
            params.blendState = CreateBlendState("Multiply");
            break;
            
        case PSOType::Particle_Screen:
            params.rootSignature = GetRootSignature("Particle");
            params.inputLayout = CreateInputLayout("Particle");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Particles/Particles.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Particle");
            params.blendState = CreateBlendState("Screen");
            break;
            
        case PSOType::PostEffect_None:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Grayscale:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Grayscale.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Sepia:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Sepia.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Vignette:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Vignette.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Invert:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Invert.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Blur:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Blur.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_RadialBlur:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/RadialBlur.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Outline:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Outline.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;
            
        case PSOType::PostEffect_Fog:
            params.rootSignature = GetRootSignature("Offscreen");
            params.inputLayout.pInputElementDescs = nullptr; // 頂点バッファ無し
            params.inputLayout.NumElements = 0;
            params.vertexShader = GetOrCompileShader(L"resources/shaders/PostEffect/CopyImage.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/PostEffect/Fog.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("None");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState.DepthEnable = FALSE; // 深度無効
            params.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            break;

        case PSOType::Line_Normal:
            params.rootSignature = GetRootSignature("Line");
            params.inputLayout = CreateInputLayout("Line");
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Line/Line.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Line/Line.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("Normal");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
            params.depthStencilState = CreateDepthStencilState("Normal");
            params.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            break;
        default:
            assert(false && "Unknown PSO type");
            break;
    }

    auto pso = CreatePSOInternal(params);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    char buffer[256];
    sprintf_s(buffer, "CreatePSOOnDemand for type %d completed in %lld ms\n", 
              static_cast<int>(type), duration.count());
    OutputDebugStringA(buffer);
    
    return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSOManager::GetOrCreatePSO(const PSOCreateParams& params) {
    // キャッシュをチェック
    auto it = customPSOCache_.find(params);
    if (it != customPSOCache_.end()) {
        psoCacheHits_++;
        return it->second;
    }
    
    // キャッシュにない場合は新規作成
    psoCacheMisses_++;
    auto pso = CreatePSOInternal(params);
    customPSOCache_[params] = pso;
    return pso;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> PSOManager::GetRootSignature(const std::string& signatureType) {
    auto it = rootSignatures_.find(signatureType);
    if (it != rootSignatures_.end()) {
        return it->second;
    }
    return nullptr;
}

void PSOManager::PrintCacheStats() const {
    OutputDebugStringA("=== PSO Manager Cache Statistics ===\n");
    
    char buffer[256];
    sprintf_s(buffer, "Shader Cache - Hits: %zu, Misses: %zu, Hit Rate: %.2f%%\n", 
              shaderCacheHits_, shaderCacheMisses_, 
              (shaderCacheHits_ + shaderCacheMisses_) > 0 ? 
              (100.0f * shaderCacheHits_ / (shaderCacheHits_ + shaderCacheMisses_)) : 0.0f);
    OutputDebugStringA(buffer);
    
    sprintf_s(buffer, "PSO Cache - Hits: %zu, Misses: %zu, Hit Rate: %.2f%%\n", 
              psoCacheHits_, psoCacheMisses_, 
              (psoCacheHits_ + psoCacheMisses_) > 0 ? 
              (100.0f * psoCacheHits_ / (psoCacheHits_ + psoCacheMisses_)) : 0.0f);
    OutputDebugStringA(buffer);
    
    sprintf_s(buffer, "Total Cached Shaders: %zu\n", shaderCache_.size());
    OutputDebugStringA(buffer);
    
    sprintf_s(buffer, "Total Cached PSOs: %zu\n", predefinedPSOs_.size() + customPSOCache_.size());
    OutputDebugStringA(buffer);
    
    // 節約できた時間の推定値も表示
    size_t totalShaderReuses = shaderCacheHits_;
    size_t totalPSOReuses = psoCacheHits_;
    
    size_t estimatedShaderTimeSaved = totalShaderReuses * 50;
    size_t estimatedPSOTimeSaved = totalPSOReuses * 20;
    
    sprintf_s(buffer, "Estimated time saved: Shaders=%zu ms, PSOs=%zu ms, Total=%zu ms\n", 
              estimatedShaderTimeSaved, estimatedPSOTimeSaved, 
              estimatedShaderTimeSaved + estimatedPSOTimeSaved);
    OutputDebugStringA(buffer);
    
    OutputDebugStringA("=====================================\n");
}

// 残りの既存メソッドは変更なし
void PSOManager::CreateRootSignatures() {
    // Object3D用のRoot Signature
    {
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // DescriptorRange
        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].BaseShaderRegister = 0;
        descriptorRange[0].NumDescriptors = 1;
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // Sampler
        D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
        staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplers[0].ShaderRegister = 0;
        staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        descriptionRootSignature.pStaticSamplers = staticSamplers;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

        // Root Parameters
        D3D12_ROOT_PARAMETER rootParameters[7] = {};
        
        // b0: Material (PS)
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[0].Descriptor.ShaderRegister = 0;

        // b1: Transformation (VS)
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[1].Descriptor.ShaderRegister = 1;

        // t0: Texture (PS)
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

        // b2: DirectionalLight (PS)
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[3].Descriptor.ShaderRegister = 2;

        // b3: Camera (PS)
        rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[4].Descriptor.ShaderRegister = 3;

        // b4: PointLight (PS)
        rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[5].Descriptor.ShaderRegister = 4;

        // b5: SpotLight (PS)
        rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[6].Descriptor.ShaderRegister = 5;

        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);

        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        assert(SUCCEEDED(hr));

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = dxCommon_->GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        rootSignatures_["Object3D"] = rootSignature;
    }

    // Skinning用のRoot Signature（Object3D + t1: MatrixPalette）
    {
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // DescriptorRange (2つ: テクスチャt0とMatrixPalette t1)
        D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = {};
        
        // t0: Texture (PS)
        descriptorRanges[0].BaseShaderRegister = 0;
        descriptorRanges[0].NumDescriptors = 1;
        descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // t1: MatrixPalette (VS) - StructuredBuffer
        descriptorRanges[1].BaseShaderRegister = 1;
        descriptorRanges[1].NumDescriptors = 1;
        descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // Sampler
        D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
        staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplers[0].ShaderRegister = 0;
        staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        descriptionRootSignature.pStaticSamplers = staticSamplers;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

        // Root Parameters (8個: Object3Dの7個 + MatrixPalette用1個)
        D3D12_ROOT_PARAMETER rootParameters[8] = {};
        
        // b0: Material (PS)
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[0].Descriptor.ShaderRegister = 0;

        // b1: Transformation (VS)
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[1].Descriptor.ShaderRegister = 1;

        // t0: Texture (PS)
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

        // b2: DirectionalLight (PS)
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[3].Descriptor.ShaderRegister = 2;

        // b3: Camera (PS)
        rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[4].Descriptor.ShaderRegister = 3;

        // b4: PointLight (PS)
        rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[5].Descriptor.ShaderRegister = 4;

        // b5: SpotLight (PS)
        rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[6].Descriptor.ShaderRegister = 5;

        // t1: MatrixPalette (VS) - StructuredBuffer
        rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[7].DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
        rootParameters[7].DescriptorTable.NumDescriptorRanges = 1;

        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);

        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        assert(SUCCEEDED(hr));

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = dxCommon_->GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        rootSignatures_["Skinning"] = rootSignature;
    }

    // Sprite用のRoot Signature（2D専用、シンプル化：ライティング不要）
    {
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // DescriptorRange
        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].BaseShaderRegister = 0;
        descriptorRange[0].NumDescriptors = 1;
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // Sampler - CLAMPを使用（テクスチャの端で黒い縁が出るのを防ぐ）
        D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
        staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplers[0].ShaderRegister = 0;
        staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        descriptionRootSignature.pStaticSamplers = staticSamplers;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

        // Root Parameters（3つのみ：Material, Transform, Texture）
        D3D12_ROOT_PARAMETER rootParameters[3] = {};
        
        // b0: Material (PS)
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[0].Descriptor.ShaderRegister = 0;

        // b1: Transformation (VS)
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[1].Descriptor.ShaderRegister = 1;

        // t0: Texture (PS)
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);

        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        assert(SUCCEEDED(hr));

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = dxCommon_->GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        rootSignatures_["Sprite"] = rootSignature;
    }

    // Particle用のRoot Signature
    {
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // 2つのDescriptorRangeを用意（テクスチャSRV用とStructuredBuffer用）
        D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = {};

        // t0 : ピクセルシェーダで使う通常テクスチャ用のSRV（gTexture）
        descriptorRanges[0].BaseShaderRegister = 0;
        descriptorRanges[0].NumDescriptors = 1;
        descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // t1 : 頂点シェーダで使う構造化バッファ（StructuredBuffer）用のSRV（gParticles）
        descriptorRanges[1].BaseShaderRegister = 1;
        descriptorRanges[1].NumDescriptors = 1;
        descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // Static Sampler
        D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
        staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplers[0].ShaderRegister = 0;
        staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        descriptionRootSignature.pStaticSamplers = staticSamplers;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

        // Root Parameters
        D3D12_ROOT_PARAMETER rootParameters[7] = {};

        // b0 : マテリアル（Pixel Shader）
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[0].Descriptor.ShaderRegister = 0;

        // b1 : カメラ＆ビルボード行列（PreView/Vertex Shader用 CBV）
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[1].Descriptor.ShaderRegister = 1;

        // t1 : パーティクル情報（構造化バッファ）（Vertex Shader用SRV）
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

        // t0 : テクスチャ（Pixel Shader用SRV）
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[3].DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
        rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

        // b2 : ディレクショナルライト（Pixel Shader用CBV）
        rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[4].Descriptor.ShaderRegister = 2;

        // b5 : エミッター（Compute/Vertex/Pixel 用CBV）
        rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[5].Descriptor.ShaderRegister = 5;

        // b6 : PerFrame（Compute/Vertex/Pixel 用CBV）
        rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[6].Descriptor.ShaderRegister = 6;

        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);

        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        assert(SUCCEEDED(hr));

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = dxCommon_->GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        rootSignatures_["Particle"] = rootSignature;
    }

    // Offscreen(CopyImage)用のRoot Signature
    {
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // DescriptorRange for SRV (t0とt1の2つ)
        D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = {};
        
        // t0: Color Texture
        descriptorRanges[0].BaseShaderRegister = 0; // t0
        descriptorRanges[0].NumDescriptors = 1;
        descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        
        // t1: Depth Texture
        descriptorRanges[1].BaseShaderRegister = 1; // t1
        descriptorRanges[1].NumDescriptors = 1;
        descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // Static Sampler for s0 (Linear) and s1 (Point)
        D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
        
        // s0: Linear Sampler (Color用)
        staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplers[0].ShaderRegister = 0; // s0
        staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        
        // s1: Point Sampler (Depth用)
        staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplers[1].ShaderRegister = 1; // s1
        staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        descriptionRootSignature.pStaticSamplers = staticSamplers;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

        // Root Parameters (3個: カラーテクスチャSRV + DepthテクスチャSRV + パラメータCBV)
        D3D12_ROOT_PARAMETER rootParameters[3] = {};
        
        // t0: Color Texture (PS)
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
        rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;

        // b0: EffectParams (PS) - パラメータ用のCBV
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[1].Descriptor.ShaderRegister = 0;
        
        // t1: Depth Texture (PS)
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);

        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        assert(SUCCEEDED(hr));

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = dxCommon_->GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        rootSignatures_["Offscreen"] = rootSignature;
    }

    // Line用のRoot Signature（シンプル: b0のみ）
    {
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // Root Parameters (1個: ViewProjection行列のみ)
        D3D12_ROOT_PARAMETER rootParameters[1] = {};
        
        // b0: ViewProjection Matrix (VS)
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[0].Descriptor.ShaderRegister = 0;

        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);

        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        assert(SUCCEEDED(hr));

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = dxCommon_->GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        rootSignatures_["Line"] = rootSignature;
    }
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSOManager::CreatePSOInternal(const PSOCreateParams& params) {

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
    pipelineDesc.pRootSignature = params.rootSignature.Get();
    pipelineDesc.InputLayout = params.inputLayout;

    // 事前チェック（nullptr 防止）
    if (!pipelineDesc.pRootSignature || !params.vertexShader || !params.pixelShader) {
        OutputDebugStringA("ERROR[CreatePSOInternal]: RootSignature or Shader is nullptr\n");
        assert(false && "RootSignature or Shader is nullptr");
        return nullptr;
    }

    pipelineDesc.VS = { params.vertexShader->GetBufferPointer(), params.vertexShader->GetBufferSize() };
    pipelineDesc.PS = { params.pixelShader->GetBufferPointer(), params.pixelShader->GetBufferSize() };
    pipelineDesc.BlendState = params.blendState;
    pipelineDesc.RasterizerState = params.rasterizerState;
    pipelineDesc.NumRenderTargets = 1;
    pipelineDesc.RTVFormats[0] = params.rtvFormat;
    pipelineDesc.PrimitiveTopologyType = params.topologyType;
    pipelineDesc.SampleDesc.Count = 1;
    pipelineDesc.SampleDesc.Quality = 0;
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineDesc.DepthStencilState = params.depthStencilState;
    pipelineDesc.DSVFormat = params.dsvFormat;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));

    if (FAILED(hr)) {
        char buf[512];
        sprintf_s(buf,
            "ERROR[CreatePSOInternal]: CreateGraphicsPipelineState FAILED (hr=0x%08X)\n"
            "  RS=%p, VS=%p, PS=%p, RTV=%d, DSV=%d, Topology=%d\n",
            hr,
            pipelineDesc.pRootSignature,
            params.vertexShader.Get(),
            params.pixelShader.Get(),
            (int)pipelineDesc.RTVFormats[0],
            (int)pipelineDesc.DSVFormat,
            (int)pipelineDesc.PrimitiveTopologyType);
        OutputDebugStringA(buf);

        assert(false && "CreateGraphicsPipelineState FAILED");
        return nullptr;
    }

    return pipelineState;
}

D3D12_BLEND_DESC PSOManager::CreateBlendState(const std::string& blendType) {
    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    if (blendType == "None") {
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    } else if (blendType == "Normal") {
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    } else if (blendType == "Add") {
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    } else if (blendType == "Subtract") {
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    } else if (blendType == "Multiply") {
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    } else if (blendType == "Screen") {
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    } else {
        // デフォルト: Normal
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    }

    return blendDesc;
}

D3D12_RASTERIZER_DESC PSOManager::CreateRasterizerState(const std::string& rasterizerType) {
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    
    if (rasterizerType == "Solid") {
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    } else if (rasterizerType == "Solid_NoCull") {
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    } else if (rasterizerType == "Wireframe") {
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    } else {
        // デフォルト
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    }

    return rasterizerDesc;
}

D3D12_DEPTH_STENCIL_DESC PSOManager::CreateDepthStencilState(const std::string& depthStencilType) {
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    
    if (depthStencilType == "Normal") {
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    } else if (depthStencilType == "Particle") {
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // パーティクルは深度書き込みしない
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    } else if (depthStencilType == "Sprite") {
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        depthStencilDesc.StencilEnable = FALSE;
    } else {
        // デフォルト
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    }

    return depthStencilDesc;
}

D3D12_INPUT_LAYOUT_DESC PSOManager::CreateInputLayout(const std::string& layoutType) {

    if (layoutType == "Line") {

        // Line用の入力レイアウト（2要素: POSITION + COLOR）
        static D3D12_INPUT_ELEMENT_DESC lineInputElementDescs[2] = {};

        // POSITION
        lineInputElementDescs[0].SemanticName = "POSITION";
        lineInputElementDescs[0].SemanticIndex = 0;
        lineInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        lineInputElementDescs[0].InputSlot = 0;
        lineInputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        lineInputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        lineInputElementDescs[0].InstanceDataStepRate = 0;

        // COLOR
        lineInputElementDescs[1].SemanticName = "COLOR";
        lineInputElementDescs[1].SemanticIndex = 0;
        lineInputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        lineInputElementDescs[1].InputSlot = 0;
        lineInputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        lineInputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        lineInputElementDescs[1].InstanceDataStepRate = 0;

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
        inputLayoutDesc.pInputElementDescs = lineInputElementDescs;
        inputLayoutDesc.NumElements = _countof(lineInputElementDescs);
        return inputLayoutDesc;
    } else if (layoutType == "Skinning") {

        // Skinning用の入力レイアウト（5要素: 通常の3つ + WEIGHT + INDEX）
        static D3D12_INPUT_ELEMENT_DESC skinningInputElementDescs[5] = {};

        // POSITION
        skinningInputElementDescs[0].SemanticName = "POSITION";
        skinningInputElementDescs[0].SemanticIndex = 0;
        skinningInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        skinningInputElementDescs[0].InputSlot = 0;
        skinningInputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        skinningInputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        skinningInputElementDescs[0].InstanceDataStepRate = 0;

        // TEXCOORD
        skinningInputElementDescs[1].SemanticName = "TEXCOORD";
        skinningInputElementDescs[1].SemanticIndex = 0;
        skinningInputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        skinningInputElementDescs[1].InputSlot = 0;
        skinningInputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        skinningInputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        skinningInputElementDescs[1].InstanceDataStepRate = 0;

        // NORMAL
        skinningInputElementDescs[2].SemanticName = "NORMAL";
        skinningInputElementDescs[2].SemanticIndex = 0;
        skinningInputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        skinningInputElementDescs[2].InputSlot = 0;
        skinningInputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        skinningInputElementDescs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        skinningInputElementDescs[2].InstanceDataStepRate = 0;

        // WEIGHT (4つのfloat)
        skinningInputElementDescs[3].SemanticName = "WEIGHT";
        skinningInputElementDescs[3].SemanticIndex = 0;
        skinningInputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        skinningInputElementDescs[3].InputSlot = 1; // InputSlot 1 (influenceバッファ)
        skinningInputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        skinningInputElementDescs[3].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        skinningInputElementDescs[3].InstanceDataStepRate = 0;

        // INDEX (4つのint)
        skinningInputElementDescs[4].SemanticName = "INDEX";
        skinningInputElementDescs[4].SemanticIndex = 0;
        skinningInputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
        skinningInputElementDescs[4].InputSlot = 1; // InputSlot 1 (influenceバッファ)
        skinningInputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        skinningInputElementDescs[4].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        skinningInputElementDescs[4].InstanceDataStepRate = 0;

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
        inputLayoutDesc.pInputElementDescs = skinningInputElementDescs;
        inputLayoutDesc.NumElements = _countof(skinningInputElementDescs);
        return inputLayoutDesc;
    } else if (layoutType == "Sprite") {

        // Sprite用の入力レイアウト（2要素のみ: POSITION + TEXCOORD、Normalなし）
        static D3D12_INPUT_ELEMENT_DESC spriteInputElementDescs[2] = {};

        // POSITION
        spriteInputElementDescs[0].SemanticName = "POSITION";
        spriteInputElementDescs[0].SemanticIndex = 0;
        spriteInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        spriteInputElementDescs[0].InputSlot = 0;
        spriteInputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        spriteInputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        spriteInputElementDescs[0].InstanceDataStepRate = 0;

        // TEXCOORD
        spriteInputElementDescs[1].SemanticName = "TEXCOORD";
        spriteInputElementDescs[1].SemanticIndex = 0;
        spriteInputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        spriteInputElementDescs[1].InputSlot = 0;
        spriteInputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        spriteInputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        spriteInputElementDescs[1].InstanceDataStepRate = 0;

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
        inputLayoutDesc.pInputElementDescs = spriteInputElementDescs;
        inputLayoutDesc.NumElements = _countof(spriteInputElementDescs);
        return inputLayoutDesc;
    } else {

        // Object3D用の入力レイアウト
        static D3D12_INPUT_ELEMENT_DESC object3dInputElementDescs[3] = {};

        // Object3D, Particle用の共通入力レイアウト
        object3dInputElementDescs[0].SemanticName = "POSITION";
        object3dInputElementDescs[0].SemanticIndex = 0;
        object3dInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        object3dInputElementDescs[0].InputSlot = 0;
        object3dInputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        object3dInputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        object3dInputElementDescs[0].InstanceDataStepRate = 0;

        object3dInputElementDescs[1].SemanticName = "TEXCOORD";
        object3dInputElementDescs[1].SemanticIndex = 0;
        object3dInputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        object3dInputElementDescs[1].InputSlot = 0;
        object3dInputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        object3dInputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        object3dInputElementDescs[1].InstanceDataStepRate = 0;

        object3dInputElementDescs[2].SemanticName = "NORMAL";
        object3dInputElementDescs[2].SemanticIndex = 0;
        object3dInputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        object3dInputElementDescs[2].InputSlot = 0;
        object3dInputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        object3dInputElementDescs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        object3dInputElementDescs[2].InstanceDataStepRate = 0;

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
        inputLayoutDesc.pInputElementDescs = object3dInputElementDescs;
        inputLayoutDesc.NumElements = _countof(object3dInputElementDescs);
        return inputLayoutDesc;
    }
}

// PSOCreateParamsの比較演算子とハッシュ関数の実装
bool PSOCreateParams::operator==(const PSOCreateParams& other) const {
    return rootSignature.Get() == other.rootSignature.Get() &&
           vertexShader.Get() == other.vertexShader.Get() &&
           pixelShader.Get() == other.pixelShader.Get() &&
           memcmp(&blendState, &other.blendState, sizeof(blendState)) == 0 &&
           memcmp(&rasterizerState, &other.rasterizerState, sizeof(rasterizerState)) == 0 &&
           memcmp(&depthStencilState, &other.depthStencilState, sizeof(depthStencilState)) == 0 &&
           rtvFormat == other.rtvFormat &&
           dsvFormat == other.dsvFormat &&
           topologyType == other.topologyType;
}

std::size_t PSOParamsHash::operator()(const PSOCreateParams& params) const {
    std::size_t hash1 = std::hash<void*>{}(params.rootSignature.Get());
    std::size_t hash2 = std::hash<void*>{}(params.vertexShader.Get());
    std::size_t hash3 = std::hash<void*>{}(params.pixelShader.Get());
    std::size_t hash4 = std::hash<int>{}(params.rtvFormat);
    std::size_t hash5 = std::hash<int>{}(params.dsvFormat);
    std::size_t hash6 = std::hash<int>{}(params.topologyType);
    
    return hash1 ^ (hash2 << 1) ^ (hash3 << 2) ^ (hash4 << 3) ^ (hash5 << 4) ^ (hash6 << 5);
}