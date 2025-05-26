#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Input
{
public:
	void Initialize(HINSTANCE hInstance,HWND hwnd);
	void Update();
private:
	HRESULT result_;
};