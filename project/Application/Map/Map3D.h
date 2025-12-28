#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include <cstdint>
#include "Math/Type/Vector3.h"
#include "Utility/Transform/Transform.h"
#include "3d/Model/Model.h"
#include "Utility/Collision/Type/AABB.h"

/// <summary>
/// タイル種別の定義
/// </summary>
enum class TileType : uint8_t {
	Empty = 0,   // 空（何もない）
	Normal = 1,  // 通常ブロック
	Slope = 2,   // スロープ（坂道）- 汎用（後方互換性のため残す）

	// スロープの向き別（より使いやすい記法）
	Slope_PlusX = 10,   // X+ 方向に上るスロープ
	Slope_MinusX = 11,  // X- 方向に上るスロープ
	Slope_PlusZ = 12,   // Z+ 方向に上るスロープ
	Slope_MinusZ = 13,  // Z- 方向に上るスロープ

	// 今後の拡張用（例）
	// Wall = 3,
	// Lava = 4,
	// Ice = 5,
};

/// <summary>
/// スロープの向き（低い方から高い方への方向）
/// </summary>
enum class SlopeDirection : uint8_t {
	PlusX = 0,   // X+ 方向に上る
	MinusX = 1,  // X- 方向に上る
	PlusZ = 2,   // Z+ 方向に上る
	MinusZ = 3,  // Z- 方向に上る
};

/// <summary>
/// ブロックデータ（各セルの情報）
/// </summary>
struct BlockData {
	TileType type = TileType::Empty;
	SlopeDirection slopeDir = SlopeDirection::PlusX;  // スロープの向き
	std::unique_ptr<Model> model = nullptr;
	Transform transform;
	AABB aabb;
};

/// <summary>
/// 固定サイズの3Dマップ管理クラス
/// </summary>
class Map3D {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="width">X方向のサイズ</param>
	/// <param name="height">Y方向のサイズ</param>
	/// <param name="depth">Z方向のサイズ</param>
	Map3D(uint32_t width, uint32_t height, uint32_t depth);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Map3D();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="camera">使用するカメラ</param>
	void Draw(Camera& camera);

	/// <summary>
	/// ImGui描画
	/// </summary>
	void DrawImGui();

	/// <summary>
	/// 指定座標のタイルタイプを取得
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <returns>タイルタイプ</returns>
	TileType GetTile(uint32_t x, uint32_t y, uint32_t z) const;

	/// <summary>
	/// 指定座標のタイルタイプを設定
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <param name="type">設定するタイルタイプ</param>
	void SetTile(uint32_t x, uint32_t y, uint32_t z, TileType type);

	/// <summary>
	/// 指定座標のタイルタイプを設定（スロープの向き指定付き）
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <param name="type">設定するタイルタイプ</param>
	/// <param name="direction">スロープの向き（type が Slope の場合のみ使用）</param>
	void SetTile(uint32_t x, uint32_t y, uint32_t z, TileType type, SlopeDirection direction);

	/// <summary>
	/// スロープを設定（向き指定）
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <param name="direction">スロープの向き</param>
	void SetSlope(uint32_t x, uint32_t y, uint32_t z, SlopeDirection direction);

	/// <summary>
	/// 指定座標のスロープ向きを取得
	/// </summary>
	SlopeDirection GetSlopeDirection(uint32_t x, uint32_t y, uint32_t z) const;

	/// <summary>
	/// スロープ上のワールド座標からY座標を計算
	/// </summary>
	/// <param name="worldPos">ワールド座標</param>
	/// <param name="outY">計算されたY座標</param>
	/// <returns>スロープ上にいる場合true</returns>
	bool GetSlopeHeight(const Vector3& worldPos, float& outY) const;

	/// <summary>
	/// 指定座標のブロックのモデルにテクスチャを設定
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <param name="texturePath">テクスチャファイルのパス（フルパス）</param>
	/// <returns>成功した場合true、ブロックが存在しないかモデルが無い場合false</returns>
	bool SetBlockTexture(uint32_t x, uint32_t y, uint32_t z, const std::string& texturePath);

