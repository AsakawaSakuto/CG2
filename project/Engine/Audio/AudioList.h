#pragma once

enum class BGM_List {
    Title = 0,
    Game1,
    Game2,
    Game3,
    Game4,
    Game5,

    Count
};

enum class SE_List {
    StartUp,
    Confirm, // 決定、キャンセル
    Select,  // SELECT○○のとき 
    LockIn,  // Titleのビルドを確定
    Chest,   // free pay Chestを開けるとき
    Jar,     // min,max Jarを壊すときの
    PlayerDamage,  // Playerがダメージを受けたとき
    Jump,    // ジャンプしたとき
    Upgrade, // アップグレード時
    ExpGet,  // 経験値取得時
    PlayerDie, // Playerが死亡したとき

    // Weapon を生成した時のSE
    FireBall,  // ファイヤーボール
    Laser,     // レーザー
    Runa,      // ルナ
    Axe,       // アックス
    Boomerang, // ブーメラン
    Dice,      // ダイス
    Toxic,     // トキシック
    Gun,       // ガン

    // 敵のダメージ音
    EnemyDamage1,
    EnemyDamage2,
    EnemyDamage3,
    EnemyDamage4,
    EnemyDamage5,

    Count
};