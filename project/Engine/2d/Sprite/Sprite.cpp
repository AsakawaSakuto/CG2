#include "Sprite.h"
#include "Core/WinApp/WinApp.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include "Core/CreateResource/CreateResource.h"

#include <cassert>
#include <filesystem>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

// PSOManagerをインクルード
#include "Core/PSOManager/PSOManager.h"

Sprite::~Sprite() {
	// マップされたリソースを全てアンマップ
	if (vertexResource_ && vertexData_) {
		vertexResource_->Unmap(0, nullptr);
		vertexData_ = nullptr;
	}
	
	if (indexResource_ && indexData_) {
		indexResource_->Unmap(0, nullptr);
		indexData_ = nullptr;
	}
	
	if (materialResource_ && materialData_) {
		materialResource_->Unmap(0, nullptr);
		materialData_ = nullptr;
	}
	
	if (transformationResource_ && transformationData_) {
		transformationResource_->Unmap(0, nullptr);
		transformationData_ = nullptr;
	}
}

void Sprite::Initialize(const std::string& fileName, Vector2 position, Vector2 scale) {
	// ServiceLocatorからDirectXCommonを取得
	DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
	
	if (!dxCommon) {
		// エラー処理：DirectXCommonが登録されていない場合
		throw std::runtime_error("DirectXCommon is not registered in ServiceLocator. Call ServiceLocator::Provide(dxCommon) first.");
	}

	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();
	
	textureName_ = "resources/image/" + fileName;
	TextureManager::GetInstance()->LoadTexture(textureName_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	// テクスチャからサイズを自動取得
	size_ = TextureManager::GetInstance()->GetTextureSize(textureName_);

	transform2D_.scale = scale;
	transform2D_.rotate = 0.0f;
	transform2D_.translate = position;

	// JsonManagerの初期化（初回のみ）
	if (!jsonManager_) {
		jsonManager_ = std::make_unique<JsonManager>();
		jsonManager_->SetBasePath("resources/Data/Json/Sprite/");
	}

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
}

void Sprite::Update() {

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix({ transform2D_.scale.x,transform2D_.scale.y,0.0f }, { 0.0f, 0.0f, transform2D_.rotate }, { transform2D_.translate.x, transform2D_.translate.y, 0.0f });
	Matrix4x4 viewMatrix = MakeIdentityMatrix();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kClientWidth_), static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrixSprite));
	transformationData_->WVP = worldViewProjectionMatrixSprite;

	Matrix4x4 scale = MakeIdentityMatrix();
	scale.m[0][0] = uvTransform_.scale.x;
	scale.m[1][1] = uvTransform_.scale.y;

	Matrix4x4 rot = MakeIdentityMatrix();
	rot.m[0][0] = cos(uvTransform_.rotate);
	rot.m[0][1] = -sin(uvTransform_.rotate);
	rot.m[1][0] = sin(uvTransform_.rotate);
	rot.m[1][1] = cos(uvTransform_.rotate);

	Matrix4x4 trans = MakeIdentityMatrix();
	trans.m[3][0] = uvTransform_.translate.x;
	trans.m[3][1] = uvTransform_.translate.y;

	// 最終変換行列
	materialData_->uvTransformMatrix = scale * rot * trans;
}

