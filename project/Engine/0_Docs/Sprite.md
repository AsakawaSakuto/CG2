# Sprite DocuUpdateion

このドキュメントでは、Engineに実装されているSpriteの使い方を説明します。<br>

## 目次
1. [初期化～描画まで](#初期化～描画まで)
2. [ImGuiの使用](#ImGuiの使用)
---

# 初期化～描画まで
## 初期化

`Engine/2d/Sprite.h`で、使用することができます：

```cpp
unique_ptr<Sprite> testSprite_ = make_unique<Sprite>();

// 第一引数 resources/Image 以降のfilePath
// 第二引数 translate デフォで {0.0f,0.0f}  
// 第三引数 scale デフォで {1.0f,1.0f}
testSprite_->Initialize("testSprite.png", { 0.0f,0.0f }, { 1.0f,1.0f });

// 後述のImGuiで作成したJsonFileから初期化後に値を入手するとも可能
// resources/Data/Json/Sprite 以降のParh
testSprite_->LoadFromJson("testSprite");
```

## 更新

Update() を呼び出します、これを使用しないと画面に描画されません：

```cpp
// 行列計算 
testSprite_->Update();
```

## 描画

Drwa() を呼び出します
```cpp
testSprite_->Draw();
```

## ImGuiの使用

呼び出す際に入力した文字列で以下の画像のImGuiが出てきます
```cpp
testSprite_->DrawImGui("testSprite");
```
![alt text](image.png)
### 操作
・ファイル名入力：操作したいJsonFile名を入力 resources/Data/Json/Sprite 以降 <br>
・読み込み　　　：ファイル内に存在するJsonFileから値を読み込みます <br>
・保存　　　　　：ファイル内に存在するJsonFileへ値を書き出します <br>
・新規作成　　　：ファイル内に入力されたFile名のJsonFileを作成します <br>

・SRT：スライダーで調整します <br> 
・AnchorPoint：左上、左下、右上、右下、中央の五か所から選べます