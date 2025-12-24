#pragma once
#include "Map/Map3D.h"
#include <memory>

/// <summary>
/// Map3D の使用サンプルコード集
/// </summary>
namespace Map3DSamples {

    /// <summary>
    /// サンプル1: シンプルな平坦な床
    /// </summary>
    inline void CreateFlatFloor(Map3D& map) {
        for (uint32_t z = 0; z < map.GetDepth(); ++z) {
            for (uint32_t x = 0; x < map.GetWidth(); ++x) {
                map.SetTile(x, 0, z, TileType::Normal);
            }
        }
    }

    /// <summary>
    /// サンプル2: チェッカーボード模様の床
    /// </summary>
    inline void CreateCheckerboardFloor(Map3D& map) {
        for (uint32_t z = 0; z < map.GetDepth(); ++z) {
            for (uint32_t x = 0; x < map.GetWidth(); ++x) {
                // チェッカーボード模様
                if ((x + z) % 2 == 0) {
                    map.SetTile(x, 0, z, TileType::Normal);
                }
            }
        }
    }

    /// <summary>
    /// サンプル3: 外周の壁を作成
    /// </summary>
    inline void CreateWalls(Map3D& map, uint32_t wallHeight = 5) {
        for (uint32_t y = 0; y < wallHeight; ++y) {
            // X軸方向の壁
            for (uint32_t z = 0; z < map.GetDepth(); ++z) {
                map.SetTile(0, y, z, TileType::Normal);
                map.SetTile(map.GetWidth() - 1, y, z, TileType::Normal);
            }
            // Z軸方向の壁
            for (uint32_t x = 0; x < map.GetWidth(); ++x) {
                map.SetTile(x, y, 0, TileType::Normal);
                map.SetTile(x, y, map.GetDepth() - 1, TileType::Normal);
            }
        }
    }

    /// <summary>
    /// サンプル4: 階段を作成
    /// </summary>
    inline void CreateStairs(Map3D& map, uint32_t startX, uint32_t startZ, uint32_t length) {
        for (uint32_t i = 0; i < length; ++i) {
            if (startX + i < map.GetWidth()) {
                map.SetTile(startX + i, i, startZ, TileType::Normal);
            }
        }
    }

    /// <summary>
    /// サンプル5: ピラミッドを作成
    /// </summary>
    inline void CreatePyramid(Map3D& map, uint32_t centerX, uint32_t centerZ, uint32_t size) {
        for (uint32_t y = 0; y < size; ++y) {
            uint32_t offset = size - y - 1;
            for (uint32_t z = 0; z <= offset * 2; ++z) {
                for (uint32_t x = 0; x <= offset * 2; ++x) {
                    uint32_t worldX = centerX - offset + x;
                    uint32_t worldZ = centerZ - offset + z;
                    
                    if (worldX < map.GetWidth() && worldZ < map.GetDepth()) {
                        map.SetTile(worldX, y, worldZ, TileType::Normal);
                    }
                }
            }
        }
    }

    /// <summary>
    /// サンプル6: ランダムな柱を複数配置
    /// </summary>
    inline void CreateRandomPillars(Map3D& map, uint32_t count, uint32_t minHeight, uint32_t maxHeight) {
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t x = rand() % map.GetWidth();
            uint32_t z = rand() % map.GetDepth();
            uint32_t height = minHeight + (rand() % (maxHeight - minHeight + 1));

            for (uint32_t y = 0; y < height && y < map.GetHeight(); ++y) {
                map.SetTile(x, y, z, TileType::Normal);
            }
        }
    }

    /// <summary>
    /// サンプル7: ボックス（中空の箱）を作成
    /// </summary>
    inline void CreateHollowBox(Map3D& map, uint32_t startX, uint32_t startY, uint32_t startZ,
                                uint32_t width, uint32_t height, uint32_t depth) {
        for (uint32_t y = startY; y < startY + height && y < map.GetHeight(); ++y) {
            for (uint32_t z = startZ; z < startZ + depth && z < map.GetDepth(); ++z) {
                for (uint32_t x = startX; x < startX + width && x < map.GetWidth(); ++x) {
                    // 外枠のみ配置（中は空洞）
                    bool isEdge = (x == startX || x == startX + width - 1 ||
                                   y == startY || y == startY + height - 1 ||
                                   z == startZ || z == startZ + depth - 1);
                    if (isEdge) {
                        map.SetTile(x, y, z, TileType::Normal);
                    }
                }
            }
        }
    }

    /// <summary>
    /// サンプル8: 迷路の通路を作成（シンプル版）
    /// </summary>
    inline void CreateSimpleMaze(Map3D& map) {
        // 全体を埋める
        for (uint32_t z = 0; z < map.GetDepth(); ++z) {
            for (uint32_t x = 0; x < map.GetWidth(); ++x) {
                for (uint32_t y = 0; y < 3; ++y) {
                    map.SetTile(x, y, z, TileType::Normal);
                }
            }
        }

        // 通路を掘る（単純な格子状）
        for (uint32_t z = 1; z < map.GetDepth() - 1; z += 2) {
            for (uint32_t x = 1; x < map.GetWidth() - 1; x += 2) {
                map.SetTile(x, 1, z, TileType::Empty);
                map.SetTile(x, 2, z, TileType::Empty);
                
                // ランダムに横方向の通路
                if (x + 1 < map.GetWidth() - 1 && rand() % 2 == 0) {
                    map.SetTile(x + 1, 1, z, TileType::Empty);
                    map.SetTile(x + 1, 2, z, TileType::Empty);
                }
                
                // ランダムに縦方向の通路
                if (z + 1 < map.GetDepth() - 1 && rand() % 2 == 0) {
                    map.SetTile(x, 1, z + 1, TileType::Empty);
                    map.SetTile(x, 2, z + 1, TileType::Empty);
                }
            }
        }
    }

    /// <summary>
    /// サンプル9: 完全なアリーナ（床 + 壁 + 柱）を作成
    /// </summary>
    inline void CreateArena(Map3D& map) {
        // 床を作成
        CreateFlatFloor(map);
        
        // 外周に壁を作成
        CreateWalls(map, 8);
        
        // 四隅に柱を配置
        uint32_t pillarHeight = 12;
        for (uint32_t y = 0; y < pillarHeight; ++y) {
            map.SetTile(2, y, 2, TileType::Normal);
            map.SetTile(map.GetWidth() - 3, y, 2, TileType::Normal);
            map.SetTile(2, y, map.GetDepth() - 3, TileType::Normal);
            map.SetTile(map.GetWidth() - 3, y, map.GetDepth() - 3, TileType::Normal);
        }
    }
}
