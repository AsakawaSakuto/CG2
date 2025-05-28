#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <wrl.h>
#include <d3d12.h>
//#include "d3dx12.h"
#include <dxcapi.h>
using namespace Microsoft::WRL;

class Input
{
public:
	void Initialize(HINSTANCE hInstance,HWND hwnd);
	void Update();
	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);
private:
	HRESULT result_;
	BYTE key_[256] = {};
	BYTE keyPre_[256] = {};
	ComPtr<IDirectInput8> directInput_;
	ComPtr<IDirectInputDevice8> keyboard_;
};