#include"Sprite.h"
#include"WinApp.h"

#include<cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

// PSOManagerをインクルード（相対パス修正）
#include "../../System/PSOManager/PSOManager.h"

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
		// 注意: CreateMaterialResourceでUnmapしているが、描画時に再度Mapされている可能性がある
		// 安全のためチェック
		materialResource_->Unmap(0, nullptr);
		materialData_ = nullptr;
	}
	
	if (transformationResource_ && transformationData_) {
		transformationResource_->Unmap(0, nullptr);
		transformationData_ = nullptr;
	}
	
	if (directionalLightResource_ && directionalLightData_) {
		directionalLightResource_->Unmap(0, nullptr);
		directionalLightData_ = nullptr;
	}
	
	if (cameraResource_ && cameraData_) {
		cameraResource_->Unmap(0, nullptr);
		cameraData_ = nullptr;
	}
	
	if (pointLightResource_ && pointLightData_) {
		pointLightResource_->Unmap(0, nullptr);
		pointLightData_ = nullptr;
	}
	
	if (spotLightResource_ && spotLightData_) {
		spotLightResource_->Unmap(0, nullptr);
		spotLightData_ = nullptr;
	}
}

void Sprite::Initialize(DirectXCommon* dxCommon, const std::string& fileName, Vector2 position, Vector2 scale) {

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

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();

	cameraResource_ = CreateBufferResource(device_.Get(), sizeof(CameraForGPU));
	assert(cameraResource_ != nullptr);
	HRESULT hr = cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	assert(SUCCEEDED(hr));

	pointLightResource_ = CreateBufferResource(device_.Get(), sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->useLight = 0;
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,5.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 20.0f;
	pointLightData_->decay = 2.0f;

	spotLightResource_ = CreateBufferResource(device_.Get(), sizeof(SpotLight));
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	spotLightData_->useLight = 0;
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 2.0f,1.25f,0.0f };
	spotLightData_->distance = 7.0f;
	spotLightData_->direction = { -1.0f,-1.0f,0.0f };
	spotLightData_->intensity = 4.0f;
	spotLightData_->decay = 2.0f;
	spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 6.0f);
	spotLightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f);
}

void Sprite::Update() {

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(
		{ transform2D_.scale.x,transform2D_.scale.y,0.0f }, 
		{ 0.0f, 0.0f, transform2D_.rotate}, 
		{ transform2D_.translate.x, transform2D_.translate.y, 0.0f });
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
	auto rootSignature = psoManager.GetRootSignature("Object3D");
	auto pso = psoManager.GetPSO(PSOType::Model_Solid_Normal); // Sprite専用PSOを使用

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	// PSOを設定
	commandList_->SetPipelineState(pso.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);  // VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);
	//
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	// 描画！ (DrawCall/ドローコール)
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::DrawImGui(const char* objectName) {

#ifdef USE_IMGUI

	ImGui::Begin(objectName);

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
	anchorPoint = anchor;
	
	// 頂点データが既に作成されている場合、再計算
	if (vertexData_) {
		float width = size_.x;
		float height = size_.y;

		float left = 0.0f - anchorPoint.x * size_.x;
		float right = 1.0f - anchorPoint.x * size_.x;
		float top = 0.0f - anchorPoint.y * size_.y;
		float bottom = 1.0f - anchorPoint.y * size_.y;

		vertexData_[0].position = { left, bottom + height, 0.0f, 1.0f };  // 左下
		vertexData_[1].position = { left, top, 0.0f, 1.0f };              // 左上
		vertexData_[2].position = { right + width, bottom + height, 0.0f, 1.0f }; // 右下
		vertexData_[3].position = { right + width, top, 0.0f, 1.0f };     // 右上
	}
}

void Sprite::CreateVertexResource() {
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(ModelVertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(ModelVertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(ModelVertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	float width = size_.x;
	float height = size_.y;

	float left = 0.0f - anchorPoint.x * size_.x;
	float right = 1.0f - anchorPoint.x * size_.x;
	float top = 0.0f - anchorPoint.y * size_.y;
	float bottom = 1.0f - anchorPoint.y * size_.y;

	vertexData_[0].position = { left,bottom + height,0.0f,1.0f };  // 左下
	vertexData_[1].position = { left,top,0.0f,1.0f };     // 左上
	vertexData_[2].position = { right + width,bottom + height,0.0f,1.0f }; // 右下
	vertexData_[3].position = { right + width,top,0.0f,1.0f };    // 右上
	
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };

	for (uint32_t i = 0; i < 4; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}
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

	// 裏面（三角形2枚、巻き方向を逆に）
	/*indexData_[6] = 2; indexData_[7] = 1; indexData_[8] = 0;
	indexData_[9] = 2; indexData_[10] = 3; indexData_[11] = 1;*/
}

void Sprite::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(ModelMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = false;
	materialData_->uvTransformMatrix = MakeIdentityMatrix();
	// マップしたままにする（Update()で書き込みを行うため）
}

void Sprite::CreateTransformationResource() {
	//
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(ModelTransformationMatrix));
	//
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
}

void Sprite::CreateDirectionalLightResource() {
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	//
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->useLight = 0;
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
}