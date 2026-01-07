# Audio System Documentation

このドキュメントでは、Engineに実装されているAudioManager、MyAudio::の使い方を説明します。

## 目次
1. [音源の登録](#音源の登録)
2. [基本的な使い方](#基本的な使い方)

---

## 音源の登録

### BGMとSEの種類を定義

`Engine/Audio/AutioList.h`で、使用するBGMとSEを列挙型で定義します：

```cpp
enum class BGM_List {
    Title,
    Gane,
    // 新しいBGMをここに追加

    Count
};

enum class SE_List {
    Move
    Jump
    // 新しいSEをここに追加
    
    Count
};
```

### 音源ファイルの読み込み

`AudioManager.cpp`内、で音源を読み込みます：

```cpp
 // 第一引数 BGM or SE のList
 // 第二引数 FilePath  
 // 第三引数 再生される際のVolume(0.0f～1.0f) デフォで1.0f
 Load(BGM_List::Title, "TitleBGM.mp3", 1.0f);
 Load(SE_List::Move, "Move.mp3", 1.0f);
```

**注意**: Loadファイルパスは<br>`resources/sound/BGM/` or `resources/sound/SE/`<br>以降のパスで指定します。

## 基本的な使い方

### 1. MyAudio::

登録した音源は namespace MyAudio::を介して使用します

```cpp
namespace MyAudio { 
    void Play()
    void Stop
}
```

### 2. 再生方法

`MyAudio::`の関数を使用します。<br> Play()の第二引数はLoopの可否、BGMはデフォでtrue、SEはデフォでfalse

```cpp
#include "Engine/Audio/MyAudio.h"

// BGMを再生
MyAudio::Play(BGM_List::Title, true); 

// SEを再生
MyAudio::Play(SE_List::Move, false); 
```

### 3. 停止方法
停止したいBGM、SEのListを入れるだけです。

```cpp
// BGMを停止 SEも同様
MyAudio::Stop(BGM_List::Title);
```

### 4. 音量の変更

特定の音源のVolumeを変更します(0.0f～1.0f)<br>
これによりBGMの遷移なども可能になります。

```cpp
// BGMの音量を変更 SEも同様
MyAudio::SetVolume(BGM_List::Title, 1.0f);
```

### 5. MasterVolume
BGMとSEのMasterVolumeを個別に設定(0.0f～1.0f)、また受け取ることも可能です。
```cpp
MyAudio::SetBgmMasterVolume(1.0f);
MyAudio::SetSeMasterVolume(1.0f);
float bgmMasterVolume = MyAudio::GetBgmMasterVolume();
float seMasterVolume = MyAudio::GetSeMasterVolume();
```