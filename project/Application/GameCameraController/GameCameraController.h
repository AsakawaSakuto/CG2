#pragma once
#include "Camera/Camera.h"
#include "3d/Line/MyDebugLine.h"
#include "Utility/Collision/Type/LineStruct.h"
#include <cmath>

// 前方宣言
class TreeManager;
class Map3D;

class GameCameraController
{
public:
	void Initialize();
	void Update();
	void DrawImgui();
	
	// TPS機能
	void SetTarget(Vector3 targetPosition);
	void SetDistance(float distance);
	
	/// <summary>
	/// カメラとプレイヤー間の障害物を半透明化
	/// </summary>
	/// <param name="map">マップへのポインタ</param>
	/// <param name="treeManager">ツリーマネージャーへのポインタ</param>
	void UpdateOccluderTransparency(TreeManager* treeManager);
	
	/// <summary>
	/// カメラとプレイヤー間のブロック衝突をチェックして、カメラ距離を調整
	/// </summary>
	/// <param name="map">マップへのポインタ</param>
	void CheckBlockOcclusion(Map3D* map);
	
	Camera& GetCamera() { return camera_; }

private:
	Camera camera_;
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 offset_ = { 0.0f, 4.0f, 0.0f };
	
	bool isMouseCameraActive_ = false; // マウスカメラ操作がアクティブかどうか

	Line cameraRay_ = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	
	float originalDistance_ = 27.5f;   // 設定された距離
	float targetDistance_ = 27.5f;     // 目標距離（遮蔽を考慮した理想的な距離）
	float currentDistance_ = 27.5f;    // 現在の実際の距離（スムーズに補間）
	
	// スムーズな距離調整用パラメータ
	float approachSpeed_ = 8.0f;       // 近づく速度（遮蔽時は速く）
	float retreatSpeed_ = 3.0f;        // 離れる速度（ゆっくり）
	
	// デバッグ用
	bool showDebugRay_ = false;
};