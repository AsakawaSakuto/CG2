#pragma once
#include <Windows.h>
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_win32.h"
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>
#include <wrl.h> 

class WinApp {
public:
    ~WinApp();

    // ウィンドウを初期化
    void Initialize(const wchar_t* title);

    // Windowsメッセージを処理
    bool ProcessMessage();

    // ウィンドウハンドルを取得
    HWND GetHWND() const { return hwnd_; }

    //
    HINSTANCE GetInstance() const { return wc_.hInstance; }

    // ウィンドウのタイトルを変更
    void SetTitle(const std::wstring& title);

    // クライアント領域サイズを取得
    void GetClientSize(uint32_t& width, uint32_t& height) const;

    // ウィンドウリサイズを有効/無効化
    void EnableResize(bool enable);

    //
    void Finalize();

private:
    HWND hwnd_ = {};
    WNDCLASS wc_ = {};
    bool allowResize_ = true; // リサイズ許可フラグ

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    static const int32_t kClientWidth_ = 1280;
    static const int32_t kClientHeight_ = 720;
};