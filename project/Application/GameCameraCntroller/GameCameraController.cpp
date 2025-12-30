#include "GameCameraController.h"
#include "Input/MyInput.h"
#include "Map/TreeManager/TreeManager.h"
#include "Map/Map3D.h"
#include "Utility/Collision/Collision.h"
#include "Utility/GameTimer/DeltaTime.h"
#include <imgui.h>
#include <algorithm>

void GameCameraController::Initialize() {
	// カメラの初期設定
	camera_.SetTarget({0.0f, 0.0f, 0.0f});   // デフォルトターゲット
	originalDistance_ = 35.0f;
	targetDistance_ = originalDistance_;
	currentDistance_ = originalDistance_;
	camera_.SetDistance(originalDistance_);              // デフォルト距離
	camera_.SetTPSAngles(0.0f, 0.3f);        // デフォルト角度（少し上から見下ろす）
}

void GameCameraController::Update() {

	// KeyConfigを使ってカメラ操作の入力を取得
	auto cameraInput = MyInput::GetVector2D(Action::CAMERA_LOOK);

	// マウスの移動量を取得
	auto mouseDelta = MyInput::GetInput()->GetMouseDelta();

	// 感度設定
	const float horizontalSensitivity = 0.05f;  // 水平方向の感度（スティック）
	const float verticalSensitivity = 0.03f;    // 垂直方向の感度（スティック）
	const float mouseHorizontalSensitivity = 0.003f;  // 水平方向の感度（マウス）
	const float mouseVerticalSensitivity = 0.003f;    // 垂直方向の感度（マウス）

	float deltaHorizontal = 0.0f;
	float deltaVertical = 0.0f;

	// スティック入力からカメラの角度を更新
	if (std::abs(cameraInput.x) > 0.1f || std::abs(cameraInput.y) > 0.1f) {
		// 水平角度：スティックのX軸
		deltaHorizontal += cameraInput.x * horizontalSensitivity;
		
		// 垂直角度：スティックのY軸（反転）
		deltaVertical += -cameraInput.y * verticalSensitivity;
	}

	// マウス右クリックでカメラ操作モードをトグル（ON/OFF切り替え）
	auto* input = MyInput::GetInput();
	if (input && input->GetWinApp()) {
		// 右クリックのトリガー（押した瞬間）でトグル
		if (input->TriggerMouseButtonR()) {
			isMouseCameraActive_ = !isMouseCameraActive_;
			
			if (isMouseCameraActive_) {
				// マウスカメラモードをON
				ShowCursor(FALSE);
			} else {
				// マウスカメラモードをOFF
				ShowCursor(TRUE);
			}
		}

		// マウスカメラモードがアクティブの場合
		if (isMouseCameraActive_) {
			// マウス入力からカメラの角度を更新
			if (std::abs(mouseDelta.x) > 0.01f || std::abs(mouseDelta.y) > 0.01f) {
				// 水平角度：マウスのX軸移動
				deltaHorizontal += mouseDelta.x * mouseHorizontalSensitivity;
				
				// 垂直角度：マウスのY軸移動
				deltaVertical += mouseDelta.y * mouseVerticalSensitivity;
			}

			// マウスを画面中央にリセット
			HWND hwnd = input->GetWinApp()->GetHWND();
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			int centerX = (clientRect.right - clientRect.left) / 2;
			int centerY = (clientRect.bottom - clientRect.top) / 2;
			
			POINT centerPoint = { centerX, centerY };
			ClientToScreen(hwnd, &centerPoint);
			SetCursorPos(centerPoint.x, centerPoint.y);
			
			// Inputクラスの内部マウス座標も更新
			input->SetMousePosition(centerX, centerY);
		}
	}

	// カメラの角度を更新
	if (deltaHorizontal != 0.0f || deltaVertical != 0.0f) {
		camera_.AddTPSAngles(deltaHorizontal, deltaVertical);
	}

	// オフセットを適用したターゲット位置を設定
	camera_.SetTarget(targetPosition_ + offset_);
	
	// スムーズな距離補間
	float deltaTime = ::GetDeltaTime();
	
	// 目標距離に向けてスムーズに補間
	if (currentDistance_ > targetDistance_) {
		// 近づく（遮蔽されているので速く）
		float speed = approachSpeed_ * deltaTime;
		currentDistance_ = std::max(targetDistance_, currentDistance_ - speed * (currentDistance_ - targetDistance_ + 1.0f));
	} else {
		// 離れる（ゆっくり）
		float speed = retreatSpeed_ * deltaTime;
		currentDistance_ = std::min(targetDistance_, currentDistance_ + speed * (targetDistance_ - currentDistance_ + 1.0f));
	}
	
	// 距離を適用
	camera_.SetDistance(currentDistance_);

	// TPSカメラの更新（地面貫通の制限はCalculateTPSPosition内で処理される）
	camera_.UpdateTPS();
}

