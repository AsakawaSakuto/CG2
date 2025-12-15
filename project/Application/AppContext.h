#pragma once
#include "Core/WinApp/WinApp.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "KeyConfig/KeyConfig.h"

struct AppContext {
	DirectXCommon dxCommon;
	KeyConfig keyConfig;
};