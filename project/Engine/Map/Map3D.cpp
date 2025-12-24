#include "Map3D.h"
#include <algorithm>

// タイルタイプとモデルパスのマッピング
const std::unordered_map<TileType, std::string> Map3D::kModelPaths_ = {
	{ TileType::Normal, "MapBlock/NormalBlock.obj" },
	// 今後の拡張用
	// { TileType::Wall, "MapBlock/WallBlock.obj" },
	// { TileType::Lava, "MapBlock/LavaBlock.obj" },
};

// タイルタイプごとのスケール設定（Blenderの半径1mキューブからの倍率）
// Normalブロックは直径(10m, 5m, 10m) → 半径(5m, 2.5m, 5m) → スケール(5.0, 2.5, 5.0)
const std::unordered_map<TileType, Vector3> Map3D::kTileScales_ = {
	{ TileType::Normal, { 5.0f, 2.5f, 5.0f } },
	// 今後の拡張用
	// { TileType::Wall, { 1.0f, 3.0f, 1.0f } },
	// { TileType::Lava, { 5.0f, 0.5f, 5.0f } },
};

Map3D::Map3D(uint32_t width, uint32_t height, uint32_t depth)
	: width_(width)
	, height_(height)
	, depth_(depth)
	, blockSize_{ 10.0f, 5.0f, 10.0f }
{
	// ブロック配列のサイズを確保
	uint32_t totalSize = width_ * height_ * depth_;
	blocks_.resize(totalSize);
}

Map3D::~Map3D() {
	// ブロックのクリーンアップ（unique_ptrが自動で行う）
}

void Map3D::Initialize() {
	// 初期化時点では何もしない（SetTileで動的に生成）
}

void Map3D::Draw(Camera& camera) {
	// 全ブロックを走査して描画
	for (uint32_t z = 0; z < depth_; ++z) {
		for (uint32_t y = 0; y < height_; ++y) {
			for (uint32_t x = 0; x < width_; ++x) {
				uint32_t index = ToIndex(x, y, z);
				BlockData& block = blocks_[index];

				// Emptyまたはモデルが無い場合は描画しない
				if (block.type == TileType::Empty || !block.model) {
					continue;
				}

				// モデルを描画
				block.model->Draw(camera, block.transform);
			}
		}
	}
}

void Map3D::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Map3D");
	ImGui::Text("Width: %u, Height: %u, Depth: %u", width_, height_, depth_);
	ImGui::Text("Block Size: (%.1f, %.1f, %.1f)", blockSize_.x, blockSize_.y, blockSize_.z);
	
	// タイル数のカウント
	uint32_t emptyCount = 0;
	uint32_t normalCount = 0;
	for (const auto& block : blocks_) {
		if (block.type == TileType::Empty) emptyCount++;
		else if (block.type == TileType::Normal) normalCount++;
	}
	ImGui::Text("Empty: %u, Normal: %u", emptyCount, normalCount);
	
	ImGui::End();
#endif
}

TileType Map3D::GetTile(uint32_t x, uint32_t y, uint32_t z) const {
	return blocks_[ToIndex(x, y, z)].type;
}

void Map3D::SetTile(uint32_t x, uint32_t y, uint32_t z, TileType type) {
	uint32_t index = ToIndex(x, y, z);
	BlockData& block = blocks_[index];

	// 既存のタイプと同じ場合は何もしない
	if (block.type == type) {
		return;
	}

	// 古いブロックを削除
	if (block.type != TileType::Empty) {
		DestroyBlock(x, y, z);
	}

	// 新しいブロックを作成
	block.type = type;
	if (type != TileType::Empty) {
		CreateBlockModel(x, y, z, type);
	}
}

bool Map3D::WorldToMap(const Vector3& worldPos, uint32_t& outX, uint32_t& outY, uint32_t& outZ) const {
	// ワールド座標をマップ座標に変換
	// 原点 (0,0,0) は配列 (0,0,0) の中心
	// 各軸で異なるブロックサイズを使用
	float fx = worldPos.x / blockSize_.x;
	float fy = worldPos.y / blockSize_.y;
	float fz = worldPos.z / blockSize_.z;

	// ブロック中心を基準にした変換
	int32_t ix = static_cast<int32_t>(std::floor(fx + 0.5f));
	int32_t iy = static_cast<int32_t>(std::floor(fy + 0.5f));
	int32_t iz = static_cast<int32_t>(std::floor(fz + 0.5f));

	// 範囲外チェック
	if (ix < 0 || iy < 0 || iz < 0) return false;
	if (static_cast<uint32_t>(ix) >= width_ || 
		static_cast<uint32_t>(iy) >= height_ || 
		static_cast<uint32_t>(iz) >= depth_) return false;

	outX = static_cast<uint32_t>(ix);
	outY = static_cast<uint32_t>(iy);
	outZ = static_cast<uint32_t>(iz);
	return true;
}

Vector3 Map3D::MapToWorld(uint32_t x, uint32_t y, uint32_t z) const {
	assert(IsInBounds(x, y, z) && "Map3D: Coordinates out of bounds!");
	
	// 配列 (0,0,0) の中心がワールド原点 (0,0,0)
	// 各ブロックの中心座標 = (x, y, z) * blockSize (軸ごとに異なる)
	return Vector3(
		static_cast<float>(x) * blockSize_.x,
		static_cast<float>(y) * blockSize_.y,
		static_cast<float>(z) * blockSize_.z
	);
}

bool Map3D::IsInBounds(uint32_t x, uint32_t y, uint32_t z) const {
	return x < width_ && y < height_ && z < depth_;
}

void Map3D::Clear() {
	// 全ブロックを削除
	for (uint32_t i = 0; i < blocks_.size(); ++i) {
		blocks_[i].type = TileType::Empty;
		blocks_[i].model.reset();
	}
}

Vector3 Map3D::GetScaleForTileType(TileType type) const {
	auto it = kTileScales_.find(type);
	if (it != kTileScales_.end()) {
		return it->second;
	}
	// デフォルトスケール（半径1m = 直径2mのキューブ）
	return { 1.0f, 1.0f, 1.0f };
}

void Map3D::CreateBlockModel(uint32_t x, uint32_t y, uint32_t z, TileType type) {
	uint32_t index = ToIndex(x, y, z);
	BlockData& block = blocks_[index];

	// モデルパスを取得
	auto pathIt = kModelPaths_.find(type);
	if (pathIt == kModelPaths_.end()) {
		// パスが定義されていないタイプ
		return;
	}

	// モデルを新規作成
	block.model = std::make_unique<Model>();
	block.model->Initialize(pathIt->second);

	// Transformを設定
	Vector3 worldPos = MapToWorld(x, y, z);
	block.transform.translate = worldPos;
	block.transform.rotate = { 0.0f, 0.0f, 0.0f };
	
	// タイルタイプに応じたスケールを設定
	block.transform.scale = GetScaleForTileType(type);
}

void Map3D::DestroyBlock(uint32_t x, uint32_t y, uint32_t z) {
	uint32_t index = ToIndex(x, y, z);
	BlockData& block = blocks_[index];
	
	block.model.reset();
	block.type = TileType::Empty;
}
