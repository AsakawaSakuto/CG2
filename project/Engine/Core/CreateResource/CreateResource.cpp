#include"CreateResource.h"
#include<cassert>

#include"Utility/ConvertString/ConvertString.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include <fstream>
#include <sstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Microsoft::WRL;

// HRESULT

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
    ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(hr));
    return descriptorHeap;
}

ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {

    // 生産するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width; // Textureの幅
    resourceDesc.Height = height; // Textureの高さ
    resourceDesc.MipLevels = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 深度 or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1; // サンプリングの数。通常は1
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う場合

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    /// 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f（最大値）でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット、Resourceに合わせる

    // Resourceの生成
    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
        &depthClearValue, // Clear用初期値
        IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
    assert(SUCCEEDED(hr));

    return resource;
}

ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
    // ★サイズチェック追加
    if (sizeInBytes == 0) {
        OutputDebugStringA("ERROR: CreateBufferResource called with size 0!\n");
        assert(false && "Buffer size is 0");
    }
    
    if (sizeInBytes > 256 * 1024 * 1024) { // 256MB以上は警告
        char buffer[256];
        sprintf_s(buffer, "WARNING: Creating large buffer: %zu bytes (%.2f MB)\n", 
                  sizeInBytes, sizeInBytes / (1024.0f * 1024.0f));
        OutputDebugStringA(buffer);
    }
    
    // アップロードヒープの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 0;
    heapProperties.VisibleNodeMask = 0;

    // リソースの設定（バッファ用）
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = sizeInBytes;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // リソースの作成
    ComPtr<ID3D12Resource> bufferResource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, // Upload Heap に必要な初期状態
        nullptr,
        IID_PPV_ARGS(&bufferResource)
    );

    // ★エラー情報を詳しく出力
    if (FAILED(hr)) {
        char buffer[512];
        sprintf_s(buffer, "ERROR: CreateCommittedResource failed!\n");
        OutputDebugStringA(buffer);
        sprintf_s(buffer, "  HRESULT: 0x%08X\n", hr);
        OutputDebugStringA(buffer);
        sprintf_s(buffer, "  Requested size: %zu bytes (%.2f MB)\n", 
                  sizeInBytes, sizeInBytes / (1024.0f * 1024.0f));
        OutputDebugStringA(buffer);
        
        // よくあるエラーコードの説明
        if (hr == E_OUTOFMEMORY) {
            OutputDebugStringA("  Error: E_OUTOFMEMORY - Not enough memory available\n");
        } else if (hr == E_INVALIDARG) {
            OutputDebugStringA("  Error: E_INVALIDARG - Invalid argument\n");
        }
    }

    assert(SUCCEEDED(hr));
    return bufferResource;
}

ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
    // metadataを基にResourceの設定 2-1
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width); // Textureの幅
    resourceDesc.Height = UINT(metadata.height); // Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format; // TextureのFormat
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。昔使っているのは2次元

    // 利用するHeapの設定 2-2
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

    // Resourceの生成 2-3
    ComPtr<ID3D12Resource> resource;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    return resource;
}

DirectX::ScratchImage LoadTexture(const std::string& filePath) {

    // テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
    assert(SUCCEEDED(hr));

    // ミップマップ付きのデータを返す
    return mipImages;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
    // Meta情報を取得
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    // 全MipMapについて
    for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
        // MipMapLevelを指定して各Imageを取得
        const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
        // Textureに転送
        HRESULT hr_ = texture->WriteToSubresource(
            UINT(mipLevel),
            nullptr,                // 全領域へコピー
            img->pixels,            // 元データアドレス
            UINT(img->rowPitch),    // 1ラインサイズ
            UINT(img->slicePitch)   // 1枚サイズ
        );
        assert(SUCCEEDED(hr_));
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

ModeldMaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    // 1.中で必要となる変数の宣言
    ModeldMaterialData materialData;  // 構築するMaterialData

    // 2.ファイルを開く
    std::string line;                                   // ファイルから読んだ1行を格納するもの
    std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
    assert(file.is_open());                             // とりあえず開けなかったら止める

    // 3.実際にファイルを読み、MaterialDataを構築していく
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // identifierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            // 連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFilename;
        }
    }
    // 4.MaterialData を返す
    return materialData;
}

