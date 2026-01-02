# Audio System Documentation

このドキュメントでは、Engineに実装されている音声システムの使い方を説明します。

## 目次
1. [概要](#概要)
2. [基本的な使い方](#基本的な使い方)
3. [音源の登録](#音源の登録)
4. [BGMの制御](#bgmの制御)
5. [SEの制御](#seの制御)
6. [詳細機能](#詳細機能)
7. [サポートされているファイル形式](#サポートされているファイル形式)

---

## 概要

このAudioシステムは、XAudio2を使用した音声再生システムです。
主な特徴:
- **BGM（背景音楽）とSE（効果音）の管理**
- **同時再生のサポート**（複数のSEを同時に再生可能）
- **ループ再生のサポート**
- **音量制御**
- **WAVおよびMP3ファイルのサポート**

システムはシングルトンパターンで実装されており、`AudioManager`クラスを通じて管理されます。

---

## 基本的な使い方

### 1. 初期化

`AudioManager`は自動的に初期化されますが、必要に応じて以下のように呼び出します：

```cpp
#include "Engine/Audio/AudioManager.h"

// 初期化
AudioManager::GetInstance()->Initialize();
```

### 2. 簡単な再生方法

`MyAudio`名前空間の関数を使用すると、より簡潔に記述できます：

```cpp
#include "Engine/Audio/MyAudio.h"

// BGMを再生
MyAudio::PlayBGM(BGM_List::GAME, 0.8f);  // 音量80%で再生

// SEを再生
MyAudio::PlaySE(SE_List::KAWAII, 1.0f);  // 音量100%で再生

// BGMを停止
MyAudio::StopBGM(BGM_List::GAME);
```

### 3. 更新処理

毎フレーム、`Update()`を呼び出して終了したサウンドインスタンスをクリーンアップします：

```cpp
// メインループ内で
AudioManager::GetInstance()->Update();
```

### 4. 終了処理

アプリケーション終了時に呼び出します：

```cpp
AudioManager::GetInstance()->Finalize();
```

---

## 音源の登録

### BGMとSEの種類を定義

`Engine/Audio/AudioManager.h`で、使用するBGMとSEを列挙型で定義します：

```cpp
enum class SE_List {
    KAWAII,
    OU,
    DON,
    KIRAKIRA,
    // 新しいSEをここに追加
    Count  // 最後に必ずCountを配置
};

enum class BGM_List {
    TEST,
    TITLE,
    GAME,
    RESULT,
    // 新しいBGMをここに追加
    Count  // 最後に必ずCountを配置
};
```

### 音源ファイルの読み込み

`AudioManager::Initialize()`内、または任意のタイミングで音源を読み込みます：

```cpp
// Initialize()内、または個別に読み込み
AudioManager::GetInstance()->LoadBGM(BGM_List::GAME, "resources/sound/BGM/game_music.mp3");
AudioManager::GetInstance()->LoadSE(SE_List::KAWAII, "resources/sound/SE/kawaii.mp3");
```

**注意**: ファイルパスは`resources/sound/`からの相対パスで指定します。

---

## BGMの制御

### BGMの再生

```cpp
// 基本的な再生（デフォルトでループ、音量100%）
AudioManager::GetInstance()->PlayBGM(BGM_List::GAME);

// 音量を指定して再生（50%の音量）
AudioManager::GetInstance()->PlayBGM(BGM_List::GAME, 0.5f);

// ループなしで再生
AudioManager::GetInstance()->PlayBGM(BGM_List::GAME, 1.0f, false);
```

### BGMの停止

```cpp
// 特定のBGMを停止
AudioManager::GetInstance()->StopBGM(BGM_List::GAME);

// すべてのBGMを停止
AudioManager::GetInstance()->StopAllBGM();
```

### BGMの読み込み確認

```cpp
if (AudioManager::GetInstance()->IsLoaded(BGM_List::GAME)) {
    // 読み込み済み
    AudioManager::GetInstance()->PlayBGM(BGM_List::GAME);
}
```

---

## SEの制御

### SEの再生

```cpp
// 基本的な再生（デフォルトでループなし、音量100%）
AudioManager::GetInstance()->PlaySE(SE_List::DON);

// 音量を指定して再生（80%の音量）
AudioManager::GetInstance()->PlaySE(SE_List::KAWAII, 0.8f);

// ループ再生
AudioManager::GetInstance()->PlaySE(SE_List::KIRAKIRA, 1.0f, true);
```

**重要**: SEは同時に複数回再生可能です。同じSEを連続して呼び出しても、それぞれ独立して再生されます。

### SEの停止

```cpp
// すべてのSEを停止
AudioManager::GetInstance()->StopAllSE();
```

### SEの読み込み確認

```cpp
if (AudioManager::GetInstance()->IsLoaded(SE_List::DON)) {
    // 読み込み済み
    AudioManager::GetInstance()->PlaySE(SE_List::DON);
}
```

---

## 詳細機能

### 音量の設定

音量は`0.0f`（無音）から`1.0f`（最大音量）の範囲で指定します：

```cpp
// 小さい音量で再生
AudioManager::GetInstance()->PlayBGM(BGM_List::TITLE, 0.3f);

// 大きい音量で再生
AudioManager::GetInstance()->PlaySE(SE_List::OU, 1.0f);
```

### 同時再生

SEは同じ音源を複数同時に再生できます：

```cpp
// 複数のプレイヤーが同時に攻撃した場合など
MyAudio::PlaySE(SE_List::DON);  // 1回目
MyAudio::PlaySE(SE_List::DON);  // 2回目（同時再生される）
MyAudio::PlaySE(SE_List::DON);  // 3回目（同時再生される）
```

### ループ再生

BGMは通常ループしますが、SEでもループ再生が可能です：

```cpp
// 環境音などをループ再生
AudioManager::GetInstance()->PlaySE(SE_List::KIRAKIRA, 0.5f, true);

// BGMのループを無効化
AudioManager::GetInstance()->PlayBGM(BGM_List::RESULT, 1.0f, false);
```

---

## サポートされているファイル形式

このAudioシステムは以下のファイル形式をサポートしています：

### WAVファイル (.wav)
- PCM形式の非圧縮オーディオ
- 高品質だがファイルサイズが大きい
- 推奨用途: 短いSE

### MP3ファイル (.mp3)
- 圧縮オーディオ（Media Foundation使用）
- ファイルサイズが小さい
- 推奨用途: BGM、長いSE

**注意**: Windows N/KN エディションでは、Media Feature Packのインストールが必要です。

---

## 実装例

### ゲームシーンでの使用例

```cpp
// ゲームシーンの初期化
void GameScene::Initialize() {
    // BGMを再生
    MyAudio::PlayBGM(BGM_List::GAME, 0.7f);
}

// プレイヤーが攻撃したとき
void Player::Attack() {
    // 攻撃SEを再生
    MyAudio::PlaySE(SE_List::DON, 0.9f);
}

// アイテムを取得したとき
void Player::GetItem() {
    // アイテム取得SEを再生
    MyAudio::PlaySE(SE_List::KIRAKIRA, 1.0f);
}

// ゲームシーンの終了
void GameScene::Finalize() {
    // BGMを停止
    MyAudio::StopBGM(BGM_List::GAME);
}
```

### タイトルシーンでの使用例

```cpp
void TitleScene::Initialize() {
    // タイトルBGMを再生
    MyAudio::PlayBGM(BGM_List::TITLE, 0.6f);
}

void TitleScene::OnButtonClick() {
    // ボタンクリック音を再生
    MyAudio::PlaySE(SE_List::KAWAII);
}
```

---

## トラブルシューティング

### 音が再生されない場合

1. **ファイルパスを確認**
   - `resources/sound/`ディレクトリ内にファイルが存在するか
   - ファイル名のスペルミスがないか

2. **読み込みを確認**
   ```cpp
   if (!AudioManager::GetInstance()->IsLoaded(BGM_List::GAME)) {
       // 読み込みに失敗している
   }
   ```

3. **音量を確認**
   - 音量が`0.0f`になっていないか

4. **Update()の呼び出し**
   - 毎フレーム`Update()`を呼び出しているか

### MP3が再生できない場合

- Windows N/KN エディションの場合、Media Feature Packをインストールしてください
- ファイルが破損していないか確認してください

---

## まとめ

このAudioシステムは、シンプルなAPIで強力な音声制御を提供します：

- **簡単**: `MyAudio::PlayBGM()` や `MyAudio::PlaySE()` で再生
- **柔軟**: 音量、ループ、同時再生を制御可能
- **安全**: メモリ管理が自動化されている
- **効率的**: リソースの共有により省メモリ

詳細な実装については、`Engine/Audio/`ディレクトリ内のソースコードを参照してください。
