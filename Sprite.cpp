#include"Sprite.h"
#include"WinApp.h"

void Sprite::Initialize(SpriteData* spriteData, const std::string& fileName) {
	//
	spriteData_ = spriteData;
	device_ = spriteData_->GetDxCommon()->GetDevice();
	commandList_ = spriteData_->GetDxCommon()->GetCommandList();
	
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(fileName);

	size_ = { 300.f,300.f };
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
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kClientWidth_), static_cast<float>(WinApp::kClientHeight_), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrixSprite));
	transformationData_->WVP = worldViewProjectionMatrixSprite;
}

void Sprite::Draw() {
	Logger::Log("Draw - TextureIndex: " + std::to_string(textureIndex_));
	Logger::Log("Material GPU Address: " + std::to_string(materialResource_->GetGPUVirtualAddress()));
	Logger::Log("Transform GPU Address: " + std::to_string(transformationResource_->GetGPUVirtualAddress()));
	Logger::Log("SRV GPU Handle: " + std::to_string(TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_).ptr));
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
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
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

	// 左下
	vertexData_[0].position = { 0.0f,height,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.f,1.0f };
	// 左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[2].position = { width,height,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.f,1.f };
	// 右上
	vertexData_[3].position = { width,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.f,0.0f };

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
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 1;	indexData_[4] = 3;	indexData_[5] = 2;
}

void Sprite::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = false;
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