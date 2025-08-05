#include"DirectXCommon.h"
#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"Winmm.lib")

using namespace Microsoft::WRL;

const uint32_t DirectXCommon::kMaxSRVCount_ = 1024;

void DirectXCommon::Initialize(WinApp* winApp) {
    // NULL
    assert(winApp);
    // メンバ変数に記録
    this->winApp_ = winApp;
    InitializeFixFPS();
    CreateDevice();
    CreateCommand();
    CreateSwapChain();
    CreateDescriptorHeaps();
    CreateDepthBuffer();
    CreateRenderTargetView();
    CreateViewportRect();
    CreateScissorRect();
    CreateShaderCompiler();
    CreateImgui();
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

void DirectXCommon::CloseFence() {
    CloseHandle(fenceEvent_);
}

void DirectXCommon::ResetCommand() {
    hr_ = commandAllocator_->Reset();
    assert(SUCCEEDED(hr_));
    hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr_));
}

void DirectXCommon::WaitForGPU() {
    // Fenceを更新してGPUの処理終了を待つ
    fenceValue_++;
    commandQueue_->Signal(fence_.Get(), fenceValue_);
    if (fence_->GetCompletedValue() < fenceValue_) {
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
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
    depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get() , winApp_->GetWidth(), winApp_->GetHeight());

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

    // SRVのヒープでディスクリプタの数は128 SRVはShader内で触るものなのでShaderVisibleはftrue
    srvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true);

    // RTV用のヒープでディスクリプタの数は2 RTVはShader内で触るものではないのでShaderVisibleはfalse
    rtvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

    // DSV用のヒープでディスクリプタの数は1、DSVはShader内で触るものではないので、ShaderVisibleはfalse 03_01
    dsvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
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

void DirectXCommon::CreateShaderCompiler() {
    // dxcCompilerを初期化
    hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    assert(SUCCEEDED(hr_));
    hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    assert(SUCCEEDED(hr_));
    // 現時点でincludeはしないが、includeに対応するための設定を行っておく
    hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateImgui() {
    // ImGuiの初期化。詳細はさして重要ではないので解説は省略する。
        // こういうもんである
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp_->GetHWND());
    ImGui_ImplDX12_Init(device_.Get(),
        swapChainDesc_.BufferCount,
        rtvDesc_.Format,
        srvDescriptorHeap_.Get(),
        srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
}

void DirectXCommon::PreDraw() {
    if (!commandList_ || !swapChainResources_[backBufferIndex_]) {
        OutputDebugStringA("commandList_ or swapChainResource is null!\n");
        return;
    }
    // これから書き込むバックバッファのインデックスを取得
    backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

    // TransitionBarrierの設定 *backBufferIndexを取得した直後、RenderTargetを設定する前に行う
    // バックバッファの番号取得
    backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

    // 今回のバリアはTransition
    barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

    // Noneにしておく
    barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

    // バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier_.Transition.pResource = swapChainResources_[backBufferIndex_].Get();

    // 遷移前（現在）のResourceState
    barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

    // 遷移後のResourceState
    barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // TransitionBarrierを指定
    commandList_->ResourceBarrier(1, &barrier_);

    // 描画先のRTVとDSVを指定する 03_01
    dsvHandle_ = GetDsvCPUHandle(0);

    // 描画先のRTVを設定する
    commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex_], false, &dsvHandle_);

    // 指定した色で画面全体をクリアする
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; // 青っぽい色、RGBAの順
    commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex_], clearColor, 0, nullptr);

    // 描画用のDescriptorHeapの設定 02_03
    ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
    commandList_->SetDescriptorHeaps(1, descriptorHeaps);

    // 指定した深度で画面全体をクリアする 03_01
    commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList_->RSSetViewports(1, &viewport_);          // Viewportを設定
    commandList_->RSSetScissorRects(1, &scissorRect_);    // Scissorを設定
}

