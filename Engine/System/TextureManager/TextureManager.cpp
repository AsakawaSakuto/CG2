#include "TextureManager.h"
#include <filesystem>

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
        textureDatas_.begin(), textureDatas_.end(),
        [&](const TextureData& data) {
            return data.filePath == filePath;
        });

    // 見つかればそのインデックスを返す
    if (it != textureDatas_.end()) {
        uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas_.begin(), it));
        return textureIndex + kSRVIndexTop_;
    }

    // なければロードしてからもう一度取得
    LoadTexture(filePath);

    // ※再帰ではなく再検索（安全性とデバッグ性のため）
    auto it2 = std::find_if(
        textureDatas_.begin(), textureDatas_.end(),
        [&](const TextureData& data) {
            return data.filePath == filePath;
        });
    assert(it2 != textureDatas_.end()); // ここで見つからないなら Load に失敗している
    uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas_.begin(), it2));
    return textureIndex + kSRVIndexTop_;
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
    // 重複チェックは最初にやる
    if (texturePathToIndex_.contains(filePath)) {
        return; // すでに読み込み済み
    }

    // ファイルから読み込み
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);

    if (!std::filesystem::exists(filePathW)) {
        Logger::Log("File not found: " + filePath);
    }

    HRESULT hr = DirectX::LoadFromWICFile(
        filePathW.c_str(),
        DirectX::WIC_FLAGS_FORCE_SRGB,
        nullptr,
        image);
    assert(SUCCEEDED(hr));

    // ミップマップ生成
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(
        image.GetImages(),
        image.GetImageCount(),
        image.GetMetadata(),
        DirectX::TEX_FILTER_SRGB,
        0,
        mipImages);
    assert(SUCCEEDED(hr));

    // 新規テクスチャデータを追加
    textureDatas_.emplace_back();
    TextureData& textureData = textureDatas_.back();

    textureData.filePath = filePath;
    textureData.matadata = mipImages.GetMetadata();
    textureData.resource = CreateTextureResource(device_.Get(), textureData.matadata);
    UploadTextureData(textureData.resource.Get(), mipImages);

    // SRV登録位置を計算
    uint32_t index = static_cast<uint32_t>(textureDatas_.size() - 1);
    uint32_t srvIndex = index + kSRVIndexTop_;
    
    // // ★テクスチャ領域の上限を超えない保証（超えたら即落として原因を洗う）
    assert(srvIndex >= DirectXCommon::kTextureSRVBegin);
    assert(srvIndex <= DirectXCommon::kTextureSRVEnd && "Texture SRV range exceeded. Increase range or free slots.");

    textureData.srvHandleCPU = GetCPUDescriptorHandle(
        dxCommon_->GetSRV().Get(),
        dxCommon_->GetDescriptorSizeSRV(),
        srvIndex);

    textureData.srvHandleGPU = GetGPUDescriptorHandle(
        dxCommon_->GetSRV().Get(),
        dxCommon_->GetDescriptorSizeSRV(),
        srvIndex);

    // SRV作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureData.matadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(textureData.matadata.mipLevels);
    device_->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

    // パス → インデックスの登録を忘れずに
    texturePathToIndex_[filePath] = index;
}