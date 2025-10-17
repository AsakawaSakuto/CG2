#pragma once
#include"WinApp.h"
#include"DirectXCommon.h"
#include"Input.h"
#include"GamePad.h"

struct AppContext {
	WinApp winApp;
	DirectXCommon dxCommon;
	Input input;
	GamePad gamePad;
	float lastScore = 0.0f;
};