ModelData LoadObject3dFile(const std::string& filepath) {
    ModelData modelData;
    std::filesystem::path path(filepath);
    std::string directoryPath = path.parent_path().string();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath.c_str(),
        aiProcess_FlipWindingOrder |
        aiProcess_FlipUVs |
        aiProcess_Triangulate);

    assert(scene && scene->HasMeshes());

    modelData.rootNode = modelData.rootNode.ReadNode(scene->mRootNode);

    // マテリアルを先に読み込む
    modelData.materials.resize(scene->mNumMaterials);
    for (uint32_t matIndex = 0; matIndex < scene->mNumMaterials; ++matIndex) {
        aiMaterial* material = scene->mMaterials[matIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString texPath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
                std::string textureFilePath = directoryPath + "/" + texPath.C_Str();
                
                // uvChecker.pngが設定されている場合、white1x1.pngに置き換え
                if (textureFilePath.find("uvChecker.png") != std::string::npos) {
                    modelData.materials[matIndex].textureFilePath = "resources/image/white1x1.png";
                } else {
                    modelData.materials[matIndex].textureFilePath = textureFilePath;
                }
            } else {
                modelData.materials[matIndex].textureFilePath = "resources/image/white1x1.png";
            }
        } else {
            modelData.materials[matIndex].textureFilePath = "resources/image/white1x1.png";
        }
    }

    // マテリアルがない場合のデフォルト
    if (modelData.materials.empty()) {
        modelData.materials.push_back({ "resources/image/white1x1.png" });
    }

    // 各メッシュを処理
    uint32_t indexOffset = 0;
    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals());
        // テクスチャ座標のチェックを削除
        // assert(mesh->HasTextureCoords(0));

        uint32_t vertexStart = static_cast<uint32_t>(modelData.vertices.size());

        // 頂点データを追加
        for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
            ModelVertexData vertex;
            aiVector3D& position = mesh->mVertices[vertexIndex];
            aiVector3D& normal = mesh->mNormals[vertexIndex];
            
            // 右手系→左手系への変換
            vertex.position = { -position.x, position.y, position.z, 1.0f };
            vertex.normal = { -normal.x, normal.y, normal.z };
            
            // テクスチャ座標が存在する場合のみ取得、なければデフォルト値(0,0)
            if (mesh->HasTextureCoords(0)) {
                aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
                vertex.texcoord = { texcoord.x, texcoord.y };
            } else {
                vertex.texcoord = { 0.0f, 0.0f };
            }
            
            modelData.vertices.push_back(vertex);
        }

        // インデックスデータを追加
        uint32_t indexStart = static_cast<uint32_t>(modelData.indeces.size());
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3); // 三角形のみ対応

            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIndex = face.mIndices[element];
                modelData.indeces.push_back(vertexStart + vertexIndex);
            }
        }
        uint32_t indexCount = static_cast<uint32_t>(modelData.indeces.size()) - indexStart;

        // サブメッシュ情報を追加
        ModelSubMesh subMesh;
        subMesh.indexStart = indexStart;
        subMesh.indexCount = indexCount;
        subMesh.materialIndex = mesh->mMaterialIndex;
        // マテリアルインデックスが範囲外の場合はクランプ
        if (subMesh.materialIndex >= modelData.materials.size()) {
            subMesh.materialIndex = 0;
        }
        modelData.subMeshes.push_back(subMesh);

        // スキニング情報の処理
        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            aiBone* bone = mesh->mBones[boneIndex];
            std::string jointName = bone->mName.C_Str();
            JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

            aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
            aiVector3D scale, translate;
            aiQuaternion rotate;
            bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
            Matrix4x4 bindPoseMatrix = MakeAffineAnimationMatrix(
                { scale.x, scale.y, scale.z }, { rotate.x, -rotate.y, -rotate.z, rotate.w }, { -translate.x, translate.y, translate.z });
            jointWeightData.inverseBindPoseMatrix = InverseMatrix(bindPoseMatrix);

            for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
                jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, vertexStart + bone->mWeights[weightIndex].mVertexId });
            }
        }
    }

    // 後方互換性のため、最初のマテリアルを material に設定
    if (!modelData.materials.empty()) {
        modelData.material = modelData.materials[0];
    } else {
        modelData.material.textureFilePath = "resources/image/uvChecker.png";
    }
    
    return modelData;
}