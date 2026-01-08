# Model クラス ドキュメント

このドキュメントでは、Engineに実装されている通常の `Model` クラスの使い方を説明します。

## 目次
1. [初期化～描画まで](#初期化～描画まで)
2. [Transformの設定](#transformの設定)
3. [テクスチャ・UVの設定](#テクスチャuvの設定)
4. [描画設定（ワイヤー・透過）](#描画設定ワイヤー透過)
5. [カリング・ビルボード](#カリングビルボード)
6. [ImGuiでの操作](#imguiでの操作)

---

# 初期化～描画まで

## 初期化

`Engine/3d/Model/Model.h` に実装された `Model` クラスを使います。通常は `resources/model/` 以下のパスを指定して初期化します：

```cpp
// ヘッダにModelをインクルード
#include "Engine/3d/Model/Model.h"

// 所有するポインタを用意
std::unique_ptr<Model> model = std::make_unique<Model>();

// 第一引数は "resources/model/" 以降のモデルファイルパス
model->Initialize("testModelPath");
```

## 更新・描画

Model自体に明示的なUpdate関数はありませんが、描画時に `Draw` を呼び出してレンダリングします。カメラと任意の `Transform` を渡せます：

```cpp
// Camera はシーンで使用中のカメラを渡す
model->Draw(sceneCamera, transform);
```

# Transformの設定

Transform（SRT）は `SetTransform` や各成分のセッターから設定できます：

```cpp
// Transform全体を設定
model->SetTransform(transform);

// 個別に設定
model->SetTranslate({0.0f, 1.0f, 0.0f});
model->SetRotate({0.0f, 3.14f, 0.0f});
model->SetScale({1.0f, 1.0f, 1.0f});
```

# テクスチャ・UVの設定

モデルに使用するテクスチャの差し替えやUV変換が可能です：

```cpp
// テクスチャ名を指定して差し替え（Pathはフルパスやリソース内の規約に合わせる）
model->SetTexture("texture.png");

// テクスチャの一部だけを切り取る場合（UVで0.0～1.0の範囲）
model->SetTextureWithRect("atlas.png", {0.0f, 0.0f}, {0.5f, 0.5f});

// UV変換（Translate/Rotate/Scale）
model->SetUvTranslate({0.1f, 0.0f});
model->SetUvRotate(0.5f);
model->SetUvScale({0.5f, 0.5f});
```

# 描画設定（ワイヤー・透過）

描画モードや透過描画の切り替えが可能です：

```cpp
// trueで通常、falseでワイヤーフレーム（メソッド名の仕様に注意）
model->SetDrawMode(true);

// 半透明描画を有効にする
model->SetTransparent(true);
```

# カリング・ビルボード

フラスタムカリングやビルボード機能を利用できます：

```cpp
// カメラ外の描画や更新を制御
model->SetDrawFrustumCulling(true);   // カメラ外は描画しない
model->SetUpdateFrustumCulling(false); // カメラ外でも更新する

// ビルボード
model->SetBillboard(true);   // 全面ビルボード
model->SetBillboardY(true);  // Y軸回転のみのビルボード
```

# ImGuiでの操作

`Model` は Transform や Material の調整を ImGui で行うことができます（JsonFileに読み書き機能を追加予定）：

```cpp
model->DrawImGui("MyModel");
```

---

注意事項
- `Initialize` の引数は `resources/model/` 以下の相対パスを想定しています。プロジェクトの資源配置に合わせて指定してください。
- `Draw` に渡す `Camera` は現在使用中のシーンカメラを渡す必要があります。カメラ情報を間違えると想定通りに表示されません。
- 大きなモデルや複数インスタンスを扱う場合、内部で共有されるジオメトリキャッシュを利用してメモリを節約します。

以上が通常の `Model` クラスの基本的な使い方です。必要に応じて実装やパラメータを参照してください。
