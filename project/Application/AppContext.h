#pragma once
#include"WinApp.h"
#include"DirectXCommon.h"
#include"Input.h"
#include"GamePad.h"

struct AppContext {
	DirectXCommon dxCommon;
	Input input;
	GamePad gamePad;
};