void GameCameraController::SetTarget(Vector3 targetPosition) {
	targetPosition_ = targetPosition;
	camera_.SetTarget(targetPosition_ + offset_);
}

void GameCameraController::SetDistance(float distance) {
	originalDistance_ = distance;
	targetDistance_ = distance;
	camera_.SetDistance(distance);
}

void GameCameraController::CheckBlockOcclusion(Map3D* map) {
	if (!map) {
		targetDistance_ = originalDistance_;
		return;
	}
	
	// プレイヤーとカメラの理想的な位置を計算（originalDistanceを使用）
	Vector3 playerPos = targetPosition_ - Vector3{ 0.0, -2.0f, 0.0f };
	
	// カメラの方向を取得（角度から計算）
	float horizontalAngle = camera_.GetHorizontalAngle();
	float verticalAngle = camera_.GetVerticalAngle();
	
	float cosVertical = std::cos(verticalAngle);
	float sinVertical = std::sin(verticalAngle);
	float cosHorizontal = std::cos(horizontalAngle);
	float sinHorizontal = std::sin(horizontalAngle);
	
	// 方向ベクトル（プレイヤーからカメラへ）
	Vector3 direction = {
		cosVertical * sinHorizontal,
		sinVertical,
		cosVertical * cosHorizontal
	};
	
	// レイキャストの最大距離
	float maxDistance = originalDistance_;
	
	// レイキャストのステップ数（より細かくチェック）
	const int steps = static_cast<int>(maxDistance / 0.5f) + 1;  // 0.5f間隔でチェック
	float stepSize = maxDistance / static_cast<float>(steps);
	
	float nearestBlockDistance = maxDistance;  // 最も近いブロックまでの距離
	bool foundOcclusion = false;
	
	// プレイヤーからカメラに向かってレイキャスト
	for (int i = 1; i <= steps; ++i) {
		float t = stepSize * static_cast<float>(i);
		Vector3 checkPos = {
			playerPos.x + direction.x * t,
			playerPos.y + direction.y * t,
			playerPos.z + direction.z * t
		};
		
		// この位置がマップのブロック内にあるかチェック
		uint32_t mapX, mapY, mapZ;
		if (map->WorldToMap(checkPos, mapX, mapY, mapZ)) {
			TileType tileType = map->GetTile(mapX, mapY, mapZ);
			
			// Normalブロックのみチェック（スロープは無視）
			if (tileType == TileType::Normal) {
				// ブロックのAABBを取得してより正確な衝突判定
				Vector3 blockCenter = map->MapToWorld(mapX, mapY, mapZ);
				AABB blockAABB;
				blockAABB.center = blockCenter;
				blockAABB.min = { -7.5f, -5.0f, -7.5f };
				blockAABB.max = { 7.5f,  5.0f,  7.5f };
				
				// チェックポイントがAABB内にあるかチェック
				Vector3 localPos = {
					checkPos.x - blockAABB.center.x,
					checkPos.y - blockAABB.center.y,
					checkPos.z - blockAABB.center.z
				};
				
				bool insideAABB = 
					localPos.x >= blockAABB.min.x && localPos.x <= blockAABB.max.x &&
					localPos.y >= blockAABB.min.y && localPos.y <= blockAABB.max.y &&
					localPos.z >= blockAABB.min.z && localPos.z <= blockAABB.max.z;
				
				if (insideAABB) {
					// ブロックが見つかった場合、プレイヤーからこのブロックまでの距離を計算
					float blockDistance = t;
					
					// ブロックの手前で止める（少しマージンを取る）
					const float margin = 1.5f;
					float safeDistance = blockDistance - margin;
					
					if (safeDistance < nearestBlockDistance) {
						nearestBlockDistance = safeDistance;
						foundOcclusion = true;
					}
					
					// デバッグ描画
					if (showDebugRay_) {
						MyDebugLine::AddShape(blockAABB, { 1.0f, 0.0f, 0.0f, 1.0f });
					}
					
					// 最初に見つかったブロックで停止
					break;
				}
			}
		}
	}
	
	// 最小距離を設定（プレイヤーに近づきすぎないように）
	const float minDistance = 3.0f;
	
	if (foundOcclusion) {
		// ブロックが遮っている場合、目標距離を設定
		targetDistance_ = std::max(minDistance, nearestBlockDistance);
	} else {
		// 遮るものがない場合は元の距離を目標にする
		targetDistance_ = originalDistance_;
	}
	
	// デバッグ用のレイを描画
	if (showDebugRay_) {
		Vector3 endPos = {
			playerPos.x + direction.x * currentDistance_,
			playerPos.y + direction.y * currentDistance_,
			playerPos.z + direction.z * currentDistance_
		};
		Line debugLine;
		debugLine.start = playerPos;
		debugLine.end = endPos;
		
		if (foundOcclusion) {
			MyDebugLine::AddShape(debugLine, { 1.0f, 1.0f, 0.0f, 1.0f });
		} else {
			MyDebugLine::AddShape(debugLine, { 0.0f, 1.0f, 0.0f, 1.0f });
		}
	}
}

