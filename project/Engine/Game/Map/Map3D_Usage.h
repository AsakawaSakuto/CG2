#pragma once
/*
================================================================================
Map3D 使用例
================================================================================

1. 基本的な使い方（直接Map3Dを使用）
--------------------------------------------------------------------------------
#include "Game/Map/Map3D.h"

void GameScene::Initialize() {
    // マップの作成と初期化
    map_ = std::make_unique<Map3D>();
    map_->Initialize(50, 20, 50); // 50x20x50のマップ

    // タイルの設定
    map_->SetTile(10, 0, 10, TileType::FLOOR);
    map_->SetTile(10, 1, 10, TileType::WALL);

    // タイルの取得
    TileType tile = map_->GetTile(10, 0, 10);
    
    // テストマップの生成
    map_->GenerateTestMap();
}

void GameScene::Update() {
    // プレイヤーの位置からタイル座標を取得
    Vector3 playerPos = player_->GetPosition();
    Vector3 tilePos = map_->WorldToTile(playerPos);

    // 足元のタイルをチェック
    int tileX = static_cast<int>(tilePos.x);
    int tileY = static_cast<int>(tilePos.y - 1);
    int tileZ = static_cast<int>(tilePos.z);
    
    TileType underTile = map_->GetTile(tileX, tileY, tileZ);
    
    if (underTile == TileType::LAVA) {
        player_->TakeDamage(10.0f);
    }
    
    // レイキャスト（前方の壁を検出）
    Vector3 direction = player_->GetForward();
    Vector3 hitPoint;
    if (map_->Raycast(playerPos, direction, 10.0f, hitPoint)) {
        // 壁に当たった
    }
}

================================================================================
2. MapManagerを使った管理（推奨）
================================================================================
#include "Game/Map/MapManager.h"

void SceneManager::Initialize() {
    // マップマネージャーの初期化
    MapManager::GetInstance()->Initialize();
    
    // 複数のマップを登録
    MapManager::GetInstance()->RegisterMap("Stage1", 50, 20, 50);
    MapManager::GetInstance()->RegisterMap("Stage2", 100, 30, 100);
    MapManager::GetInstance()->RegisterMap("Boss", 60, 40, 60);
    
    // アクティブなマップを設定
    MapManager::GetInstance()->SetActiveMap("Stage1");
    
    // テストマップの生成
    Map3D* stage1 = MapManager::GetInstance()->GetMap("Stage1");
    if (stage1) {
        stage1->GenerateTestMap();
    }
}

void GameScene::Update() {
    // アクティブなマップを取得
    Map3D* map = MapManager::GetInstance()->GetActiveMap();
    if (!map) return;
    
    // マップ操作
    Vector3 playerPos = player_->GetPosition();
    Vector3 tilePos = map->WorldToTile(playerPos);
    TileType tile = map->GetTile(
        static_cast<int>(tilePos.x),
        static_cast<int>(tilePos.y),
        static_cast<int>(tilePos.z)
    );
}

void GameScene::ChangeStage() {
    // ステージ切り替え
    MapManager::GetInstance()->SetActiveMap("Stage2");
}

void SceneManager::Finalize() {
    // マップマネージャーの終了処理
    MapManager::GetInstance()->Finalize();
}

================================================================================
3. MyMapヘルパーを使った簡潔な記述（最も簡単）
================================================================================
#include "Game/Map/MyMap.h"

void SceneManager::Initialize() {
    // 初期化
    MyMap::Initialize();
    
    // マップ登録
    MyMap::RegisterMap("Stage1", 50, 20, 50);
    
    // テストマップ生成
    Map3D* map = MyMap::GetMap("Stage1");
    if (map) {
        map->GenerateTestMap();
    }
}

void GameScene::Update() {
    // プレイヤーの位置からタイル取得
    Vector3 playerPos = player_->GetPosition();
    Vector3 tilePos = MyMap::WorldToTile(playerPos);
    
    TileType tile = MyMap::GetTile(
        static_cast<int>(tilePos.x),
        static_cast<int>(tilePos.y - 1),
        static_cast<int>(tilePos.z)
    );
    
    // 固体チェック
    if (MyMap::IsSolid(10, 1, 10)) {
        // 壁がある
    }
    
    // レイキャスト
    Vector3 hitPoint;
    if (MyMap::Raycast(playerPos, { 0.0f, 0.0f, 1.0f }, 10.0f, hitPoint)) {
        // 何かに当たった
    }
}

void SceneManager::Finalize() {
    MyMap::Finalize();
}

================================================================================
4. ファイルの保存と読み込み
================================================================================
void GameScene::SaveMap() {
    Map3D* map = MyMap::GetMap("Stage1");
    if (map) {
        map->SaveToFile("resources/map/stage1.map");
    }
}

void GameScene::LoadMap() {
    MyMap::LoadMap("Stage1", "resources/map/stage1.map");
}

================================================================================
5. 当たり判定への応用
================================================================================
bool CheckMapCollision(const Vector3& position, Map3D* map) {
    Vector3 tilePos = map->WorldToTile(position);
    
    int x = static_cast<int>(tilePos.x);
    int y = static_cast<int>(tilePos.y);
    int z = static_cast<int>(tilePos.z);
    
    return map->IsSolid(x, y, z);
}

void Player::Update() {
    // 移動前の位置を保存
    Vector3 oldPos = position_;
    
    // 移動
    position_.x += velocity_.x * deltaTime;
    
    // X方向の当たり判定
    if (CheckMapCollision(position_, map_)) {
        position_.x = oldPos.x; // 戻す
        velocity_.x = 0.0f;
    }
    
    position_.z += velocity_.z * deltaTime;
    
    // Z方向の当たり判定
    if (CheckMapCollision(position_, map_)) {
        position_.z = oldPos.z;
        velocity_.z = 0.0f;
    }
}

================================================================================
*/
