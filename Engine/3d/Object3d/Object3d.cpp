#include "Object3d.h"
#include "WinApp.h"

//"resources/uvChecker.png"

void Object3d::Initialize(Object3dData* object3dData,  const std::string& file , const std::string&modelName, const std::string& fileName) {
	object3dData_ = object3dData;
	device_ = object3dData_->GetDxCommon()->GetDevice();
	commandList_ = object3dData_->GetDxCommon()->GetCommandList();

	transform_ = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	modelData_ = LoadObject3dFile(file,modelName);

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(fileName);
	// 読み込んだテクスチャの番号を取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(fileName);

	CreateVertexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();
}

void Object3d::Update(Camera& useCamera) {

	transform_.rotate.y += 0.03f;
	// 行列の内容を更新して三角形を動かす
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));
	// シーン上で三角形を描画
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldViewProjectionMatrix;
}

void Object3d::Draw() {
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(nullptr); // インデックス使っていないので設定しない
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// RootParams 設定（そのままでOK）
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 頂点描画（DrawIndexedではなくDrawInstanced）
	commandList_->DrawInstanced(
		static_cast<UINT>(modelData_.vertices.size()), // 頂点数ぶん描画
		1, 0, 0);
}

void Object3d::CreateVertexResource() {
	// 頂点リソースをつくる
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * modelData_.vertices.size());
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size()); 
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	// 頂点リソースにデータを書き込む、書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点データをリソースにコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size()); 
}

void Object3d::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentityMatrix();
}

void Object3d::CreateTransformationResource() {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	// 単位行列を書き込む
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
}

void Object3d::CreateDirectionalLightResource() {
	// ライトのリソースを作る
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	// リソースをマップしてデータを書き込む
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
}