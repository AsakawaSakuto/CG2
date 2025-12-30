#pragma once
#include "2d/Sprite/SpriteDataStruct.h"

#include "Math/Type/Transform.h"
#include "Core/TextureManager/TextureManager.h"
#include "Math/MatrixFunction/MatrixFunction.h"
#include "Utility/FileFormat/Json/JsonManager.h"

#include <cmath>
#include <numbers>
#include <memory>

// 前方宣言
class DirectXCommon;

/// <summary>
/// 2Dスプライトクラス
/// </summary>
class Sprite
{
public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Sprite();

	/// <summary>
	/// Spriteの初期化
	/// </summary>
	/// <param name="fileName">"resources/image/" 以降のPathを渡す</param>
	/// <param name="position">Vector2型でスクリーン座標を設定(任意)</param>
	/// <param name="scale">Vector2型でScaleを設定(任意)</param>
	void Initialize(const std::string& fileName, Vector2 position = { 0.0f,0.0f }, Vector2 scale = { 1.0f,1.0f });

	/// <summary>
	/// 行列計算などの更新
	/// </summary>
	void Update();

	/// <summary>
	/// Spriteの描画
	/// </summary>
	void Draw();

	/// <summary>
	/// TransformやMaterialのImGui描画
	/// </summary>
	/// <param name="objectName">適切な名前を入力</param>
	void DrawImGui(const char* objectName);

	/// <summary>
	/// Transform2Dを設定
	/// </summary>
	/// <param name="transform">更新したいTransform2Dを渡す</param>
	void SetTransform(const Transform2D& transform) { transform2D_ = transform; }

	/// <summary>
	/// Vector2型でTranslateを設定
	/// </summary>
	/// <param name="position">更新したいTranslateを渡す</param>
	void SetPosition(const Vector2& position) { transform2D_.translate = position; }

	/// <summary>
	/// Vector2型でScaleを設定
	/// </summary>
	/// <param name="scale">更新したいScaleを渡す</param>
	void SetScale(const Vector2& scale) { transform2D_.scale = scale; }

	/// <summary>
	/// floatでRotateを設定
	/// </summary>
	/// <param name="rotate">更新したいRotateを渡す</param>
	void SetRotate(float rotate) { transform2D_.rotate = rotate; }

	/// <summary>
	/// Colorを設定 Alpha含む
	/// </summary>
	/// <param name="Color">更新したいColorを渡す</param>
	void SetColor(const Vector4& Color) { materialData_->color = Color; }

	/// <summary>
	/// UV用のTransform2Dを設定
	/// </summary>
	/// <param name="transform">更新したいUV用のTransform2Dを渡す</param>
	void SetUVTransform(const Transform2D& uvTransform) { uvTransform_ = uvTransform; }

	/// <summary>
	/// Vector2型で画像サイズを取得
	/// </summary>
	/// <returns>Vector2型で画像サイズを取得</returns>
	Vector2& GetSize() { return size_; }

	/// <summary>
	/// 使用してるTextureを変更
	/// </summary>
	/// <param name="textureName">resources/image/" 以降のPathを渡す</param>
	void SetTexture(const std::string& textureName);

	/// <summary>
	/// アンカーポイントを設定
	/// </summary>
	/// <param name="anchor">アンカーポイント（0.0～1.0）</param>
	void SetAnchorPoint(const Vector2& anchor);

	/// <summary>
	/// JsonFileに設定を保存
	/// </summary>
	/// <param name="filePath">保存先のファイルパス（拡張子不要）</param>
	void SaveToJson(const std::string& filePath);

	/// <summary>
	/// JsonFileから設定を読み込み
	/// </summary>
	/// <param name="filePath">読み込み元のファイルパス（拡張子不要）</param>
	void LoadFromJson(const std::string& filePath);

	/// <summary>
	/// 新規JsonFileを作成
	/// </summary>
	/// <param name="filePath">作成するファイルパス（拡張子不要）</param>
	void CreateNewJsonFile(const std::string& filePath);

private:
	DirectXCommon* dxCommon_ = nullptr;
	HRESULT hr_;

	uint32_t textureIndex_ = 0;

	Transform2D transform2D_ = {};
	Transform2D uvTransform_ = {};

	Vector2 size_ = {};
	Vector2 anchorPoint_ = { 0.5f,0.5f };

	std::string textureName_;

	// JSON管理
	std::unique_ptr<JsonManager> jsonManager_;
	std::string loadToSaveName_ = "filePath";

	//----------------------------------------------//

	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// コマンドリスト(まとまった命令群)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;

	// リソースデータ（2D Sprite専用構造体を使用）
	SpriteVertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	SpriteMaterial* materialData_ = nullptr;
	SpriteTransformationMatrix* transformationData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	//----------------------------------------------//

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
};