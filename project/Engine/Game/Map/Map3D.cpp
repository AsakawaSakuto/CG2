#include "Map3D.h"
#include <fstream>
#include <cmath>
#include <windows.h>

void Map3D::Initialize(int sizeX, int sizeY, int sizeZ) {
    assert(sizeX > 0 && sizeY > 0 && sizeZ > 0 && "Map size must be positive");
    
    sizeX_ = sizeX;
    sizeY_ = sizeY;
    sizeZ_ = sizeZ;
    
    // メモリ確保（全て空で初期化）
    mapData_.resize(sizeX * sizeY * sizeZ, TileType::EMPTY);

    char buffer[256];
    sprintf_s(buffer, "Map3D initialized: %dx%dx%d (%d tiles)\n", 
              sizeX, sizeY, sizeZ, sizeX * sizeY * sizeZ);
    OutputDebugStringA(buffer);
}

TileType Map3D::GetTile(int x, int y, int z) const {
    if (!IsInBounds(x, y, z)) {
        return TileType::EMPTY;
    }
    return mapData_[ToIndex(x, y, z)];
}

void Map3D::SetTile(int x, int y, int z, TileType type) {
    if (IsInBounds(x, y, z)) {
        mapData_[ToIndex(x, y, z)] = type;
    }
}

Vector3 Map3D::WorldToTile(const Vector3& worldPos, float tileSize) const {
    return {
        std::floor(worldPos.x / tileSize),
        std::floor(worldPos.y / tileSize),
        std::floor(worldPos.z / tileSize)
    };
}

Vector3 Map3D::TileToWorld(int x, int y, int z, float tileSize) const {
    return {
        (x + 0.5f) * tileSize,
        (y + 0.5f) * tileSize,
        (z + 0.5f) * tileSize
    };
}

bool Map3D::IsInBounds(int x, int y, int z) const {
    return x >= 0 && x < sizeX_ && 
           y >= 0 && y < sizeY_ && 
           z >= 0 && z < sizeZ_;
}

bool Map3D::IsSolid(int x, int y, int z) const {
    TileType type = GetTile(x, y, z);
    
    // 固体タイプの判定
    switch (type) {
        case TileType::WALL:
        case TileType::CEILING:
        case TileType::SPIKE:
            return true;
        default:
            return false;
    }
}

void Map3D::Clear(TileType fillType) {
    std::fill(mapData_.begin(), mapData_.end(), fillType);
    
    OutputDebugStringA("Map3D cleared\n");
}

void Map3D::LoadFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        char buffer[512];
        sprintf_s(buffer, "Failed to open map file: %s\n", filePath.c_str());
        OutputDebugStringA(buffer);
        assert(false && "Map file not found");
        return;
    }

    // マップサイズを読み込み
    file.read(reinterpret_cast<char*>(&sizeX_), sizeof(int));
    file.read(reinterpret_cast<char*>(&sizeY_), sizeof(int));
    file.read(reinterpret_cast<char*>(&sizeZ_), sizeof(int));
    file.read(reinterpret_cast<char*>(&tileSize_), sizeof(float));

    // データサイズの検証
    int totalSize = sizeX_ * sizeY_ * sizeZ_;
    if (totalSize <= 0 || totalSize > 1000000) { // 100万タイル以上は異常
        OutputDebugStringA("Invalid map size in file\n");
        file.close();
        assert(false && "Invalid map data");
        return;
    }

    // データを読み込み
    mapData_.resize(totalSize);
    file.read(reinterpret_cast<char*>(mapData_.data()), mapData_.size() * sizeof(TileType));
    
    file.close();

    char buffer[256];
    sprintf_s(buffer, "Map loaded: %s (%dx%dx%d)\n", 
              filePath.c_str(), sizeX_, sizeY_, sizeZ_);
    OutputDebugStringA(buffer);
}

