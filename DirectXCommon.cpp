#include"DirectXCommon.h"
#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Initialize(WinApp* winApp) {
    // NULL
    assert(winApp);
    // メンバ変数に記録
    this->winApp_ = winApp;
    CreateDevice();
    CreateCommand();
    CreateSwapChain();
    CreateDescriptorHeaps();
    CreateDepthBuffer();
    CreateRenderTargetView();
    CreateViewportRect();
    CreateScissorRect();
}

void DirectXCommon::CreateDevice() {
#ifdef _DEBUG
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_))))
    {
        // デバッグレイヤーを有効化する
        debugController_->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController_->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    /*ウィンドウを作成した後、メインループが始まる前に記述する*/
    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    // どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr_));

    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプターの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr_ = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr_)); // 取得できないのは一大事 
        // ソフトウェアアダプタ出なければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 採用したアダプタの情報をログに出力。wstringの方なので注意
            Logger::Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter_ = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
    }
    // 適切なアダプタが見つからなかったので起動できない
    assert(useAdapter_ != nullptr);

    CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    dxgiFactory_->EnumAdapters1(0, reinterpret_cast<IDXGIAdapter1**>(useAdapter_.GetAddressOf()));

    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0 };
    const char* featureLevelStrings[] = { "12.2","12.1" ,"12.0" };
    // 高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); i++)
    {
        // 採用したアダプターでデバイスを生成
        hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(hr_))
        {
            // 生成できたのでログ出力を行ってループを抜ける
            Logger::Log(std::format("FearureLevel:{}\n", featureLevelStrings[i]));
            break;
        }
    }
    // デバイスの生成がうまくいかなかったので起動できない
    assert(device_ != nullptr);
    Logger::Log("Complete create D3D12Device!!!\n"); // 初期化完了ログを出す

#ifdef _DEBUG
    // エラー・警告で停止 *この対応はdeviceに対して行うので上のLog(初期化完了ログ)の直後に記述する 01_01
    ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバイエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 解除の直前 01_01
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッガレイヤーとDX12デバッガレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
    }
#endif
}

void DirectXCommon::CreateCommand() {

    // コマンドキュー作成
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

    hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    // コマンドキューの生成がうまくいかなかったら起動できない
    assert(SUCCEEDED(hr_));

    // コマンドアロケーターの作成
    hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    // コマンドアロケータの生成がうまくいかなかったので起動できない 
    assert(SUCCEEDED(hr_));

    // コマンドリストの作成
    hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
    // コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr_));

    // フェンス作成
    hr_ = device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr_));

    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(fenceEvent_ != nullptr);
}

void DirectXCommon::CreateSwapChain() {
    // SwapChainの設定
    swapChainDesc_.Width = winApp_->GetWidth();                   // 画面の幅
    swapChainDesc_.Height = winApp_->GetHeight();                 // 画面の高さ
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // 色の形式
    swapChainDesc_.SampleDesc.Count = 1;                          // マルチサンプルしない
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
    swapChainDesc_.BufferCount = 2;                               // ダブルバッファ
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // モニタに写したら、中身を破棄

    // 一時的なSwapChain1 コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
    hr_ = dxgiFactory_->CreateSwapChainForHwnd(
        commandQueue_.Get(),
        winApp_->GetHWND(),
        &swapChainDesc_,
        nullptr,
        nullptr,
        tempSwapChain.GetAddressOf()
    );
    assert(SUCCEEDED(hr_));

    // SwapChain4にキャスト
    hr_ = tempSwapChain.As(&swapChain_);
    assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateDepthBuffer() {
    // DepthStencilTextureをウィンドウのサイズで作成 03_01
    depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), winApp_->GetWidth(), winApp_->GetHeight());

    // DSVの設定 Heap上にDSVを構築する 03_01
    dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にResourceに合わせる
    dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2Dテクスチャ

    // DSVHeapの配列にDSVを作る
    device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc_, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::CreateDescriptorHeaps() {
    // DescriptorsSizeを取得しておく
    descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    // RTV用のヒープでディスクリプタの数は2 RTVはShader内で触るものではないのでShaderVisibleはfalse
    rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

    // SRVのヒープでディスクリプタの数は128 SRVはShader内で触るものなのでShaderVisibleはftrue
    srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

    // DSV用のヒープでディスクリプタの数は1、DSVはShader内で触るものではないので、ShaderVisibleはfalse 03_01
    dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

void DirectXCommon::CreateRenderTargetView() {
    hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
    assert(SUCCEEDED(hr_));
    hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
    assert(SUCCEEDED(hr_));
    // RTVの設定
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をsRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
    // ディスクリプタの先頭を取得する
    rtvStartHandle_ = GetCPUDescriptorHandle(rtvDescriptorHeap_.Get(), descriptorSizeRTV_, 0);
    // まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
    rtvHandles_[0] = rtvStartHandle_;
    device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
    // 2つ目のディスクリプタハンドルを得る（自力で）
    rtvHandles_[1].ptr = rtvHandles_[0].ptr + descriptorSizeRTV_;
    // 2つ目を作る
    device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}

void DirectXCommon::CreateViewportRect() {
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport_.Width = FLOAT(winApp_->GetWidth());
    viewport_.Height = FLOAT(winApp_->GetHeight());
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
}

void DirectXCommon::CreateScissorRect() {
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect_.left = 0;
    scissorRect_.right = winApp_->GetWidth();
    scissorRect_.top = 0;
    scissorRect_.bottom = winApp_->GetHeight();
}

ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
    ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(hr));
    return descriptorHeap;
}

ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {

    // 生産するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width; // Textureの幅
    resourceDesc.Height = height; // Textureの高さ
    resourceDesc.MipLevels = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 深度 or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1; // サンプリングの数。通常は1
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う場合

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    /// 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f（最大値）でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット、Resourceに合わせる

    // Resourceの生成
    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
        &depthClearValue, // Clear用初期値
        IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
    assert(SUCCEEDED(hr));

    return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSrvCPUHandle(uint32_t index) {
    return GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorSizeSRV_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSrvGPUHandle(uint32_t index) {
    return GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorSizeSRV_, index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetDsvCPUHandle(uint32_t index) {
    return GetCPUDescriptorHandle(dsvDescriptorHeap_.Get(), descriptorSizeDSV_, index);
}