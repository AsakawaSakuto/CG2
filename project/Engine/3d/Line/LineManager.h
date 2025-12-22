#pragma once
#include <memory>
#include "Data/Line.h"
#include "Utility/Collision/CollisionShape.h"
#include "Math/Type/Vector4.h"
#include "Camera/Camera.h"

/// <summary>
/// Singleton パターンで Line3d を管理するクラス
/// デバッグ描画ラインの一元管理とAPI統一を提供
/// </summary>
class LineManager {
public:
    /// <summary>
    /// Singleton インスタンスの取得（ポインタ）
    /// </summary>
    static LineManager* GetInstance();

    // コピー・ムーブ禁止
    LineManager(const LineManager&) = delete;
    LineManager& operator=(const LineManager&) = delete;
    LineManager(LineManager&&) = delete;
    LineManager& operator=(LineManager&&) = delete;

    /// <summary>
    /// 明示的な初期化（オプション）
    /// ※初回の Add または Draw 時に自動で遅延初期化されるため、呼ばなくても可
    /// </summary>
    void Initialize();

    /// <summary>
    /// 形状を追加（統一API）
    /// </summary>
    void AddShape(const Shape& shape, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

	/// <summary>
	/// 
	/// </summary>
	/// <param name="size"></param>
	/// <param name="divisions"></param>
	/// <param name="color"></param>
	void AddGrid(float size, int divisions, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// すべての線を描画
    /// </summary>
    void Draw(Camera& camera);

private:
    LineManager() = default;
    ~LineManager() = default;

private:
    bool initialized_ = false;
    std::unique_ptr<Line3d> line_;
};