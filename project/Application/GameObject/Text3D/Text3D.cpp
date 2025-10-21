#include "Text3D.h"
#include <algorithm>

void Text3D::Initialize(DirectXCommon* dxCommon) { 
	dxCommon_ = dxCommon;

	for (int i = 0; i < model_.size(); i++) {
		model_[i] = std::make_unique<Model>();
	}

	model_[0]->Initialize(dxCommon_, "TitleLogo/o.obj");
	model_[1]->Initialize(dxCommon_, "TitleLogo/ka.obj");
	model_[2]->Initialize(dxCommon_, "TitleLogo/si.obj");
	model_[3]->Initialize(dxCommon_, "TitleLogo/a.obj");
	model_[4]->Initialize(dxCommon_, "TitleLogo/tu.obj");
	model_[5]->Initialize(dxCommon_, "TitleLogo/me.obj");
	model_[6]->Initialize(dxCommon_, "TitleLogo/te.obj");
	model_[7]->Initialize(dxCommon_, "TitleLogo/ku.obj");
	model_[8]->Initialize(dxCommon_, "TitleLogo/ma.obj");
	model_[9]->Initialize(dxCommon_, "TitleLogo/sa.obj");
	model_[10]->Initialize(dxCommon_, "TitleLogo/i.obj");
	model_[11]->Initialize(dxCommon_, "TitleLogo/da.obj");
	model_[12]->Initialize(dxCommon_, "TitleLogo/-.obj");

	// Transform配列を初期化
	for (int i = 0; i < transform_.size(); i++) {
		model_[i]->SetTexture("resources/image/0.png");
		transform_[i].rotate = { 0.0f, 0.0f, 0.0f };

		timer_[i].Start(1.5f + (i * 0.15f), false);
	}

	for (int i = 0; i < 7; i++) {
		transform_[i].scale = { 0.6f, 0.6f, 1.0f };
	}

	for (int i = 7; i < 11; i++) {
		transform_[i].scale = { 0.9f, 0.9f, 1.0f };
	}

	transform_[11].scale = { 0.85f, 0.85f, 1.0f };
	transform_[12].scale = { 1.0f, 1.0f, 1.0f };

	// 各文字の最終位置を設定
	transform_[0].translate = { -5.1f, 2.2f, 0.0f };
	transform_[1].translate = { -4.35f, 2.2f, 0.0f };
	transform_[2].translate = { -3.5f, 2.2f, 0.0f };
	transform_[3].translate = { -2.9f, 2.2f, 0.0f };
	transform_[4].translate = { -2.2f, 2.2f, 0.0f };
	transform_[5].translate = { -1.5f, 2.2f, 0.0f };
	transform_[6].translate = { -0.85f, 2.2f, 0.0f };
	transform_[7].translate = { -5.05f, 1.15f, 0.0f };
	transform_[8].translate = { -4.05f, 1.15f, 0.0f };
	transform_[9].translate = { -2.95f, 1.15f, 0.0f };
	transform_[10].translate = { -1.9f, 1.15f, 0.0f };
	transform_[11].translate = { -1.0f, 1.15f, 0.0f };
	transform_[12].translate = { 20.0f, 1.15f, 0.0f };
}

