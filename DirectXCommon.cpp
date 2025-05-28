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