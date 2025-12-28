#include "Map3D.h"
#include <algorithm>
#include "3d/Line/MyDebugLine.h"

// タイルタイプとモデルパスのマッピング
const std::unordered_map<TileType, std::string> Map3D::kModelPaths_ = {
	{ TileType::Normal, "MapBlock/NormalBlock.obj" },
	{ TileType::Slope, "MapBlock/SlopeBlock.obj" },  // スロープのモデルパス（汎用）
	{ TileType::Slope_PlusX, "MapBlock/SlopeBlock.obj" },
	{ TileType::Slope_MinusX, "MapBlock/SlopeBlock.obj" },
	{ TileType::Slope_PlusZ, "MapBlock/SlopeBlock.obj" },
	{ TileType::Slope_MinusZ, "MapBlock/SlopeBlock.obj" },
};

// タイルタイプごとのスケール設定（Blenderの半径1mキューブからの倍率）
// Normalブロックは直径(10m, 5m, 10m) → 半径(5m, 2.5m, 5m) → スケール(5.0, 2.5, 5.0)
const std::unordered_map<TileType, Vector3> Map3D::kTileScales_ = {
	{ TileType::Normal, { 5.0f, 2.5f, 5.0f } },
	{ TileType::Slope, { 5.0f, 2.5f, 5.0f } },
	{ TileType::Slope_PlusX, { 5.0f, 2.5f, 5.0f } },
	{ TileType::Slope_MinusX, { 5.0f, 2.5f, 5.0f } },
	{ TileType::Slope_PlusZ, { 5.0f, 2.5f, 5.0f } },
	{ TileType::Slope_MinusZ, { 5.0f, 2.5f, 5.0f } },
};

const std::unordered_map<TileType, AABB> Map3D::kNormalAABB_ = {
	{ TileType::Normal, AABB{ {0.0f, 0.0f, 0.0f}, 
	{-5.0f, -2.5f, -5.0f}, 
	{ 5.0f,  2.5f,  5.0f} } },
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

void Map3D::Update() {
	// ブロックの形状更新
	BlockShapeUpdate();
}

void Map3D::BlockShapeUpdate() {
	// 全ブロックを走査して描画
	for (uint32_t z = 0; z < depth_; ++z) {
		for (uint32_t y = 0; y < height_; ++y) {
			for (uint32_t x = 0; x < width_; ++x) {
				uint32_t index = ToIndex(x, y, z);
				BlockData& block = blocks_[index];

				if (block.type == TileType::Empty || !block.model) {
					continue;
				}

				// Normalタイプのブロックに定義済みのAABBを設定
				// スロープはAABB判定を行わないか、別のロジックで処理
				if (block.type == TileType::Normal) {
					auto it = kNormalAABB_.find(TileType::Normal);
					if (it != kNormalAABB_.end()) {
						block.aabb = it->second;
						// AABBの中心位置をブロックのワールド座標に設定
						block.aabb.center = block.transform.translate;
					}
					
					MyDebugLine::AddShape(block.aabb, { 1.0f, 0.0f, 0.0f, 1.0f });
				}
			}
		}
	}
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
	uint32_t slopeCount = 0;
	for (const auto& block : blocks_) {
		if (block.type == TileType::Empty) emptyCount++;
		else if (block.type == TileType::Normal) normalCount++;
		else if (IsSlopeType(block.type)) slopeCount++;
	}
	ImGui::Text("Empty: %u, Normal: %u, Slope: %u", emptyCount, normalCount, slopeCount);
	
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
		// スロープタイプの場合は向きを設定
		if (IsSlopeType(type)) {
			block.slopeDir = GetDirectionFromTileType(type);
		}
		
		CreateBlockModel(x, y, z, type);
		
		// y より下のブロックが空なら Normal で埋める
		for (uint32_t fillY = 0; fillY < y; ++fillY) {
			if (GetTile(x, fillY, z) == TileType::Empty) {
				uint32_t fillIndex = ToIndex(x, fillY, z);
				BlockData& fillBlock = blocks_[fillIndex];
				fillBlock.type = TileType::Normal;
				CreateBlockModel(x, fillY, z, TileType::Normal);
				SetBlockTexture(x, fillY, z, "resources/model/MapBlock/NormalBlockTexture2.png");
			}
		}
	}
}

void Map3D::SetTile(uint32_t x, uint32_t y, uint32_t z, TileType type, SlopeDirection direction) {
	// 新しいスロープタイプが指定されている場合は、そのまま通常のSetTileを呼び出す
	if (type == TileType::Slope_PlusX || type == TileType::Slope_MinusX || 
	    type == TileType::Slope_PlusZ || type == TileType::Slope_MinusZ) {
		SetTile(x, y, z, type);
		return;
	}
	
	// Slope以外のタイプの場合は通常のSetTileを呼び出し
	if (type != TileType::Slope) {
		SetTile(x, y, z, type);
		return;
	}

	// 汎用Slopeの場合はSetSlopeを呼び出し
	SetSlope(x, y, z, direction);
}

