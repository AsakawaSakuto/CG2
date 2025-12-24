#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "Math/Type/Vector3.h"

// マップチップの種類
enum class TileType : uint8_t {
    EMPTY = 0,
    FLOOR,
    WALL,
    CEILING,
    LAVA,
    WATER,
    SPAWN_POINT,
    GOAL,
    CHECKPOINT,
    JUMP_PAD,
    SPIKE,
    MOVING_PLATFORM,

    Count
};

/// <summary>
/// 3Dマップクラス（固定サイズ、高速アクセス）
/// 3Dアクションゲーム用のマップ管理
/// </summary>
class Map3D {
public:
    /// <summary>
    /// マップの初期化
    /// </summary>
    /// <param name="sizeX">X方向のサイズ</param>
    /// <param name="sizeY">Y方向のサイズ（高さ）</param>
    /// <param name="sizeZ">Z方向のサイズ</param>
    void Initialize(int sizeX, int sizeY, int sizeZ);

    /// <summary>
    /// タイルの取得
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="z">Z座標</param>
    /// <returns>タイルの種類</returns>
    TileType GetTile(int x, int y, int z) const;

    /// <summary>
    /// タイルの設定
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="z">Z座標</param>
    /// <param name="type">タイルの種類</param>
    void SetTile(int x, int y, int z, TileType type);

    /// <summary>
    /// ワールド座標からタイル座標に変換
    /// </summary>
    /// <param name="worldPos">ワールド座標</param>
    /// <param name="tileSize">1タイルのサイズ</param>
    /// <returns>タイル座標（x, y, z）</returns>
    Vector3 WorldToTile(const Vector3& worldPos, float tileSize = 1.0f) const;

    /// <summary>
    /// タイル座標からワールド座標に変換（タイルの中心）
    /// </summary>
    /// <param name="x">タイルのX座標</param>
    /// <param name="y">タイルのY座標</param>
    /// <param name="z">タイルのZ座標</param>
    /// <param name="tileSize">1タイルのサイズ</param>
    /// <returns>ワールド座標</returns>
    Vector3 TileToWorld(int x, int y, int z, float tileSize = 1.0f) const;

    /// <summary>
    /// 指定座標が範囲内か判定
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="z">Z座標</param>
    /// <returns>範囲内ならtrue</returns>
    bool IsInBounds(int x, int y, int z) const;

    /// <summary>
    /// 指定座標が固体（歩けない）タイルか判定
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="z">Z座標</param>
    /// <returns>固体ならtrue</returns>
    bool IsSolid(int x, int y, int z) const;

    /// <summary>
    /// 全タイルをクリア
    /// </summary>
    /// <param name="fillType">埋めるタイルの種類</param>
    void Clear(TileType fillType = TileType::EMPTY);

    /// <summary>
    /// マップサイズを取得
    /// </summary>
    int GetSizeX() const { return sizeX_; }
    int GetSizeY() const { return sizeY_; }
    int GetSizeZ() const { return sizeZ_; }

    /// <summary>
    /// 1タイルのサイズを取得
    /// </summary>
    float GetTileSize() const { return tileSize_; }

    /// <summary>
    /// 1タイルのサイズを設定
    /// </summary>
    void SetTileSize(float size) { tileSize_ = size; }

    /// <summary>
    /// ファイルからマップを読み込み
    /// </summary>
    /// <param name="filePath">マップファイルのパス</param>
    void LoadFromFile(const std::string& filePath);

    /// <summary>
    /// ファイルにマップを保存
    /// </summary>
    /// <param name="filePath">保存先のファイルパス</param>
    void SaveToFile(const std::string& filePath);

    /// <summary>
    /// 簡易的なテストマップを生成
    /// </summary>
    void GenerateTestMap();

    /// <summary>
    /// レイキャスト（指定方向に最初に当たるタイルを検出）
    /// </summary>
    /// <param name="origin">始点</param>
    /// <param name="direction">方向</param>
    /// <param name="maxDistance">最大距離</param>
    /// <param name="hitPoint">当たった位置（出力）</param>
    /// <returns>当たったらtrue</returns>
    bool Raycast(const Vector3& origin, const Vector3& direction, float maxDistance, Vector3& hitPoint) const;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Map3D() = default;

private:
    Map3D() = default;
    Map3D(const Map3D&) = delete;
    Map3D& operator=(const Map3D&) = delete;

    // マップデータ（1次元配列）
    std::vector<TileType> mapData_;

    // マップサイズ
    int sizeX_ = 0;
    int sizeY_ = 0;
    int sizeZ_ = 0;

    // 1タイルのワールド座標でのサイズ
    float tileSize_ = 1.0f;

    /// <summary>
    /// 3D座標を1Dインデックスに変換
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="z">Z座標</param>
    /// <returns>1次元インデックス</returns>
    int ToIndex(int x, int y, int z) const {
        return x + y * sizeX_ + z * sizeX_ * sizeY_;
    }
};
