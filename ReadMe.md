# CG3 評価課題3 説明書

## カメラの操作方法

Aキー DキーでX移動 ・ Wキー SキーでY移動 ・ マウスホイールで Z移動<br>
左クリックしながらドラッグで視点移動<br>
右下のDebugCamera用ImGuiにあるリセットボタンで初期位置に移動<br>

---
## Objectに連動したImGuiの出し方

画面右端のImGuiで操作したいObjectの名前にチェックをつけると出てきます<br>

---

## ImGuiの説明

基本的に上から Transform Material Light の値を触れるようにしてます<br>
それぞれの要素にResetボタンがあるので活用してください<br>

---

## GamePadの説明

Lボタンで左側のモーターが振動<br> Rボタンで右側のモーターが振動<br> Aボタンで両方のモーターが振動<br>振動の強さは画面右側のImGuiで操作できます(0.0f～1.0f)

## Soundの説明

Zキーを押すことで効果音(fanfare)が流れます

---

## 追加した加点要素

* 球の描画(3)<br>

* UVTransform(2)<br>

* Lambertian Reflectance(2)<br>

* Harf Lambert(3)<br>

* Lighting方式の変更(2)<br>

* 複数モデルの描画(3)<br>

* Utah Teapotの描画(1)<br>

* Stanford Bunntの描画(1)<br>

* Suzanneの描画(6)<br>

* MultiMesh対応(3)<br>

* MultiMaterial対応(6)予定<br>

* GamePad(4)<br>

* Sound(3)<br>

---

## その他(5)

* SpotLight PointLight を実装 ImGuiで動的切り替え可能
* GPUパーティクルを実装 パラメーターをいじって遊んでみて下さい