void DirectXCommon::PostDraw() {
    if (!commandList_) {
        OutputDebugStringA("commandList_ is null! PostDraw aborted.\n");
        return; // これでクラッシュ防止（仮対応）
    }
    assert(commandList_ != nullptr);   

    // 諸諸の処理が終わった後にコマンドを積む、GUIは画面の最前面に映すので最後の描画
    // ただしResourceBarrierによってD3D12_RESOURCE_STATE_RENDER_TARGET→D3D12_RESOURCE_STATE_PRESENTへ遷移させる前
    // 実際のcommandListのImGuiの描画コマンドを積む
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());

    // TransitionBarrierを貼ってPresent状態へ遷移
    barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList_->ResourceBarrier(1, &barrier_);

    // コマンドリストを閉じる
    hr_ = commandList_->Close();
    assert(SUCCEEDED(hr_));

    // コマンドを実行
    ID3D12CommandList* commandLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists);

    // 画面に表示
    swapChain_->Present(1, 0);

    WaitForGPU();

    // 次のフレーム用のコマンドリストを準備
    hr_ = commandAllocator_->Reset();
    assert(SUCCEEDED(hr_));
    hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr_));

    // FPS固定
    UpdateFixFPS();
}

void DirectXCommon::InitializeFixFPS() {
    // 現在時間を記録する
    reference_ = std::chrono::steady_clock::now();
    // システムタイマーの分解能を上げる
    timeBeginPeriod(1);
}

void DirectXCommon::UpdateFixFPS() {
    // 1/60秒ぴったりの時間
    const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
    // 1/60秒よりわずかに短い時間
    const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

    // 現在時間を取得する
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // 前回記録からの経過時間を取得する
    std::chrono::microseconds elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    // 1/60秒（よりわずかに短い時間）経っていない場合
    if (elapsed < kMinCheckTime) {
        // 1/60秒経過するまで微小なスリープを繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            // 1マイクロ秒スリープ
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    // 現在の時間を記録する
    reference_ = std::chrono::steady_clock::now();
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

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetUavCPUHandle(uint32_t index) {
    return GetCPUDescriptorHandle(uavDescriptorHeap_.Get(), descriptorSizeUAV_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetUavGPUHandle(uint32_t index) {
    return GetGPUDescriptorHandle(uavDescriptorHeap_.Get(), descriptorSizeUAV_, index);
}

ComPtr<IDxcBlob> DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile) {

#pragma region 1 hlslファイルを読む

    // これからシェーダーをコンパイルする旨をログに出す
    Logger::Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
    // hlslファイルを読む
    ComPtr<IDxcBlobEncoding> shaderSource;
    hr_ = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら止める
    assert(SUCCEEDED(hr_));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

#pragma endregion

#pragma region 2 Compileする

    LPCWSTR arguments[] = {
     filePath.c_str(),             // コンパイル対象のhlslファイル名
     L"-E", L"main",               // エントリーポイントの指定
     L"-T", profile,               // ShaderProfileの設定
     L"-Zi", L"-Qembed_debug",     // デバッグ用の情報を埋め込む
     L"-Od",                       // 最適化を外しておく
     L"-Zpr",                      // メモリレイアウトは行優先
    };

    // 実際にShaderをコンパイルする
    ComPtr<IDxcResult> shaderResult;
    hr_ = dxcCompiler_->Compile(
        &shaderSourceBuffer, // 読み込んだファイル
        arguments,           // コンパイルオプション
        _countof(arguments), // コンパイルオプションの数
        includeHandler_.Get(),      // includeが含まれた場合
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );

    // コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr_));

#pragma endregion

#pragma region 3 警告・エラーが出ていないか確認する

    // 署名：エラーが出てきたらログに出して止める
    ComPtr<IDxcBlobUtf8> shaderError;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        OutputDebugStringA(shaderError->GetStringPointer()); // ←これ追加
        Logger::Log(shaderError->GetStringPointer());
        // 署名：エラーメッセタイ
        assert(false);
    }

#pragma endregion

#pragma region 4 Compile結果を受け取って返す

    // コンパイル結果から実行用のバイナリ部分を取得
    ComPtr<IDxcBlob> shaderBlob;
    hr_ = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr_));
    // 成功したらログを作成
    Logger::Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    //shaderSource->Release();
    //shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;

#pragma endregion

}