	/// <summary>
	/// ワールド座標からマップ座標に変換
	/// </summary>
	/// <param name="worldPos">ワールド座標</param>
	/// <param name="outX">出力X座標</param>
	/// <param name="outY">出力Y座標</param>
	/// <param name="outZ">出力Z座標</param>
	/// <returns>範囲内ならtrue</returns>
	bool WorldToMap(const Vector3& worldPos, uint32_t& outX, uint32_t& outY, uint32_t& outZ) const;

	/// <summary>
	/// マップ座標からワールド座標（ブロック中心）に変換
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <returns>ワールド座標</returns>
	Vector3 MapToWorld(uint32_t x, uint32_t y, uint32_t z) const;

	/// <summary>
	/// 指定座標が範囲内かチェック
	/// </summary>
	bool IsInBounds(uint32_t x, uint32_t y, uint32_t z) const;

	/// <summary>
	/// マップをクリア（全てEmptyにする）
	/// </summary>
	void Clear();

	/// <summary>
	/// ブロックサイズを取得
	/// </summary>
	Vector3 GetBlockSize() const { return blockSize_; }

	/// <summary>
	/// マップサイズを取得
	/// </summary>
	uint32_t GetWidth() const { return width_; }
	uint32_t GetHeight() const { return height_; }
	uint32_t GetDepth() const { return depth_; }

	void Update();
private:
	/// <summary>
	/// 3D座標から1次元インデックスに変換
	/// </summary>
	inline uint32_t ToIndex(uint32_t x, uint32_t y, uint32_t z) const {
		assert(IsInBounds(x, y, z) && "Map3D: Index out of bounds!");
		return x + width_ * (y + height_ * z);
	}

	/// <summary>
	/// TileTypeがスロープかどうかを判定
	/// </summary>
	inline bool IsSlopeType(TileType type) const {
		return type == TileType::Slope || 
		       type == TileType::Slope_PlusX || 
		       type == TileType::Slope_MinusX || 
		       type == TileType::Slope_PlusZ || 
		       type == TileType::Slope_MinusZ;
	}

	/// <summary>
	/// TileTypeからSlopeDirectionを取得
	/// </summary>
	inline SlopeDirection GetDirectionFromTileType(TileType type) const {
		switch (type) {
			case TileType::Slope_PlusX:  return SlopeDirection::PlusX;
			case TileType::Slope_MinusX: return SlopeDirection::MinusX;
			case TileType::Slope_PlusZ:  return SlopeDirection::PlusZ;
			case TileType::Slope_MinusZ: return SlopeDirection::MinusZ;
			default: return SlopeDirection::PlusX; // デフォルト
		}
	}

	/// <summary>
	/// 指定座標のブロックにモデルを作成
	/// </summary>
	void CreateBlockModel(uint32_t x, uint32_t y, uint32_t z, TileType type);

	/// <summary>
	/// 指定座標のブロックを削除
	/// </summary>
	void DestroyBlock(uint32_t x, uint32_t y, uint32_t z);

	/// <summary>
	/// タイルタイプに応じたスケールを取得
	/// </summary>
	Vector3 GetScaleForTileType(TileType type) const;

	void BlockShapeUpdate();
private:
	// マップサイズ
	uint32_t width_;
	uint32_t height_;
	uint32_t depth_;

	// ブロックサイズ（各軸の直径）
	Vector3 blockSize_ = { 10.0f, 5.0f, 10.0f };

	// ブロックデータ（1次元配列）
	std::vector<BlockData> blocks_;

	// モデルパスのマッピング
	static const std::unordered_map<TileType, std::string> kModelPaths_;

	// タイルタイプごとのスケール設定（半径1mのキューブからの倍率）
	static const std::unordered_map<TileType, Vector3> kTileScales_;

	static const std::unordered_map<TileType, AABB> kNormalAABB_;
};