void Sprite::Draw() {

	// PSOManagerからRootSignatureとPSOを取得
	auto& psoManager = PSOManager::GetInstance();
	auto rootSignature = psoManager.GetRootSignature("Sprite"); // Sprite専用RootSignature（CLAMPサンプラー）
	auto pso = psoManager.GetPSO(PSOType::Sprite_Normal); // Sprite専用PSOを使用

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	// PSOを設定
	commandList_->SetPipelineState(pso.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);  // VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);
	
	// 2D Sprite用のシンプルなルートパラメータ設定
	// b0: Material, b1: Transform, t0: Texture
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	
	// 描画！ (DrawCall/ドローコール)
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::DrawImGui(const char* objectName) {

#ifdef USE_IMGUI

	ImGui::Begin(objectName);

	// ファイル名入力
	static char fileNameBuffer[256] = "filePath";
	strncpy_s(fileNameBuffer, loadToSaveName_.c_str(), sizeof(fileNameBuffer));
	if (ImGui::InputText("ファイル名", fileNameBuffer, sizeof(fileNameBuffer))) {
		loadToSaveName_ = fileNameBuffer;
	}

	if (ImGui::Button("読み込み")) {
		LoadFromJson(loadToSaveName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("保存")) {
		SaveToJson(loadToSaveName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("新規作成")) {
		CreateNewJsonFile(loadToSaveName_);
	}

	ImGui::Separator();

	ImGui::Text("Transform");
	ImGui::DragFloat2("translate", &transform2D_.translate.x, 1.f);
	ImGui::DragFloat2("scale", &transform2D_.scale.x, 0.01f);
	ImGui::DragFloat("rotate", &transform2D_.rotate, 0.01f);

	if (ImGui::Button("tReset")) {
		transform2D_.translate = { 0.0f ,0.0f };
		transform2D_.scale = { 1.0f, 1.0f};
		transform2D_.rotate = 0.0f;
	}

	ImGui::Separator();

	ImGui::Text("MaterialEdit");
	ImGui::DragFloat2("uvTranslate", &uvTransform_.translate.x, 0.01f);
	ImGui::DragFloat2("uvScale", &uvTransform_.scale.x, 0.01f);
	ImGui::DragFloat("uvRotate", &uvTransform_.rotate, 0.01f);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	
	if (ImGui::Button("mReset")) {
		uvTransform_.translate = { 0.0f,0.0f };
		uvTransform_.scale = { 1.0f,1.0f };
		uvTransform_.rotate = 0.0f;
		materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	}

	ImGui::End();

#endif
}

void Sprite::SetTexture(const std::string& textureName) {
	// すでに同じテクスチャなら処理をスキップ
	if (textureName_ == "resources/image/" + textureName) {
		return;
	}
	textureName_ = "resources/image/" + textureName;
	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	// 読み込んだテクスチャの番号を取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
}

void Sprite::SetAnchorPoint(const Vector2& anchor) {
	anchorPoint_ = anchor;
	
	// 頂点データが既に作成されている場合、再計算
	if (vertexData_) {
		float width = size_.x;
		float height = size_.y;

		float left = 0.0f - anchorPoint_.x * size_.x;
		float right = 1.0f - anchorPoint_.x * size_.x;
		float top = 0.0f - anchorPoint_.y * size_.y;
		float bottom = 1.0f - anchorPoint_.y * size_.y;

		vertexData_[0].position = { left, bottom + height, 0.0f, 1.0f };  // 左下
		vertexData_[1].position = { left, top, 0.0f, 1.0f };              // 左上
		vertexData_[2].position = { right + width, bottom + height, 0.0f, 1.0f }; // 右下
		vertexData_[3].position = { right + width, top, 0.0f, 1.0f };     // 右上
	}
}

void Sprite::SaveToJson(const std::string& filePath) {
	// ディレクトリが存在しない場合は作成
	std::filesystem::path fullPath = jsonManager_->GetBasePath() + filePath + ".json";
	std::filesystem::path directory = fullPath.parent_path();
	
	if (!directory.empty() && !std::filesystem::exists(directory)) {
		try {
			std::filesystem::create_directories(directory);
			printf("[INFO] Created directory: %s\n", directory.string().c_str());
		} catch (const std::filesystem::filesystem_error& e) {
			printf("[ERROR] Failed to create directory: %s\n", e.what());
			return;
		}
	}

	// Spriteの各フィールドをJsonManagerに登録
	jsonManager_->RegistOutput(transform2D_.translate, "position");
	jsonManager_->RegistOutput(transform2D_.scale, "scale");
	jsonManager_->RegistOutput(transform2D_.rotate, "rotate");
	jsonManager_->RegistOutput(uvTransform_.translate, "uvTranslate");
	jsonManager_->RegistOutput(uvTransform_.scale, "uvScale");
	jsonManager_->RegistOutput(uvTransform_.rotate, "uvRotate");
	jsonManager_->RegistOutput(materialData_->color, "color");
	jsonManager_->RegistOutput(anchorPoint_, "anchorPoint");

	// JSONファイルに書き込み
	jsonManager_->Write(filePath);
}

void Sprite::LoadFromJson(const std::string& filePath) {
	// JSONファイルから読み込み
	auto values = jsonManager_->Read(filePath);
	
	if (values.empty()) {
		printf("[WARNING] Failed to load JSON file: %s.json\n", filePath.c_str());
		return;
	}

	// 読み込んだ値を順番に取得して適用
	size_t index = 0;
	if (index < values.size()) {
		transform2D_.translate = JsonManager::Reverse<Vector2>(values[index++]);
		transform2D_.scale = JsonManager::Reverse<Vector2>(values[index++]);
		transform2D_.rotate = JsonManager::Reverse<float>(values[index++]);
		uvTransform_.translate = JsonManager::Reverse<Vector2>(values[index++]);
		uvTransform_.scale = JsonManager::Reverse<Vector2>(values[index++]);
		uvTransform_.rotate = JsonManager::Reverse<float>(values[index++]);
		materialData_->color = JsonManager::Reverse<Vector4>(values[index++]);
		anchorPoint_ = JsonManager::Reverse<Vector2>(values[index++]);
	}

	// アンカーポイントが変更された場合は頂点データを更新
	SetAnchorPoint(anchorPoint_);
}

void Sprite::CreateNewJsonFile(const std::string& filePath) {
	// ディレクトリが存在しない場合は作成
	std::filesystem::path fullPath = jsonManager_->GetBasePath() + filePath + ".json";
	std::filesystem::path directory = fullPath.parent_path();
	
	if (!directory.empty() && !std::filesystem::exists(directory)) {
		try {
			std::filesystem::create_directories(directory);
			printf("[INFO] Created directory: %s\n", directory.string().c_str());
		} catch (const std::filesystem::filesystem_error& e) {
			printf("[ERROR] Failed to create directory: %s\n", e.what());
			return;
		}
	}

	// デフォルト値でJSONファイルを作成
	SaveToJson(filePath);
}

void Sprite::CreateVertexResource() {
	// 2D Sprite専用の頂点データ（Normalなし）
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(SpriteVertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(SpriteVertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(SpriteVertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	float width = size_.x;
	float height = size_.y;

	float left = 0.0f - anchorPoint_.x * size_.x;
	float right = 1.0f - anchorPoint_.x * size_.x;
	float top = 0.0f - anchorPoint_.y * size_.y;
	float bottom = 1.0f - anchorPoint_.y * size_.y;

	vertexData_[0].position = { left,bottom + height,0.0f,1.0f };  // 左下
	vertexData_[1].position = { left,top,0.0f,1.0f };     // 左上
	vertexData_[2].position = { right + width,bottom + height,0.0f,1.0f }; // 右下
	vertexData_[3].position = { right + width,top,0.0f,1.0f };    // 右上
	
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
}

void Sprite::CreateIndexResource() {
	// IndexResource
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// 表面（三角形2枚）
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
    indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;
}

void Sprite::CreateMaterialResource() {
	// 2D Sprite専用のマテリアルデータ（ライティングなし）
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(SpriteMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransformMatrix = MakeIdentityMatrix();
	// マップしたままにする（Update()で書き込みを行うため）
}

void Sprite::CreateTransformationResource() {
	// 2D Sprite専用の変換行列（WVPのみ）
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(SpriteTransformationMatrix));
	//
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//
	transformationData_->WVP = MakeIdentityMatrix();
}