void Map3D::SetSlope(uint32_t x, uint32_t y, uint32_t z, SlopeDirection direction) {
	uint32_t index = ToIndex(x, y, z);
	BlockData& block = blocks_[index];

	// 古いブロックを削除
	if (block.type != TileType::Empty) {
		DestroyBlock(x, y, z);
	}

	// スロープブロックを作成
	block.type = TileType::Slope;
	block.slopeDir = direction;
	CreateBlockModel(x, y, z, TileType::Slope);

	// スロープの向きに応じてモデルを回転
	float rotationY = 0.0f;
	switch (direction) {
		case SlopeDirection::PlusX:  rotationY = 0.0f; break;           // 0度
		case SlopeDirection::MinusX: rotationY = 3.14159265f; break;    // 180度
		case SlopeDirection::PlusZ:  rotationY = -1.57079633f; break;   // -90度
		case SlopeDirection::MinusZ: rotationY = 1.57079633f; break;    // 90度
	}
	block.transform.rotate.y = rotationY;
	
	// y より下のブロックが空なら Normal で埋める
	for (uint32_t fillY = 0; fillY < y; ++fillY) {
		if (GetTile(x, fillY, z) == TileType::Empty) {
			uint32_t fillIndex = ToIndex(x, fillY, z);
			BlockData& fillBlock = blocks_[fillIndex];
			fillBlock.type = TileType::Normal;
			CreateBlockModel(x, fillY, z, TileType::Normal);
		}
	}
}

SlopeDirection Map3D::GetSlopeDirection(uint32_t x, uint32_t y, uint32_t z) const {
	return blocks_[ToIndex(x, y, z)].slopeDir;
}

bool Map3D::GetSlopeHeight(const Vector3& worldPos, float& outY) const {
	// ワールド座標からマップ座標を取得
	uint32_t mx, my, mz;
	if (!WorldToMap(worldPos, mx, my, mz)) {
		return false;
	}

	// 該当セルがスロープでない場合は失敗
	TileType tileType = GetTile(mx, my, mz);
	if (!IsSlopeType(tileType)) {
		return false;
	}

	// ブロックの中心座標を取得
	Vector3 blockCenter = MapToWorld(mx, my, mz);
	SlopeDirection dir = GetSlopeDirection(mx, my, mz);

	// ブロックの半サイズ
	float halfWidth = blockSize_.x * 0.5f;
	float halfHeight = blockSize_.y;  // スロープは高さ全体を使用
	float halfDepth = blockSize_.z * 0.5f;

	// ブロック内のローカル座標を計算 (-0.5 ~ 0.5)
	float localX = (worldPos.x - blockCenter.x) / blockSize_.x;
	float localZ = (worldPos.z - blockCenter.z) / blockSize_.z;

	// スロープの向きに応じて高さを計算
	float heightRatio = 0.0f;  // 0.0 (底) ~ 1.0 (頂上)
	bool isOnSlope = false;

	switch (dir) {
		case SlopeDirection::PlusX:
			// X+ 方向に上る（X=-0.5で底、X=+0.5で頂上）
			if (localX >= -0.5f && localX <= 0.5f && 
			    localZ >= -0.5f && localZ <= 0.5f) {
				heightRatio = (localX + 0.5f);  // 0.0 ~ 1.0
				isOnSlope = true;
			}
			break;

		case SlopeDirection::MinusX:
			// X- 方向に上る（X=+0.5で底、X=-0.5で頂上）
			if (localX >= -0.5f && localX <= 0.5f && 
			    localZ >= -0.5f && localZ <= 0.5f) {
				heightRatio = (0.5f - localX);  // 0.0 ~ 1.0
				isOnSlope = true;
			}
			break;

		case SlopeDirection::PlusZ:
			// Z+ 方向に上る（Z=-0.5で底、Z=+0.5で頂上）
			if (localX >= -0.5f && localX <= 0.5f && 
			    localZ >= -0.5f && localZ <= 0.5f) {
				heightRatio = (localZ + 0.5f);  // 0.0 ~ 1.0
				isOnSlope = true;
			}
			break;

		case SlopeDirection::MinusZ:
			// Z- 方向に上る（Z=+0.5で底、Z=-0.5で頂上）
			if (localX >= -0.5f && localX <= 0.5f && 
			    localZ >= -0.5f && localZ <= 0.5f) {
				heightRatio = (0.5f - localZ);  // 0.0 ~ 1.0
				isOnSlope = true;
			}
			break;
	}

	if (isOnSlope) {
		// スロープの底面Y座標
		float bottomY = blockCenter.y - (blockSize_.y * 0.5f);
		// スロープ上のY座標を計算
		outY = bottomY + (heightRatio * halfHeight);
		return true;
	}

	return false;
}

bool Map3D::SetBlockTexture(uint32_t x, uint32_t y, uint32_t z, const std::string& texturePath) {
	// 範囲外チェック
	if (!IsInBounds(x, y, z)) {
		return false;
	}

	uint32_t index = ToIndex(x, y, z);
	BlockData& block = blocks_[index];

	// ブロックが空、またはモデルが無い場合は失敗
	if (block.type == TileType::Empty || !block.model) {
		return false;
	}

	// モデルにテクスチャを設定
	block.model->SetTexture(texturePath);
	return true;
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
	
	// スロープの場合、向きに応じて回転を設定
	if (IsSlopeType(type)) {
		SlopeDirection direction = block.slopeDir;
		float rotationY = 0.0f;
		switch (direction) {
			case SlopeDirection::PlusX:  rotationY = 0.0f; break;           // 0度
			case SlopeDirection::MinusX: rotationY = 3.14159265f; break;    // 180度
			case SlopeDirection::PlusZ:  rotationY = -1.57079633f; break;   // -90度
			case SlopeDirection::MinusZ: rotationY = 1.57079633f; break;    // 90度
		}
		block.transform.rotate.y = rotationY;
	}
}

void Map3D::DestroyBlock(uint32_t x, uint32_t y, uint32_t z) {
	uint32_t index = ToIndex(x, y, z);
	BlockData& block = blocks_[index];
	
	block.model.reset();
	block.type = TileType::Empty;
}
