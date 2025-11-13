#pragma once
#include <wrl.h>
#include <wrl/client.h>

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <dxgidebug.h>

#include <cassert>
#include <format>
#include <filesystem>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"
#endif

#include"Object3dModelData.h"
#include"Object3dMaterialData.h"

#include "../Object3d/Animation/AnimationStruct.h"

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

DirectX::ScratchImage LoadTexture(const std::string& filePath);

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

Object3dMaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

Object3dModelData LoadObject3dFile(const std::string& filepath);

Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);