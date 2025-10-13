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
    // 事前定義PSOの作成は削除（遅延初期化に移行）
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    char buffer[256];
    sprintf_s(buffer, "PSOManager::Initialize completed in %lld ms\n", duration.count());
    OutputDebugStringA(buffer);
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSOManager::GetPSO(PSOType type) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // キャッシュをチェック
    auto it = predefindedPSOs_.find(type);
    if (it != predefindedPSOs_.end()) {
        psoCacheHits_++;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        char buffer[256];
        sprintf_s(buffer, "PSO Cache HIT for type %d in %lld μs\n", static_cast<int>(type), duration.count());
        OutputDebugStringA(buffer);
        
        return it->second;
    }
    
    // キャッシュにない場合は遅延作成
    psoCacheMisses_++;
    OutputDebugStringA("PSO Cache MISS - Creating PSO on demand...\n");
    
    auto pso = CreatePSOOnDemand(type);
    predefindedPSOs_[type] = pso;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    char buffer[256];
    sprintf_s(buffer, "PSO Cache MISS for type %d - Created in %lld ms\n", static_cast<int>(type), duration.count());
    OutputDebugStringA(buffer);
    
    return pso;
}

Microsoft::WRL::ComPtr<IDxcBlob> PSOManager::GetOrCompileShader(const std::wstring& filePath, const wchar_t* profile) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // キャッシュキーを生成
    std::wstring cacheKey = filePath + L"_" + profile;
    
    // キャッシュをチェック
    auto it = shaderCache_.find(cacheKey);
    if (it != shaderCache_.end()) {
        shaderCacheHits_++;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        char buffer[512];
        sprintf_s(buffer, "Shader Cache HIT for %ws_%ws in %lld μs\n", 
                  filePath.c_str(), profile, duration.count());
        OutputDebugStringA(buffer);
        
        return it->second;
    }
    
    // キャッシュミス：新規コンパイル
    shaderCacheMisses_++;
    char buffer[512];
    sprintf_s(buffer, "Shader Cache MISS - Compiling %ws_%ws...\n", 
              filePath.c_str(), profile);
    OutputDebugStringA(buffer);
    
    auto shader = dxCommon_->CompileShader(filePath, profile);
    shaderCache_[cacheKey] = shader;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    sprintf_s(buffer, "Shader Cache MISS for %ws_%ws - Compiled in %lld ms\n", 
              filePath.c_str(), profile, duration.count());
    OutputDebugStringA(buffer);
    
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
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
            break;
            
        case PSOType::Model_Wireframe_Normal:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Wireframe");
            break;
            
        case PSOType::Model_Solid_Add:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
            params.blendState = CreateBlendState("Add");
            break;
            
        case PSOType::Sprite_Normal:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
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
            
        case PSOType::Triangle_Normal:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
            break;
            
        case PSOType::Sphere_Normal:
            params.vertexShader = GetOrCompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
            params.pixelShader = GetOrCompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
            params.rasterizerState = CreateRasterizerState("Solid_NoCull");
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
    
    sprintf_s(buffer, "Total Cached PSOs: %zu\n", predefindedPSOs_.size() + customPSOCache_.size());
    OutputDebugStringA(buffer);
    
    // 節約できた時間の推定値も表示
    size_t totalShaderReuses = shaderCacheHits_;
    size_t totalPSOReuses = psoCacheHits_;
    
    // 仮定: シェーダーコンパイル1回=50ms、PSO作成1回=20ms
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
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSOManager::CreatePSOInternal(const PSOCreateParams& params) {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
    pipelineDesc.pRootSignature = params.rootSignature.Get();
    pipelineDesc.InputLayout = params.inputLayout;
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
    assert(SUCCEEDED(hr));

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
    static D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    
    // 共通の入力レイアウト（Object3D, Particle共通）
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].InputSlot = 0;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    inputElementDescs[0].InstanceDataStepRate = 0;

    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].InputSlot = 0;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    inputElementDescs[1].InstanceDataStepRate = 0;

    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].InputSlot = 0;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    inputElementDescs[2].InstanceDataStepRate = 0;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    return inputLayoutDesc;
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