void Map3D::SaveToFile(const std::string& filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        char buffer[512];
        sprintf_s(buffer, "Failed to create map file: %s\n", filePath.c_str());
        OutputDebugStringA(buffer);
        assert(false && "Cannot create map file");
        return;
    }

    // マップサイズを書き込み
    file.write(reinterpret_cast<const char*>(&sizeX_), sizeof(int));
    file.write(reinterpret_cast<const char*>(&sizeY_), sizeof(int));
    file.write(reinterpret_cast<const char*>(&sizeZ_), sizeof(int));
    file.write(reinterpret_cast<const char*>(&tileSize_), sizeof(float));

    // データを書き込み
    file.write(reinterpret_cast<const char*>(mapData_.data()), mapData_.size() * sizeof(TileType));
    
    file.close();

    char buffer[256];
    sprintf_s(buffer, "Map saved: %s (%dx%dx%d)\n", 
              filePath.c_str(), sizeX_, sizeY_, sizeZ_);
    OutputDebugStringA(buffer);
}

void Map3D::GenerateTestMap() {
    if (mapData_.empty()) {
        OutputDebugStringA("Map not initialized. Call Initialize() first.\n");
        return;
    }

    // 全体をクリア
    Clear(TileType::EMPTY);

    // 地面を配置（Y=0の全タイル）
    for (int x = 0; x < sizeX_; ++x) {
        for (int z = 0; z < sizeZ_; ++z) {
            SetTile(x, 0, z, TileType::FLOOR);
        }
    }

    // 外周の壁を配置
    for (int y = 1; y < sizeY_; ++y) {
        // X方向の壁
        for (int x = 0; x < sizeX_; ++x) {
            SetTile(x, y, 0, TileType::WALL);
            SetTile(x, y, sizeZ_ - 1, TileType::WALL);
        }
        
        // Z方向の壁
        for (int z = 0; z < sizeZ_; ++z) {
            SetTile(0, y, z, TileType::WALL);
            SetTile(sizeX_ - 1, y, z, TileType::WALL);
        }
    }

    // スポーン地点（中央）
    int centerX = sizeX_ / 2;
    int centerZ = sizeZ_ / 2;
    SetTile(centerX, 1, centerZ, TileType::SPAWN_POINT);

    // ゴール地点（奥の中央）
    SetTile(centerX, 1, sizeZ_ - 5, TileType::GOAL);

    // いくつか障害物を配置
    for (int i = 0; i < 5; ++i) {
        int x = 5 + i * 5;
        int z = 5;
        if (x < sizeX_ - 1 && z < sizeZ_ - 1) {
            SetTile(x, 1, z, TileType::WALL);
            SetTile(x, 2, z, TileType::WALL);
        }
    }

    // ジャンプ台をいくつか配置
    SetTile(10, 1, 10, TileType::JUMP_PAD);
    SetTile(20, 1, 15, TileType::JUMP_PAD);

    // 水たまり
    for (int x = 15; x < 20; ++x) {
        for (int z = 20; z < 25; ++z) {
            if (x < sizeX_ && z < sizeZ_) {
                SetTile(x, 0, z, TileType::WATER);
            }
        }
    }

    OutputDebugStringA("Test map generated successfully\n");
}

bool Map3D::Raycast(const Vector3& origin, const Vector3& direction, float maxDistance, Vector3& hitPoint) const {
    // 正規化された方向ベクトル
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length < 0.0001f) return false;

    Vector3 normalizedDir = {
        direction.x / length,
        direction.y / length,
        direction.z / length
    };

    // レイの進行ステップ（タイルサイズの半分）
    float stepSize = tileSize_ * 0.5f;
    int maxSteps = static_cast<int>(maxDistance / stepSize);

    // レイを進めながらチェック
    for (int step = 0; step < maxSteps; ++step) {
        float distance = step * stepSize;
        Vector3 currentPos = {
            origin.x + normalizedDir.x * distance,
            origin.y + normalizedDir.y * distance,
            origin.z + normalizedDir.z * distance
        };

        // タイル座標に変換
        Vector3 tilePos = WorldToTile(currentPos, tileSize_);
        int tileX = static_cast<int>(tilePos.x);
        int tileY = static_cast<int>(tilePos.y);
        int tileZ = static_cast<int>(tilePos.z);

        // 固体タイルに当たったか判定
        if (IsSolid(tileX, tileY, tileZ)) {
            hitPoint = currentPos;
            return true;
        }
    }

    return false;
}
