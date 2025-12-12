#pragma once
#include <Windows.h>
#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_win32.h"
#endif
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>
#include <wrl.h>
#include <windowsx.h>

class WinApp {
public:
    ~WinApp();

    void Initialize(const wchar_t* title);
    bool ProcessMessage();
    HWND GetHWND() const { return hwnd_; }
    HINSTANCE GetInstance() const { return wc_.hInstance; }

    void SetTitle(const std::wstring& title);

    // クライアント領域サイズを返す関数はそのまま
    void GetClientSize(uint32_t& width, uint32_t& height) const;

    void EnableResize(bool enable);
    void Finalize();

    // アイコン設定機能
    void SetIconFromTexture(const std::string& texturePath);
    void SetIconFromTexture(const std::wstring& texturePath);

    // 既定サイズ（初期ウィンドウ作成にのみ使用）
    static const int32_t kClientWidth_ = 1280;
    static const int32_t kClientHeight_ = 720;

    // "動的取得"に変更（ハードコード返却を廃止）
    int32_t GetWidth()  const { RECT rc{}; GetClientRect(hwnd_, &rc); return rc.right - rc.left; }
    int32_t GetHeight() const { RECT rc{}; GetClientRect(hwnd_, &rc); return rc.bottom - rc.top; }

    short GetWheelDelta() const { return wheelDelta_; }
    void  AddWheelDelta(short delta) { wheelDelta_ += delta; }
    void  ResetWheelDelta() { wheelDelta_ = 0; }

    bool MouseLButtonDown() const { return isLButtonDown_; }
    bool MouseRButtonDown() const { return isRButtonDown_; }
    void SetLButtonDown(bool flag) { isLButtonDown_ = flag; }
    void SetRButtonDown(bool flag) { isRButtonDown_ = flag; }

    LONG GetMouseX() const { return mouseX_; }
    LONG GetMouseY() const { return mouseY_; }
    void SetMousePosition(LONG x, LONG y) { mouseX_ = x; mouseY_ = y; }

    // ボーダーレス・フルスクリーン制御API
    bool IsFullscreen() const { return isFullscreen_; }
    void EnterBorderlessFullscreen();
    void ExitBorderlessFullscreen();
private:
    HWND hwnd_ = {};
    WNDCLASS wc_ = {};
    bool allowResize_ = true;
    short wheelDelta_ = 0;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    bool isLButtonDown_ = false;
    bool isRButtonDown_ = false;
    LONG mouseX_ = 0;
    LONG mouseY_ = 0;

    // アイコン関連
    HICON hIcon_ = nullptr;
    HICON hIconSmall_ = nullptr;

    // プライベートヘルパー関数
    HICON LoadIconFromTexture(const std::wstring& texturePath, int iconSize);
    void SetWindowIcon();
    void CleanupIcons();

    // フルスクリーン復帰用の保存値
    bool  isFullscreen_ = false; 
    DWORD windowStyle_ = 0;      
    DWORD windowExStyle_ = 0;    
    RECT  windowRectBeforeFS_{}; 
};
