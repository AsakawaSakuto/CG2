#include "WinApp.h"
#include <cassert>
#include <filesystem>
#include "DirectXTex.h"
#include "Utility/ConvertString/ConvertString.h"

WinApp::~WinApp() {
    CleanupIcons();
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
    
    // デフォルトアイコンを設定（後で変更可能）
    wc_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    
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

    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

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

void WinApp::SetIconFromTexture(const std::string& texturePath) {
    std::wstring wTexturePath = ConvertString(texturePath);
    SetIconFromTexture(wTexturePath);
}

void WinApp::SetIconFromTexture(const std::wstring& texturePath) {
    // ファイルが存在するかチェック
    if (!std::filesystem::exists(texturePath)) {
        OutputDebugStringA(("Icon texture file not found: " + ConvertString(texturePath) + "\n").c_str());
        return;
    }

    // 古いアイコンをクリーンアップ
    CleanupIcons();

    // 大きいアイコン (32x32) と小さいアイコン (16x16) を作成
    hIcon_ = LoadIconFromTexture(texturePath, 32);
    hIconSmall_ = LoadIconFromTexture(texturePath, 16);

    // ウィンドウにアイコンを設定
    SetWindowIcon();
}

HICON WinApp::LoadIconFromTexture(const std::wstring& texturePath, int iconSize) {
    try {
        // DirectXTexを使ってテクスチャを読み込み
        DirectX::ScratchImage image;
        HRESULT hr = DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
        if (FAILED(hr)) {
            OutputDebugStringA(("Failed to load texture: " + ConvertString(texturePath) + "\n").c_str());
            return nullptr;
        }

        // 画像をリサイズ
        DirectX::ScratchImage resizedImage;
        hr = DirectX::Resize(image.GetImages(), image.GetImageCount(), image.GetMetadata(), 
                           iconSize, iconSize, DirectX::TEX_FILTER_DEFAULT, resizedImage);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to resize texture for icon\n");
            return nullptr;
        }

        // RGBA形式に変換
        DirectX::ScratchImage rgbaImage;
        hr = DirectX::Convert(resizedImage.GetImages(), resizedImage.GetImageCount(), resizedImage.GetMetadata(),
                            DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, rgbaImage);
        if (FAILED(hr)) {
            OutputDebugStringA("Failed to convert texture to RGBA format\n");
            return nullptr;
        }

        const DirectX::Image* img = rgbaImage.GetImage(0, 0, 0);
        if (!img) {
            OutputDebugStringA("Failed to get image data\n");
            return nullptr;
        }

        // ビットマップ作成用のデータを準備
        BITMAPV5HEADER bi = {};
        bi.bV5Size = sizeof(BITMAPV5HEADER);
        bi.bV5Width = iconSize;
        bi.bV5Height = -iconSize; // 上下反転を防ぐため負の値
        bi.bV5Planes = 1;
        bi.bV5BitCount = 32;
        bi.bV5Compression = BI_BITFIELDS;
        bi.bV5RedMask = 0x00FF0000;
        bi.bV5GreenMask = 0x0000FF00;
        bi.bV5BlueMask = 0x000000FF;
        bi.bV5AlphaMask = 0xFF000000;

        // DIBセクションを作成
        HDC hdc = GetDC(nullptr);
        void* pBits = nullptr;
        HBITMAP hBitmap = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pBits, nullptr, 0);
        ReleaseDC(nullptr, hdc);

        if (!hBitmap || !pBits) {
            OutputDebugStringA("Failed to create DIB section\n");
            return nullptr;
        }

        // ピクセルデータをコピー（RGBAからBGRAに変換）    
        const uint8_t* srcPixels = img->pixels;
        uint8_t* destPixels = static_cast<uint8_t*>(pBits);
        
        for (int y = 0; y < iconSize; ++y) {
            for (int x = 0; x < iconSize; ++x) {
                int srcIndex = (y * iconSize + x) * 4;
                int destIndex = (y * iconSize + x) * 4;
                
                // RGBA -> BGRA変換
                destPixels[destIndex + 0] = srcPixels[srcIndex + 2]; // B
                destPixels[destIndex + 1] = srcPixels[srcIndex + 1]; // G
                destPixels[destIndex + 2] = srcPixels[srcIndex + 0]; // R
                destPixels[destIndex + 3] = srcPixels[srcIndex + 3]; // A
            }
        }

        // マスクビットマップを作成（アルファチャンネルがあるので不要だが、必要な場合がある）
        HBITMAP hMaskBitmap = CreateBitmap(iconSize, iconSize, 1, 1, nullptr);

        // アイコンを作成
        ICONINFO iconInfo = {};
        iconInfo.fIcon = TRUE;
        iconInfo.xHotspot = 0;
        iconInfo.yHotspot = 0;
        iconInfo.hbmMask = hMaskBitmap;
        iconInfo.hbmColor = hBitmap;

        HICON hIcon = CreateIconIndirect(&iconInfo);

        // リソースをクリーンアップ
        DeleteObject(hBitmap);
        DeleteObject(hMaskBitmap);

        if (!hIcon) {
            OutputDebugStringA("Failed to create icon\n");
        }

        return hIcon;
    }
    catch (const std::exception& e) {
        OutputDebugStringA(("Exception in LoadIconFromTexture: " + std::string(e.what()) + "\n").c_str());
        return nullptr;
    }
}

