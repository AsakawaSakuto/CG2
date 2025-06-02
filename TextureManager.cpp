#include "TextureManager.h"

TextureManager* TextureManager::instance = nullptr;

uint32_t TextureManager::kSRVIndexTop_ = 1;

TextureManager* TextureManager::GetInstance() {
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    device_ = dxCommon_->GetDevice();
	//
	textureDatas_.reserve(DirectXCommon::kMaxSRVCount_);
}

void TextureManager::Finalize() {
	delete instance;
	instance = nullptr;
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath) {
    // 読み込み済みテクスチャデータを検索
    auto it = std::find_if(
        textureDatas_.begin(),
        textureDatas_.end(),
        [&](const TextureData& data) {
            return data.filePath == filePath;
        });
    if (it != textureDatas_.end()) {
        // 読み込み済みなら要素番号を返す
        uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas_.begin(), it));
        return textureIndex + kSRVIndexTop_;
    }

    LoadTexture(filePath);
    return static_cast<uint32_t>(textureDatas_.size() - 1) + kSRVIndexTop_;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex) {
    // 範囲外アクセスチェック
    assert(textureIndex >= kSRVIndexTop_);
    uint32_t index = textureIndex - kSRVIndexTop_;
    assert(index < textureDatas_.size());

    // テクスチャデータの参照を取得
    TextureData& textureData = textureDatas_[index];
    return textureData.srvHandleGPU;
}

void TextureManager::LoadTexture(const std::string& filePath) {

    // テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
    assert(SUCCEEDED(hr));

    //
    textureDatas_.resize(textureDatas_.size() + 1);
    //
    TextureData& textureData = textureDatas_.back();

    //
    textureData.filePath = filePath;
    textureData.matadata = mipImages.GetMetadata();
    textureData.resource = CreateTextureResource(device_.Get(), textureData.matadata);
    UploadTextureData(textureData.resource.Get(), mipImages);
    //
    uint32_t srvIndex = static_cast<uint32_t>(textureDatas_.size() - 1) + kSRVIndexTop_;

    textureData.srvHandleCPU = GetCPUDescriptorHandle(dxCommon_->GetSRV().Get(), dxCommon_->GetDescriptorSizeSRV(), srvIndex);
    textureData.srvHandleGPU = GetGPUDescriptorHandle(dxCommon_->GetSRV().Get(), dxCommon_->GetDescriptorSizeSRV(), srvIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureData.matadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(textureData.matadata.mipLevels);
    //
    assert(textureData.resource);
    device_.Get()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

    //
    auto it = std::find_if(
        textureDatas_.begin(),
        textureDatas_.end(),
        [&](TextureData& textureData) {return textureData.filePath == filePath; }
    );
    if (it != textureDatas_.end()) {
        //
        return;
    }

    //
    assert(textureDatas_.size() + kSRVIndexTop_ < DirectXCommon::kMaxSRVCount_);
}
