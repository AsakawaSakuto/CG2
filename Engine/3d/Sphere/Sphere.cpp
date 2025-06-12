#include"Sphere.h"

void Sphere::Initialize(SphereData* sphereData, const std::string& fileName) {

	sphereData_ = sphereData;

	device_ = sphereData_->GetDxCommon()->GetDevice();
	commandList_ = sphereData_->GetDxCommon()->GetCommandList();
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(fileName);

	rootSignature_ = sphereData_->GetRootsignature();

	graphicsPipelineState_ = sphereData_->GetPipelineState();

	transform_.scale = { 1.f,1.f,1.f };
	transform_.rotate = { 0.f,0.f,0.f };
	transform_.translate = { 0.f,0.f,0.f };

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();
}

void Sphere::Update(Camera& useCamera) {
	//transform_.rotate.x += 0.03f;
	//transform_.rotate.y += 0.03f;
	//transform_.rotate.z += 0.03f;

	// 行列の内容を更新して三角形を動かす
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));
	// シーン上で三角形を描画
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
}

void Sphere::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// RootParams 設定（そのままでOK）
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// インデックス描画
	commandList_->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		1, 0, 0, 0);
}

void Sphere::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	ImGui::Text("Transform");
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);

	ImGui::Text("ColorEdit");
	ImGui::ColorEdit4("Color", &materialData_->color.x);

	ImGui::End();

}

void Sphere::CreateVertexResource() {
	CreateIndexedSphereMesh(vertices_, indices_, subdivision_);

	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * vertices_.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexResource_->Unmap(0, nullptr);
}

void Sphere::CreateIndexResource() {
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * indices_.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexResource_->Unmap(0, nullptr);
}

void Sphere::CreateTransformationResource() {
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

void Sphere::CreateMaterialResource() {
	//
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
	//
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
}

void Sphere::CreateDirectionalLightResource() {
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	//
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
}

// 球体メッシュをインデックス付きで作成する関数
void CreateIndexedSphereMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, int subdivision) {
	const float pi = 3.1415926535f;
	const float lonEvery = 2.0f * pi / subdivision;
	const float latEvery = pi / subdivision;

	vertices.clear();
	indices.clear();

	auto Normalize = [](const Vector3& v) -> Vector3 {
		float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len > 0.0f) return { v.x / len, v.y / len, v.z / len };
		else return { 0.0f, 1.0f, 0.0f };
		};

	for (int lat = 0; lat <= subdivision; ++lat) {
		float theta = -pi / 2.0f + latEvery * lat;
		float v = 1.0f - float(lat) / subdivision;

		for (int lon = 0; lon <= subdivision; ++lon) {
			float phi = lonEvery * lon;
			float u = float(lon) / subdivision;

			Vector3 pos = {
				cosf(theta) * cosf(phi),
				sinf(theta),
				cosf(theta) * sinf(phi)
			};
			Vector3 normal = Normalize(pos);
			vertices.push_back({ { pos.x, pos.y, pos.z, 1.0f }, { u, v }, normal });
		}
	}

	for (int lat = 0; lat < subdivision; ++lat) {
		for (int lon = 0; lon < subdivision; ++lon) {
			int i0 = lat * (subdivision + 1) + lon;
			int i1 = i0 + 1;
			int i2 = i0 + (subdivision + 1);
			int i3 = i2 + 1;

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i1);

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}
}