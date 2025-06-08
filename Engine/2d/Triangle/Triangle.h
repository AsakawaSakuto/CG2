#pragma once
#pragma region Include
#include"DirectXCommon.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>
#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include "externals/DirectXTex/DirectXTex.h"
#pragma endregion

class Triangle
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

private:
	DirectXCommon* dxCommon_ = nullptr;
};