#pragma once
#include "Utility/ConvertString/ConvertString.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/CreateResource/CreateResource.h"

#include <d3d12.h>
#include <dxcapi.h>    
#include <dxgi1_6.h>
#include <dxgidebug.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <wrl.h>
#include <wrl/client.h>

#include "DirectXTex.h"
#include "Math/Type/Vector2.h"

class TextureManager
{
public:
	//
	static TextureManager* GetInstance();

	//
	void Finalize();

	//
	void Initialize(DirectXCommon* dxCommon);

	//
	void LoadTexture(const std::string& filePath);

	//
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	// テクスチャのサイズを取得
	Vector2 GetTextureSize(const std::string& filePath);
	Vector2 GetTextureSizeByIndex(uint32_t textureIndex);

	/// <summary>
	/// テクスチャのメタデータを取得
	/// </summary>
	/// <param name="textureIndex">テクスチャインデックス</param>
	/// <returns>テクスチャのメタデータ</returns>
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex) const;

	//
	size_t GetTextureCount() const {return textureDatas_.size();}

	//
	size_t GetPathToIndexMapSize() const {return texturePathToIndex_.size();}

private:
	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	static TextureManager* instance;
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;
	//
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata matadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};
	//
	std::vector<TextureData> textureDatas_;
	//
	static uint32_t kSRVIndexTop_;
	//
	std::unordered_map<std::string, uint32_t> texturePathToIndex_;
};