#pragma once
#include "Camera/Camera.h"
#include "3d/Line/MyDebugLine.h"
#include "Utility/Collision/Type/LineStruct.h"
#include <cmath>

// 前方宣言
class TreeManager;

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
	
	Camera& GetCamera() { return camera_; }

private:
	Camera camera_;
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 offset_ = { 0.0f, 4.0f, 0.0f };
	
	bool isMouseCameraActive_ = false; // マウスカメラ操作がアクティブかどうか

	Line cameraRay_ = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
};