#include"Sprite.h"
#include"WinApp.h"

void Sprite::Initialize(SpriteData* spriteData, const std::string& fileName) {
	//
	spriteData_ = spriteData;
	device_ = spriteData_->GetDxCommon()->GetDevice();
	commandList_ = spriteData_->GetDxCommon()->GetCommandList();
	
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(fileName);

	rootSignature_ = spriteData_->GetRootsignature();

	graphicsPipelineState_ = spriteData_->GetPipelineState();

	size_ = { 256.f,256.f };
	transform_.scale = { 1.f,1.f,1.f };
	transform_.translate = { 0.f,0.f,0.f };

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();
}

void Sprite::Update() {
	transform_.translate.x = position_.x;
	transform_.translate.y = position_.y;

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentityMatrix();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kClientWidth_), static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrixSprite));
	transformationData_->WVP = worldViewProjectionMatrixSprite;
}

void Sprite::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);  // VBVを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);
	//
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	// Spriteを常にuvCheckerにする
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	//
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 描画！ (DrawCall/ドローコール)
	commandList_->DrawIndexedInstanced(12, 1, 0, 0, 0);
}

void Sprite::CreateVertexResource() {
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
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
	
	vertexData_[0].texcoord = { 0.f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.f,1.f };
	vertexData_[3].texcoord = { 1.f,0.0f };

	for (uint32_t i = 0; i < 4; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}
}

void Sprite::CreateIndexResource() {
	// IndexResource
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 12);
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 12;
	// format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// 表面（三角形2枚）
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;
	// 裏面（三角形2枚、巻き方向を逆に）
	indexData_[6] = 2; indexData_[7] = 1; indexData_[8] = 0;
	indexData_[9] = 2; indexData_[10] = 3; indexData_[11] = 1;
}

void Sprite::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentityMatrix();

	materialResource_->Unmap(0, nullptr);
}

void Sprite::CreateTransformationResource() {
	//
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
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
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
}