void WinApp::SetWindowIcon() {
    if (hwnd_) {
        if (hIcon_) {
            SendMessage(hwnd_, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon_));
        }
       /* if (hIconSmall_) {
            SendMessage(hwnd_, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIconSmall_));
        }*/
    }
}

void WinApp::CleanupIcons() {
    if (hIcon_) {
        DestroyIcon(hIcon_);
        hIcon_ = nullptr;
    }
    if (hIconSmall_) {
        DestroyIcon(hIconSmall_);
        hIconSmall_ = nullptr;
    }
}

// ウィンドウプロシャージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef USE_IMGUI
    // これ書かないとImgui使えない
    extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    // ImGui にメッセージを渡す
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    {
        return true; // ImGuiが処理したらそれを返す
    }
#endif
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
        break;
    }
    case WM_MOUSEWHEEL:
    {
        WinApp* window = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (window) {
            short delta = GET_WHEEL_DELTA_WPARAM(wparam);
            window->AddWheelDelta(delta);
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->SetLButtonDown(true);
        break;
    }
    case WM_LBUTTONUP:
    {
        reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->SetLButtonDown(false);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->SetRButtonDown(true);
        break;
    }
    case WM_RBUTTONUP:
    {
        reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->SetRButtonDown(false);
        break;
    }
    case WM_MOUSEMOVE: {
        WinApp* window = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (window) {
            LONG x = GET_X_LPARAM(lparam);
            LONG y = GET_Y_LPARAM(lparam);
            window->SetMousePosition(x, y);
        }
        break;
    }
    break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Finalize() {
    CleanupIcons();
    CloseWindow(hwnd_);
    CoUninitialize();
}

void WinApp::EnterBorderlessFullscreen() {
    if (isFullscreen_) return;
    HWND hwnd = GetHWND();

    // いまのスタイルと位置を保存
    windowStyle_ = GetWindowLong(hwnd, GWL_STYLE);
    windowExStyle_ = GetWindowLong(hwnd, GWL_EXSTYLE);
    GetWindowRect(hwnd, &windowRectBeforeFS_);

    // 表示中のモニタ全面サイズを取得
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfo(hMon, &mi);
    const RECT r = mi.rcMonitor;

    //枠を消して全面に拡張
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, 0);
    SetWindowPos(hwnd, HWND_TOP, r.left, r.top,
        r.right - r.left, r.bottom - r.top,
        SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_MAXIMIZE);

    isFullscreen_ = true;
}

void WinApp::ExitBorderlessFullscreen() {
    if (!isFullscreen_) return;
    HWND hwnd = GetHWND();

    // もとのスタイルと位置に戻す
    SetWindowLong(hwnd, GWL_STYLE, windowStyle_);
    SetWindowLong(hwnd, GWL_EXSTYLE, windowExStyle_);
    SetWindowPos(hwnd, nullptr,
        windowRectBeforeFS_.left, windowRectBeforeFS_.top,
        windowRectBeforeFS_.right - windowRectBeforeFS_.left,
        windowRectBeforeFS_.bottom - windowRectBeforeFS_.top,
        SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_SHOWNORMAL);

    isFullscreen_ = false;
}