void Text3D::Update() {
	// 位置のアニメーション（各タイマーの進行度を使用）
	// バウンスアニメーション中でない場合のみ、通常の位置アニメーションを適用
	if (!muniTimer_[0].IsActive()) {
		transform_[0].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[0].GetProgress(),Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[1].IsActive()) {
		transform_[1].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[1].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[2].IsActive()) {
		transform_[2].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[2].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[3].IsActive()) {
		transform_[3].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[3].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[4].IsActive()) {
		transform_[4].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[4].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[5].IsActive()) {
		transform_[5].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[5].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[6].IsActive()) {
		transform_[6].translate.y = Easing::Lerp(10.0f, 2.2f, timer_[6].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[7].IsActive()) {
		transform_[7].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[7].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[8].IsActive()) {
		transform_[8].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[8].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[9].IsActive()) {
		transform_[9].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[9].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[10].IsActive()) {
		transform_[10].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[10].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	if (!muniTimer_[11].IsActive()) {
		transform_[11].translate.y = Easing::Lerp(10.0f, 1.15f, timer_[11].GetProgress(), Easing::Type::EaseInOutBounce);
	}
	
	transform_[12].translate.x = Easing::Lerp(20.0f, 0.1f, timer_[12].GetProgress(), Easing::Type::EaseInOutBounce);

	// muniStartTimerの開始条件（1回だけ実行するように）
	if (timer_[12].IsFinished() && !muniStartTimer_.IsActive() && !muniStartTimer_.IsFinished()) {
		muniStartTimer_.Start(5.0f, false); // 待機時間を短縮
	}

	// 最初のmuniTimerを開始
	if (muniStartTimer_.IsFinished() && !muniTimer_[0].IsActive() && !muniTimer_[0].IsFinished()) {
		muniTimer_[0].Start(0.6f, false);
	}

	// 連続してmuniTimerを開始（波のように重複させる）
	for (int i = 1; i < muniTimer_.size(); i++) {
		// 前の文字が開始してから0.1秒後に次の文字を開始（重複して動く）
		if (muniTimer_[i - 1].IsActive() && muniTimer_[i - 1].GetProgress() >= 0.15f && !muniTimer_[i].IsActive() && !muniTimer_[i].IsFinished()) {
			muniTimer_[i].Start(0.6f, false);
		}
	}

	// スケールアニメーション → 位置バウンスアニメーションに変更
	for (int i = 0; i < 7; i++) {
		if (muniTimer_[i].IsActive()) {
			float progress = muniTimer_[i].GetProgress();
			// バウンス効果: 上に浮いてから元の位置に戻る
			float bounceHeight = 0.5f; // バウンスの高さ
			float baseY = 2.2f; // 元のY位置
			
			if (progress <= 0.3f) {
				// 最初の30%: 上昇
				transform_[i].translate.y = Easing::Lerp(baseY, baseY + bounceHeight, progress / 0.3f, Easing::Type::EaseOutQuad);
			} else {
				// 残りの70%: バウンスしながら落下
				float fallProgress = (progress - 0.3f) / 0.7f;
				transform_[i].translate.y = Easing::Lerp(baseY + bounceHeight, baseY, fallProgress, Easing::Type::EaseOutBounce);
			}
		}
	}

	for (int i = 7; i < 11; i++) {
		if (muniTimer_[i].IsActive()) {
			float progress = muniTimer_[i].GetProgress();
			// バウンス効果: 上に浮いてから元の位置に戻る
			float bounceHeight = 0.5f; // バウンスの高さ
			float baseY = 1.15f; // 元のY位置
			
			if (progress <= 0.3f) {
				// 最初の30%: 上昇
				transform_[i].translate.y = Easing::Lerp(baseY, baseY + bounceHeight, progress / 0.3f, Easing::Type::EaseOutQuad);
			} else {
				// 残りの70%: バウンスしながら落下
				float fallProgress = (progress - 0.3f) / 0.7f;
				transform_[i].translate.y = Easing::Lerp(baseY + bounceHeight, baseY, fallProgress, Easing::Type::EaseOutBounce);
			}
		}
	}

	if (muniTimer_[11].IsActive()) {
		float progress = muniTimer_[11].GetProgress();
		// バウンス効果: 上に浮いてから元の位置に戻る
		float bounceHeight = 0.5f; // バウンスの高さ
		float baseY = 1.15f; // 元のY位置
		
		if (progress <= 0.3f) {
			// 最初の30%: 上昇
			transform_[11].translate.y = Easing::Lerp(baseY, baseY + bounceHeight, progress / 0.3f, Easing::Type::EaseOutQuad);
		} else {
			// 残りの70%: バウンスしながら落下
			float fallProgress = (progress - 0.3f) / 0.7f;
			transform_[11].translate.y = Easing::Lerp(baseY + bounceHeight, baseY, fallProgress, Easing::Type::EaseOutBounce);
		}
	}

	if (muniTimer_[12].IsActive()) {
		float progress = muniTimer_[12].GetProgress();
		// バウンス効果: 上に浮いてから元の位置に戻る
		float bounceHeight = 0.5f; // バウンスの高さ
		float baseY = 0.1f; // 元のY位置
		
		if (progress <= 0.3f) {
			// 最初の30%: 上昇
			transform_[12].translate.x = Easing::Lerp(baseY, baseY + bounceHeight, progress / 0.3f, Easing::Type::EaseOutQuad);
		} else {
			// 残りの70%: バウンスしながら落下
			float fallProgress = (progress - 0.3f) / 0.7f;
			transform_[12].translate.x = Easing::Lerp(baseY + bounceHeight, baseY, fallProgress, Easing::Type::EaseOutBounce);
		}
	}

	// 全ての文字のバウンスが終了したら、再びループを開始
	bool allFinished = true;
	for (int i = 0; i < muniTimer_.size(); i++) {
		if (muniTimer_[i].IsActive() || !muniTimer_[i].IsFinished()) {
			allFinished = false;
			break;
		}
	}
	
	// 全て終了したら、少し待ってから再開
	if (allFinished && muniStartTimer_.IsFinished()) {
		// タイマーをリセットして再開準備
		muniStartTimer_.Start(5.0f, false); // 1秒待機してから再開
		for (int i = 0; i < muniTimer_.size(); i++) {
			muniTimer_[i].Reset(); // タイマーをリセット
		}
	}

	// 全タイマーの更新
	for (int i = 0; i < timer_.size(); i++) {
		timer_[i].Update();
		muniTimer_[i].Update();
	}
	muniStartTimer_.Update();

	for (int i = 0; i < model_.size(); i++) {
		model_[i]->SetTransform(transform_[i]);
		model_[i]->Update();
	}
}

void Text3D::Draw(Camera& useCamera) {
	for (int i = 0; i < model_.size(); i++) {
		model_[i]->Draw(useCamera);
	}
}

void Text3D::DrawImGui(){

	ImGui::Begin("titleLogo");

	for (int i = 0; i < transform_.size(); i++) {
		std::string label = "Transform " + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &transform_[i].translate.x, 0.01f);
	}

	ImGui::End();

	ImGui::Begin("titleLogo2");

	for (int i = 0; i < transform_.size(); i++) {
		std::string label = "rotate" + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &transform_[i].rotate.x, 0.01f);
	}

	ImGui::End();

	ImGui::Begin("titleLogo3");

	for (int i = 0; i < transform_.size(); i++) {
		std::string label = "scale" + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &transform_[i].scale.x, 0.01f);
	}

	ImGui::End();

	// Debug window for timers
	ImGui::Begin("Timer Debug");
	
	ImGui::Text("Main Timer 12 Finished: %s", timer_[12].IsFinished() ? "true" : "false");
	ImGui::Text("Muni Start Timer Active: %s", muniStartTimer_.IsActive() ? "true" : "false");
	ImGui::Text("Muni Start Timer Finished: %s", muniStartTimer_.IsFinished() ? "true" : "false");
	
	for (int i = 0; i < muniTimer_.size(); i++) {
		ImGui::Text("MuniTimer[%d] Active: %s, Progress: %.3f", i, 
			muniTimer_[i].IsActive() ? "true" : "false", 
			muniTimer_[i].GetProgress());
	}
	
	ImGui::End();

}