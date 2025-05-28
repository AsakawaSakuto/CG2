#include "WinApp.h"
#include <cassert>

WinApp::~WinApp() {
    if (hwnd_) {
        CloseWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

void WinApp::Initialize(const wchar_t* title) {
#pragma region ウィンドウクラス登録

    wc_.lpfnWndProc = WindowProc;
    wc_.lpszClassName = L"WindowClass";
    wc_.hInstance = GetModuleHandle(nullptr);
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc_);

#pragma endregion

#pragma region ウィンドウサイズ調整

    RECT wrc = { 0,0,static_cast<LONG>(kClientWidth_), static_cast<LONG>(kClientHeight_) };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

#pragma endregion

#pragma region ウィンドウ生成と表示

    hwnd_ = CreateWindow(
        wc_.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr, nullptr, wc_.hInstance, nullptr);

    assert(hwnd_ != nullptr);

    ShowWindow(hwnd_, SW_SHOW);

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

#pragma endregion
}

bool WinApp::ProcessMessage() {
    MSG msg{};
    if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) {
           return true;
        }
    }
    return false;
}

void WinApp::SetTitle(const std::wstring& title) {
    // ウィンドウのタイトルを変更
    SetWindowText(hwnd_, title.c_str());
}

void WinApp::GetClientSize(uint32_t& width, uint32_t& height) const {
    RECT rect{};
    GetClientRect(hwnd_, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}

void WinApp::EnableResize(bool enable) {
    allowResize_ = enable;

    LONG style = GetWindowLong(hwnd_, GWL_STYLE);
    if (enable) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    } else {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    SetWindowLong(hwnd_, GWL_STYLE, style);

    // 反映のためにウィンドウを再配置
    SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

// ウィンドウプロシャージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // これ書かないとImgui使えない
    extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    // ImGui にメッセージを渡す
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    {
        return true; // ImGuiが処理したらそれを返す
    }
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // ウィンドウのリサイズ禁止の場合は処理する
    case WM_GETMINMAXINFO:
    {
        WinApp* window = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (window && !window->allowResize_) {
            MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lparam);
            RECT rect = { 0,0,kClientWidth_,kClientHeight_ }; // 固定サイズ
            AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
            info->ptMinTrackSize.x = rect.right - rect.left;
            info->ptMinTrackSize.y = rect.bottom - rect.top;
            info->ptMaxTrackSize.x = rect.right - rect.left;
            info->ptMaxTrackSize.y = rect.bottom - rect.top;
            return 0;
        }
    }
    break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Finalize() {
    CloseWindow(hwnd_);
    CoUninitialize();
}