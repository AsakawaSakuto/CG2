#pragma once
#include "Core/WinApp/WinApp.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Input/Input.h"
#include "Input/GamePad.h"

struct AppContext {
	DirectXCommon dxCommon;
	Input input;
	GamePad gamePad;
};