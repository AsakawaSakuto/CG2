#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <memory>

#include "DirectXCommon.h"
#include "Camera.h"
#include "MatrixFunction.h"
#include "Transform.h"
#include "Vector3.h"
#include "Sphere.h"
#include "AABB.h"
#include "OBB.h"
#include "OvalSphere.h"

/// <summary>
/// 3D空間に線を描画するクラス（デバッグ表示、UI、可視化用）
/// </summary>
class Line {
public:
    /// <summary>
    /// 線の頂点データ
    /// </summary>
    struct LineVertex {
        Vector3 position; // 位置
        Vector4 color;    // 色
    };

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Initialize(DirectXCommon* dxCommon);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Line();

    /// <summary>
    /// 線を追加（2点指定）
    /// </summary>
    /// <param name="start">開始点</param>
    /// <param name="end">終了点</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddLine(const Vector3& start, const Vector3& end, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// 複数の線を一度に追加
    /// </summary>
    /// <param name="points">頂点配列</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddLines(const std::vector<Vector3>& points, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// グリッド描画
    /// </summary>
    /// <param name="size">グリッドサイズ</param>
    /// <param name="divisions">分割数</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddGrid(float size = 10.0f, int divisions = 10, const Vector4& color = { 0.25f, 0.25f, 0.25f, 1.0f });

    /// <summary>
    /// ボックス（中心とサイズで指定）描画
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="size">サイズ</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddBox(const Vector3& center, const Vector3& size, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// ボックス（AABB）描画
    /// </summary>
    /// <param name="aabb">AABB構造体</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddBox(const AABB& aabb, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// ボックス（OBB）描画
    /// </summary>
    /// <param name="obb">OBB構造体</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddBox(const OBB& obb, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// 球体（ワイヤーフレーム）描画
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="radius">半径</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddSphere(const Sphere& sphere, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// 楕円球体（ワイヤーフレーム）描画
    /// </summary>
    /// <param name="ovalSphere">楕円球体構造体</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddOvalSphere(const OvalSphere& ovalSphere, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// 円描画
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="radius">半径</param>
    /// <param name="normal">法線方向</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddCircle(const Vector3& center, float radius, const Vector3& normal, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// XZ平面、円描画
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="radius">半径</param>
    /// <param name="normal">法線方向</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddCircleXZ(const Vector3& center, float radius, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// レイ（半直線）描画
    /// </summary>
    /// <param name="origin">始点</param>
    /// <param name="direction">方向</param>
    /// <param name="length">長さ</param>
    /// <param name="color">色（デフォルト：白）</param>
    void AddRay(const Vector3& origin, const Vector3& direction, float length, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="camera">使用するカメラ</param>
    void Draw(Camera& camera);

private:
    // DirectX関連
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> transformBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    // GPU転送用データ
    LineVertex* vertexData_ = nullptr;
    struct TransformMatrix {
        Matrix4x4 viewProjection;
    };
    TransformMatrix* transformData_ = nullptr;

    // 線データ
    std::vector<LineVertex> vertices_;
    size_t maxVertices_ = 1000;

    // リソース作成関数
    void CreateVertexBuffer();
    void CreateTransformBuffer();

    // リソース更新関数
    void UpdateVertexBuffer();

	// 円の分割数
	int segments_ = 16;

	// 球体描画用の分割数
    const int latitudeDivisions_ = 8;   // 緯度の分割数（水平方向の円の数）
    const int longitudeDivisions_ = 16; // 経度の分割数（垂直方向の線の数）
};
