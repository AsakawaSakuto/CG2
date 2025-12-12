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
    // テクスチャリソースをクリア
    textureDatas_.clear();
    texturePathToIndex_.clear();
    
    // デバイス参照をクリア
    device_.Reset();
    dxCommon_ = nullptr;
    
    // インスタンスを削除
	delete instance;
	instance = nullptr;
    
    OutputDebugStringA("TextureManager::Finalize completed - All textures released\n");
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

Vector2 TextureManager::GetTextureSize(const std::string& filePath) {
    // まず該当するテクスチャがロード済みかチェック
    auto it = std::find_if(
        textureDatas_.begin(), textureDatas_.end(),
        [&](const TextureData& data) {
            return data.filePath == filePath;
        });

    if (it == textureDatas_.end()) {
        // ロードされていなければ先にロード
        LoadTexture(filePath);
        
        // 再度検索
        it = std::find_if(
            textureDatas_.begin(), textureDatas_.end(),
            [&](const TextureData& data) {
                return data.filePath == filePath;
            });
        
        // ロード後も見つからない場合は緊急処理
        if (it == textureDatas_.end()) {
            Logger::Error("FATAL: Texture not found after loading: " + filePath);
            // デフォルトサイズを返す（クラッシュ防止）
            return Vector2{ 1.0f, 1.0f };
        }
    }
    
    return Vector2{
        static_cast<float>(it->matadata.width),
        static_cast<float>(it->matadata.height)
    };
}

Vector2 TextureManager::GetTextureSizeByIndex(uint32_t textureIndex) {
    // 範囲外アクセスチェック
    assert(textureIndex >= kSRVIndexTop_);
    uint32_t index = textureIndex - kSRVIndexTop_;
    assert(index < textureDatas_.size());

    // テクスチャデータの参照を取得
    const TextureData& textureData = textureDatas_[index];
    return Vector2{
        static_cast<float>(textureData.matadata.width),
        static_cast<float>(textureData.matadata.height)
    };
}

void TextureManager::LoadTexture(const std::string& filePath) {
    // 先に filePath の完全コピーを作成
    std::string safeFilePath = filePath;
    
    OutputDebugStringA(("Loading texture: " + safeFilePath + "\n").c_str());
    
    // 重複チェック（コピーした文字列で）
    if (texturePathToIndex_.contains(safeFilePath)) {
        OutputDebugStringA("  -> Already loaded\n");
        return;
    }
    
    OutputDebugStringA(("  -> New texture, index=" + std::to_string(textureDatas_.size()) + "\n").c_str());

    // ファイルから読み込み
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(safeFilePath);

    if (!std::filesystem::exists(filePathW)) {
        Logger::Error("Texture file not found: " + safeFilePath);
        return;
    }

    // sRGB変換を無効化（リニア色空間で読み込む）
    HRESULT hr = DirectX::LoadFromWICFile(
        filePathW.c_str(),
        DirectX::WIC_FLAGS_NONE,  // WIC_FLAGS_FORCE_SRGBを削除
        nullptr,
        image);
    
    if (FAILED(hr)) {
        Logger::Error("Failed to load texture (WIC): " + safeFilePath);
        return;
    }

    // ミップマップ生成（リニアフィルタに変更）
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(
        image.GetImages(),
        image.GetImageCount(),
        image.GetMetadata(),
        DirectX::TEX_FILTER_DEFAULT,  // TEX_FILTER_SRGBをDEFAULTに変更
        0,
        mipImages);
    
    if (FAILED(hr)) {
        Logger::Error("Failed to generate mipmaps: " + safeFilePath);
        return;
    }

    // 先に index を確保
    uint32_t index = static_cast<uint32_t>(textureDatas_.size());
    
    // 先に map に登録（これで safeFilePath の寿命が保証される）
    texturePathToIndex_[safeFilePath] = index;
    
    // その後で textureData を追加
    textureDatas_.emplace_back();
    TextureData& textureData = textureDatas_.back();

    // map の key から取得（安全）
    auto it = texturePathToIndex_.find(safeFilePath);
    assert(it != texturePathToIndex_.end());
    textureData.filePath = it->first;  // map の key を参照（寿命が保証される）
    
    textureData.matadata = mipImages.GetMetadata();
    textureData.resource = CreateTextureResource(device_.Get(), textureData.matadata);
    UploadTextureData(textureData.resource.Get(), mipImages);

    // SRV登録位置を計算
    uint32_t srvIndex = index + kSRVIndexTop_;
    
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
}