void GameCameraController::DrawImgui() {
#ifdef  USE_IMGUI

	if (ImGui::TreeNode("GameCameraController")) {
		// マウスカメラモードの状態表示
		ImGui::Text("Mouse Camera Mode: %s", isMouseCameraActive_ ? "ON" : "OFF");
		ImGui::Text("(Right Click to Toggle)");
		ImGui::Separator();
		
		// ターゲット位置の調整
		ImGui::DragFloat3("Target Position", &targetPosition_.x, 0.1f);
		
		// オフセットの調整
		if (ImGui::DragFloat3("Offset", &offset_.x, 0.1f)) {
			camera_.SetTarget(targetPosition_ + offset_);
		}
		
		// カメラの距離調整
		if (ImGui::DragFloat("Original Distance", &originalDistance_, 0.1f, 1.0f, 100.0f)) {
			targetDistance_ = originalDistance_;
		}
		
		ImGui::Text("Target Distance: %.2f", targetDistance_);
		ImGui::Text("Current Distance: %.2f", currentDistance_);
		
		ImGui::Separator();
		ImGui::Text("Smooth Settings:");
		ImGui::DragFloat("Approach Speed", &approachSpeed_, 0.1f, 0.1f, 50.0f);
		ImGui::DragFloat("Retreat Speed", &retreatSpeed_, 0.1f, 0.1f, 20.0f);
		
		// デバッグレイの表示切り替え
		ImGui::Checkbox("Show Debug Ray", &showDebugRay_);
		
		// カメラの角度調整
		float horizontalAngle = camera_.GetHorizontalAngle();
		float verticalAngle = camera_.GetVerticalAngle();
		
		if (ImGui::DragFloat("Horizontal Angle", &horizontalAngle, 0.01f)) {
			camera_.SetTPSAngles(horizontalAngle, verticalAngle);
		}
		
		if (ImGui::DragFloat("Vertical Angle", &verticalAngle, 0.01f, -1.5f, 1.5f)) {
			camera_.SetTPSAngles(horizontalAngle, verticalAngle);
		}
		
		// カメラ内部のImGuiも表示
		camera_.DrawImgui();
		
		ImGui::TreePop();
	}

#endif //  USE_IMGUI
}

void GameCameraController::UpdateOccluderTransparency(TreeManager* treeManager) {
	// カメラとプレイヤー（ターゲット）の位置を取得
	Vector3 cameraPos = camera_.GetTranslate();
	Vector3 playerPos = targetPosition_;
	
	if (treeManager) {
		treeManager->ResetAllTransparency();
	}
	
	if (treeManager) {
		treeManager->SetOccludersTransparent(cameraPos, playerPos);
	}
}