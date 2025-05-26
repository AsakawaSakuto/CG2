#include "Input.h"
#include <cassert> 
#include <wrl.h>
using namespace Microsoft::WRL;

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	ComPtr<IDirectInput8> directInput;
	result_ = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result_));

	ComPtr<IDirectInputDevice8> keyboard;
	result_ = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result_));

	result_ = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result_));

	result_ = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result_));
}

void Input::Update() {

}